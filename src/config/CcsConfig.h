#ifndef CCS_CONFIG_H_
#define CCS_CONFIG_H_

#include <string>
#include "yaml-cpp/yaml.h"
#include <experimental/filesystem>
#include "../globals.cpp"
#include "../CcsUtil.h"

namespace fse = std::experimental::filesystem;
namespace fs = std::filesystem;

using std::string;
using std::vector;

class CcsConfig {

private:
    string filename;
    YAML::Node yaml;
    bool allowExtendConfig;
    string extendKeyName;
    const char keySeparator = '/';

public:
    CcsConfig(string filename, bool allowExtendConfig, string extendKeyName = "extend");
    YAML::Node loadConfig(string filename);
    map<string,string> getVariables(YAML::Node &variablesNode);
    void replaceVariables();
    void replaceVariables(YAML::Node &yaml, map<string,string> variables);
    void mergeYaml(YAML::Node target, YAML::Node const& source);
    bool keyExists(string key);
    bool keyExists(string key, YAML::Node* rootNode);
    string getValue(string key);
    string getValue(string key, YAML::Node* rootNode);
    vector<string> getListValues(string key);
    vector<string> getListValues(string key, YAML::Node* rootNode);
    YAML::Node getMapValue(string key);
    YAML::Node getMapValue(string key, YAML::Node* rootNode);
    YAML::Node _getValue(vector<string> keyParts, YAML::Node node);
    vector<string> debugNode(YAML::Node node);
    void mergeYamlNode(YAML::Node target, YAML::Node const& source);
    void mergeYamlMap(YAML::Node target, YAML::Node const& source);
    void mergeYamlSequences(YAML::Node target, YAML::Node const& source);
};

#endif