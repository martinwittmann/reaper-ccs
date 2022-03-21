#include <string>
#include <vector>
#include "CcsConfig.h"

using std::string;
using std::vector;

class MidiControllerConfig : public CcsConfig {
public:
    MidiControllerConfig(string baseDir) : CcsConfig(
      baseDir,
      true,
      "extends"
    ) {
      replaceVariables();
    }

    vector<YAML::Node> getControls2() {
      for (const auto& item : getMapValue("controls")) {
        auto key = item.first.as<string>();
        unsigned int status = defaultStatusByte;
        YAML::Node control = item.second;
        if (control["status"]) {
          status = CcsUtil::hexToInt(control["status"].as<string>());
        }
    }
};