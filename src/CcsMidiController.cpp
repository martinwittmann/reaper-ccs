#include <string>
#include "csurf.h"
#include "CcsMidiControlElement.cpp"
#include "yaml-cpp/yaml.h"
#include "CcsUtil.cpp"

using namespace std;
using std::string;

/**
 * This represents a midi controller
 */
class CcsMidiController {
    string id;
    string name;
    int midiDeviceId;
    midi_Output *midiOutput;
    vector<CcsMidiControlElement*> controls;
    YAML::Node config;
    unsigned int defaultStatusByte;

public:
    CcsMidiController(
      string id,
      string name,
      int deviceId,
      midi_Output *output,
      YAML::Node config
    ) {
      this->id = id;
      this->name = name;
      this->midiOutput = output;
      this->config = config;
      this->defaultStatusByte = CcsUtil::hexToInt(config["default_status"].as<string>());
      initializeControls();
    }

    ~CcsMidiController() {
      for (auto it = controls.begin(); it != controls.end(); ++it) {
        delete *it;
      }
    }

    void initializeControls() {
      for (const auto& item : config["controls"]) {
        auto key = item.first.as<string>();
        unsigned int status = defaultStatusByte;
        YAML::Node control = item.second;
        if (control["status"]) {
          status = CcsUtil::hexToInt(control["status"].as<string>());
        }

        auto ccsControl = new CcsMidiControlElement(
          CcsMidiControlElement::getType(item.second["type"].as<string>()),
          status,
          CcsUtil::hexToInt(control["data1"].as<string>())
        );
        controls.push_back(ccsControl);
      }
    }
};