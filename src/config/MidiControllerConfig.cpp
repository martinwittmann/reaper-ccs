#include <string>
#include <vector>
#include "BaseConfig.h"

namespace CCS {

  class MidiControllerConfig : public BaseConfig {
  public:
    MidiControllerConfig(std::string baseDir) : BaseConfig(
      baseDir,
      true,
      "extends"
    ) {
      replaceVariables();
    }
  };
}