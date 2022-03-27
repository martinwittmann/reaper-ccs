#include "reaper/csurf.h"
#include "reaper/reaper_plugin_functions.h"

static WDL_DLGRET process_ui(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_INITDIALOG: {
      int parms[4];

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
