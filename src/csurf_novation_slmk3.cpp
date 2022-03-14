
//#include "WDL/mutex.h"
//#include "WDL/swell/swell-types.h"
//#include "WDL/swell/swell-functions.h"
//#include "WDL/swell/swell-internal.h"
//#include "WDL/swell/swell-dlggen.h"
//#include "WDL/swell/swell-gdi-internalpool.h"
//#include "WDL/swell/swell-menugen.h"


#include "csurf.h"
#include <iostream>

class CSurf_NovationSlMk3 : public IReaperControlSurface {
    int m_midi_in_dev,m_midi_out_dev;
    midi_Output *m_midiout;
    midi_Input *m_midiin;

    WDL_String descspace;
    char configtmp[1024];

    void OnMIDIEvent(MIDI_event_t *evt) {
      std::cout << "Midi Event received.";
    }


public:
    CSurf_NovationSlMk3(int indev, int outdev, int *errStats) {
      m_midi_in_dev = indev;
      m_midi_out_dev = outdev;
      std::cout << "Csurf initalized.";

      // Create midi hardware access.
      m_midiin = m_midi_in_dev >= 0 ? CreateMIDIInput(m_midi_in_dev) : NULL;
      m_midiout = m_midi_out_dev >= 0 ? CreateThreadedMIDIOutput(CreateMIDIOutput(m_midi_out_dev, false, NULL)) : NULL;

      if (errStats) {
        if (m_midi_in_dev >=0  && !m_midiin) *errStats|=1;
        if (m_midi_out_dev >=0  && !m_midiout) *errStats|=2;
      }

      if (m_midiin) {
        m_midiin->start();
      }

      if (m_midiout) {
      }

    }
    ~CSurf_NovationSlMk3() {
      if (m_midiout) { }
      DELETE_ASYNC(m_midiout);
      DELETE_ASYNC(m_midiin);
    }

    const char *GetTypeString() { return "Novation SL MK III"; }
    const char *GetDescString() {
      descspace.SetFormatted(
        512,
        "Novation SL MK III (Device %d,%d)",
        m_midi_in_dev,
        m_midi_out_dev
      );
      return descspace.Get();
    }
    const char *GetConfigString() {
    // string of configuration data
      sprintf(configtmp,"0 0 %d %d",m_midi_in_dev,m_midi_out_dev);
      return configtmp;
    }

    void CloseNoReset() {
      DELETE_ASYNC(m_midiout);
      DELETE_ASYNC(m_midiin);
      m_midiout=0;
      m_midiin=0;
    }

    void Run() {
      if (m_midiin) {
        m_midiin->SwapBufs(timeGetTime());
        int l = 0;
        MIDI_eventlist *list = m_midiin->GetReadBuf();
        MIDI_event_t *evts;
        while ((evts = list->EnumItems(&l))) OnMIDIEvent(evts);
      }
    }

    void SetSurfaceVolume(MediaTrack *trackid, double volume) { }
    void SetSurfacePan(MediaTrack *trackid, double pan) { }
    void SetSurfaceMute(MediaTrack *trackid, bool mute) { }
    void SetSurfaceSelected(MediaTrack *trackid, bool selected) { }
    void SetSurfaceSolo(MediaTrack *trackid, bool solo) { }
    void SetSurfaceRecArm(MediaTrack *trackid, bool recarm) { }
    void SetPlayState(bool play, bool pause, bool rec) { }
    void SetRepeatState(bool rep) {}

    void SetTrackTitle(MediaTrack *trackid, const char *title) { }

    bool IsKeyDown(int key)
    {
      return false;
    }


    virtual int Extended(int call, void *parm1, void *parm2, void *parm3)
    {
      DEFAULT_DEVICE_REMAP()
      return 0;
    }
};

static void parseParms(const char *str, int parms[4]) {
  parms[0] = 0;
  parms[1] = 9;
  parms[2] = parms[3] = -1;

  const char *p = str;
  if (p) {
    int x = 0;
    while (x < 4) {
      while (*p == ' ') p++;
      if ((*p < '0' || *p > '9') && *p != '-') break;
      parms[x++] = atoi(p);
      while (*p && *p != ' ') p++;
    }
  }
}

static IReaperControlSurface *createFunc(const char *type_string, const char *configString, int *errStats) {
  int parms[4];
  parseParms(configString,parms);
  return new CSurf_NovationSlMk3(parms[2],parms[3],errStats);
}

static WDL_DLGRET dlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  return 0;
}

static HWND configFunc(const char *type_string, HWND parent, const char *initConfigString) {
  return CreateDialogParam(
    g_hInst,
    MAKEINTRESOURCE(IDD_SURFACEEDIT_MCU),
    parent,
    dlgProc,
    (LPARAM)initConfigString
  );
}


reaper_csurf_reg_t csurf_novation_slmk3_reg = {
  "Wittis Control Surface",
  // !WANT_LOCALIZE_STRINGS_BEGIN:csurf_type
  "Wiuiui noch ein Text",
  // !WANT_LOCALIZE_STRINGS_END
  createFunc,
  configFunc,
};
