#ifndef CCS_FX_PLUGIN_CONFIG_H
#define CCS_FX_PLUGIN_CONFIG_H

#include <string>
#include "BaseConfig.h"

namespace CCS {
  class FxPluginConfig : public BaseConfig {
  public:
    explicit FxPluginConfig(std::string baseDir);
  };
}


#endif
