#ifndef CCS_FX_PARAMETER_CHANGED_SUBSCRIPTION_H
#define CCS_FX_PARAMETER_CHANGED_SUBSCRIPTION_H

#include "ReaperEventSubscriber.h"
#include "ReaperDataTracker.h"

namespace CCS {

  class ReaperApi;

  class FxParameterChangedSubscription : ReaperDataTracker {
  public:
    MediaTrack* track;
    int fxId;
    int paramId;
    ReaperEventSubscriber* subscriber;
    double currentValue;

    FxParameterChangedSubscription(
      MediaTrack* track,
      int fxId,
      int paramId,
      ReaperEventSubscriber* subscriber,
      ReaperApi* apiManager
    );

    virtual void triggerEvent();
    void getData() override;
    bool isChanged() override;
  };
}

#endif
