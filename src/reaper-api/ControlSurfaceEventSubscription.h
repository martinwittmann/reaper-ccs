#ifndef CCS_CONTROL_SURFACE_EVENT_SUBSCRIPTION_H
#define CCS_CONTROL_SURFACE_EVENT_SUBSCRIPTION_H

#include "ReaperEventSubscriber.h"

namespace CCS {

  class ReaperApi;

  class ControlSurfaceEventSubscription {
    MediaTrack *m_track;
    int m_eventId;
    ReaperEventSubscriber *m_subscriber;
    ReaperApi *m_apiManager;
    double m_value;

  public:
    ControlSurfaceEventSubscription(
      int eventId,
      MediaTrack *track,
      ReaperEventSubscriber *subscriber,
      ReaperApi *apiManager
    );

    MediaTrack *getTrack();

    ReaperEventSubscriber *getSubscriber();

    void triggerEvent();

    int getEventId();
  };
}


#endif