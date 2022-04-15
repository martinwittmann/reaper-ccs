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
    m_ccs = new CCS::Ccs(std::string(resourcePath), m_midiOutput);
  }

  Ccs_ControlSurface::~Ccs_ControlSurface() { }

  void Ccs_ControlSurface::Run() {
    if (m_midiInput) {
      m_midiInput->SwapBufs(timeGetTime());
      MIDI_eventlist *list = m_midiInput->GetReadBuf();
      MIDI_event_t *event;
      int l = 0;
      while ((event = list->EnumItems(&l))) {
        m_ccs->onMidiEvent(event);
      }
    }

    m_ccs->reaperApi->pollReaperData();
  }

  void Ccs_ControlSurface::SetPlayState(bool play, bool pause, bool record) {
    if (!play && !record) {
      m_ccs->reaperApi->triggerOnStop(true);
    }
    else if (!play && pause && !record) {
      m_ccs->reaperApi->triggerOnPause(true);
    }
    else if (play && !pause && !record) {
      m_ccs->reaperApi->triggerOnPlay(true);
    }
    else if (play && !pause && record) {
      m_ccs->reaperApi->triggerOnRecord(true);
    }
  }

  void Ccs_ControlSurface::SetRepeatState(bool repeat) {
    m_ccs->reaperApi->triggerOnRepeatChanged(repeat);
  }

  void Ccs_ControlSurface::SetSurfaceVolume(MediaTrack *track, double volume) {
    m_ccs->reaperApi->triggerOnTrackVolumeChanged(track, volume);
  }

  void Ccs_ControlSurface::SetSurfaceMute(MediaTrack *track, bool mute) {
    m_ccs->reaperApi->triggerOnTrackMuteChanged(track, mute);
  }

  void Ccs_ControlSurface::SetSurfaceSolo(MediaTrack *track, bool solo) {
    m_ccs->reaperApi->triggerOnTrackSoloChanged(track, solo);
  }

  void Ccs_ControlSurface::SetSurfaceRecArm(MediaTrack *track, bool recarm) {
    m_ccs->reaperApi->triggerOnTrackRecordArmChanged(track, recarm);
  }

  void Ccs_ControlSurface::SetTrackListChange() {
    m_ccs->reaperApi->triggerOnTrackListChanged();
  }

  int Ccs_ControlSurface::Extended(int event, void *param1, void *param2, void *param3) {
    switch (event) {
      case CSURF_EXT_RESET:
        // Reading code from the CSI project I think this is called when reaper
        // is ready for extensions to run.
        // When the extension is initialized reaper might still be loading.
        m_ccs->initialize();
        break;

      case CSURF_EXT_TRACKFX_PRESET_CHANGED:
        MediaTrack *track = (MediaTrack*)param1;
        int fxId = *((int*)param2);
        int presetIndex = m_ccs->reaperApi->getFxPresetIndex(track, fxId);
        string presetName = m_ccs->reaperApi->getCurrentFxPresetName(track, fxId);
        m_ccs->reaperApi->triggerOnFxPresetChanged(track, fxId, presetIndex, presetName);
        break;
    }

    return 1;
  }
}