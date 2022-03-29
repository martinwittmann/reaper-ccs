#include "FxParameterChangedSubscription.h"
#include <iostream>
#include <string>
#include "../reaper/reaper_plugin.h"
#include "../reaper/reaper_plugin_functions.h"
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


    char buffer[512];
    buffer[0] = 0;
    GetTrackName(track, buffer, sizeof buffer);
    m_trackName = std::string(buffer);

    memset(buffer, 0, sizeof buffer);
    TrackFX_GetFXName(track, fxId, buffer, sizeof buffer);
    m_fxName = std::string(buffer);

    memset(buffer, 0, sizeof buffer);
    TrackFX_GetParamName(track, fxId, paramId, buffer, sizeof buffer);
    m_paramName = std::string(buffer);

    // Set the current values when initializing but don't trigger the event.
    update(false);
  }

  void FxParameterChangedSubscription::update(bool triggerOnChange) {
    double newValue = TrackFX_GetParamEx(
      m_track,
      m_fxId,
      m_paramId,
      &m_minValue,
      &m_maxValue,
      &m_midValue
    );

    if (newValue != m_currentValue) {
      m_currentValue = newValue;

      // Only update the formatted value if we know the actual value has changed.
      char formattedValue[64];
      TrackFX_GetFormattedParamValue(m_track, m_fxId, m_paramId, formattedValue, sizeof formattedValue);
      m_formattedValue = std::string(formattedValue);

      if (triggerOnChange) {
        triggerEvent();
      }
    }
  }

  void FxParameterChangedSubscription::triggerEvent() {
    apiManager->triggerOnFxParameterChanged(
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