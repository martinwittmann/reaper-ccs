#include "ReaperApi.h"
#include <vector>
#include "../reaper/reaper_plugin.h"
#include "../reaper/reaper_plugin_functions.h"
#include <iostream>
#include <string>
#include "../ccs/Util.h"
#include "../WDL/db2val.h"
#include "ControlSurfaceEventSubscription.h"
#include "FxParameterChangedSubscription.h"
#include "FxPresetChangedSubscription.h"
#include "../ccs/CcsException.h"

namespace CCS {

  using std::string;

  ReaperApi::ReaperApi() {}

  /**
   * This does not include the master track which has track index 0.
   * @return
   */
  int ReaperApi::getNumTracks() {
    return CSurf_NumTracks(false);
  }

  std::vector<string> ReaperApi::getTrackNames() {
    std::vector<string> result;
    int numTracks = getNumTracks() + 1;
    // getNumTracks does not contain the master track which is index 0.
    // This is why we start at index 1 to exclude the master track, but add 1
    // to the number of tracks to not lose the last track.
    for (int i = 1; i < numTracks; ++i) {
      result.push_back(getTrackName(i));
    }
    return result;
  }

  string ReaperApi::getTrackName(int trackIndex) {
    MediaTrack *track = getTrack(trackIndex);
    return getTrackName(track);
  }

  string ReaperApi::getTrackName(MediaTrack *track) {
    char buffer[512];
    buffer[0] = 0;
    GetTrackName(track, buffer, sizeof buffer);
    return string(buffer);
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

  string ReaperApi::getFxName(MediaTrack *track, int fxId) {
    char buffer[512];
    buffer[0] = 0;
    memset(buffer, 0, sizeof buffer);
    TrackFX_GetFXName(track, fxId, buffer, sizeof buffer);
    return std::string(buffer);
  }

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
    m_fxParamChangedSubscriptions.push_back(subscription);
  }

  void ReaperApi::unsubscribeFromFxParameterChanged(
    MediaTrack *track,
    int fxId,
    int paramId,
    ReaperEventSubscriber *subscriber
  ) {
    if (!isSubscribedToFxParameterChanged(track, fxId, paramId, subscriber)) {
      return;
    }

    for (
      auto it = m_fxParamChangedSubscriptions.begin();
      it != m_fxParamChangedSubscriptions.end();
      ++it
    ) {
      FxParameterChangedSubscription* subscription = *it;
      if (
        subscription->getTrack() == track &&
        subscription->getFxId() == fxId &&
        subscription->getParamId() == paramId &&
        subscription->getSubscriber() == subscriber) {
        m_fxParamChangedSubscriptions.erase(it);
        return;
      }
    }
  }

  void ReaperApi::subscribeToFxPresetChanged(
    MediaTrack *track,
    int fxId,
    ReaperEventSubscriber *subscriber
  ) {
    if (isSubscribedToFxPresetChanged(track, fxId, subscriber)) {
      return;
    }
    auto subscription = new FxPresetChangedSubscription(
      track,
      fxId,
      subscriber,
      this
    );
    m_fxPresetChangedSubscriptions.push_back(subscription);
  }

  void ReaperApi::unsubscribeFromFxPresetChanged(
    MediaTrack *track,
    int fxId,
    ReaperEventSubscriber *subscriber
  ) {
    if (!isSubscribedToFxPresetChanged(track, fxId, subscriber)) {
      return;
    }

    for (
      auto it = m_fxPresetChangedSubscriptions.begin();
      it != m_fxPresetChangedSubscriptions.end();
      ++it
      ) {
      FxPresetChangedSubscription* subscription = *it;
      if (
        subscription->getTrack() == track &&
        subscription->getFxId() == fxId &&
        subscription->getSubscriber() == subscriber) {
        m_fxPresetChangedSubscriptions.erase(it);
        return;
      }
    }
  }

  bool ReaperApi::isSubscribedToFxPresetChanged(
    MediaTrack *track,
    int fxId,
    ReaperEventSubscriber *subscriber
  ) {
    for (auto subscription : m_fxPresetChangedSubscriptions) {
      if (
        subscription->getTrack() == track &&
        subscription->getFxId() == fxId &&
        subscription->getSubscriber() == subscriber) {
        return true;
      }
    }
    return false;
  }

