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
#include "../Session.h"
#include "MidiControlElementAction.h"

namespace CCS {
  class MidiControlElementMapping :
    public MidiEventSubscriber,
    public ReaperEventSubscriber
  {
    int m_midiEventId;
    MappingConfig *m_config;
    MidiControlElement *m_controlElement;
    std::string m_controlId;
    std::string m_controllerId;
    short m_controlType;
    short m_onPressValue;
    short m_onReleaseValue;
    // We store the current value as double like reaper and convert it when needed.
    double m_value;
    std::string m_formattedValue;

    MediaTrack *m_reaperTrack;
    ReaperApi *m_api;
    Session *m_session;

    std::string m_paramMapping;
    MediaTrack *m_mappedTrack;
    int m_mappedFxId;
    int m_mappedParamId;
    std::vector<MidiControlElementAction*> m_actions;
    bool m_hasMappedFxParam = false;

    double m_mappedMinValue;
    double m_mappedMaxValue;
    double m_mappedMidValue;

  public:

    MidiControlElementMapping(
      int midiEventId,
      std::string controlId,
      YAML::Node config,
      MidiControlElement *controlElement,
      ReaperApi *api,
      Session *session
    );

    ~MidiControlElementMapping();

    int getMidiEventId();

    void onMidiEvent(int eventId, unsigned char data2) override;

    void onFxParameterChanged(
      MediaTrack *track,
      int fxId,
      int paramId,
      double value,
      double minValue,
      double maxValue,
      std::string formattedValue
    ) override;
    void initializeMappingValues(std::string rawMapping);
    void createActions();
    void invokeActions();

    void addValueDiff(char diff);
  };
}

#endif