#include <string>
#include <vector>
#include "csurf.h"
#include "MidiControlElement.h"
#include "yaml-cpp/yaml.h"
#include "Util.h"
#include "MidiEventType.cpp"
#include "config/MidiControllerConfig.h"
#include "MidiController.h"
#include "globals.cpp"
#include "actions/ActionProvider.h"
#include "actions/Actions.h"

namespace CCS {

  using std::string;
  using std::vector;

  MidiController::MidiController(
    string configFilename,
    int deviceId,
    Actions* actionsManager
    //midi_Output *output
  ) {
    config = new MidiControllerConfig(configFilename);
    id = config->getValue("id");
    name = config->getValue("name");
    actionProvider = new ActionProvider(id, actionsManager);

    //this->midiOutput = output;
    this->defaultStatusByte = Util::hexToInt(config->getValue("default_status"));
    initializeControls();
  }

  MidiController::~MidiController() {
    delete config;
    for (auto it = controls.begin(); it != controls.end(); ++it) {
      delete *it;
    }
  }

  void MidiController::initializeControls() {
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
        Util::hexToInt(config->getValue("message.data1", &controlNode))
      );
      controls.push_back(controlElement);
    }
  }

  bool MidiController::isMidiControllerConfigFile(fse::path path) {
    string filename = path.filename();
    filename.length();
    // Note that we ignore files starting with _ here. This allows us to use
    // those as templates to inherit from, like _default.yml.
    if (filename.starts_with(".") || filename.starts_with("_") || !filename.ends_with(YAML_EXT)) {
      return false;
    }
    return is_regular_file(path);
  }

  vector<MidiEventType> MidiController::getMidiEventTypes() {
    vector<MidiEventType> result;
    for (auto it = controls.begin(); it != controls.end(); ++it) {
      MidiControlElement *element = *it;
      MidiEventType event = element->getEventType();
      result.push_back(event);
    }
    return result;
  }

  vector<Action> getActions() {

  }
}