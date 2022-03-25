#include "ReaperDataTracker.h"
#include "ReaperApi.h"
#include "../sdk/reaper_plugin.h"
#include "../sdk/reaper_plugin_functions.h"
#include <iostream>

namespace CCS {
  // eventType corresponds to one of the event types in ReaperApi.h
  ReaperDataTracker::ReaperDataTracker(ReaperApi* apiManager) {
    this->apiManager = apiManager;
  }

  void ReaperDataTracker::update() {
    // Retrieve the data.
    getData();
    if (isChanged()) {
      triggerEvent();
    }
  }

  void ReaperDataTracker::getData() {}

  bool ReaperDataTracker::isChanged() {
    return false;
  }
  void ReaperDataTracker::triggerEvent() {}

  ReaperTrackFxParam::ReaperTrackFxParam(
    MediaTrack* track,
    int fxId,
    int paramId,
    ReaperApi* apiManager
    ) : ReaperDataTracker(apiManager) {
    this->track = track;
    this->fxId = fxId;
    this->paramId = paramId;
  }

  bool ReaperTrackFxParam::isChanged() {
    return false;
  }

  void ReaperTrackFxParam::triggerEvent(
    MediaTrack* track,
    int fxId,
    int paramId,
    double value
  ) {
    apiManager->triggerOnFxParameterChanged(track, fxId, paramId, value);
  }

  void ReaperTrackFxParam::getData() {
    char buffer[512];
    buffer[0] = 0;
    double val = TrackFX_GetParamNormalized(track, fxId, paramId);
    std::cout << val << "\n";
    TrackFX_GetFormattedParamValue(track, fxId, paramId, buffer, sizeof(buffer));
    std::cout << buffer << "\n";
  }
}