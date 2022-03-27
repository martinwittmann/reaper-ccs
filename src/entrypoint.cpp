#include "control_surface.cpp"
#include "reaper/reaper_plugin_functions.h"

void debug(std::string message) {
  std::cout << "\n[DEBUG] " + message + "\n";
}

/**
 * This function is called when reaper loads the extension.
 * Needs to return 1 on success.
 */
int load_reaper_extension(reaper_plugin_info_t* reaper) {
  debug("Plugin entrypoint executed.");

  // Try to import all api functions and stop if something went wrong.
  // See: https://forum.cockos.com/showthread.php?t=151862
  if (REAPERAPI_LoadAPI(reaper->GetFunc) > 0) {
    debug("Could not load all Reaper Api functions.");
    return 0;
  }

  reaper->Register("csurf", &ccs_control_surface);
  //reaper->Register("custom_action", &)
  return 1;
}

// This function needs to return 0;
int unload_reaper_extension(reaper_plugin_info_t* reaper) {
  debug("Extension unloaded");
  return 0;
}
