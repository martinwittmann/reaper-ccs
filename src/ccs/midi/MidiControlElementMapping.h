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
#include "../actions/CompositeAction.h"

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

    // The action types that are being used for the given control element.
    std::vector<std::string> m_actionTypes;
    std::string m_mappingType;
    std::string m_paramMapping;
    // A map of label:parameterValue for each available enum value.
    std::map<string,double> m_enumValues;

    MediaTrack *m_track;
    int m_fxId;
    int m_paramId;
    std::string m_paramIdStr;
    std::map<std::string,CompositeAction*> m_actions;
    bool m_hasMappedFxParam = false;

    double m_minValue;
    double m_maxValue;
    double m_midValue;

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
    void invokeActions(string actionType);

    void addValueDiff(char diff);

    std::map<string,string> getActionVariables();
    void toggleValue();

    vector <string> getAvailableActionTypes(short controlElementType);

    void setNextEnumValue();

    void updateControlElement();
  };
}

#endif