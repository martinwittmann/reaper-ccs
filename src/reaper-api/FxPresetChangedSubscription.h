#ifndef CCS_FX_PRESET_CHANGED_SUBSCRIPTION_H
#define CCS_FX_PRESET_CHANGED_SUBSCRIPTION_H

#include "ReaperEventSubscriber.h"
#include <string>

namespace CCS {

  class ReaperApi;

  class FxPresetChangedSubscription {
    ReaperApi *m_api = nullptr;
    MediaTrack *m_track = nullptr;
    int m_fxId;
    ReaperEventSubscriber *m_subscriber = nullptr;
    double m_currentValue;
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

    void triggerEvent();

    MediaTrack *getTrack();
    int getFxId();
    ReaperEventSubscriber *getSubscriber();
  };
}

#endif
