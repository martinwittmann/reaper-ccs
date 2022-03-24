#ifndef CCS_MIDI_CONTROL_ELEMENT_MAPPING
#define CCS_MIDI_CONTROL_ELEMENT_MAPPING

#include <string>
#include <vector>
#include "yaml-cpp/yaml.h"
#include "MidiController.h"
#include "MidiEventSubscriber.h"

namespace CCS {
  class MidiControlElementMapping : public MidiEventSubscriber {
    int midiEventId;
    YAML::Node config;
    MidiControlElement* midiControlElement;
    std::string controlId;
    std::string controllerId;

  public:
    static const int BUTTON = 0;
    static const int ENCODER_RELATIVE = 0;

    MidiControlElementMapping(
      int midiEventId,
      std::string rawControlId,
      YAML::Node config,
      MidiControlElement* controller
    );

    int getMidiEventId();

    void onMidiEvent(int eventId, unsigned char dataByte) override;
    void createMidiControlElementMappings();
  };
}

#endif