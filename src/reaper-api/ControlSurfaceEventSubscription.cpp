#include "ControlSurfaceEventSubscription.h"
#include <iostream>
#include <string>
#include "../reaper/reaper_plugin.h"
#include "../reaper/reaper_plugin_functions.h"
#include "ReaperApi.h"

namespace CCS {

  ControlSurfaceEventSubscription::ControlSurfaceEventSubscription(
    int eventId,
    MediaTrack *track,
    ReaperEventSubscriber *subscriber,
    ReaperApi *apiManager
  ) {
    m_track = track;
    m_eventId = eventId;
    m_subscriber = subscriber;
    m_apiManager = apiManager;
  }

  void ControlSurfaceEventSubscription::triggerEvent() {
    m_apiManager->triggerOnTrackVolumeChanged(m_track, m_value);
  }

  int ControlSurfaceEventSubscription::getEventId() {
    return m_eventId;
  }

  MediaTrack *ControlSurfaceEventSubscription::getTrack() {
    return m_track;
  }

  ReaperEventSubscriber *ControlSurfaceEventSubscription::getSubscriber() {
    return m_subscriber;
  }
}
