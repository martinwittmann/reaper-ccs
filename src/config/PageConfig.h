#ifndef CCS_PAGE_CONFIG_H
#define CCS_PAGE_CONFIG_H

#include <string>
#include "BaseConfig.h"

namespace CCS {

  class PageConfig : public BaseConfig {
  public:
    explicit PageConfig(std::string baseDir);
  };
}
#endif