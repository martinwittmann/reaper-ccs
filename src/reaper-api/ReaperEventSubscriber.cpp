#include "ReaperEventSubscriber.h"
#include <string>

namespace CCS {

  void ReaperEventSubscriber::onPlay() {}
  void ReaperEventSubscriber::onPause() {}
  void ReaperEventSubscriber::onStop() {}
  void ReaperEventSubscriber::onRecord() {}
  void ReaperEventSubscriber::onRepeatChanged(bool repeat) {}
  void ReaperEventSubscriber::onTrackVolumeChanged(double volume) {}
  void ReaperEventSubscriber::onTrackMuteChanged(bool mute) {}
  void ReaperEventSubscriber::onTrackSoloChanged(bool solo) {}
  void ReaperEventSubscriber::onTrackRecordArmChanged(bool recordArm) {}
  void ReaperEventSubscriber::onFxParameterChanged(
    MediaTrack *track,
    int fxId,
    int paramId,
    double value,
    double minValue,
    double maxValue,
    std::string formattedValue
  ) {}

  void ReaperEventSubscriber::onFxPresetChanged(
    MediaTrack *track,
    int fxId,
    int value,
    std::string formattedValue
  ) {}
}
