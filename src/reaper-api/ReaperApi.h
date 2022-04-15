#ifndef CCS_REAPERAPI_H
#define CCS_REAPERAPI_H

#include <string>
#include <vector>
#include <map>
#include "ReaperEventSubscriber.h"
#include "ReaperDataTracker.h"

namespace CCS {

  using std::string;
  using std::vector;

  class FxParameterChangedSubscription;
  class FxPresetChangedSubscription;
  class ControlSurfaceEventSubscription;

  class ReaperApi {
    vector<FxParameterChangedSubscription*> m_fxParamChangedSubscriptions;
    vector<FxPresetChangedSubscription*> m_fxPresetChangedSubscriptions;
    vector<ControlSurfaceEventSubscription*> m_controlSurfaceEventSubscriptions;

  public:

    // Event ids:
    static const unsigned short ON_PLAY = 0;
    static const unsigned short ON_PAUSE = 1;
    static const unsigned short ON_RECORD = 2;
    static const unsigned short ON_STOP = 3;
    static const unsigned short ON_REPEAT_CHANGED = 4;
    static const unsigned short ON_REWIND = 5;
    static const unsigned short ON_FAST_FORWARD = 6;

    static const unsigned short ON_TRACK_LIST_CHANGED = 100;

    static const unsigned short ON_TRACK_VOLUME_CHANGED = 1000;
    static const unsigned short ON_TRACK_MUTE_CHANGED = 1001;
    static const unsigned short ON_TRACK_SOLO_CHANGED = 1002;
    static const unsigned short ON_TRACK_RECORD_ARM_CHANGED = 1003;

    static const unsigned short ON_FX_PARAM_CHANGED = 2000;

    ReaperApi();

    void subscribeToFxParameterChanged(
      MediaTrack *track,
      int fxId,
      int paramId,
      ReaperEventSubscriber *subscriber
    );

    void unsubscribeFromFxParameterChanged(
      MediaTrack *track,
      int fxId,
      int paramId,
      ReaperEventSubscriber *subscriber
    );

    bool isSubscribedToFxParameterChanged(
      MediaTrack *track,
      int fxId,
      int paramId,
      ReaperEventSubscriber *subscriber
    );

    int getNumTracks();
    string getFxParameterName(
      MediaTrack *track,
      int fxId,
      int parameterId
    );

    std::vector<string> getTrackNames();

    string getTrackName(int trackIndex);
    string getTrackName(MediaTrack *track);

    std::tuple<double,double> getFxParameterMinMax(
      MediaTrack *track,
      int fxId,
      int parameterId
    );

    void pollReaperData();

    void triggerOnPlay(bool play);
    void triggerOnPause(bool pause);
    void triggerOnRecord(bool record);
    void triggerOnStop(bool stop);
    void triggerOnRepeatChanged(bool repeat);

    void triggerOnTrackVolumeChanged(MediaTrack *track, double volume);
    void triggerOnTrackMuteChanged(MediaTrack *track, bool repeat);
    void triggerOnTrackSoloChanged(MediaTrack *track, bool repeat);
    void triggerOnTrackRecordArmChanged(MediaTrack *track, bool repeat);

    void triggerOnFxParameterChanged(
      MediaTrack *track,
      int fxId,
      int paramId,
      double value,
      double minValue,
      double maxValue,
      string formattedValue
    );

    void triggerOnFxPresetChanged(
      MediaTrack *track,
      int fxId,
      int presetIndex,
      string presetName
    );

    ReaProject *getProject(int index = -1);
    MediaTrack *getTrack(int index = 0);

    void setFxParameterValue(MediaTrack *track, int fxId, int paramId, double value);

    std::string getFormattedParamValue(MediaTrack* track, int fxId, int paramId);

    std::tuple<double,double,double,double> getParamValueEx(
      MediaTrack *track,
      int paramId,
      int fxId
    );

    double getParamValue(MediaTrack *track, int paramId, int fxId);

    void setTrackVolume(MediaTrack *track, double volume);

    void setTrackMute(MediaTrack *track, bool mute);

    void setTrackSolo(MediaTrack *track, bool solo);

    void setTrackRecordArm(MediaTrack *track, bool recordArm);

    bool isSubscribedToControlSurfaceEvent(
      int eventId,
      MediaTrack *track,
      ReaperEventSubscriber *subscriber
    );

    void subscribeToControlSurfaceEvent(
      int eventId,
      MediaTrack *track,
      ReaperEventSubscriber *subscriber
    );

    void unsubscribeFromControlSurfaceEvent(
      int eventId,
      MediaTrack *track,
      ReaperEventSubscriber *subscriber
    );

    vector<ReaperEventSubscriber *> getControlSurfaceEventSubscribers(MediaTrack *track, int eventId);

    string getFxName(MediaTrack *track, int fxId);

    double getTrackVolume(MediaTrack *track);

    void loadFxPreset(MediaTrack *track, int fxId, string presetName);

    MediaTrack *getTrackByGenericName(string genericName);

    int getFxIdByGenericName(string genericName);

    int getFxPresetIndex(MediaTrack *track, int fxId);

    std::string getCurrentFxPresetName(MediaTrack *track, int fxId);

    bool isSubscribedToFxPresetChanged(MediaTrack *track, int fxId, ReaperEventSubscriber *subscriber);

    void subscribeToFxPresetChanged(MediaTrack *track, int fxId, ReaperEventSubscriber *subscriber);

    void unsubscribeFromFxPresetChanged(MediaTrack *track, int fxId, ReaperEventSubscriber *subscriber);

    void loadFxPreset(MediaTrack *track, int fxId, int index);

    void loadNextFxPreset(MediaTrack *track, int fxId);

    void loadPreviousFxPreset(MediaTrack *track, int fxId);

    void triggerOnTrackListChanged();

    void play();
    void pause();
    void stop();

    void record();

    void rewind();

    void fastForward();

    void toggleRepeat();

    bool isPlaying();

    bool isPaused();

    bool isRecording();

    bool isStopped();

    bool isRepeating();
  };
}


#endif
