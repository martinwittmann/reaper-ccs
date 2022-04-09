#include "FxParameterChangedSubscription.h"
#include <iostream>
#include <string>
#include "ReaperDataTracker.h"
#include "ReaperApi.h"

namespace CCS {

  FxParameterChangedSubscription::FxParameterChangedSubscription(
    MediaTrack *track,
    int fxId,
    int paramId,
    ReaperEventSubscriber *subscriber,
    ReaperApi *apiManager
  ) : ReaperDataTracker(apiManager) {

    m_track = track;
    m_fxId = fxId;
    m_paramId = paramId;
    m_subscriber = subscriber;

    m_trackName = m_api->getTrackName(track);
    m_fxName = m_api->getFxName(track, fxId);
    m_paramName = m_api->getFxParameterName(track, fxId, paramId);

    // Set the current values when initializing but don't trigger the event.
    update(false);
  }

  void FxParameterChangedSubscription::update(bool triggerOnChange) {
    const auto [newValue, minValue, maxValue, midValue] = m_api->getParamValueEx(
      m_track,
      m_fxId,
      m_paramId
    );

    m_minValue = minValue;
    m_maxValue = maxValue;
    m_midValue = midValue;

    if (newValue != m_currentValue) {
      m_currentValue = newValue;

      // Only update the formatted value if we know the actual value has changed.
      m_formattedValue = m_api->getFormattedParamValue(m_track, m_fxId, m_paramId);

      if (triggerOnChange) {
        triggerEvent();
      }
    }
  }

  void FxParameterChangedSubscription::triggerEvent() {
    m_api->triggerOnFxParameterChanged(
      m_track,
      m_fxId,
      m_paramId,
      m_currentValue,
      m_minValue,
      m_maxValue,
      m_formattedValue
    );
  }

  MediaTrack *FxParameterChangedSubscription::getTrack() {
    return m_track;
  }

  int FxParameterChangedSubscription::getFxId() {
    return m_fxId;
  }

  int FxParameterChangedSubscription::getParamId() {
    return m_paramId;
  }

  ReaperEventSubscriber *FxParameterChangedSubscription::getSubscriber() {
    return m_subscriber;
  }
}