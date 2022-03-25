#include "ReaperApi.h"
#include <vector>

namespace CCS {

  ReaperApi::ReaperApi() {

  }

  void ReaperApi::subscribeToEvent(int eventId, ReaperEventSubscriber* subscriber) {
    if (isSubscribedToEvent(subscriber, eventId)) {
      return;
    }
    subscribersMap.at(eventId).push_back(subscriber);
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
      subscriber->onFxParameterChanged();
    }
  };

  void ReaperApi::triggerOnPause() {
    std::vector subscribers = subscribersMap.at(ON_PAUSE);
    for (auto subscriber: subscribers) {
      subscriber->onFxParameterChanged();
    }
  };

  void ReaperApi::triggerOnRecord() {
    std::vector subscribers = subscribersMap.at(ON_RECORD);
    for (auto subscriber: subscribers) {
      subscriber->onFxParameterChanged();
    }
  };

  void ReaperApi::triggerOnStop() {
    std::vector subscribers = subscribersMap.at(ON_STOP);
    for (auto subscriber: subscribers) {
      subscriber->onFxParameterChanged();
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

  void ReaperApi::triggerOnFxParamChanged() {
    std::vector subscribers = subscribersMap.at(ON_FX_PARAM_CHANGED);
    for (auto subscriber: subscribers) {
      subscriber->onFxParameterChanged();
    }
  };


  void ReaperApi::pollReaperData() {
    // Retrieve the data we're susbcribed to and trigger the according events.
    for (auto tracker : trackers) {
      tracker->update();
    }
  }
}
