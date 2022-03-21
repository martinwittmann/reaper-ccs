#include <string>
#include "BaseConfig.h"

namespace CCS {
  using std::string;

  class PageConfig : public BaseConfig {
  public:
    PageConfig(string baseDir) : BaseConfig(
      baseDir,
      true,
      "extends"
    ) {
      replaceVariables();
    }
  };
}