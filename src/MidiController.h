#ifndef CCS_MIDI_CONTROLLER_H
#define CCS_MIDI_CONTROLLER_H

#include <string>
#include <vector>
#include "csurf.h"
#include "MidiControlElement.h"
#include "yaml-cpp/yaml.h"
#include "Util.h"
#include "MidiEventType.cpp"
#include "config/MidiControllerConfig.h"
#include<experimental/filesystem>
#include "actions/Action.h"
#include "actions/Actions.h"
#include "actions/ActionProvider.h"

namespace CCS {

  namespace fse = std::experimental::filesystem;
  using std::string;
  using std::vector;

  /**
   * This represents a midi controller
   */
  class MidiController : ActionProvider {
    string controllerId;
    string name;
    int midiDeviceId;
    midi_Output *midiOutput;
    vector<MidiControlElement *> controls;
    MidiControllerConfig *config;
    unsigned int defaultStatusByte;
    ActionProvider* actionProvider;
    vector<Action*> actions;

  public:
    MidiController(
      string configFilename,
      int deviceId,
      Actions* actionManager
      //midi_Output *output
    );

    ~MidiController();

    void initializeControls();

    static bool isMidiControllerConfigFile(fse::path path);
    vector<MidiEventType> getMidiEventTypes();

    void createActions();

    void sendMidiMessageToController(vector<string> arguments);

    Action* createMidiControllerAction(
      string actionId,
      YAML::Node node,
      std::map<string,string> variables
    );

    bool isMacroAction(string rawAction);

    vector<string> getProcessedSubActions(vector<string> rawSubActions);
  };
}

#endif