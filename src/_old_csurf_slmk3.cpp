/*
#include "reaper/csurf.h"
#include <iostream>
#include "ccs/Ccs.h"
#include "ccs/Util.h"

static IReaperControlSurface *createFunc(
  const char *type_string,
  const char *configString,
  int *errStats
) {
  // Since I was not yet able to actually store a control surface, let's always use the
  // correct id so we don't need to manually select them everytime when testing.
  return new CSurf_NovationSlMk3(3, 3, errStats);
}

static HWND configFunc(const char *type_string, HWND parent, const char *initConfigString) {
  return CreateDialogParam(
    g_hInst,
    MAKEINTRESOURCE(IDD_SURFACEEDIT_CCS),
    parent,
    dlgProc,
    (LPARAM)initConfigString
  );
}

reaper_csurf_reg_t csurf_novation_slmk3_reg = {
  "NOVATIONSLMK3",
  // !WANT_LOCALIZE_STRINGS_BEGIN:csurf_type
  __LOCALIZE_VERFMT("Novation SL MK III", "csurf"),
  // !WANT_LOCALIZE_STRINGS_END
  createFunc,
  configFunc,
};


*/

