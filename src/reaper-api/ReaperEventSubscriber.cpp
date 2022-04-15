#include "ReaperEventSubscriber.h"
#include <string>

namespace CCS {

  void ReaperEventSubscriber::onPlay(bool play) {}
  void ReaperEventSubscriber::onPause(bool pause) {}
  void ReaperEventSubscriber::onStop(bool stop) {}
  void ReaperEventSubscriber::onRecord(bool record) {}
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
    int presetIndex,
    std::string presetName
  ) {}

  void ReaperEventSubscriber::onTrackListChanged(int numTracks) {}
}
