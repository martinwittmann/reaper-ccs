#include <string>
#include "BaseConfig.h"

namespace CCS {

  class PageConfig : public BaseConfig {
  public:
    PageConfig(std::string baseDir) : BaseConfig(
      baseDir,
      true,
      std::string("extends")
    ) {
      replaceVariables();
    }
  };
}