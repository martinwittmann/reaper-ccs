#include <string>
#include <vector>
#include "../../reaper/csurf.h"
#include "yaml-cpp/yaml.h"
#include "../Util.h"
#include "MidiEventType.h"
#include "../config/MidiControllerConfig.h"
#include "../globals.cpp"
#include "../actions/ActionProvider.h"
#include "../actions/ActionsManager.h"
#include "../actions/Action.h"
#include "../Variables.h"
#include "../../reaper/reaper_plugin.h"
#include <iostream>
#include "../CcsException.h"
#include "MidiController.h"
#include "MidiControlElement.h"

namespace CCS {

  using std::string;
  using std::vector;

  MidiController::MidiController(
    string configFilename,
    midi_Output *output,
    ActionsManager *actionsManager
  ) : ActionProvider(actionsManager) {
    m_config = new MidiControllerConfig(configFilename);
    m_controllerId = m_config->getValue("id");
    m_name = m_config->getValue("name");
    registerActionProvider(m_controllerId);
    m_midiOutput = output;

    m_defaultStatusByte = Util::hexToInt(m_config->getValue("default_status"));

    initializeControls();
    createActions();
  }

  MidiController::~MidiController() {
    delete m_config;
    for (auto it = m_controls.begin(); it != m_controls.end(); ++it) {
      delete *it;
    }
    for (auto it = m_providedActions.begin(); it != m_providedActions.end(); ++it) {
      delete *it;
    }
  }

