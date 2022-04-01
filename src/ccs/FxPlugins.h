#ifndef CCS_FX_PLUGIN_H
#define CCS_FX_PLUGIN_H

#include <string>
#include <vector>
#include <map>
#include <experimental/filesystem>
#include "../reaper/csurf.h"

namespace CCS {
  namespace fse = std::experimental::filesystem;

  class FxPluginConfig;

  class FxPlugins {
    std::string m_pluginsDir;
    std::vector<FxPluginConfig*> m_pluginConfigs;

  public:
    FxPlugins(std::string pluginsDir);
    ~FxPlugins();

    std::vector<std::string> getFxPluginNames();
    bool isFxPluginConfigFile(fse::path path);
    FxPluginConfig *getConfig(std::string nameId);
    int getParamId(MediaTrack *track, int fxId, std::string strParamId);

    FxPluginConfig *getConfig(MediaTrack *track, int fxId);

    std::map<std::string,double> getParamEnumValues(MediaTrack *track, int fxId, std::string paramId);
  };
}


#endif
