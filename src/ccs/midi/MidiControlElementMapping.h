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
#include "../FxPlugins.h"

namespace CCS {
  class MidiControlElementMapping :
    public MidiEventSubscriber,
    public ReaperEventSubscriber
  {
    int m_midiEventId;
    MappingConfig* m_config;
    MidiControlElement* m_controlElement;
    std::string m_controlId;
    std::string m_controllerId;
    short m_controlType;
    short m_onPressValue;
    short m_onReleaseValue;
    short m_value;
    MediaTrack* m_reaperTrack;
    ReaperApi* m_api;
    FxPlugins* m_pluginManager;

    std::string m_paramMapping;
    MediaTrack* m_mappedTrack;
    int m_mappedFxId;
    int m_mappedParamId;

  public:

    MidiControlElementMapping(
      int midiEventId,
      std::string controlId,
      YAML::Node config,
      MidiControlElement* controlElement,
      ReaperApi* api,
      FxPlugins* pluginManager
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

    void setMappingValues(std::string rawMapping);
  };
}

#endif