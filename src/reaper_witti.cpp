#define REAPERAPI_IMPLEMENT
#include <iostream>
#include <string>
#include "sdk/reaper_plugin.h"
#include "csurf.h"
#include "csurf_novation_slmk3.cpp"

HWND g_hwnd;

void debug(std::string message) {
  std::cout << "\n[DEBUG] " + message + "\n";
}

extern reaper_csurf_reg_t csurf_witti_reg;

const char* (*GetAppVersion)();


extern "C"
{
  REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t *rec) {
    debug("Plugin entrypoint executed.");
    if (!rec || rec->caller_version != REAPER_PLUGIN_VERSION || !rec->GetFunc) {
      return 0;
    }

//    if (REAPERAPI_LoadAPI(rec->GetFunc) > 0) {
//      debug("Returned 0 loadApi.");
//      return 0;
//    }

    if (!rec || rec->caller_version != REAPER_PLUGIN_VERSION || !rec->GetFunc) {
      debug("Returned 0 plugin version conflict.");
      return 0;
    }

    g_hwnd = rec->hwnd_main;
    int errcnt = 0;
    #define IMPAPI(x) if (!((*((void **)&(x)) = (void *)rec->GetFunc(#x)))) errcnt++;

    IMPAPI(GetAppVersion)
    debug(GetAppVersion());

    rec->Register("csurf",&csurf_witti_reg);

    debug("Returned 1.");
    return 1;
  }
}
