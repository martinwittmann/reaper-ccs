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

namespace CCS {

  typedef void (ActionProvider::*pActionCallback)(vector<string>);

  using std::string;
  using std::vector;

  MidiController::MidiController(
    string configFilename,
    int deviceId,
    Actions* actionsManager
    //midi_Output *output
  ) : ActionProvider(actionsManager) {
    config = new MidiControllerConfig(configFilename);
    controllerId = config->getValue("id");
    name = config->getValue("name");
    registerActionProvider(controllerId);

    //this->midiOutput = output;
    this->defaultStatusByte = Util::hexToInt(config->getValue("default_status"));
    initializeControls();
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

  }

  void MidiController::createActions() {
    // Each midi controller needs to provide at least 1 action for sending
    // midi messages;

    auto provider = dynamic_cast<ActionProvider*>(this);
    providedActions.push_back(new Action(
      controllerId,
      "send_midi_message",
      provider
    ));


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
        result.push_back(subAction);
      }
      else {
        // Here subAction is simply a string of midi messages, possibly containing
        // variables.
        midiMessages.push_back(subAction);
      }
    }
    return result;
  }

  bool MidiController::isMacroAction(string rawAction) {
    // If the string contains brackets we think it's a macro action because we
    // define that macros/subactions are being called by using
    // [action_provider.action_id:argument1:argument2:...]
    return rawAction.find("[") == -1;
  }

  void MidiController::sendMidiMessageToController(vector<string> arguments) {
    // TODO Use the provided midi output to actually send the message.
  }
}