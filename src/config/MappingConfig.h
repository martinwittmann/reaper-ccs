#ifndef CCS_MAPPING_CONFIG_H
#define CCS_MAPPING_CONFIG_H

#include <string>
#include <vector>
#include "BaseConfig.h"
#include "yaml-cpp/yaml.h"

namespace CCS {

  class MappingConfig : public BaseConfig {
  public:
    explicit MappingConfig(YAML::Node* root);
  };
}

#endif
