#ifndef CCS_REAPER_EVENT_SUBSCRIBER
#define CCS_REAPER_EVENT_SUBSCRIBER

#include "../sdk/reaper_plugin.h"

namespace CCS {

  /**
   * Classes can derive from ReaperEventSubscriber if they want to provide
   * callbacks for certain events in reaper.
   * In order for these callbacks to actually be called they need to subscribe
   * to specific events by calling reaperApi->subscribeTo[event type]() methods.
   */
  class ReaperEventSubscriber {

  public:
    virtual void onPlay();
    virtual void onStop();
    virtual void onRecord();
    virtual void onTrackVolumeChanged(double volume);
    virtual void onRepeatChanged(bool repeat);
    virtual void onTrackMuteChanged(bool mute);
    virtual void onTrackSoloChanged(bool solo);
    virtual void onTrackRecordArmChanged(bool recordArm);
    virtual void onFxParameterChanged(
      MediaTrack* track,
      int fxId,
      int paramId,
      double value
    );
  };
}

#endif
