#ifndef CCS_FX_PARAMETER_CHANGED_SUBSCRIPTION_H
#define CCS_FX_PARAMETER_CHANGED_SUBSCRIPTION_H

#include "ReaperEventSubscriber.h"
#include "ReaperDataTracker.h"
#include <string>

namespace CCS {

  class ReaperApi;

  class FxParameterChangedSubscription : public ReaperDataTracker {
    MediaTrack *m_track;
    int m_fxId;
    int m_paramId;
    ReaperEventSubscriber *m_subscriber;
    double m_currentValue;
    double m_minValue;
    double m_maxValue;
    double m_midValue;
    std::string m_formattedValue;
    std::string m_trackName;
    std::string m_fxName;
    std::string m_paramName;

  public:

    FxParameterChangedSubscription(
      MediaTrack *track,
      int fxId,
      int paramId,
      ReaperEventSubscriber *subscriber,
      ReaperApi *apiManager
    );

    void triggerEvent() override;
    void update(bool triggerOnChange) override;

    MediaTrack *getTrack();
    int getFxId();
    int getParamId();
    ReaperEventSubscriber *getSubscriber();
  };
}

#endif
