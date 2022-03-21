#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include <string>
#include <vector>
#include "BaseConfig.h"

namespace CCS {

  class GlobalConfig : public BaseConfig {
  public:
    GlobalConfig(std::string baseDir);
    std::string getLastSessionId();
    std::vector<std::string> getSessionMidiControllers();
  };
}

#endif