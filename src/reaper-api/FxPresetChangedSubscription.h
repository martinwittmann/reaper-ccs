#ifndef CCS_FX_PRESET_CHANGED_SUBSCRIPTION_H
#define CCS_FX_PRESET_CHANGED_SUBSCRIPTION_H

#include "ReaperEventSubscriber.h"
#include "ReaperDataTracker.h"
#include <string>

namespace CCS {

  class ReaperApi;

  class FxPresetChangedSubscription : public ReaperDataTracker {
    MediaTrack *m_track;
    int m_fxId;
    ReaperEventSubscriber *m_subscriber;
    double m_currentValue;
    double m_minValue;
    double m_maxValue;
    double m_midValue;
    std::string m_formattedValue;
    std::string m_trackName;
    std::string m_fxName;

  public:

    FxPresetChangedSubscription(
      MediaTrack *track,
      int fxId,
      ReaperEventSubscriber *subscriber,
      ReaperApi *apiManager
    );

    void triggerEvent() override;
    void update(bool triggerOnChange) override;

    MediaTrack *getTrack();
    int getFxId();
    ReaperEventSubscriber *getSubscriber();
  };
}

#endif
