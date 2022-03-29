#ifndef CCS_COMPOSITE_ACTION_CONFIG_H
#define CCS_COMPOSITE_ACTION_CONFIG_H

#include "BaseConfig.h"
#include "yaml-cpp/yaml.h"


namespace CCS {
  class CompositeActionConfig : public BaseConfig {

  public:
    CompositeActionConfig(YAML::Node *config);
  };
}


#endif
