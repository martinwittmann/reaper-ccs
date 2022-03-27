//
// Created by martin on 27.03.22.
//

#include "ReaperActions.h"
#include "reaper/reaper_plugin.h"


namespace CCS {

  void ReaperActions::createActions() {
    char name[128] = "CCS Write config file select Fx plugin";
    custom_action_register_t customAction1 = {
      // 0 / 100 = main / main alt,
      // 32063 = media explorer
      // 32060 = midi editor
      // 32061 = midi event list editor
      // 32062 = midi inline editor, etc
      0,

      // This must be unique across all sections for actions.
      // Set to NULL for reascripts (automatically generated).
      "CCS_WRITE_CONFIG_FOR_SELECTED_FX_PLUGIN",

      // The name as displayed in the action list, or the full path to a reascript file.
      name
    };
  }
}