#include <string>
#include <vector>
#include "BaseConfig.h"

namespace CCS {

  class GlobalConfig : public BaseConfig {
  public:
    GlobalConfig(std::string baseDir) : BaseConfig(baseDir, false) {}

    std::string getLastSessionId() {
      return getValue("last_session");
    }

    std::vector<std::string> getSessionMidiControllers() {
      return getListValues("required_midi_controllers");
    }
  };
}