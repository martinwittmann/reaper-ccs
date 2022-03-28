#include "FxPluginConfig.h"
#include "BaseConfig.h"
#include <string>

namespace CCS {
  FxPluginConfig::FxPluginConfig(std::string baseDir) : BaseConfig(baseDir, false) { }
}
