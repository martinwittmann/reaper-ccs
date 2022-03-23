#include <string>
#include <vector>
#include "csurf.h"
#include "MidiControlElement.h"
#include "yaml-cpp/yaml.h"
#include "Util.h"
#include "MidiEventType.cpp"
#include "config/MidiControllerConfig.h"
#include "MidiController.h"
#include "globals.cpp"
#include "actions/ActionProvider.h"
#include "actions/Actions.h"
#include "Variables.h"
#include "sdk/reaper_plugin.h"
#include <iostream>

namespace CCS {

  typedef void (ActionProvider::*pActionCallback)(vector<string>);

  using std::string;
  using std::vector;

  MidiController::MidiController(
    string configFilename,
    int deviceId,
    midi_Output *output,
    Actions* actionsManager
  ) : ActionProvider(actionsManager) {
    config = new MidiControllerConfig(configFilename);
    controllerId = config->getValue("id");
    name = config->getValue("name");
    registerActionProvider(controllerId);
    midiOutput = output;

    this->defaultStatusByte = Util::hexToInt(config->getValue("default_status"));

    initializeControls();
    createActions();
  }

  MidiController::~MidiController() {
    delete config;
    for (auto it = controls.begin(); it != controls.end(); ++it) {
      delete *it;
    }
    for (auto it = providedActions.begin(); it != providedActions.end(); ++it) {
      delete *it;
    }
  }

  void MidiController::initializeControls() {
    YAML::Node controlConfigs = config->getMapValue("controls");

    for (const auto &item: controlConfigs) {
      auto id = item.first.as<string>();
      YAML::Node controlNode = item.second;
      unsigned int status = defaultStatusByte;
      if (config->keyExists("status", &controlNode)) {
        status = Util::hexToInt(config->getValue("status", &controlNode));
      }

      auto controlElement = new MidiControlElement(
        config->getValue("type", &controlNode),
        status,
        Util::hexToInt(config->getValue("message.data1", &controlNode))
      );
      controls.push_back(controlElement);
    }
  }

  bool MidiController::isMidiControllerConfigFile(fse::path path) {
    string filename = path.filename();
    filename.length();
    // Note that we ignore files starting with _ here. This allows us to use
    // those as templates to inherit from, like _default.yml.
    if (filename.starts_with(".") || filename.starts_with("_") || !filename.ends_with(YAML_EXT)) {
      return false;
    }
    return is_regular_file(path);
  }

  vector<MidiEventType> MidiController::getMidiEventTypes() {
    vector<MidiEventType> result;
    for (auto it = controls.begin(); it != controls.end(); ++it) {
      MidiControlElement *element = *it;
      MidiEventType event = element->getEventType();
      result.push_back(event);
    }
    return result;
  }

  void MidiController::actionCallback(std::string actionName, std::vector<std::string> arguments) {
    if (actionName == "send_midi_message") {
      Util::debug(controllerId + ": Send midi message: " + arguments.at(0));
      vector<unsigned char> bytes = Util::splitToBytes(arguments.at(0));

      // I think this is a dirty hack, but I copied it from the examples and
      // the CSI source code.
      // I believe that we define a custom struct and additionally to the midi
      // event reserve enough memory to hold our additional bytes.
      // Additional because for some reason MIDI_event_t hard-codes messages to
      // be 4 bytes long. This seems to work though.
      // Additionally, I think we need to reseve more memory than actually
      // because I could not find a way of creating a struct and define the
      // size of data depending on the number of bytes our message will have.
      struct {
        MIDI_event_t message;
        char data[1024];
      } event;

      event.message.frame_offset = 0;
      event.message.size = bytes.size();
      for (int i = 0; i < bytes.size(); ++i) {
        event.message.midi_message[i] = bytes.at(i);
      }

      if (midiOutput) {
        midiOutput->SendMsg(&event.message, -1);
      }
      else {
        Util::debug("Midi output not available");
      }
    }
  }

  void MidiController::createActions() {
    // Each midi controller needs to provide at least 1 action for sending
    // midi messages;

    auto provider = dynamic_cast<ActionProvider*>(this);
    auto sendMidiAction = new Action(
      controllerId,
      "send_midi_message",
      provider
    );
    providedActions.push_back(sendMidiAction);
    actionsManager->registerAction(*sendMidiAction);

    // Get Actions from config.
    YAML::Node actionsNode = config->getMapValue("actions");
    YAML::Node variablesNode = config->getMapValue("variables");
    std::map<string,string> variables = Variables::getVariables(variablesNode);
    for (const auto &item: actionsNode) {
      auto actionId = item.first.as<string>();
      Action* action = createMidiControllerAction(
        actionId,
        item.second,
        variables
      );
      providedActions.push_back(action);
      actionsManager->registerAction(*action);
    }
  }

  Action* MidiController::createMidiControllerAction(
    string actionId,
    YAML::Node node,
    std::map<string,string> variables
  ) {
    vector<string> rawSubActions = config->getListValues("message", &node);
    vector<string> processedSubActions = getProcessedSubActions(rawSubActions);
    vector<string> argumentNames = config->getListValues("arguments", &node);

    return new Action(
      controllerId,
      actionId,
      argumentNames,
      processedSubActions,
      actionsManager
    );
  }

  vector<string> MidiController::getProcessedSubActions(vector<string> rawSubActions) {
    vector<string> result;

    vector<string>midiMessages;
    for (auto it = rawSubActions.begin(); it != rawSubActions.end(); ++it) {
      string subAction = *it;
      // Actions for midi controllers are always composite actions.
      // To make writing actions less verbose we allow users to skip defining
      // the action type and simply default to send midi message.
      // This means that we need to preprocess each message item here and add
      // the appropriate action.


      // For each message item, check if it is an action/macro (contains []).
      // If not add the [controllerId.sendMidiMessage:messageString] action.
      // Also try to merge multiple subsequent send midi messages into a single
      // send midi message to not add performance bottlenecks by sending lots
      // of small messages after each other.
      if (isMacroAction(subAction)) {
        if (!midiMessages.empty()) {
          // Since the current action is not a midi action, but the last one(s)
          // were, we need to add the accumulated send midi actions before adding
          // the current macro action.
          string rawMessage = Util::joinStrVector(midiMessages, ' ');
          string midiMessage = "[" + controllerId + ".send_midi_message:" + rawMessage + "]";
          midiMessages.clear();
          result.push_back(midiMessage);
        }

        // Now add the current macro action.
        result.push_back("[" + controllerId + "." + Util::removePrefixSuffix(subAction) + "]");
      }
      else {
        // Here subAction is simply a string of midi messages, possibly containing
        // variables.
        midiMessages.push_back(subAction);
      }
    }

    // At the and we need to add an action for the rest of the added midi messages.
    if (!midiMessages.empty()) {
      // Since the current action is not a midi action, but the last one(s)
      // were, we need to add the accumulated send midi actions before adding
      // the current macro action.
      string rawMessage = Util::joinStrVector(midiMessages, ' ');
      string midiMessage = "[" + controllerId + ".send_midi_message:" + rawMessage + "]";
      midiMessages.clear();
      result.push_back(midiMessage);
    }
    return result;
  }

  bool MidiController::isMacroAction(string rawAction) {
    // If the string contains brackets we think it's a macro action because we
    // define that macros/subactions are being called by using
    // [action_provider.action_id:argument1:argument2:...]
    return rawAction.find("[") != -1;
  }
}