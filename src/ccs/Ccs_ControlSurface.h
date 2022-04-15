#ifndef CCS__CCS_CONTROL_SURFACE_H
#define CCS__CCS_CONTROL_SURFACE_H

#include "../reaper/ControlSurface.h"

namespace CCS {

  class Ccs;

  class Ccs_ControlSurface : public Reaper::ControlSurface {
    CCS::Ccs *m_ccs = nullptr;

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
    void SetTrackListChange() override;
    int Extended(int call, void *parm1, void *parm2, void *parm3) override;
  };
}

#endif
