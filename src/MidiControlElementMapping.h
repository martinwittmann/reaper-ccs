#ifndef CCS_MIDI_CONTROL_ELEMENT_MAPPING
#define CCS_MIDI_CONTROL_ELEMENT_MAPPING

#include <string>
#include <vector>
#include "yaml-cpp/yaml.h"
#include "MidiController.h"
#include "MidiEventSubscriber.h"

namespace CCS {
  class MidiControlElementMapping {
    YAML::Node config;
    MidiController* midiController;
    MidiEventSubscriber* subscriber;
    std::string controlId;
    std::string controllerId;

  public:
    MidiControlElementMapping(
      std::string rawControlId,
      YAML::Node config,
      MidiController* controller,
      MidiEventSubscriber* subscriber
    );

    int getMidiEventId();
  };
}

#endif