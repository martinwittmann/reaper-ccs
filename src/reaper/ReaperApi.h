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

  class ReaperApi {
    std::map<int, vector<ReaperEventSubscriber*>> subscribersMap;
    std::vector<ReaperDataTracker*> trackers;

  public:

    // Event ids:
    static const unsigned short ON_PLAY = 0;
    static const unsigned short ON_PAUSE = 1;
    static const unsigned short ON_RECORD = 2;
    static const unsigned short ON_STOP = 3;
    static const unsigned short ON_REPEAT_CHANGED = 4;

    static const unsigned short ON_TRACK_VOLUME_CHANGED = 1000;
    static const unsigned short ON_TRACK_MUTE_CHANGED = 1001;
    static const unsigned short ON_TRACK_SOLO_CHANGED = 1002;
    static const unsigned short ON_TRACK_RECORD_ARM_CHANGED = 1003;

    static const unsigned short ON_FX_PARAM_CHANGED = 2000;

    ReaperApi();
    void pollReaperData();


    void triggerOnPlay();
    void triggerOnPause();
    void triggerOnRecord();
    void triggerOnStop();
    void triggerOnRepeatChanged(bool repeat);

    void triggerOnTrackVolumeChanged(MediaTrack* track, double volume);
    void triggerOnTrackMuteChanged(MediaTrack* track, bool repeat);
    void triggerOnTrackSoloChanged(MediaTrack* track, bool repeat);
    void triggerOnTrackRecordArmChanged(MediaTrack* track, bool repeat);

    void triggerOnFxParamChanged();
    void subscribeToEvent(int eventId, ReaperEventSubscriber* subscriber);
    bool isSubscribedToEvent(ReaperEventSubscriber* subscriber, int eventId);
  };
}


#endif
