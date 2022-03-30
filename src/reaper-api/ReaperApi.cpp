#include "ReaperApi.h"
#include <vector>
#include "../reaper/reaper_plugin.h"
#include "../reaper/reaper_plugin_functions.h"
#include <iostream>

namespace CCS {


  ReaperApi::ReaperApi() {

  }

  int ReaperApi::getNumTracks() {
    return CSurf_NumTracks(false);
  }

  string ReaperApi::getFxParameterName(
    MediaTrack *track,
    int fxId,
    int parameterId
  ) {
    char buffer[32];
    buffer[0] = 0;
    TrackFX_GetParamName(track, fxId, parameterId, buffer, sizeof(buffer));
    return string(buffer);
  };

  void ReaperApi::subscribeToFxParameterChanged(
    MediaTrack *track,
    int fxId,
    int paramId,
    ReaperEventSubscriber *subscriber
  ) {
    if (isSubscribedToFxParameterChanged(track, fxId, paramId, subscriber)) {
      return;
    }
    auto subscription = new FxParameterChangedSubscription(
      track,
      fxId,
      paramId,
      subscriber,
      this
    );
    fxParamChangedSubscriptions.push_back(subscription);
  }

  bool ReaperApi::isSubscribedToFxParameterChanged(
    MediaTrack *track,
    int fxId,
    int paramId,
    ReaperEventSubscriber *subscriber
  ) {
    for (auto subscription : fxParamChangedSubscriptions) {
      if (
        subscription->getTrack() == track &&
        subscription->getFxId() == fxId &&
        subscription->getParamId() == paramId &&
        subscription->getSubscriber() == subscriber) {
        return true;
      }
    }
    return false;
  }

  bool ReaperApi::isSubscribedToEvent(ReaperEventSubscriber *subscriber, int eventId) {
    vector<ReaperEventSubscriber*> subscribers = subscribersMap.at(eventId);
    for (auto current : subscribers) {
      if (current == subscriber) {
        return true;
      }
    }
    return false;
  }

  void ReaperApi::triggerOnPlay() {
    if (!subscribersMap.contains(ON_PLAY)) {
      return;
    }
    std::vector subscribers = subscribersMap.at(ON_PLAY);
    for (auto subscriber: subscribers) {
      subscriber->onPlay();
    }
  };

  void ReaperApi::triggerOnPause() {
    if (!subscribersMap.contains(ON_PAUSE)) {
      return;
    }
    std::vector subscribers = subscribersMap.at(ON_PAUSE);
    for (auto subscriber: subscribers) {
      subscriber->onPlay();
    }
  };

  void ReaperApi::triggerOnRecord() {
    if (!subscribersMap.contains(ON_RECORD)) {
      return;
    }
    std::vector subscribers = subscribersMap.at(ON_RECORD);
    for (auto subscriber: subscribers) {
      subscriber->onRecord();
    }
  };

  void ReaperApi::triggerOnStop() {
    if (!subscribersMap.contains(ON_STOP)) {
      return;
    }
    std::vector subscribers = subscribersMap.at(ON_STOP);
    for (auto subscriber: subscribers) {
      subscriber->onStop();
    }
  };

  void ReaperApi::triggerOnRepeatChanged(bool repeat) {
    if (!subscribersMap.contains(ON_REPEAT_CHANGED)) {
      return;
    }
    std::vector subscribers = subscribersMap.at(ON_REPEAT_CHANGED);
    for (auto subscriber: subscribers) {
      subscriber->onRepeatChanged(repeat);
    }
  };

  void ReaperApi::triggerOnTrackVolumeChanged(MediaTrack *track, double volume) {
    if (!subscribersMap.contains(ON_TRACK_VOLUME_CHANGED)) {
      return;
    }
    std::vector subscribers = subscribersMap.at(ON_TRACK_VOLUME_CHANGED);
    for (auto subscriber: subscribers) {
      subscriber->onTrackVolumeChanged(volume);
    }
  }

