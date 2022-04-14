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
  ) : ReaperDataTracker(apiManager) {
    m_track = track;
    m_fxId = fxId;
    m_subscriber = subscriber;

    m_trackName = m_api->getTrackName(track);
    m_fxName = m_api->getFxName(track, fxId);

    // Set the current values when initializing but don't trigger the event.
    update(false);
  }

  void FxPresetChangedSubscription::update(bool triggerOnChange) {
    string newValue = m_api->getCurrentFxPresetName(m_track, m_fxId);

    if (newValue != m_formattedValue) {
      // We're retrieving the preset index, but it looks like reaper is not
      // returning anything useful, so we're using the preset name for checking.
      m_currentValue = m_api->getFxPresetIndex(m_track, m_fxId);
      m_formattedValue = m_api->getCurrentFxPresetName(m_track, m_fxId);

      if (triggerOnChange) {
        triggerEvent();
      }
    }
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
