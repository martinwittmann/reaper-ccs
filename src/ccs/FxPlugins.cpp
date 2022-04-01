#include "FxPlugins.h"
#include <string>
#include <vector>
#include <experimental/filesystem>
#include "globals.cpp"
#include <iostream>
#include "../reaper/reaper_plugin_functions.h"
#include "Util.h"
#include "CcsException.h"
#include "config/FxPluginConfig.h"

namespace CCS {

  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;
  using std::vector;

  FxPlugins::FxPlugins(string pluginsDir) {
    m_pluginsDir = pluginsDir;
    vector<string> pluginNames = getFxPluginNames();
    for (auto pluginName : pluginNames) {
      m_pluginConfigs.push_back(new FxPluginConfig(pluginName));
    }
  }

  FxPlugins::~FxPlugins() {
    for (auto config : m_pluginConfigs) {
      delete config;
    }
  }

  vector<string> FxPlugins::getFxPluginNames() {
    vector<string> result;
    for (const auto &entry: fse::directory_iterator(m_pluginsDir)) {
      fse::path entry_path = fse::path(entry.path());
      if (is_directory(entry_path) || !isFxPluginConfigFile(entry_path.string())) {
        continue;
      }
      result.push_back(entry_path.string());
    }
    return result;
  }

  bool FxPlugins::isFxPluginConfigFile(fse::path path) {
    string filename = path.filename();
    filename.length();
    // Note that we ignore files starting with _ here. This allows us to use
    // those as templates to inherit from, like _default.yml.
    if (filename.starts_with(".") || filename.starts_with("_") || !filename.ends_with(YAML_EXT)) {
      return false;
    }
    return is_regular_file(path);
  }

  FxPluginConfig *FxPlugins::getConfig(string nameId) {
    for (auto config : m_pluginConfigs) {
      string currentId = config->getValue("id");
      if (currentId == nameId) {
        return config;
      }
    }
    throw CcsException("Config with id " + nameId + " not found!");
  }

  FxPluginConfig *FxPlugins::getConfig(MediaTrack *track, int fxId) {
    char buffer[256];
    TrackFX_GetFXName(track, fxId, buffer, sizeof buffer);
    string fxNameId = Util::cleanId(string(buffer));
    return getConfig(fxNameId);
  }

  int FxPlugins::getParamId(MediaTrack *track, int fxId, string strParamId) {
    FxPluginConfig *config = getConfig(track, fxId);
    try {
      string rawId = config->getValue("parameters." + strParamId + ".id", false);
      return stoi(rawId);
    }
    catch (CcsException &e) {
      throw CcsException("Trying to get fx plugin parameter: '" + strParamId + "' that is not in config.");
    }
  }

  std::map<string,double> FxPlugins::getParamEnumValues(
    MediaTrack *track,
    int fxId,
    string paramId
  ) {
    FxPluginConfig *config = getConfig(track, fxId);
    std::map<string,double> result;

    string type = config->getValue("parameters." + paramId + ".type");
    if (config->getValue("parameters." + paramId + ".type") != "enum") {
      string message = "Trying to get available parameter enum values for fxId ";
      string label = config->getValue("parameters." + paramId + ".label");
      message += std::to_string(fxId) + ": " + label;
      message += ", which is not an enum parameter.";
      Util::error(message);
    }

    YAML::Node valuesNode = config->getMapValue("parameters." + paramId + ".values");
    for (auto node : valuesNode) {
      result.insert(std::pair(node.first.as<string>(), node.second.as<double>()));
    }

    return result;
  }
}