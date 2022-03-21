#ifndef SESSION_CONFIG_H
#define SESSION_CONFIG_H

#include <string>
#include "BaseConfig.h"

namespace CCS {

  class SessionConfig : public BaseConfig {
  public:
    explicit SessionConfig(std::string baseDir);
  };
}

#endif