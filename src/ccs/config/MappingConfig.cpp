#include <string>
#include <vector>
#include "MappingConfig.h"
#include "yaml-cpp/yaml.h"

namespace CCS {

  MappingConfig::MappingConfig(YAML::Node *root) : BaseConfig(root) {}
}
