#ifndef CCS_MIDI_CONTROLLER_H
#define CCS_MIDI_CONTROLLER_H

#include <string>
#include <vector>
#include <map>
#include <experimental/filesystem>
#include "yaml-cpp/yaml.h"
#include "../../reaper/csurf.h"
#include "../../reaper/reaper_plugin.h"
#include "MidiEventType.h"
#include "../actions/ActionProvider.h"

namespace CCS {

  namespace fse = std::experimental::filesystem;
  using std::string;
  using std::vector;

  class MidiControllerConfig;
  class Action;
  class ActionsManager;
  class MidiControlElement;

  /**
    *This represents a midi controller
   */
  class MidiController : public ActionProvider {
    string m_controllerId;
    string m_name;
    midi_Output *m_midiOutput = nullptr;
    vector<MidiControlElement*> m_controls;
    MidiControllerConfig *m_config = nullptr;
    unsigned char m_defaultStatusByte;
    ActionProvider *m_actionProvider = nullptr;
    vector<Action*> m_providedActions;

    // Since std::map is ordered by key and not adding order we need a separate
    // member keeping track of the order of opened buffers.
    // Messages will always be added to the last added buffer, or sent immediately
    // if no buffers exist.
    std::map<string,vector<unsigned char>> m_midiMessageBuffers;
    string m_activeMessageBuffer;

    void _sendMidiMessage(vector<unsigned char> *buffer);
  public:
    MidiController(
      string configFilename,
      midi_Output *output,
      ActionsManager *actionsManager
    );

    ~MidiController();
    void initializeControls();
    static bool isMidiControllerConfigFile(fse::path path);
    vector<string> getProcessedSubActions(vector<string> rawSubActions);
    void actionCallback(string actionId, vector<string> arguments) override;
    void createActions();
    void sendMidiMessage(string actionId, vector<string> arguments);
    vector<int> getSubscribedMidiEventIds();
    int getMidiEventIdForControl(string controlId);
    string getControllerId();
    MidiControlElement *getMidiControlElement(string id);
    void preprocessActionConfigNode(YAML::Node node);
    bool messagesBufferExists(string name);
    void addMessagesBuffer(string name);
    void removeMessagesBuffer(string name);
    void flushMessagesBuffer(string name);
    vector<unsigned char> *getMessagesBuffer(string name);

    void addMessagesToBuffer(string name, vector<unsigned char> bytes);

    void startMessagesBuffer(string name);

    void stopMessagesBuffer(string name);
  };
}

#endif