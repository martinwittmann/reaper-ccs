#include <string>
#include "yaml-cpp/yaml.h"
#include <experimental/filesystem>
#include "../globals.cpp"
#include "../Util.h"
#include "BaseConfig.h"

namespace CCS {

  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;

  using std::string;
  using std::vector;

  BaseConfig::BaseConfig(string filename, bool allowExtendConfig, string extendKeyName) {
    this->filename = filename;
    this->allowExtendConfig = allowExtendConfig;
    this->extendKeyName = extendKeyName;
    yaml = loadConfig(filename);
  }

  YAML::Node BaseConfig::loadConfig(string filename) {
    fse::path path = fse::path(filename);
    YAML::Node tmpConfig = YAML::LoadFile(filename);
    if (!allowExtendConfig || !tmpConfig[extendKeyName].IsDefined()) {
      return tmpConfig;
    }

    string templateFilename = path.parent_path().string() + SEP + tmpConfig[extendKeyName].as<string>() + YAML_EXT;
    // Note that we're also using loadConfig for the template config to allow
    // multiple levels of extending/inheritance.
    YAML::Node result = loadConfig(templateFilename);
    mergeYaml(result, tmpConfig);

    return result;
  }

  map <string, string> BaseConfig::getVariables(YAML::Node &variablesNode) {
    map <string, string> result;
    for (const auto &item: variablesNode) {
      auto key = item.first.as<string>();
      auto value = item.second.as<string>();
      result.insert(std::pair(key, value));
    }
    return result;
  }

  void BaseConfig::replaceVariables() {
    YAML::Node variablesNode = yaml["variables"];
    auto variables = getVariables(variablesNode);
    replaceVariables(yaml, variables);
  }

  void BaseConfig::replaceVariables(YAML::Node &yaml, map <string, string> variables) {
    switch (yaml.Type()) {
      case YAML::NodeType::Scalar:
        // Actually replace variables.
        yaml = Util::processString(yaml.as<string>(), variables);
        break;
      case YAML::NodeType::Map:
        // For maps we simply recurse.
        for (const auto &item: yaml) {
          auto node = item.second;
          string dd = item.first.as<string>();
          replaceVariables(node, variables);
        }
        break;
      case YAML::NodeType::Sequence:
        for (const auto &item: yaml) {
          auto node = item;
          replaceVariables(node, variables);
        }
        break;
      default:
        Util::logError("BaseConfig::mergeYaml: Unknown source node type");
    }
  }

  // Copied from https://github.com/coryan/jaybeams/blob/master/jb/merge_yaml.cpp
  void BaseConfig::mergeYaml(YAML::Node target, YAML::Node const &source) {
    switch (source.Type()) {
      case YAML::NodeType::Scalar:
        target = source.Scalar();
        break;
      case YAML::NodeType::Map:
        mergeYamlMap(target, source);
        break;
      case YAML::NodeType::Sequence:
        mergeYamlSequences(target, source);
        break;
      default:
        Util::logError("BaseConfig::mergeYaml: Unknown source node type");
    }
  }

  bool BaseConfig::keyExists(string key) {
    vector<string> keyParts = Util::splitString(key, &keySeparator);
    YAML::Node actualNode = _getValue(keyParts, yaml);
    return actualNode[keyParts.back()];
  }

  bool BaseConfig::keyExists(string key, YAML::Node *rootNode) {
    vector<string> keyParts = Util::splitString(key, &keySeparator);
    YAML::Node actualNode = _getValue(keyParts, *rootNode);
    return actualNode[keyParts.back()];
  }

  string BaseConfig::getValue(string key) {
    vector<string> keyParts = Util::splitString(key, &keySeparator);
    YAML::Node actualNode = _getValue(keyParts, yaml);
    return actualNode[keyParts.back()].as<string>();
  }

  string BaseConfig::getValue(string key, YAML::Node *rootNode) {
    vector<string> keyParts = Util::splitString(key, &keySeparator);
    YAML::Node actualNode = _getValue(keyParts, *rootNode);
    return actualNode[keyParts.back()].as<string>();
  }

  vector<string> BaseConfig::getListValues(string key) {
    vector<string> keyParts = Util::splitString(key, &keySeparator);
    YAML::Node actualNode = _getValue(keyParts, yaml);
    return actualNode[keyParts.back()].as<vector<string>>();
  }

  vector<string> BaseConfig::getListValues(string key, YAML::Node *rootNode) {
    vector<string> keyParts = Util::splitString(key, &keySeparator);
    YAML::Node actualNode = _getValue(keyParts, *rootNode);
    return actualNode[keyParts.back()].as<vector<string>>();
  }

  YAML::Node BaseConfig::getMapValue(string key) {
    vector<string> keyParts = Util::splitString(key, &keySeparator);
    YAML::Node actualNode = _getValue(keyParts, yaml);
    return actualNode[keyParts.back()];
  }

  YAML::Node BaseConfig::getMapValue(string key, YAML::Node *rootNode) {
    vector<string> keyParts = Util::splitString(key, &keySeparator);
    YAML::Node actualNode = _getValue(keyParts, *rootNode);
    return actualNode[keyParts.back()];
  }

  YAML::Node BaseConfig::_getValue(vector<string> keyParts, YAML::Node node) {
    if (keyParts.size() == 1) {
      return node;
    }
    string firstKey = keyParts.at(0);
    keyParts.erase(keyParts.begin());
    return _getValue(keyParts, node[firstKey]);
  }

  vector<string> BaseConfig::debugNode(YAML::Node node) {
    vector<string> result;
    for (const auto &item: node) {
      result.push_back(item.first.as<string>());
    }
    return result;
  }

  void BaseConfig::mergeYamlNode(YAML::Node target, YAML::Node const &source) {
    mergeYaml(target, source);
  }

  void BaseConfig::mergeYamlMap(YAML::Node target, YAML::Node const &source) {
    for (auto const &j: source) {
      mergeYamlNode(target[j.first.Scalar()], j.second);
    }
  }

  void BaseConfig::mergeYamlSequences(YAML::Node target, YAML::Node const &source) {
    for (std::size_t i = 0; i != source.size(); ++i) {
      if (i < target.size()) {
        mergeYamlNode(target[i], source[i]);
      } else {
        target.push_back(YAML::Clone(source[i]));
      }
    }
  }
}