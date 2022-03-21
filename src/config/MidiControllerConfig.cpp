#include <string>
#include <vector>
#include "BaseConfig.h"

namespace CCS {
  using std::string;
  using std::vector;

  class MidiControllerConfig : public BaseConfig {
  public:
    MidiControllerConfig(string baseDir) : BaseConfig(
      baseDir,
      true,
      "extends"
    ) {
      replaceVariables();
    }
  };
}