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

    bool m_pickupValue = false;
    // The last physical position of the control, if an absolute control. This
    // is used to check if the physical value differs from the actual value.
    double m_physicalValue;
    double m_lastPhysicalValueDiff;
    bool m_allowChangingAbsoluteValue = false;

    MediaTrack *m_reaperTrack = nullptr;
    ReaperApi *m_api = nullptr;
    Session *m_session = nullptr;
    Page* m_page = nullptr;

    // The action types that are being used for the given control element.
    std::vector<string> m_actionTypes;
    string m_mappingType;
    string m_paramMapping;

    // We're defaulting to 0.5 since this appears to be a good compromise of
    // speed and control. This can be overridden per mapping in the config files.
    double m_relativeSpeedFactor = 0.5;

    short m_mappingTarget = MAPPING_TARGET_UNKNOWN;
    // A map of label:parameterValue for each available enum value.
    std::map<string,double> m_enumValues;

    bool m_trackExists = false;
    int m_trackId = -1;
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
    string m_radioFormattedValue;
    double m_radioValue;
    RadioGroup *m_radioGroup;

  public:
    const static short MAPPING_TARGET_UNKNOWN = -1;

    const static short TRANSPORT_PLAY = 1;
    const static short TRANSPORT_PAUSE = 2;
    const static short TRANSPORT_STOP = 3;
    const static short TRANSPORT_RECORD = 4;
    const static short TRANSPORT_REPEAT = 5;
    const static short TRANSPORT_REWIND = 6;
    const static short TRANSPORT_FAST_FORWARD = 7;

    const static short TRACK_VOLUME = 50;
    const static short TRACK_MUTE = 51;
    const static short TRACK_SOLO = 52;
    const static short TRACK_RECORD_ARM = 53;

    const static short FX_PARAMETER = 100;
    const static short FX_PRESET = 101;

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

    void onFxPresetChanged(
      MediaTrack *track,
      int fxId,
      int presetIndex,
      string presetName
    ) override;

    void initializeMappingValues(string rawMapping);
    void createActions();
    void invokeActions(string actionType);

    std::map<string,string> getActionVariables();
    double getToggledValue();

    std::vector<string> getAvailableActionTypes(
      string mappingType,
      short controlElementType
    );

    double getNextEnumValue();

    void updateControlElement();

    void updateValuesFromReaper();

    void activate();

    void deactivate();

    void onTrackVolumeChanged(double volume) override;

    int getControlSurfaceEventId(int mappingType);

    void onTrackMuteChanged(bool mute);

    void onTrackSoloChanged(bool solo);

    void onTrackRecordArmChanged(bool recordArm);

    void invokeOnValueChangeAction(bool forceUpdate = false);

    void updateFxParamValuesFromReaper();

    void selectThisRadioItem();

    void unselectThisRadioItem();

    void onButtonPress();

    void onButtonRelease();

    void applyValueChangeFromMidiEvent(double newValue);

    void onAbsoluteMidiValue(unsigned char data2);

    void onRelativeMidiValue(char diff);

    bool trackIsAvailable();

    void updateTrack();

    void onTrackListChanged(int numTracks) override;

    double getPreviousEnumValue();

    void onPlay(bool play);
    void onPause(bool play);

    void onStop(bool stop);

    void onRecord(bool record);

    void onRepeatChanged(bool repeat);
  };
}

#endif