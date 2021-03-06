#ifndef CCS_REAPER_EVENT_SUBSCRIBER
#define CCS_REAPER_EVENT_SUBSCRIBER

#include "../reaper/reaper_plugin.h"
#include <string>

namespace CCS {

  /**
    *Classes can derive from ReaperEventSubscriber if they want to provide
    *callbacks for certain events in reaper.
    *In order for these callbacks to actually be called they need to subscribe
    *to specific events by calling reaperApi->subscribeTo[event type]() methods.
   */
  class ReaperEventSubscriber {

  public:
    virtual void onPlay(bool play);
    virtual void onPause(bool pause);
    virtual void onStop(bool stop);
    virtual void onRecord(bool record);
    virtual void onTrackVolumeChanged(double volume);
    virtual void onRepeatChanged(bool repeat);
    virtual void onTrackMuteChanged(bool mute);
    virtual void onTrackSoloChanged(bool solo);
    virtual void onTrackRecordArmChanged(bool recordArm);
    virtual void onFxParameterChanged(
      MediaTrack *track,
      int fxId,
      int paramId,
      double value,
      double minValue,
      double maxValue,
      std::string formattedValue
    );
    virtual void onFxPresetChanged(
      MediaTrack *track,
      int fxId,
      int presetIndex,
      std::string presetName
    );

    virtual void onTrackListChanged(int numTracks);
  };
}

#endif
