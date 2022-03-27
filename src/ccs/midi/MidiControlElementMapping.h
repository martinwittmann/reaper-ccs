#ifndef CCS_MIDI_CONTROL_ELEMENT_MAPPING
#define CCS_MIDI_CONTROL_ELEMENT_MAPPING

#include <string>
#include <vector>
#include "yaml-cpp/yaml.h"
#include "MidiController.h"
#include "MidiEventSubscriber.h"
#include "../../reaper-api/ReaperEventSubscriber.h"
#include "../../reaper-api/ReaperApi.h"
#include "../config/MappingConfig.h"

namespace CCS {
  class MidiControlElementMapping :
    public MidiEventSubscriber,
    public ReaperEventSubscriber
  {
    int midiEventId;
    MappingConfig* config;
    MidiControlElement* controlElement;
    std::string controlId;
    std::string controllerId;
    short controlType;
    short onPressValue;
    short onReleaseValue;
    short value;
    MediaTrack* reaperTrack;
    ReaperApi* api;

  public:

    MidiControlElementMapping(
      int midiEventId,
      std::string controlId,
      YAML::Node config,
      MidiControlElement* controlElement,
      ReaperApi* api
    );

    ~MidiControlElementMapping();

    int getMidiEventId();

    void onMidiEvent(int eventId, unsigned char data2) override;
    MediaTrack* getTrack(int trackIndex);
    std::string getFxParameterName(
      MediaTrack* track,
      int fxId,
      int parameterId
    );

    void onFxParameterChanged(
      MediaTrack* track,
      int fxId,
      int paramId,
      double value
    ) override;
  };
}

#endif