#include "Ccs_ControlSurface.h"
#include "../reaper/reaper_plugin_functions.h"
#include "Ccs.h"
#include "../reaper-api/ReaperApi.h"
#include "../ccs/Util.h"

namespace CCS {

  Ccs_ControlSurface::Ccs_ControlSurface(int *errorStats) {
    setupMidi(3, 3, errorStats);

    char resourcePath[1024];
    strcpy(resourcePath, GetResourcePath());
    this->ccs = new CCS::Ccs(std::string(resourcePath), m_midiOutput);
  }

  Ccs_ControlSurface::~Ccs_ControlSurface() { }

  void Ccs_ControlSurface::Run() {
    if (m_midiInput) {
      m_midiInput->SwapBufs(timeGetTime());
      MIDI_eventlist *list = m_midiInput->GetReadBuf();
      MIDI_event_t *event;
      int l = 0;
      while ((event = list->EnumItems(&l))) {
        ccs->onMidiEvent(event);
      }
    }

    //if (pollingIndex % 30 == 0) {
    ccs->reaperApi->pollReaperData();
    //}

    pollingIndex++;
    // Reset after 12 times. 12 because its dividable by 2, 3, 4 and 6.
    // Since the internet says that Run is supposed to be called ~30 times
    // per second we can use pollingIndex with a modulo to run things
    // 15, 10, 8 and 5 times per second to not use too much resources.
    if (pollingIndex >= 30) {
      pollingIndex = 0;
    }
  }

  void Ccs_ControlSurface::SetPlayState(bool play, bool pause, bool record) {
    if (play && !pause && !record) {
      ccs->reaperApi->triggerOnPlay();
    }
  }

  void Ccs_ControlSurface::SetRepeatState(bool repeat) {
    ccs->reaperApi->triggerOnRepeatChanged(repeat);
  }

  void Ccs_ControlSurface::SetSurfaceVolume(MediaTrack *track, double volume) {
    ccs->reaperApi->triggerOnTrackVolumeChanged(track, volume);
  }

  void Ccs_ControlSurface::SetSurfaceMute(MediaTrack *track, bool mute) {
    ccs->reaperApi->triggerOnTrackMuteChanged(track, mute);
  }

  void Ccs_ControlSurface::SetSurfaceSolo(MediaTrack *track, bool solo) {
    ccs->reaperApi->triggerOnTrackSoloChanged(track, solo);
  }

  void Ccs_ControlSurface::SetSurfaceRecArm(MediaTrack *track, bool recarm) {
    ccs->reaperApi->triggerOnTrackRecordArmChanged(track, recarm);
  }

  void Ccs_ControlSurface::SetTrackListChange() {
    ccs->reaperApi->triggerOnTrackListChanged();
  }

  int Ccs_ControlSurface::Extended(int event, void *param1, void *param2, void *param3) {
    switch (event) {
      case CSURF_EXT_RESET:
        // Reading code from the CSI project I think this is called when reaper
        // is ready for extensions to run.
        // When the extension is initialized reaper might still be loading.
        ccs->initialize();
        break;

      case CSURF_EXT_TRACKFX_PRESET_CHANGED:
        MediaTrack *track = (MediaTrack*)param1;
        int fxId = *((int*)param2);
        int presetIndex = ccs->reaperApi->getFxPresetIndex(track, fxId);
        string presetName = ccs->reaperApi->getCurrentFxPresetName(track, fxId);
        ccs->reaperApi->triggerOnFxPresetChanged(track, fxId, presetIndex, presetName);
        break;
    }

    return 1;
  }
}