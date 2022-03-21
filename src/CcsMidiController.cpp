#include <string>
#include <vector>
#include "csurf.h"
#include "CcsMidiControlElement.cpp"
#include "yaml-cpp/yaml.h"
#include "CcsUtil.h"
#include "CcsMidiEventType.cpp"
#include "config/MidiControllerConfig.cpp"

using namespace std;
using std::string;
using std::vector;

/**
 * This represents a midi controller
 */
class CcsMidiController {
    string id;
    string name;
    int midiDeviceId;
    midi_Output *midiOutput;
    vector<CcsMidiControlElement*> controls;
    MidiControllerConfig* config;
    unsigned int defaultStatusByte;

public:
    CcsMidiController(
      string configFilename,
      int deviceId
      //midi_Output *output
    ) {
      this->config = new MidiControllerConfig(configFilename);
      id = config->getValue("id");
      name = config->getValue("name");
      //this->midiOutput = output;
      this->defaultStatusByte = CcsUtil::hexToInt(config->getValue("default_status"));
      initializeControls();
    }

    ~CcsMidiController() {
      for (auto it = controls.begin(); it != controls.end(); ++it) {
        delete *it;
      }
    }

    void initializeControls() {
      YAML::Node controlConfigs = config->getMapValue("controls");

      for (const auto& item : controlConfigs) {
        auto id = item.first.as<string>();
        YAML::Node controlNode = item.second;
        unsigned int status = defaultStatusByte;
        if (config->keyExists("status", &controlNode)) {
          status = CcsUtil::hexToInt(config->getValue("status", &controlNode));
        }

        auto ccsControl = new CcsMidiControlElement(
          config->getValue("type", &controlNode),
          status,
          CcsUtil::hexToInt(config->getValue("data1", &controlNode))
        );
        controls.push_back(ccsControl);
      }
    }

    static bool isMidiControllerConfigFile(fse::path path) {
      string filename = path.filename();
      filename.length();
      // Note that we ignore files starting with _ here. This allows us to use
      // those as templates to inherit from, like _default.yml.
      if (filename.starts_with(".") || filename.starts_with("_") || !filename.ends_with(YAML_EXT)) {
        return false;
      }
      return is_regular_file(path);
    }

    vector<CcsMidiEventType> getMidiEventTypes() {
      vector<CcsMidiEventType> result;
      for (auto it = controls.begin(); it != controls.end(); ++it) {
        CcsMidiControlElement* element = *it;
        CcsMidiEventType event = element->getEventType();
        result.push_back(event);
      }
      return result;
    }
};