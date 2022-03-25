#include "ReaperApi.h"
#include <vector>
#include "../sdk/reaper_plugin_functions.h"

namespace CCS {

  ReaperApi::ReaperApi() {

  }

  int ReaperApi::getNumTracks() {
    return CSurf_NumTracks(false);
  }

  string ReaperApi::getFxParameterName(
    MediaTrack* track,
    int fxId,
    int parameterId
  ) {
    char buffer[32];
    buffer[0] = 0;
    TrackFX_GetParamName(track, fxId, parameterId, buffer, sizeof(buffer));
    return string(buffer);
  };

  void ReaperApi::subscribeToFxParameterChanged(
    MediaTrack* track,
    int fxId,
    int paramId,
    ReaperEventSubscriber* subscriber
  ) {
    if (isSubscribedToFxParameterChanged(track, fxId, paramId, subscriber)) {
      return;
    }
    auto subscription = new FxParameterChangedSubscription(
      track,
      fxId,
      paramId,
      subscriber
    );
    fxParamChangedSubscriptions.push_back(subscription);
  }

  bool ReaperApi::isSubscribedToFxParameterChanged(
    MediaTrack* track,
    int fxId,
    int paramId,
    ReaperEventSubscriber* subscriber
  ) {
    for (auto subscription : fxParamChangedSubscriptions) {
      if (
        subscription->track == track &&
        subscription->fxId == fxId &&
        subscription->paramId == paramId &&
        subscription->subscriber == subscriber) {
        return true;
      }
    }
    return false;
  }

  bool ReaperApi::isSubscribedToEvent(ReaperEventSubscriber* subscriber, int eventId) {
    vector<ReaperEventSubscriber*> subscribers = subscribersMap.at(eventId);
    for (auto current : subscribers) {
      if (current == subscriber) {
        return true;
      }
    }
    return false;
  }

  void ReaperApi::triggerOnPlay() {
    std::vector subscribers = subscribersMap.at(ON_PLAY);
    for (auto subscriber: subscribers) {
      subscriber->onPlay();
    }
  };

  void ReaperApi::triggerOnPause() {
    std::vector subscribers = subscribersMap.at(ON_PAUSE);
    for (auto subscriber: subscribers) {
      subscriber->onPlay();
    }
  };

  void ReaperApi::triggerOnRecord() {
    std::vector subscribers = subscribersMap.at(ON_RECORD);
    for (auto subscriber: subscribers) {
      subscriber->onRecord();
    }
  };

  void ReaperApi::triggerOnStop() {
    std::vector subscribers = subscribersMap.at(ON_STOP);
    for (auto subscriber: subscribers) {
      subscriber->onStop();
    }
  };

  void ReaperApi::triggerOnRepeatChanged(bool repeat) {
    std::vector subscribers = subscribersMap.at(ON_REPEAT_CHANGED);
    for (auto subscriber: subscribers) {
      subscriber->onRepeatChanged(repeat);
    }
  };

  void ReaperApi::triggerOnTrackVolumeChanged(MediaTrack* track, double volume) {
    std::vector subscribers = subscribersMap.at(ON_TRACK_VOLUME_CHANGED);
    for (auto subscriber: subscribers) {
      subscriber->onTrackVolumeChanged(volume);
    }
  }

  void ReaperApi::triggerOnTrackMuteChanged(MediaTrack* track, bool mute) {
    std::vector subscribers = subscribersMap.at(ON_TRACK_MUTE_CHANGED);
    for (auto subscriber: subscribers) {
      subscriber->onTrackMuteChanged(mute);
    }
  }

  void ReaperApi::triggerOnTrackSoloChanged(MediaTrack* track, bool solo) {
    std::vector subscribers = subscribersMap.at(ON_TRACK_SOLO_CHANGED);
    for (auto subscriber: subscribers) {
      subscriber->onTrackSoloChanged(solo);
    }
  }

  void ReaperApi::triggerOnTrackRecordArmChanged(MediaTrack* track, bool recordArm) {
    std::vector subscribers = subscribersMap.at(ON_TRACK_RECORD_ARM_CHANGED);
    for (auto subscriber: subscribers) {
      subscriber->onTrackRecordArmChanged(recordArm);
    }
  }

  void ReaperApi::triggerOnFxParameterChanged(
    MediaTrack* track,
    int fxId,
    int paramId,
    double value
  ) {
    std::vector subscriptions = fxParamChangedSubscriptions;
    for (auto subscription: subscriptions) {
      subscription->subscriber->onFxParameterChanged(track, fxId, paramId, value);
    }
  };

  void ReaperApi::pollReaperData() {
    // Retrieve the data we're subscribed to and trigger the corresponding events.
    for (auto tracker : trackers) {
      tracker->update();
    }
  }

  MediaTrack* ReaperApi::getTrack(int id) {
    return CSurf_TrackFromID(id, false);
  }
}
