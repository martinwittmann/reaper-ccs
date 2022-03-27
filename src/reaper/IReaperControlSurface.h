#ifndef IREAPER_CONTROL_SURFACE_H
#define IREAPER_CONTROL_SURFACE_H


/*
** Control Surface API
*/

class ReaProject;
class MediaTrack;
class MediaItem;
class MediaItem_Take;
class TrackEnvelope;

class IReaperControlSurface {
public:
  IReaperControlSurface() { }
  virtual ~IReaperControlSurface() { }

  // A simple unique string with only A-Z, 0-9, no spaces or other characters.
  virtual const char *GetTypeString() = 0;

  // Human readable description - can include instance specific information.
  virtual const char *GetDescString() = 0;

  // String of configuration data.
  virtual const char *GetConfigString() = 0;

  // Close without sending "reset" messages, prevent "reset" being sent on destructor.
  virtual void CloseNoReset() { }

  // This is called ~30 times per second and is the place where you can handle
  // incoming midi messages.
  // You're not supposed to do work on UI stuff here.
  virtual void Run() { }

  // These methods will be called by reaper when things change.
  virtual void SetTrackListChange() { }
  virtual void SetSurfaceVolume(MediaTrack *track, double volume) { }
  virtual void SetSurfacePan(MediaTrack *track, double pan) { }
  virtual void SetSurfaceMute(MediaTrack *track, bool mute) { }
  virtual void SetSurfaceSelected(MediaTrack *track, bool selected) { }

  // If track is the master track, this means that at least 1 track is soloed.
  virtual void SetSurfaceSolo(MediaTrack *track, bool solo) { }
  virtual void SetSurfaceRecArm(MediaTrack *track, bool recordArm) { }
  virtual void SetPlayState(bool play, bool pause, bool record) { }
  virtual void SetRepeatState(bool repeat) { }
  virtual void SetTrackTitle(MediaTrack *track, const char *title) { }
  virtual bool GetTouchState(MediaTrack *track, int isPan) { return false; }

  // Automation mode for current track.
  virtual void SetAutoMode(int mode) { }

  // It's good to flush your control states here.
  virtual void ResetCachedVolPanStates() { }

  virtual void OnTrackSelection(MediaTrack *track) { }

  // VK_CONTROL, VK_MENU, VK_SHIFT, etc, whatever makes sense for your surface.
  virtual bool IsKeyDown(int key) { return false; }

  // Return 0 if unsupported.
  virtual int Extended(int call, void *parm1, void *parm2, void *parm3) { return 0; }
};

#endif
