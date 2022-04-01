#ifndef CCS__CCS_CONTROL_SURFACE_H
#define CCS__CCS_CONTROL_SURFACE_H

#include "../reaper/ControlSurface.h"
#include "Ccs.h"

namespace CCS {

  class Ccs_ControlSurface : public Reaper::ControlSurface {
    CCS::Ccs *ccs = nullptr;
    char pollingIndex = 0;

  public:
    explicit Ccs_ControlSurface(int *errorStats);
    ~Ccs_ControlSurface();
    void Run() override;
    void SetPlayState(bool play, bool pause, bool record) override;
    void SetRepeatState(bool repeat) override;
    void SetSurfaceVolume(MediaTrack *track, double volume) override;
    void SetSurfaceMute(MediaTrack *track, bool mute) override;
    void SetSurfaceSolo(MediaTrack *track, bool solo) override;
    void SetSurfaceRecArm(MediaTrack *track, bool recarm) override;
  };
}

#endif
