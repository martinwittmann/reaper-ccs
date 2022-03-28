#include "FxParameterChangedSubscription.h"
#include <iostream>
#include <string>
#include "../reaper/reaper_plugin.h"
#include "../reaper/reaper_plugin_functions.h"
#include "ReaperDataTracker.h"
#include "ReaperApi.h"

namespace CCS {

  FxParameterChangedSubscription::FxParameterChangedSubscription(
    MediaTrack *track,
    int fxId,
    int paramId,
    ReaperEventSubscriber *subscriber,
    ReaperApi *apiManager
  ) : ReaperDataTracker(apiManager) {

    this->track = track;
    this->fxId = fxId;
    this->paramId = paramId;
    this->subscriber = subscriber;


    char buffer[512];
    buffer[0] = 0;
    GetTrackName(track, buffer, sizeof buffer);
    this->trackName = std::string(buffer);

    memset(buffer, 0, sizeof buffer);
    TrackFX_GetFXName(track, fxId, buffer, sizeof buffer);
    this->fxName = std::string(buffer);

    memset(buffer, 0, sizeof buffer);
    TrackFX_GetParamName(track, fxId, paramId, buffer, sizeof buffer);
    this->paramName = std::string(buffer);

    // Set the current values when initializing but don't trigger the event.
    update(false);
  }

  void FxParameterChangedSubscription::update(bool triggerOnChange) {
    //std::cout << "Updating FX param value: " << trackName << " / " << fxName << " / " << paramName << "\n";
    newValue = TrackFX_GetParamEx(
      track,
      fxId,
      paramId,
      &minValue,
      &maxValue,
      &midValue
    );

    if (newValue != currentValue) {
      currentValue = newValue;
      if (triggerOnChange) {
        triggerEvent();
      }
    }
  }

  void FxParameterChangedSubscription::triggerEvent() {
    apiManager->triggerOnFxParameterChanged(track, fxId, paramId, currentValue);
  }

  MediaTrack *FxParameterChangedSubscription::getTrack() {
    return track;
  }

  int FxParameterChangedSubscription::getFxId() {
    return fxId;
  }

  int FxParameterChangedSubscription::getParamId() {
    return paramId;
  }

  ReaperEventSubscriber *FxParameterChangedSubscription::getSubscriber() {
    return subscriber;
  }
}