  void MidiController::initializeControls() {
    YAML::Node controlConfigs = m_config->getMapValue("controls");

    for (const auto &item: controlConfigs) {
      auto controlId = item.first.as<string>();
      YAML::Node controlNode = item.second;
      unsigned char status = m_defaultStatusByte;
      if (m_config->keyExists("status", &controlNode)) {
        status = Util::hexToInt(m_config->getValue("status", &controlNode));
      }

      // The values for press and release are only used for type: button.
      // Here we default to values that most midi controllers will use:
      // 0x7F for press and 0x00 for release.
      // The reason why we're always handing them over is, that it's less
      // code and easier to organize than call different constructors for
      // different control types.
      unsigned char data2Press = 0x7F;
      if (m_config->keyExists("data2_press", &controlNode)) {
        status = Util::hexToInt(m_config->getValue("data2_press", &controlNode));
      }

      unsigned char data2Release = 0x00;
      if (m_config->keyExists("data2_release", &controlNode)) {
        status = Util::hexToInt(m_config->getValue("data2_release", &controlNode));
      }

      m_controls.push_back(new MidiControlElement(
        controlId,
        MidiControlElement::getTypeByName(m_config->getValue("type", &controlNode)),
        status,
        Util::hexToInt(m_config->getValue("data1", &controlNode)),
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
    else if (actionName == "start_buffer") {
      if (arguments.empty()) {
        Util::error("Invoked action " + m_controllerId + "." + actionName + " without arguments. You need to provide a buffer name as argument.");
        return;
      }
      string bufferName = arguments.at(0);
      startMessagesBuffer(bufferName);
    }
    else if (actionName == "stop_buffer") {
      if (arguments.empty()) {
        Util::error("Invoked action " + m_controllerId + "." + actionName + " without arguments. You need to provide a buffer name as argument.");
        return;
      }
      string bufferName = arguments.at(0);
      stopMessagesBuffer(bufferName);
    }
    else if (actionName == "flush_buffer") {
      string bufferName = arguments.at(0);
      flushMessagesBuffer(bufferName);
    }
  }

  void MidiController::startMessagesBuffer(string name) {
    if (!messagesBufferExists(name)) {
      addMessagesBuffer(name);
    }
    m_activeMessageBuffer = name;
  }

  void MidiController::stopMessagesBuffer(string name) {
    m_activeMessageBuffer = "";
  }

  void MidiController::flushMessagesBuffer(string name) {
    vector<unsigned char> *buffer = getMessagesBuffer(name);
    _sendMidiMessage(buffer);
    buffer->clear();
    stopMessagesBuffer(name);
    removeMessagesBuffer(name);
  }

  bool MidiController::messagesBufferExists(string name) {
    for (auto buffer : m_midiMessageBuffers) {
      if (buffer.first == name) {
        return true;
      }
    }
    return false;
  }

  vector<unsigned char> *MidiController::getMessagesBuffer(string name) {
    for (auto &buffer : m_midiMessageBuffers) {
      if (buffer.first == name) {
        return &buffer.second;
      }
    }
    throw CcsException("Trying to get unknown midi messages buffer: " + name + ".");
  }

  void MidiController::addMessagesBuffer(string name) {
    vector<unsigned char> buffer;
    m_midiMessageBuffers.insert(std::pair(name, buffer));
  }

  void MidiController::removeMessagesBuffer(string name) {
    for (
      auto it = m_midiMessageBuffers.begin();
      it != m_midiMessageBuffers.end();
      ++it
    ) {
      if (it->first == name) {
        m_midiMessageBuffers.erase(it);
        break;
      }
    }
  }

  void MidiController::addMessagesToBuffer(string name, vector<unsigned char> bytes) {
    vector<unsigned char> *buffer = getMessagesBuffer(name);
    buffer->insert(buffer->end(), bytes.begin(), bytes.end());
  }

  void MidiController::sendMidiMessage(std::string actionName, std::vector<std::string> arguments) {
    if (arguments.empty()) {
      Util::error("Invoked action " + m_controllerId + "." + actionName + " without arguments. It needs 1 argument:\n- MESSAGE");
      return;
    }
    vector<unsigned char> bytes = Util::splitToBytes(arguments.at(0));

    // Always add the messages to the currently active buffer if any.
    // When the buffer is flushed it is removed from m_midiMessagesBuffers.
    if (!m_activeMessageBuffer.empty()) {
      addMessagesToBuffer(m_activeMessageBuffer, bytes);
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

    //Util::log("Send midi ", false);
    event.message.frame_offset = 0;
    event.message.size = buffer->size();
    for (int i = 0; i < buffer->size(); ++i) {
      Util::log(Util::formatHexByte(buffer->at(i)) + " ", false);
      event.message.midi_message[i] = buffer->at(i);
    }
    Util::log("");

#ifndef MOCK_MIDI
    if (m_midiOutput) {
      m_midiOutput->SendMsg(&event.message, -1);
    }
#endif
  }

  void MidiController::createActions() {
    // Each midi controller needs to provide at least 1 action for sending
    // midi messages;

    auto provider = dynamic_cast<ActionProvider*>(this);

    auto sendMidiAction = new Action(
      m_controllerId,
      "send_midi_message",
      provider
    );
    provideAction(sendMidiAction);

    auto startBufferAction = new Action(
      m_controllerId,
      "start_buffer",
      provider
    );
    provideAction(startBufferAction);

    auto stopBufferAction = new Action(
      m_controllerId,
      "stop_buffer",
      provider
    );
    provideAction(stopBufferAction);

    auto flushMidiMessagesBufferAction = new Action(
      m_controllerId,
      "flush_buffer",
      provider
    );
    provideAction(flushMidiMessagesBufferAction);

    // Get Actions from config.
    YAML::Node actionsNode = m_config->getMapValue("actions");
    for (const auto &item: actionsNode) {
      auto actionId = item.first.as<string>();
      YAML::Node actionNode = item.second;
      preprocessActionConfigNode(actionNode);
      Action *action = new Action(
        m_controllerId,
        actionId,
        actionNode,
        m_actionsManager
      );
      provideAction(action);
    }
  }

  // Since actions in midi controllers have some special features, we transform
  // a given config node in a way that Action and CompositeAction can handle.
  void MidiController::preprocessActionConfigNode(YAML::Node node) {
    // Basically we convert each message into a send midi action and merge all
    // send midi actions together for performance reasons.
    vector<string> rawSubActions = m_config->getListValues("message", &node);
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
          string midiMessage = "[" + m_controllerId + ".send_midi_message:" + rawMessage + "]";
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

    // At the and we need to add an action for the rest of the added midi messages.
    if (!midiMessages.empty()) {
      // Since the current action is not a midi action, but the last one(s)
      // were, we need to add the accumulated send midi actions before adding
      // the current macro action.
      string rawMessage = Util::joinStrVector(midiMessages, ' ');
      string midiMessage = "[" + m_controllerId + ".send_midi_message:" + rawMessage + "]";
      midiMessages.clear();
      result.push_back(midiMessage);
    }
    return result;
  }

  vector<int> MidiController::getSubscribedMidiEventIds() {
    vector<int> result;
    for (auto control : m_controls) {
      result.push_back(control->getInputEventId());
    }
    return result;
  }

  int MidiController::getMidiEventIdForControl(string controlId) {
    string dataByteRaw = m_config->getValue("controls." + controlId + ".data1");
    unsigned char dataByte = 0;
    if (!dataByteRaw.empty()) {
      dataByte = Util::hexToInt(dataByteRaw);
    }

    string statusByteRaw = m_config->getValue("controls." + controlId + ".status");
    unsigned char statusByte = m_defaultStatusByte;
    if (!statusByteRaw.empty()) {
      statusByte = Util::hexToInt(statusByteRaw);
    }
    int result = Util::getMidiEventId(statusByte, dataByte);
    return result;
  }

  string MidiController::getControllerId() {
    return m_controllerId;
  }

  MidiControlElement *MidiController::getMidiControlElement(string id) {
    for (auto control : m_controls) {
      if (control->getControlId() == id) {
        return control;
      }
    }
    throw CcsException("Could not find midi control element with id " + id);
  }
}