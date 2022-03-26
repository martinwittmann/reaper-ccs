#include "FxParameterChangedSubscription.h"
#include <iostream>
#include "../sdk/reaper_plugin_functions.h"
#include "ReaperDataTracker.h"
#include "ReaperApi.h"

namespace CCS {

  FxParameterChangedSubscription::FxParameterChangedSubscription(
    MediaTrack* track,
    int fxId,
    int paramId,
    ReaperEventSubscriber* subscriber,
    ReaperApi* apiManager
  ) : ReaperDataTracker(apiManager) {
    this->track = track;
    this->fxId = fxId;
    this->paramId = paramId;
    this->subscriber = subscriber;
  }

  bool FxParameterChangedSubscription::isChanged() {
    return false;
  }

  void FxParameterChangedSubscription::triggerEvent() {
    apiManager->triggerOnFxParameterChanged(track, fxId, paramId, currentValue);
  }

  void FxParameterChangedSubscription::getData() {
    std::cout << "Retrieving fx param data\n";
    char buffer[512];
    buffer[0] = 0;
    double val = TrackFX_GetParamNormalized(track, fxId, paramId);
    std::cout << val << "\n";
    TrackFX_GetFormattedParamValue(track, fxId, paramId, buffer, sizeof(buffer));
    std::cout << buffer << "\n";
  }
}