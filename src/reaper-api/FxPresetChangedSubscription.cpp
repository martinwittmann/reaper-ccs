#include "FxPresetChangedSubscription.h"
#include <string>
#include "ReaperDataTracker.h"
#include "ReaperApi.h"
#include "boost/math/special_functions/round.hpp"
#include "../ccs/Util.h"

namespace CCS {

  FxPresetChangedSubscription::FxPresetChangedSubscription(
    MediaTrack *track,
    int fxId,
    ReaperEventSubscriber *subscriber,
    ReaperApi *apiManager
  ) {
    m_api = apiManager;
    m_track = track;
    m_fxId = fxId;
    m_subscriber = subscriber;

    m_trackName = m_api->getTrackName(track);
    m_fxName = m_api->getFxName(track, fxId);
  }

  void FxPresetChangedSubscription::triggerEvent() {
    m_api->triggerOnFxPresetChanged(
      m_track,
      m_fxId,
      m_currentValue,
      m_formattedValue
    );
  }

  MediaTrack *FxPresetChangedSubscription::getTrack() {
    return m_track;
  }

  int FxPresetChangedSubscription::getFxId() {
    return m_fxId;
  }

  ReaperEventSubscriber *FxPresetChangedSubscription::getSubscriber() {
    return m_subscriber;
  }
}
