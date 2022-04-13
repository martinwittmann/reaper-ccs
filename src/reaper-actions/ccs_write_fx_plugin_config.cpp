#include "../reaper/reaper_plugin_functions.h"
#include <iostream>
#include <fstream>
#include "../ccs/Util.h"
#include "yaml-cpp/yaml.h"

using std::string;

char name[128] = "CCS Write config file select Fx plugin";
custom_action_register_t ccs_write_fx_plugin_config_action = {
  0,

  // This must be unique across all sections for actions.
  // Set to NULL for reascripts (automatically generated).
  "CCS_WRITE_CONFIG_FOR_SELECTED_FX_PLUGIN",

  // The name as displayed in the action list, or the full path to a reascript file.
  name
};

void create_enum_parameter(
  MediaTrack* track,
  int fxId,
  int paramId,
  string fxName,
  string paramName,
  double minValue,
  double maxValue,
  YAML::Node* root
) {
  std::map<double, string> values;
  char formattedValue[256];
  string strParamId = CCS::Util::cleanId(paramName);
  (*root)["parameters"][strParamId]["type"] = "enum";

  // Note that we start out by setting each param to its lowest value.
  // If we just iterated to 1, we might miss values if the current value is not
  // the lowest one.
  // Setting param values to its minValue.
  TrackFX_SetParam(track, fxId, paramId, minValue);

  TrackFX_GetFormattedParamValue(track, fxId, paramId, formattedValue, sizeof formattedValue);
  string currentValue = string(formattedValue);
  values.insert(std::pair(0, currentValue));
  string newValue;

  for (int i = int(minValue * 100); i <= int(maxValue * 100); ++i) {
    double tmpVal = double(i) / 100;
    TrackFX_SetParam(track, fxId, paramId, tmpVal);
    TrackFX_GetFormattedParamValue(track, fxId, paramId, formattedValue, sizeof formattedValue);
    newValue = string(formattedValue);
    if (newValue != currentValue) {
      values.insert(std::pair(tmpVal, string(formattedValue)));
      currentValue = newValue;
    }
  }

  (*root)["parameters"][strParamId]["id"] = paramId;
  (*root)["parameters"][strParamId]["label"] = paramName;
  for (auto it = values.begin(); it != values.end(); ++it) {
    (*root)["parameters"][strParamId]["values"][it->second] = it->first;
  }

  // Since the calling function takes care of setting the current parameter
  // back to its original value, we don't need to.
}

void create_regular_parameter(
  MediaTrack* track,
  int fxId,
  int paramId,
  string fxName,
  string paramName,
  double minValue,
  double maxValue,
  YAML::Node* root
) {
  string strParamId = CCS::Util::cleanId(paramName);
  (*root)["parameters"][strParamId]["id"] = paramId;
  (*root)["parameters"][strParamId]["label"] = paramName;
  (*root)["parameters"][strParamId]["min"] = minValue;
  (*root)["parameters"][strParamId]["max"] = maxValue;
}

bool is_enum_value(string input) {
  using CCS::Util;
  string result = Util::regexReplace(input, "Hz$", "");
  result = Util::regexReplace(result, "m?s$", "");
  result = Util::regexReplace(result, "\\s+", "");
  // Note that we're not using \d here, because we want detect values like "+1"
  // as enum values.
  return !CCS::Util::regexMatch(result, "^[0-9\\.]+$");
}

bool ccs_write_fx_plugin_config_callback() {
  MediaTrack* track = GetSelectedTrack2(0, 0, true);
  if (!track) {
    return false;
  }

  int trackNumber;
  int itemNumber;
  int fxId;
  int focussed = GetFocusedFX2(&trackNumber, &itemNumber, &fxId);
  if (focussed != 1) {
    // This means that the fx is a take/item fx or no longer focussed but open.
    // For simplicity, let's keep it to track fx.
    return false;
  }
  // The 8 MSB in fxId can have special meaning, which is not relevant for us.
  // Strip it down to a short which has plenty of room.
  // For details see: https://www.reaper.fm/sdk/reascript/reascripthelp.html#GetFocusedFX2
  fxId = short(fxId);
  int numParams = TrackFX_GetNumParams(track, fxId);


  char fxName[256];
  char paramName[256];
  char formattedValue[256];
  double minValue;
  double maxValue;
  double midValue;
  string strParamName;

  TrackFX_GetFXName(track, fxId, fxName, sizeof fxName);

  YAML::Node root;
  root["name"] = string(fxName);
  root["id"] = CCS::Util::cleanId(fxName);

  for (int paramId = 0; paramId < numParams; ++paramId) {
    minValue = 0;
    maxValue = 0;


    TrackFX_GetParamIdent(track, fxId, paramId, paramName, sizeof paramName);
    double oldValue = TrackFX_GetParam(track, fxId, paramId, &minValue, &maxValue);

    // We're setting the param to its max value before detecting if this is an
    // enum param because there are parameters that have 'off' for their lowest
    // value and then are regular double values.
    // Since as far as I know this does not exist for the highest value we can
    // work around that problem this way.
    TrackFX_SetParam(track, fxId, paramId, maxValue);
    TrackFX_GetFormattedParamValue(track, fxId, paramId, formattedValue, sizeof formattedValue);
    string strParamName = CCS::Util::regexReplace(paramName, "^[0-9:_]+", "");
    if (is_enum_value(string(formattedValue))) {
      create_enum_parameter(
        track,
        fxId,
        paramId,
        string(fxName),
        strParamName,
        minValue,
        maxValue,
        &root
      );
    }
    else {
      create_regular_parameter(
        track,
        fxId,
        paramId,
        string(fxName),
        strParamName,
        minValue,
        maxValue,
        &root
      );
    }

    TrackFX_SetParam(track, fxId, paramId, oldValue);
  }

  char resourcePath[1024];
  strcpy(resourcePath, GetResourcePath());
  YAML::Emitter emitter;
  emitter << root;


  string pluginName = CCS::Util::cleanId(fxName);
  string filename = string(resourcePath) + "/ccs/fx_plugins/" + pluginName + ".yml";
  std::ofstream file(filename);
  file << emitter.c_str();
  return true;
}