#include <string>
#include <vector>
#include "csurf.h"
#include "MidiControlElement.cpp"
#include "yaml-cpp/yaml.h"
#include "Util.h"
#include "MidiEventType.cpp"
#include "config/MidiControllerConfig.cpp"

namespace CCS {

  using std::string;
  using std::vector;

  /**
   * This represents a midi controller
   */
  class MidiController {
    string id;
    string name;
    int midiDeviceId;
    midi_Output *midiOutput;
    vector<MidiControlElement *> controls;
    MidiControllerConfig *config;
    unsigned int defaultStatusByte;

  public:
    MidiController(
      string configFilename,
      int deviceId
      //midi_Output *output
    ) {
      this->config = new MidiControllerConfig(configFilename);
      id = config->getValue("id");
      name = config->getValue("name");
      //this->midiOutput = output;
      this->defaultStatusByte = Util::hexToInt(config->getValue("default_status"));
      initializeControls();
    }

    ~MidiController() {
      for (auto it = controls.begin(); it != controls.end(); ++it) {
        delete *it;
      }
    }

    void initializeControls() {
      YAML::Node controlConfigs = config->getMapValue("controls");

      for (const auto &item: controlConfigs) {
        auto id = item.first.as<string>();
        YAML::Node controlNode = item.second;
        unsigned int status = defaultStatusByte;
        if (config->keyExists("status", &controlNode)) {
          status = Util::hexToInt(config->getValue("status", &controlNode));
        }

        auto controlElement = new MidiControlElement(
          config->getValue("type", &controlNode),
          status,
          Util::hexToInt(config->getValue("data1", &controlNode))
        );
        controls.push_back(controlElement);
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

    vector<MidiEventType> getMidiEventTypes() {
      vector<MidiEventType> result;
      for (auto it = controls.begin(); it != controls.end(); ++it) {
        MidiControlElement *element = *it;
        MidiEventType event = element->getEventType();
        result.push_back(event);
      }
      return result;
    }
  };
}