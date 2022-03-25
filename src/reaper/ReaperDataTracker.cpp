#include "ReaperDataTracker.h"
#include "ReaperApi.h"
#include "../sdk/reaper_plugin.h"
#include "../sdk/reaper_plugin_functions.h"

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

  void ReaperTrackFxParam::triggerEvent() {
    apiManager->triggerOnFxParamChanged();
  }

  void ReaperTrackFxParam::getData() {
    char buffer[512];
    buffer[0] = 0;
    int val = TrackFX_GetParamNormalized(track, fxId, paramId);
    TrackFX_GetFormattedParamValue(track, fxId, paramId, buffer, sizeof(buffer));
  }
}