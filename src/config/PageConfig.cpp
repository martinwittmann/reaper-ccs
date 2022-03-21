#include <string>
#include "PageConfig.h"

namespace CCS {

  PageConfig::PageConfig(std::string baseDir) : BaseConfig(
    baseDir,
    true,
    std::string("extends")
  ) {
    replaceVariables();
  }
}