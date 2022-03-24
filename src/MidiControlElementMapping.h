#include <string>
#include <vector>
#include "yaml-cpp/yaml.h"
#include "MidiController.h"
#include "MidiEventSubscriber.h"

namespace CCS {
  class MidiControlElementMapping {
    YAML::Node config;
    MidiController* controller;
    std::string controlId;
    std::string controllerId;

  public:
    MidiControlElementMapping(
      std::string rawControlId,
      YAML::Node config,
      MidiController* controller,
      MidiEventSubscriber* subscriber
    );

    int getMidiEventId();
  };
}