#ifndef CCS_FX_PARAMETER_CHANGED_SUBSCRIPTION_H
#define CCS_FX_PARAMETER_CHANGED_SUBSCRIPTION_H

#include "ReaperEventSubscriber.h"
#include "ReaperDataTracker.h"
#include <string>

namespace CCS {

  class ReaperApi;

  class FxParameterChangedSubscription : public ReaperDataTracker {
    MediaTrack* track;
    int fxId;
    int paramId;
    ReaperEventSubscriber* subscriber;
    double currentValue;
    double newValue;
    double minValue;
    double maxValue;
    double midValue;
    std::string trackName;
    std::string fxName;
    std::string paramName;

  public:

    FxParameterChangedSubscription(
      MediaTrack* track,
      int fxId,
      int paramId,
      ReaperEventSubscriber* subscriber,
      ReaperApi* apiManager
    );

    void triggerEvent() override;
    void update(bool triggerOnChange = false) override;

    MediaTrack* getTrack();
    int getFxId();
    int getParamId();
    ReaperEventSubscriber* getSubscriber();
  };
}

#endif
