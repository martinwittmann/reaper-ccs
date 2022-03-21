#include <string>
#include <vector>
#include "GlobalConfig.h"

namespace CCS {

  GlobalConfig::GlobalConfig(std::string baseDir) : BaseConfig(baseDir, false) {}

  std::string getLastSessionId() {
    return getValue("last_session");
  }

  std::vector<std::string> GlobalConfig::getSessionMidiControllers() {
    return getListValues("required_midi_controllers");
  };
}