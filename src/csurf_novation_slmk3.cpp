#include "csurf.h"
#include <iostream>
#include "Ccs.h"
#include "Util.h"

class CSurf_NovationSlMk3 : public IReaperControlSurface {
    int m_midi_in_dev,m_midi_out_dev;
    midi_Output *m_midiout;
    midi_Input *m_midiin;

    WDL_String descspace;
    char configtmp[1024];

    CCS::Ccs* ccs;

    void OnMIDIEvent(MIDI_event_t *evt) {
      ccs->onMidiEvent(evt);
    }

public:
    CSurf_NovationSlMk3(int indev, int outdev, int *errStats) {
      m_midi_in_dev = indev;
      m_midi_out_dev = outdev;


      // Create midi hardware access.
      if (m_midi_in_dev >= 0) {
        m_midiin = CreateMIDIInput(m_midi_in_dev);
      }
      else {
        m_midiin = NULL;
      }

      if (m_midi_out_dev >= 0) {
        m_midiout = CreateThreadedMIDIOutput(CreateMIDIOutput(
          m_midi_out_dev,
          false,
          NULL
        ));
      }
      else {
        m_midiout = NULL;
      }

      if (errStats) {
        if (m_midi_in_dev >=0  && !m_midiin) *errStats|=1;
        if (m_midi_out_dev >=0  && !m_midiout) *errStats|=2;
      }

      if (m_midiin) {
        m_midiin->start();
      }

      // TODO How to get the reaper resource dir via the api?
      this->ccs = new CCS::Ccs("/home/martin/.config/REAPER/ccs/", m_midiout);
    }
    ~CSurf_NovationSlMk3() {
      if (m_midiout) {
        DELETE_ASYNC(m_midiout);
      }
      if (m_midiin) {
        DELETE_ASYNC(m_midiin);
      }
    }

    const char *GetTypeString() { return "Novation SL MK III"; }
    const char *GetDescString() {
      descspace.SetFormatted(
        512,
        __LOCALIZE_VERFMT("Novation SL MK III (Device %d,%d)", "csurf"),
        m_midi_in_dev,
        m_midi_out_dev
      );
      return descspace.Get();
    }
    const char *GetConfigString() {
      // String of configuration data. We just store the midi device ids as integers.
      sprintf(configtmp, "0 0 %d %d", m_midi_in_dev, m_midi_out_dev);
      return configtmp;
    }

    void CloseNoReset() {
      if (m_midiout) {
        DELETE_ASYNC(m_midiout);
        m_midiout = 0;
      }
      if (m_midiin) {
        DELETE_ASYNC(m_midiin);
        m_midiin = 0;
      }
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
  // Since I was not yet able to actually store a control surface, let's always use the
  // correct id so we don't need to manually select them everytime when testing.
  return new CSurf_NovationSlMk3(3, 3, errStats);
}

static WDL_DLGRET dlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_INITDIALOG: {
      int parms[4];
      parseParms((const char *)lParam, parms);

      ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT1), SW_HIDE);
      ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT1_LBL), SW_HIDE);
      ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT2), SW_HIDE);
      ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT2_LBL), SW_HIDE);
      ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT2_LBL2), SW_HIDE);

      WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO2));
      WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO3));

      int n = GetNumMIDIInputs();
      int x = SendDlgItemMessage(
        hwndDlg,
        IDC_COMBO2,
        CB_ADDSTRING,
        0,
        (LPARAM)__LOCALIZE("None", "csurf")
      );
      SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_SETITEMDATA, x, -1);
      x = SendDlgItemMessage(
        hwndDlg,
        IDC_COMBO3,
        CB_ADDSTRING,
        0,
        (LPARAM)__LOCALIZE("None", "csurf")
      );
      SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_SETITEMDATA, x, -1);
      for (x = 0; x < n; x ++) {
        char buf[512];
        if (GetMIDIInputName(x, buf, sizeof(buf))) {
          int a = SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)buf);
          SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_SETITEMDATA, a, x);
          if (x == parms[2]) {
            SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_SETCURSEL, a, 0);
          }
        }
      }
      n = GetNumMIDIOutputs();
      for (x = 0; x < n; x ++) {
        char buf[512];
        if (GetMIDIOutputName(x, buf, sizeof(buf))) {
          int a = SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)buf);
          SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_SETITEMDATA, a, x);
          if (x == parms[3]) {
            SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_SETCURSEL, a, 0);
          }
        }
      }
    }
    break;

    case WM_USER + 1024:
      if (wParam > 1 && lParam) {
        char tmp[512];

        int indev = -1, outdev = -1;
        int r = SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_GETCURSEL, 0, 0);
        if (r != CB_ERR) {
          indev = SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_GETITEMDATA, r, 0);
        }
        r = SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_GETCURSEL, 0, 0);
        if (r != CB_ERR) {
          outdev = SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_GETITEMDATA, r, 0);
        }

        sprintf(tmp,"0 0 %d %d", indev, outdev);
        lstrcpyn((char *)lParam, tmp, wParam);
      }
    break;
  }
  return 0;
}

static HWND configFunc(const char *type_string, HWND parent, const char *initConfigString) {
  std::cout << "\nconfigFunc: " << initConfigString << "\n";

  return CreateDialogParam(
    g_hInst,
    MAKEINTRESOURCE(IDD_SURFACEEDIT_SLMK3),
    parent,
    dlgProc,
    (LPARAM)initConfigString
  );
}


reaper_csurf_reg_t csurf_novation_slmk3_reg = {
  "NOVATIONSLMKIII",
  // !WANT_LOCALIZE_STRINGS_BEGIN:csurf_type
  __LOCALIZE_VERFMT("Novation SL MK III", "csurf"),
  // !WANT_LOCALIZE_STRINGS_END
  createFunc,
  configFunc,
};




