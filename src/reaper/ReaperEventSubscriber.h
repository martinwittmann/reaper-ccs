#ifndef CCS_REAPER_EVENT_SUBSCRIBER
#define CCS_REAPER_EVENT_SUBSCRIBER

namespace CCS {
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
    virtual void onFxParameterChanged();
  };
}

#endif
