#include <string>
#include <vector>
#include "CcsConfig.h"

using std::string;
using std::vector;

class GlobalConfig : public CcsConfig {
public:
    GlobalConfig(string baseDir) : CcsConfig(baseDir, false) {
    }

    string getLastSessionId() {
      return getValue("last_session");
    }

    vector<string> getSessionMidiControllers() {
      return getListValues("required_midi_controllers");
    }
};
