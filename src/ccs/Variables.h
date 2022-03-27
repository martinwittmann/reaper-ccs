#ifndef VARIABLES_H
#define VARIABLES_H

#include "yaml-cpp/yaml.h"

namespace CCS {
  class Variables {
  public:

    static std::map<std::string, std::string> getVariables(YAML::Node &variablesNode);

    static std::map<std::string, std::string> getVariables(
      std::vector<std::string> argumentNames
    );

    static std::string replaceVariables(std::string, std::map<std::string,std::string> variables);

    static void replaceVariables(YAML::Node &yaml, std::map<std::string,std::string> variables);

    /*
    static std::string replaceArgumentVariables(
      YAML::Node &yaml,
      std::map<std::string,std::string> variables
    );
    */
  };
}

#endif