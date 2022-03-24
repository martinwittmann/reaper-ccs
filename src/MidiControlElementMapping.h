#include <string>
#include <vector>
#include "yaml-cpp/yaml.h"

namespace CCS {
  class MidiControlElementMapping {

  public:
    MidiControlElementMapping(std::string rawControlId, YAML::Node mapping);
  };
}