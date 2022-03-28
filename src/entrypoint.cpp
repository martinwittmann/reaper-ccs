#include "control-surfaces/ccs_control_surface.cpp"
#include "reaper-actions/ccs_write_fx_plugin_config.cpp"
#include "reaper/reaper_plugin.h"
#include "reaper/reaper_plugin_functions.h"
#include <iostream>
#include <map>
#include <string>

std::map<int, std::string> commandIds;

bool reaper_hook_command(
  KbdSectionInfo *section,
  int commandId,
  int value,
  int value2,
  int relmode,
  HWND hwnd
) {
  if (!commandIds.contains(commandId)) {
    return false;
  }

  std::string actionName = commandIds.at(commandId);
  if (actionName == "CCS_WRITE_FX_PLUGIN_CONFIG") {
    return ccs_write_fx_plugin_config_callback();
  }

  return false;
}

/**
 * This function is called when reaper loads the extension.
 * Needs to return 1 on success.
 */
int load_reaper_extension(reaper_plugin_info_t* reaper) {

  // Try to import all api functions and stop if something went wrong.
  // See: https://forum.cockos.com/showthread.php?t=151862
  if (REAPERAPI_LoadAPI(reaper->GetFunc) > 0) {
    return 0;
  }

  reaper->Register("csurf", &ccs_control_surface);

  int commandId = reaper->Register("custom_action", &ccs_write_fx_plugin_config_action);
  reaper->Register("hookcommand2", (void*)reaper_hook_command);
  commandIds.insert(std::pair(commandId, "CCS_WRITE_FX_PLUGIN_CONFIG"));
  return 1;
}

// This function needs to return 0;
int unload_reaper_extension(reaper_plugin_info_t* reaper) {
  return 0;
}