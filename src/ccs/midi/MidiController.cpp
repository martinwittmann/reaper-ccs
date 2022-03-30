#include <string>
#include <vector>
#include "../../reaper/csurf.h"
#include "MidiControlElement.h"
#include "yaml-cpp/yaml.h"
#include "../Util.h"
#include "MidiEventType.cpp"
#include "../config/MidiControllerConfig.h"
#include "MidiController.h"
#include "../globals.cpp"
#include "../actions/ActionProvider.h"
#include "../actions/ActionsManager.h"
#include "../Variables.h"
#include "../../reaper/reaper_plugin.h"
#include <iostream>
#include "../CcsException.h"

namespace CCS {

  using std::string;
  using std::vector;

  MidiController::MidiController(
    string configFilename,
    midi_Output *output,
    ActionsManager *actionsManager
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
      auto controlId = item.first.as<string>();
      YAML::Node controlNode = item.second;
      unsigned char status = defaultStatusByte;
      if (config->keyExists("status", &controlNode)) {
        status = Util::hexToInt(config->getValue("status", &controlNode));
      }

      // The values for press and release are only used for type: button.
      // Here we default to values that most midi controllers will use:
      // 0x7F for press and 0x00 for release.
      // The reason why we're always handing them over is, that it's less
      // code and easier to organize than call different constructors for
      // different control types.
      unsigned char data2Press = 0x7F;
      if (config->keyExists("data2_press", &controlNode)) {
        status = Util::hexToInt(config->getValue("data2_press", &controlNode));
      }

      unsigned char data2Release = 0x00;
      if (config->keyExists("data2_release", &controlNode)) {
        status = Util::hexToInt(config->getValue("data2_release", &controlNode));
      }

      controls.push_back(new MidiControlElement(
        controlId,
        MidiControlElement::getTypeByName(config->getValue("type", &controlNode)),
        status,
        Util::hexToInt(config->getValue("data1", &controlNode)),
        this,
        data2Press,
        data2Release
      ));
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

  void MidiController::actionCallback(std::string actionName, std::vector<std::string> arguments) {
    if (actionName == "send_midi_message") {
      return sendMidiMessage(actionName, arguments);
    }
    else if (actionName == "buffer_messages") {
      shouldBufferMidiMessages = true;
    }
    else if (actionName == "end_buffer") {
      shouldBufferMidiMessages = false;
      flushMidiMessagesBuffer();
    }
  }

  void MidiController::flushMidiMessagesBuffer() {
    _sendMidiMessage(&midiMessagesBuffer);
    midiMessagesBuffer.clear();
  }

  void MidiController::sendMidiMessage(std::string actionName, std::vector<std::string> arguments) {
    vector<unsigned char> bytes = Util::splitToBytes(arguments.at(0));
    if (shouldBufferMidiMessages) {
      midiMessagesBuffer.insert(midiMessagesBuffer.end(), bytes.begin(), bytes.end());
    }
    else {
      _sendMidiMessage(&bytes);
    }
  }

  void MidiController::_sendMidiMessage(vector<unsigned char> *buffer) {
    // I think this is a dirty hack, but I copied it from the examples and
    // the CSI source code.
    // I believe that we define a custom struct and additionally to the midi
    // event reserve enough memory to hold our additional bytes.
    // Additional because for some reason MIDI_event_t hard-codes messages to
    // be 4 bytes long. This seems to work though.
    // Additionally, I think we need to reserve more memory than actually
    // because I could not find a way of creating a struct and define the
    // size of data depending on the number of bytes our message will have.
    struct {
      MIDI_event_t message;
      unsigned char data[4096];
    } event;

    //std::cout << "send midi\n";
    event.message.frame_offset = 0;
    event.message.size = buffer->size();
    for (int i = 0; i < buffer->size(); ++i) {
      //std::cout << Util::formatHexByte(buffer->at(i)) << " ";
      event.message.midi_message[i] = buffer->at(i);
    }
    //std::cout << "\n\n";

#ifndef MOCK_MIDI
    if (midiOutput) {
      midiOutput->SendMsg(&event.message, -1);
    }
#endif
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
    provideAction(sendMidiAction);

    auto startBufferingMidiMessagesAction = new Action(
      controllerId,
      "buffer_messages",
      provider
    );
    provideAction(startBufferingMidiMessagesAction);

    auto flushMidiMessagesBufferAction = new Action(
      controllerId,
      "end_buffer",
      provider
    );
    provideAction(flushMidiMessagesBufferAction);

    // Get Actions from config.
    YAML::Node actionsNode = config->getMapValue("actions");
    for (const auto &item: actionsNode) {
      auto actionId = item.first.as<string>();
      YAML::Node actionNode = item.second;
      preprocessActionConfigNode(actionNode);
      Action *action = new Action(
        controllerId,
        actionId,
        actionNode,
        actionsManager
      );
      provideAction(action);
    }
  }

  // Since actions in midi controllers have some special features, we transform
  // a given config node in a way that Action and CompositeAction can handle.
  void MidiController::preprocessActionConfigNode(YAML::Node node) {
    // Basically we convert each message into a send midi action and merge all
    // send midi actions together for performance reasons.
    vector<string> rawSubActions = config->getListValues("message", &node);
    vector<string> processedSubActions = getProcessedSubActions(rawSubActions);
    node["actions"];
    for (auto subAction : processedSubActions) {
      node["actions"].push_back(subAction);
    }
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

  vector<int> MidiController::getSubscribedMidiEventIds() {
    vector<int> result;
    for (auto control : controls) {
      result.push_back(control->getInputEventId());
    }
    return result;
  }

  int MidiController::getMidiEventIdForControl(string controlId) {
    string dataByteRaw = config->getValue("controls." + controlId + ".data1");
    unsigned char dataByte = 0;
    if (!dataByteRaw.empty()) {
      dataByte = Util::hexToInt(dataByteRaw);
    }

    string statusByteRaw = config->getValue("controls." + controlId + ".status");
    unsigned char statusByte = defaultStatusByte;
    if (!statusByteRaw.empty()) {
      statusByte = Util::hexToInt(statusByteRaw);
    }
    int result = Util::getMidiEventId(statusByte, dataByte);
    return result;
  }

  string MidiController::getControllerId() {
    return controllerId;
  }

  MidiControlElement *MidiController::getMidiControlElement(string id) {
    for (auto control : controls) {
      if (control->getControlId() == id) {
        return control;
      }
    }
    throw CcsException("Could not find midi control element with id " + id);
  }
}