/**
 * Do not edit this file.
 * To get started with creating a reaper extension please see entrypoint.cpp.
 */

#define REAPERAPI_IMPLEMENT
#include "reaper_plugin.h"
#include "resource.h"

#define LOCALIZE_IMPORT_PREFIX "csurf_"
#include "localize-import.h"

#ifndef _WIN32
#include "../WDL/swell/swell-dlggen.h"
#include "res.rc_mac_dlg"
#include "../WDL/swell/swell-menugen.h"
#include "res.rc_mac_menu"
#endif

// This file is supposed to contain the functions
// load_reaper_extension() and unload_reaper_extension().
// These are being called from in here to set up the extension.
// We do this in order to abstract away the more complicated and not so clean
// aspects of creating reaper extensions.
#include "../entrypoint.cpp"

HWND g_hwnd;

extern "C" {
  REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(
    REAPER_PLUGIN_HINSTANCE hInstance,
    reaper_plugin_info_t* rec
  ) {
    // This should be reaper's main window.
    g_hwnd = rec->hwnd_main;

    if (rec != nullptr) {
      return load_reaper_extension(rec);
    }
    else {
      return unload_reaper_extension(rec);
    }
  }
}

