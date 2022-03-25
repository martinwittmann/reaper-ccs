#ifndef CCS_FX_PARAMETER_CHANGED_SUBSCRIPTION_H
#define CCS_FX_PARAMETER_CHANGED_SUBSCRIPTION_H

#include "ReaperEventSubscriber.h"

namespace CCS {
  class FxParameterChangedSubscription {
  public:
    MediaTrack* track;
    int fxId;
    int paramId;
    ReaperEventSubscriber* subscriber;

    FxParameterChangedSubscription(
      MediaTrack* track,
      int fxId,
      int paramId,
      ReaperEventSubscriber* subscriber
    );
  };
}

#endif