  void ReaperApi::triggerOnTrackMuteChanged(MediaTrack *track, bool mute) {
    if (!subscribersMap.contains(ON_TRACK_MUTE_CHANGED)) {
      return;
    }
    std::vector subscribers = subscribersMap.at(ON_TRACK_MUTE_CHANGED);
    for (auto subscriber: subscribers) {
      subscriber->onTrackMuteChanged(mute);
    }
  }

  void ReaperApi::triggerOnTrackSoloChanged(MediaTrack *track, bool solo) {
    if (!subscribersMap.contains(ON_TRACK_SOLO_CHANGED)) {
      return;
    }
    std::vector subscribers = subscribersMap.at(ON_TRACK_SOLO_CHANGED);
    for (auto subscriber: subscribers) {
      subscriber->onTrackSoloChanged(solo);
    }
  }

  void ReaperApi::triggerOnTrackRecordArmChanged(MediaTrack *track, bool recordArm) {
    if (!subscribersMap.contains(ON_TRACK_RECORD_ARM_CHANGED)) {
      return;
    }
    std::vector subscribers = subscribersMap.at(ON_TRACK_RECORD_ARM_CHANGED);
    for (auto subscriber: subscribers) {
      subscriber->onTrackRecordArmChanged(recordArm);
    }
  }

  void ReaperApi::triggerOnFxParameterChanged(
    MediaTrack *track,
    int fxId,
    int paramId,
    double value,
    double minValue,
    double maxValue,
    string formattedValue
  ) {
    std::vector subscriptions = fxParamChangedSubscriptions;
    for (auto subscription: subscriptions) {
      if (
        subscription->getTrack() != track ||
        subscription->getFxId() != fxId ||
        subscription->getParamId() != paramId
      ) {
        continue;
      }
      subscription->getSubscriber()->onFxParameterChanged(
        track,
        fxId,
        paramId,
        value,
        minValue,
        maxValue,
        formattedValue
      );
    }
  };

  void ReaperApi::pollReaperData() {
    // Retrieve the data we're subscribed to and trigger the corresponding events.
    for (auto tracker : fxParamChangedSubscriptions) {
      tracker->update();
    }
  }

  MediaTrack *ReaperApi::getTrack(int id) {
    // Note that track 0 is the master track. Since we're using
    // 1-based indices in the configs we can simply convert this to int.
    return CSurf_TrackFromID(id, false);
  }

  std::tuple<double,double> ReaperApi::getFxParameterMinMax(
    MediaTrack *track,
    int fxId,
    int paramId
  ) {
    double minValue;
    double maxValue;
    TrackFX_GetParam(track, fxId, paramId, &minValue, &maxValue);
    return std::tuple<double,double>(minValue, maxValue);
  }

  ReaProject *ReaperApi::getProject(int index) {
    return EnumProjects(index, nullptr, 0);
  }

  void ReaperApi::setFxParameterValue(
    MediaTrack *track,
    int fxId,
    int paramId,
    double value
  ) {
    TrackFX_SetParam(track, fxId, paramId, value);
  }

  std::string ReaperApi::getFormattedParamValue(MediaTrack* track, int fxId, int paramId) {
    char formattedValue[64];
    TrackFX_GetFormattedParamValue(
      track,
      fxId,
      paramId,
      formattedValue,
      sizeof formattedValue
    );
    return std::string(formattedValue);
  }

  std::tuple<double,double,double,double> ReaperApi::getParamValueEx(
    MediaTrack * track,
    int paramId,
    int fxId
  ) {

    double minValue;
    double maxValue;
    double midValue;
    double rawValue = TrackFX_GetParamEx(
      track,
      fxId,
      paramId,
      &minValue,
      &maxValue,
      &midValue
    );

    return { rawValue, minValue, maxValue, midValue };
  }
}
