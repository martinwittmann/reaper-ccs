#include <string>
#include <vector>
#include "csurf.h"
#include "MidiControlElement.cpp"
#include "yaml-cpp/yaml.h"
#include "Util.h"
#include "MidiEventType.cpp"
#include "config/MidiControllerConfig.h"
#include<experimental/filesystem>

#ifndef MIDI_CONTROLLER_H
#define MIDI_CONTROLLER_H

namespace CCS {

  namespace fse = std::experimental::filesystem;
  using std::string;
  using std::vector;

  /**
   * This represents a midi controller
   */
  class MidiController {
    string id;
    string name;
    int midiDeviceId;
    midi_Output *midiOutput;
    vector<MidiControlElement *> controls;
    MidiControllerConfig *config;
    unsigned int defaultStatusByte;

  public:
    MidiController(
      string configFilename,
      int deviceId
      //midi_Output *output
    );

    ~MidiController();

    void initializeControls();

    static bool isMidiControllerConfigFile(fse::path path);
    vector<MidiEventType> getMidiEventTypes();
  };
}

#endif