#include <string>
#include "BaseConfig.h"

namespace CCS {
  using std::string;

  class SessionConfig : public BaseConfig {
  public:
    SessionConfig(string baseDir) : BaseConfig(baseDir, false) {
    }
  };
}