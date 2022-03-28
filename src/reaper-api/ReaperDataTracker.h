#ifndef CCS_REAPER_DATA_TRACKER_H
#define CCS_REAPER_DATA_TRACKER_H

#include "../reaper/reaper_plugin.h"

namespace CCS {
  // Since reaper does not provide callback for things like fx parameter
  // changes, a common method according to the reaper forums is to do
  // polling from the extension's Run method.
  // ReaperDataTracker objects represent a single item of data that we want to
  // trigger callbacks on, if it changes.

  class ReaperApi;

  class ReaperDataTracker {
  protected:
    ReaperApi *apiManager;

  public:
    ReaperDataTracker(ReaperApi *apiManager);
    virtual void update(bool triggerOnChange = true);
    virtual void triggerEvent();
  };
}

#endif
