#include "ReaperEventSubscriber.h"

namespace CCS {

  void ReaperEventSubscriber::onPlay() {}
  void ReaperEventSubscriber::onStop() {}
  void ReaperEventSubscriber::onRecord() {}
  void ReaperEventSubscriber::onRepeatChanged(bool repeat) {}
  void ReaperEventSubscriber::onTrackVolumeChanged(double volume) {}
  void ReaperEventSubscriber::onTrackMuteChanged(bool mute) {}
  void ReaperEventSubscriber::onTrackSoloChanged(bool solo) {}
  void ReaperEventSubscriber::onTrackRecordArmChanged(bool recordArm) {}
  void ReaperEventSubscriber::onFxParameterChanged() {}
}