  bool ReaperApi::isSubscribedToFxParameterChanged(
    MediaTrack *track,
    int fxId,
    int paramId,
    ReaperEventSubscriber *subscriber
  ) {
    for (auto subscription : m_fxParamChangedSubscriptions) {
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

  void ReaperApi::triggerOnPlay() {
    vector<ReaperEventSubscriber*> subscribers = getControlSurfaceEventSubscribers(
      nullptr,
      ON_PLAY
    );
    for (auto subscriber: subscribers) {
      subscriber->onPlay();
    }
  };

  void ReaperApi::triggerOnPause() {
    vector<ReaperEventSubscriber*> subscribers = getControlSurfaceEventSubscribers(
      nullptr,
      ON_PAUSE
    );
    for (auto subscriber: subscribers) {
      subscriber->onPause();
    }
  };

  void ReaperApi::triggerOnRecord() {
    vector<ReaperEventSubscriber*> subscribers = getControlSurfaceEventSubscribers(
      nullptr,
      ON_RECORD
    );
    for (auto subscriber: subscribers) {
      subscriber->onRecord();
    }
  };

  void ReaperApi::triggerOnStop() {
    vector<ReaperEventSubscriber*> subscribers = getControlSurfaceEventSubscribers(
      nullptr,
      ON_STOP
    );
    for (auto subscriber: subscribers) {
      subscriber->onStop();
    }
  };

  void ReaperApi::triggerOnRepeatChanged(bool repeat) {
    vector<ReaperEventSubscriber*> subscribers = getControlSurfaceEventSubscribers(
      nullptr,
      ON_REPEAT_CHANGED
    );
    for (auto subscriber: subscribers) {
      subscriber->onRepeatChanged(repeat);
    }
  };

  void ReaperApi::triggerOnTrackVolumeChanged(MediaTrack *track, double volume) {
    vector<ReaperEventSubscriber*> subscribers = getControlSurfaceEventSubscribers(
      track,
      ON_TRACK_VOLUME_CHANGED
    );
    for (auto subscriber: subscribers) {
      subscriber->onTrackVolumeChanged(volume);
    }
  }

  void ReaperApi::triggerOnTrackMuteChanged(MediaTrack *track, bool mute) {
    vector<ReaperEventSubscriber*> subscribers = getControlSurfaceEventSubscribers(
      track,
      ON_TRACK_MUTE_CHANGED
    );
    for (auto subscriber: subscribers) {
      subscriber->onTrackMuteChanged(mute);
    }
  }

  void ReaperApi::triggerOnTrackSoloChanged(MediaTrack *track, bool solo) {
    vector<ReaperEventSubscriber*> subscribers = getControlSurfaceEventSubscribers(
      track,
      ON_TRACK_SOLO_CHANGED
    );
    for (auto subscriber: subscribers) {
      subscriber->onTrackSoloChanged(solo);
    }
  }

  void ReaperApi::triggerOnTrackRecordArmChanged(MediaTrack *track, bool recordArm) {
    vector<ReaperEventSubscriber*> subscribers = getControlSurfaceEventSubscribers(
      track,
      ON_TRACK_RECORD_ARM_CHANGED
    );
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
    std::vector subscriptions = m_fxParamChangedSubscriptions;
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

  void ReaperApi::triggerOnFxPresetChanged(
    MediaTrack *track,
    int fxId,
    int presetIndex,
    string presetName
  ) {
    std::vector subscriptions = m_fxPresetChangedSubscriptions;
    for (auto subscription: subscriptions) {
      if (subscription->getTrack() != track || subscription->getFxId() != fxId) {
        continue;
      }
      subscription->getSubscriber()->onFxPresetChanged(
        track,
        fxId,
        presetIndex,
        presetName
      );
    }
  };

  void ReaperApi::pollReaperData() {
    // Retrieve the data we're subscribed to and trigger the corresponding events.
    for (auto tracker : m_fxParamChangedSubscriptions) {
      tracker->update(true);
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

  double ReaperApi::getParamValue(MediaTrack *track, int fxId, int paramId) {
    return TrackFX_GetParam(track, fxId, paramId, nullptr, nullptr);
  }

  std::tuple<double,double,double,double> ReaperApi::getParamValueEx(
    MediaTrack * track,
    int fxId,
    int paramId
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

  void ReaperApi::setTrackVolume(MediaTrack *track, double value) {
    SetMediaTrackInfo_Value(track, "D_VOL", Util::volumeToSlider(value));
  }

  void ReaperApi::setTrackMute(MediaTrack *track, bool mute) {
    SetMediaTrackInfo_Value(track, "B_MUTE", mute);
  }

  void ReaperApi::setTrackSolo(MediaTrack *track, bool solo) {
    SetMediaTrackInfo_Value(track, "I_SOLO", solo ? 1 : 0);
  }

  void ReaperApi::setTrackRecordArm(MediaTrack *track, bool recordArm) {
    SetMediaTrackInfo_Value(track, "I_RECARM", recordArm ? 1 : 0);
  }

  void ReaperApi::subscribeToControlSurfaceEvent(
    int eventId,
    MediaTrack *track,
    ReaperEventSubscriber *subscriber
  ) {
    if (isSubscribedToControlSurfaceEvent(eventId, track, subscriber)) {
      return;
    }
    auto subscription = new ControlSurfaceEventSubscription(
      eventId,
      track,
      subscriber,
      this
    );
    m_controlSurfaceEventSubscriptions.push_back(subscription);
  }

  void ReaperApi::unsubscribeFromControlSurfaceEvent(
    int eventId,
    MediaTrack *track,
    ReaperEventSubscriber *subscriber
  ) {
    if (!isSubscribedToControlSurfaceEvent(eventId, track, subscriber)) {
      return;
    }

    for (
      auto it = m_controlSurfaceEventSubscriptions.begin();
      it != m_controlSurfaceEventSubscriptions.end();
      ++it
      ) {
      ControlSurfaceEventSubscription* subscription = *it;
      if (
        subscription->getEventId() == eventId &&
        subscription->getTrack() == track &&
        subscription->getSubscriber() == subscriber) {
        m_controlSurfaceEventSubscriptions.erase(it);
        return;
      }
    }
  }

  bool ReaperApi::isSubscribedToControlSurfaceEvent(
    int eventId,
    MediaTrack *track,
    ReaperEventSubscriber *subscriber
  ) {
    for (auto subscription : m_controlSurfaceEventSubscriptions) {
      if (
        subscription->getTrack() == track &&
        subscription->getEventId() == eventId &&
        subscription->getSubscriber() == subscriber) {
        return true;
      }
    }
    return false;
  }

  vector<ReaperEventSubscriber*> ReaperApi::getControlSurfaceEventSubscribers(
    MediaTrack *track,
    int eventId
  ) {
    vector<ReaperEventSubscriber*> result;
    for (auto subscription : m_controlSurfaceEventSubscriptions) {
      if (
        subscription->getTrack() == track &&
        subscription->getEventId() == eventId) {
        result.push_back(subscription->getSubscriber());
      }
    }
    return result;
  }

  double ReaperApi::getTrackVolume(MediaTrack *track) {
    double volume = GetMediaTrackInfo_Value(track, "D_VOL");
    return Util::sliderToVolume(volume);
  }

  void ReaperApi::loadFxPreset(MediaTrack *track, int fxId, std::string presetName) {
    TrackFX_SetPreset(track, fxId, presetName.c_str());
  }

  void ReaperApi::loadFxPreset(MediaTrack *track, int fxId, int index) {
    TrackFX_SetPresetByIndex(track, fxId, index);
  }

  MediaTrack *ReaperApi::getTrackByGenericName(std::string genericName) {
    if (!Util::regexMatch(genericName, "^track")) {
      throw CcsException("Trying to get track by invalid generic name: " + genericName);
    }
    string rawIndex = Util::regexReplace(genericName, "[^0-9]+", "");
    // Track indexes in configs are 1-based, but since reaper uses track 0
    // for the master track, we can use the index as is.
    int trackIndex = std::stoi(rawIndex);
    return getTrack(trackIndex);
  }

  int ReaperApi::getFxIdByGenericName(std::string genericName) {
    std::string rawIndex = Util::regexReplace(genericName, "[^0-9]+", "");
    // We're using 1-based indices. There is no special fx index 0 in reaper.
    // Index 0 is just the first fx plugin.
    return stoi(rawIndex) - 1;
  }

  int ReaperApi::getFxPresetIndex(MediaTrack *track, int fxId) {
    int presetIndex;
    int result = TrackFX_GetPresetIndex(track, fxId, &presetIndex);
    if (result < 0) {
      throw CcsException("Error getting fx preset index for fxId: " + fxId);
    }
    string name = getCurrentFxPresetName(track, fxId);
    return presetIndex;
  }

  string ReaperApi::getCurrentFxPresetName(MediaTrack *track, int fxId) {
    char buffer[512];
    buffer[0] = 0;
    TrackFX_GetPreset(track, fxId, buffer, sizeof buffer);
    return string(buffer);
  }

  void ReaperApi::loadNextFxPreset(MediaTrack *track, int fxId) {
    TrackFX_NavigatePresets(track, fxId, -1);
  }

  void ReaperApi::loadPreviousFxPreset(MediaTrack *track, int fxId) {
    TrackFX_NavigatePresets(track, fxId, 1);
  }

  void ReaperApi::triggerOnTrackListChanged() {
    vector<ReaperEventSubscriber*> subscribers = getControlSurfaceEventSubscribers(
      nullptr,
      ON_TRACK_LIST_CHANGED
    );
    int numTracks = getNumTracks();
    for (auto subscriber: subscribers) {
      subscriber->onTrackListChanged(numTracks);
    }
  }
}
