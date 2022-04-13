#include "FxParameterChangedSubscription.h"
#include <string>
#include "ReaperDataTracker.h"
#include "ReaperApi.h"
#include "boost/math/special_functions/round.hpp"
#include "../ccs/Util.h"

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
    string newFormattedValue = m_api->getFormattedParamValue(m_track, m_fxId, m_paramId);
    // Clion reports std::round as not defined even though we're using c++20.
    // See: https://youtrack.jetbrains.com/issue/CPP-25870
    //int normalizedNewValue = boost::math::round(newValue * 100.0);
    //int normalizedCurrentValue = boost::math::round(m_currentValue * 100.0);

    // We're checking formatted value, because some plugins seem to set different
    // values when changing parameters to the same value.
    // Since this would trigger multiple change events, we work around this by
    // checking the formatted value.
    if (newFormattedValue != m_formattedValue) {
      const auto [newValue, minValue, maxValue, midValue] = m_api->getParamValueEx(
        m_track,
        m_fxId,
        m_paramId
      );

      m_minValue = minValue;
      m_maxValue = maxValue;
      m_midValue = midValue;
      m_currentValue = newValue;
      m_formattedValue = newFormattedValue;
      Util::log(m_paramName + " " + m_formattedValue + " ", false);
      Util::log(m_currentValue);

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