#include "../reaper/reaper_plugin_functions.h"
#include <iostream>
#include <fstream>
#include "../ccs/Util.h"
#include "yaml-cpp/yaml.h"

char name[128] = "CCS Write config file select Fx plugin";
custom_action_register_t ccs_write_fx_plugin_config_action = {
  0,

  // This must be unique across all sections for actions.
  // Set to NULL for reascripts (automatically generated).
  "CCS_WRITE_CONFIG_FOR_SELECTED_FX_PLUGIN",

  // The name as displayed in the action list, or the full path to a reascript file.
  name
};

void createEnumParameter(
  MediaTrack* track,
  int fxId,
  int paramId,
  std::string fxName,
  std::string paramName,
  YAML::Node* root
) {
  std::map<double, std::string> values;
  char formattedValue[256];
  double minValue;
  double maxValue;

  std::string strParamId = CCS::Util::cleanId(paramName);
  (*root)["parameters"][strParamId]["type"] = "enum";

  double oldValue = TrackFX_GetParam(track, fxId, paramId, &minValue, &maxValue);

  // Note that we start out by setting each param to its lowest value.
  // If we just iterated to 1, we might miss values if the current value is not
  // the lowest one.
  // I think setting param values o to is ok and the correct way to do this.
  TrackFX_SetParam(track, fxId, paramId, 0);

  TrackFX_GetFormattedParamValue(track, fxId, paramId, formattedValue, sizeof formattedValue);
  std::string currentValue = std::string(formattedValue);
  values.insert(std::pair(0, currentValue));
  std::string newValue;

  for (int i = int(minValue * 100); i <= int(maxValue * 100); ++i) {
    double tmpVal = double(i) / 100;
    TrackFX_SetParam(track, fxId, paramId, tmpVal);
    TrackFX_GetFormattedParamValue(track, fxId, paramId, formattedValue, sizeof formattedValue);
    newValue = std::string(formattedValue);
    if (newValue != currentValue) {
      values.insert(std::pair(tmpVal, std::string(formattedValue)));
      currentValue = newValue;
    }
  }

  // Reset the value back to its original value.
  TrackFX_SetParam(track, fxId, paramId, oldValue);

  (*root)["parameters"][strParamId]["label"] = paramName;
  for (auto it = values.begin(); it != values.end(); ++it) {
    (*root)["parameters"][strParamId]["values"][it->second] = it->first;
  }
}

void createRegularParameter(
  MediaTrack* track,
  int fxId,
  int paramId,
  std::string fxName,
  std::string paramName,
  double minValue,
  double maxValue,
  YAML::Node* root
) {
  std::string strParamId = CCS::Util::cleanId(paramName);
  (*root)["parameters"][strParamId]["label"] = paramName;
  (*root)["parameters"][strParamId]["min"] = minValue;
  (*root)["parameters"][strParamId]["max"] = maxValue;
}

bool ccs_write_fx_plugin_config_callback() {
  MediaTrack* track = GetSelectedTrack2(0, 0, true);
  std::cout << "Running action\n";
  if (!track) {
    std::cout << "Aborting action\n";
    return false;
  }

  int trackNumber;
  int itemNumber;
  int fxNumber;
  int focussed = GetFocusedFX2(&trackNumber, &itemNumber, &fxNumber);
  if (focussed != 1) {
    // This means that the fx is a take/item fx or no longer focussed but open.
    // For simplicity, let's keep it to track fx.
    return false;
  }
  // The 8 MSB in fxNumber can have special meaning, which is not relevant for us.
  // Strip it down to a short which has plenty of room.
  // For details see: https://www.reaper.fm/sdk/reascript/reascripthelp.html#GetFocusedFX2
  fxNumber = short(fxNumber);
  int numParams = TrackFX_GetNumParams(track, fxNumber);


  char fxName[256];
  char paramName[256];
  char formattedValue[256];
  double minValue;
  double maxValue;
  double midValue;

  TrackFX_GetFXName(track, fxNumber, fxName, sizeof fxName);

  YAML::Node root;
  root["name"] = std::string(fxName);
  root["id"] = CCS::Util::cleanId(fxName);
  std::string strParamName;

  for (int i = 0; i < numParams; ++i) {
    minValue = 0;
    maxValue = 0;
    midValue = 0;

    TrackFX_GetParamIdent(track, fxNumber, i, paramName, sizeof paramName);
    //std::cout << "param name " << paramName << "\n";

    TrackFX_GetFormattedParamValue(track, fxNumber, i, formattedValue, sizeof formattedValue);
    std::string preprocessedValue = CCS::Util::regexReplace(std::string(formattedValue), "\\sHz$", "");
    preprocessedValue = CCS::Util::regexReplace(std::string(formattedValue), "\\sm?s$", "");
    strParamName = CCS::Util::regexReplace(paramName, "^[0-9:_]+", "");
    bool isEnum = !CCS::Util::regexMatch(preprocessedValue, "[\\d.]+");
    if (isEnum) {
      createEnumParameter(
        track,
        fxNumber,
        i,
        std::string(fxName),
        strParamName,
        &root
      );
    }
    else {
      createRegularParameter(
        track,
        fxNumber,
        i,
        std::string(fxName),
        strParamName,
        minValue,
        maxValue,
        &root
      );
    }

    //std::cout << "formatted value: " << formattedValue << "\n";
  }

  char resourcePath[1024];
  strcpy(resourcePath, GetResourcePath());
  YAML::Emitter emitter;
  emitter << root;


  std::string pluginName = CCS::Util::cleanId(fxName);
  std::string filename = std::string(resourcePath) + "/ccs/fx_plugins/" + pluginName + ".yml";
  std::ofstream file(filename);
  file << emitter.c_str();
  return true;
}