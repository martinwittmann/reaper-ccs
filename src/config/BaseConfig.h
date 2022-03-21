#ifndef CCS_CONFIG_H
#define CCS_CONFIG_H

#include <string>
#include <map>
#include <vector>
#include "yaml-cpp/yaml.h"

namespace CCS {

  class BaseConfig {
    std::string filename;
    YAML::Node yaml;
    bool allowExtendConfig;
    std::string extendKeyName;
    const char keySeparator = '.';

  public:
    BaseConfig(std::string filename, bool allowExtendConfig, std::string extendKeyName = "extends");

    YAML::Node loadConfig(std::string filename);

    std::map<std::string, std::string> getVariables(YAML::Node &variablesNode);

    void replaceVariables();

    void replaceVariables(YAML::Node &yaml, std::map<std::string, std::string> variables);

    void mergeYaml(YAML::Node target, YAML::Node const &source);

    bool keyExists(std::string key);

    bool keyExists(std::string key, YAML::Node *rootNode);

    std::string getValue(std::string key);

    std::string getValue(std::string key, YAML::Node *rootNode);

    std::vector<std::string> getListValues(std::string key);

    std::vector<std::string> getListValues(std::string key, YAML::Node *rootNode);

    YAML::Node getMapValue(std::string key);

    YAML::Node getMapValue(std::string key, YAML::Node *rootNode);

    YAML::Node _getValue(std::vector<std::string> keyParts, YAML::Node node);

    std::vector<std::string> debugNode(YAML::Node node);

    void mergeYamlNode(YAML::Node target, YAML::Node const &source);

    void mergeYamlMap(YAML::Node target, YAML::Node const &source);

    void mergeYamlSequences(YAML::Node target, YAML::Node const &source);
  };
}
#endif