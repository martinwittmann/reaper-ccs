#include "ReaperDataTracker.h"
#include "ReaperApi.h"
#include "../reaper/reaper_plugin.h"
#include "../reaper/reaper_plugin_functions.h"
#include <iostream>

namespace CCS {
  // eventType corresponds to one of the event types in ReaperApi.h
  ReaperDataTracker::ReaperDataTracker(ReaperApi *api) {
    m_api = api;
  }

  void ReaperDataTracker::update(bool triggerOnChange) {
    // This method is supposed to retrieve the current value, check if the value
    // has changed and call triggerEvent if it did.
  }

  void ReaperDataTracker::triggerEvent() {}
}