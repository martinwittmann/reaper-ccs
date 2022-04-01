#ifndef CCS_MIDI_CONTROLLER_H
#define CCS_MIDI_CONTROLLER_H

#include <string>
#include <vector>
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
    string controllerId;
    string name;
    midi_Output *midiOutput = nullptr;
    vector<MidiControlElement*> controls;
    MidiControllerConfig *config = nullptr;
    unsigned char defaultStatusByte;
    ActionProvider *actionProvider = nullptr;
    vector<Action*> providedActions;
    bool shouldBufferMidiMessages = false;
    vector<unsigned char> midiMessagesBuffer;

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
    void flushMidiMessagesBuffer();

    vector<int> getSubscribedMidiEventIds();

    int getMidiEventIdForControl(string controlId);
    string getControllerId();

    MidiControlElement *getMidiControlElement(string id);

    void preprocessActionConfigNode(YAML::Node node);
  };
}

#endif