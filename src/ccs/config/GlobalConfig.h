#ifndef CCS_GLOBAL_CONFIG_H
#define CCS_GLOBAL_CONFIG_H

#include <string>
#include <vector>
#include "BaseConfig.h"

namespace CCS {

  class GlobalConfig : public BaseConfig {
  public:
    explicit GlobalConfig(std::string baseDir);
    std::string getLastSessionId();
    std::vector<std::string> getSessionMidiControllers();
  };
}

#endif