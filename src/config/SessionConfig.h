#include <string>
#include "BaseConfig.h"

namespace CCS {

  class SessionConfig : public BaseConfig {
  public:
    SessionConfig(std::string baseDir);
  };
}
