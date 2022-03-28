#include "FxPlugins.h"
#include <string>
#include <vector>
#include <experimental/filesystem>
#include "globals.cpp"
#include <iostream>
#include "../reaper/reaper_plugin_functions.h"
#include "Util.h"

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

  FxPluginConfig* FxPlugins::getConfig(string nameId) {
    for (auto config : m_pluginConfigs) {
      string currentId = config->getValue("id");
      if (currentId == nameId) {
        return config;
      }
    }

    throw "Config with id " + nameId + " not found!";
  }

  int FxPlugins::getParamId(MediaTrack* track, int fxId, string strParamId) {
    char buffer[256];
    TrackFX_GetFXName(track, fxId, buffer, sizeof buffer);
    string fxNameId = Util::cleanId(string(buffer));
    FxPluginConfig* config = getConfig(fxNameId);
    string rawId = config->getValue("parameters." + strParamId + ".id");
    return stoi(rawId);
  }
}