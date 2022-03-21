#ifndef PAGE_CONFIG_H
#define PAGE_CONFIG_H

#include <string>
#include "BaseConfig.h"

namespace CCS {

  class PageConfig : public BaseConfig {
  public:
    PageConfig(std::string baseDir);
  };
}
#endif