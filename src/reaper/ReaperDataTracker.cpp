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
}