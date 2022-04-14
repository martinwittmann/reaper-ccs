#include "../reaper/csurf.h"
#include "../ccs/Util.h"
#include "../ccs/Ccs_ControlSurface.h"
#include "../control_surface_ui.cpp"

static IReaperControlSurface* control_surface_create(
  const char *type_string,
  const char *configString,
  int *errStats
) {
  return new CCS::Ccs_ControlSurface(errStats);
}

static HWND control_surface_configure(const char *type_string, HWND parent, const char *initConfigString) {
  return CreateDialogParam(
    g_hInst,
    MAKEINTRESOURCE(IDD_SURFACEEDIT_CCS),
    parent,
    process_ui,
    (LPARAM)initConfigString
  );
}

reaper_csurf_reg_t ccs_control_surface = {
  "CCS",
  // !WANT_LOCALIZE_STRINGS_BEGIN:csurf_type
  __LOCALIZE_VERFMT("Custom Control Surface", "csurf"),
  // !WANT_LOCALIZE_STRINGS_END
  control_surface_create,
  control_surface_configure,
};



