#include "FxParameterChangedSubscription.h"
namespace CCS {

  FxParameterChangedSubscription::FxParameterChangedSubscription(
    MediaTrack* track,
    int fxId,
    int paramId,
    ReaperEventSubscriber* subscriber
  ) {
    this->track = track;
    this->fxId = fxId;
    this->paramId = paramId;
    this->subscriber = subscriber;
  }
}