#ifndef CCS_MIDI_CONTROL_ELEMENT_MAPPING
#define CCS_MIDI_CONTROL_ELEMENT_MAPPING

#include <string>
#include <vector>
#include "yaml-cpp/yaml.h"
#include "MidiEventSubscriber.h"
#include "../../reaper-api/ReaperEventSubscriber.h"
#include <chrono>

namespace CCS {

  using std::string;

  class MidiControlElement;
  class MappingConfig;
  class Session;
  class CompositeAction;
  class Page;
  class ReaperApi;
  class RadioGroup;

  class MidiControlElementMapping :
    public MidiEventSubscriber,
    public ReaperEventSubscriber
  {
    int m_midiEventId;
    MappingConfig *m_config = nullptr;
    MidiControlElement *m_controlElement = nullptr;
    string m_controlId;
    string m_controllerId;
    short m_controlType;
    short m_onPressValue;
    short m_onReleaseValue;
    // We store the current value as double like reaper and convert it when needed.
    double m_value;
    string m_formattedValue;

    MediaTrack *m_reaperTrack = nullptr;
    ReaperApi *m_api = nullptr;
    Session *m_session = nullptr;
    Page* m_page = nullptr;

    // The action types that are being used for the given control element.
    std::vector<string> m_actionTypes;
    string m_mappingType;
    string m_paramMapping;

    short m_mappingTarget = MAPPING_TARGET_UNKNOWN;
    // A map of label:parameterValue for each available enum value.
    std::map<string,double> m_enumValues;

    MediaTrack *m_track = nullptr;
    int m_fxId;
    int m_paramId;
    string m_paramIdStr;
    std::map<string,CompositeAction*> m_actions;
    bool m_hasMapping = false;

    double m_minValue;
    double m_maxValue;
    double m_midValue;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastOnValueChangeAction = std::chrono::high_resolution_clock::now();
    string m_radioGroupId;
    string m_radioValue;
    RadioGroup *m_radioGroup;

  public:
    const static short MAPPING_TARGET_UNKNOWN = -1;
    const static short TRACK_VOLUME = 0;
    const static short TRACK_MUTE = 1;
    const static short TRACK_SOLO = 2;
    const static short TRACK_RECORD_ARM = 3;
    const static short FX_PARAMETER = 10;

    MidiControlElementMapping(
      int midiEventId,
      string controlId,
      YAML::Node config,
      MidiControlElement *controlElement,
      ReaperApi *api,
      Session *session,
      Page *page
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
      string formattedValue
    ) override;
    void initializeMappingValues(string rawMapping);
    void createActions();
    void invokeActions(string actionType);

    void addValueDiff(char diff);

    std::map<string,string> getActionVariables();
    void toggleValue();

    std::vector<string> getAvailableActionTypes(
      string mappingType,
      short controlElementType
    );

    void setNextEnumValue();

    void updateControlElement();

    void updateValuesFromReaper();

    void activate();

    void deactivate();

    double getRelativeValueDiff(unsigned char rawDiff);

    void onTrackVolumeChanged(double volume) override;

    int getControlSurfaceEventId(int mappingType);

    void onTrackMuteChanged(bool mute);

    void onTrackSoloChanged(bool solo);

    void onTrackRecordArmChanged(bool recordArm);

    void invokeOnValueChangeAction(bool forceUpdate = false);

    void updateFxParamValuesFromReaper();

    void selectThisRadioItem();

    void unselectThisRadioItem();
  };
}

#endif