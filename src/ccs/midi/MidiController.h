#ifndef CCS_MIDI_CONTROLLER_H
#define CCS_MIDI_CONTROLLER_H

#include <string>
#include <vector>
#include "../../reaper/csurf.h"
#include "MidiControlElement.h"
#include "yaml-cpp/yaml.h"
#include "../Util.h"
#include "MidiEventType.cpp"
#include "../config/MidiControllerConfig.h"
#include <experimental/filesystem>
#include "../actions/Action.h"
#include "../actions/ActionsManager.h"
#include "../actions/ActionProvider.h"
#include "../../reaper/reaper_plugin.h"

namespace CCS {

  namespace fse = std::experimental::filesystem;
  using std::string;
  using std::vector;

  /**
    *This represents a midi controller
   */
  class MidiController : public ActionProvider {
    string controllerId;
    string name;
    midi_Output *midiOutput;
    vector<MidiControlElement*> controls;
    MidiControllerConfig *config;
    unsigned char defaultStatusByte;
    ActionProvider *actionProvider;
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
    vector<MidiEventType> getMidiEventTypes();


    Action *createMidiControllerAction(string actionId, YAML::Node node);

    vector<string> getProcessedSubActions(vector<string> rawSubActions);

    void actionCallback(string actionId, vector<string> arguments) override;
    void createActions();

    void sendMidiMessage(string actionId, vector<string> arguments);
    void flushMidiMessagesBuffer();

    vector<int> getSubscribedMidiEventIds();

    int getMidiEventIdForControl(string controlId);
    string getControllerId();

    MidiControlElement *getMidiControlElement(string id);
  };
}

#endif