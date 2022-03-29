#ifndef VARIABLES_H
#define VARIABLES_H

#include "yaml-cpp/yaml.h"

namespace CCS {
  class Variables {
  public:

    static std::map<std::string, std::string> getVariables(YAML::Node &variablesNode);

    static std::string replaceVariables(
      std::string input,
      std::map<std::string,std::string> variables,
      std::string type = "default"
    );

    static void replaceVariables(YAML::Node &yaml, std::map<std::string,std::string> variables);
  };
}

#endif