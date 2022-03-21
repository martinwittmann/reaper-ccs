#include <string>
#include <vector>
#include "BaseConfig.h"

namespace CCS {

  using std::string;
  using std::vector;

  class GlobalConfig : public BaseConfig {
  public:
    GlobalConfig(string baseDir) : BaseConfig(baseDir, false) {}

    string getLastSessionId() {
      return getValue("last_session");
    }

    vector<string> getSessionMidiControllers() {
      return getListValues("required_midi_controllers");
    }
  };
}