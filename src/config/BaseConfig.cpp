#include <string>
#include "yaml-cpp/yaml.h"
#include <experimental/filesystem>
#include "../globals.cpp"
#include "../Util.h"
#include "BaseConfig.h"
#include "../Variables.h"

namespace CCS {

  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;

  using std::string;
  using std::vector;
  using std::map;

  BaseConfig::BaseConfig(string filename, bool allowExtendConfig, string extendKeyName) :
    filename(filename),
    allowExtendConfig(allowExtendConfig),
    extendKeyName(extendKeyName)
  {
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

  void BaseConfig::replaceVariables() {
    YAML::Node variablesNode = yaml["variables"];
    auto variables = Variables::getVariables(variablesNode);
    Variables::replaceVariables(yaml, variables);
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
    vector<string> keyParts = Util::splitString(key, keySeparator);
    YAML::Node actualNode = _getValue(keyParts, yaml);
    return actualNode[keyParts.back()];
  }

  bool BaseConfig::keyExists(string key, YAML::Node *rootNode) {
    vector<string> keyParts = Util::splitString(key, keySeparator);
    YAML::Node actualNode = _getValue(keyParts, *rootNode);
    return actualNode[keyParts.back()];
  }

  string BaseConfig::getValue(string key) {
    return getValue(key, &yaml);
  }

  string BaseConfig::getValue(string key, YAML::Node *rootNode) {
    vector<string> keyParts = Util::splitString(key, keySeparator);
    YAML::Node actualNode = _getValue(keyParts, *rootNode);
    YAML::Node resultNode = actualNode[keyParts.back()];
    if (resultNode) {
      return resultNode.as<string>();
    }
    throw "Did not find values for key: '" + key + "'";
  }

  vector<string> BaseConfig::getListValues(string key) {
    vector<string> keyParts = Util::splitString(key, keySeparator);
    YAML::Node actualNode = _getValue(keyParts, yaml);
    try {
      return actualNode[keyParts.back()].as<vector<string>>();
    }
    catch (YAML::BadConversion &e) {
      string message = "getListValues: " + key;
      Util::debug(message);
      throw message;
    }
  }

  vector<string> BaseConfig::getListValues(string key, YAML::Node *rootNode) {
    vector<string> keyParts = Util::splitString(key, keySeparator);
    YAML::Node actualNode = _getValue(keyParts, *rootNode);
    if (actualNode[keyParts.back()]) {
      return actualNode[keyParts.back()].as<vector<string>>();
    }

    vector<string> result;
    return result;
  }

  YAML::Node BaseConfig::getMapValue(string key) {
    vector<string> keyParts = Util::splitString(key, keySeparator);
    YAML::Node actualNode = _getValue(keyParts, yaml);
    try {
      return actualNode[keyParts.back()];
    }
    catch (YAML::BadConversion &e) {
      string message = "getMapValue: " + key;
      Util::debug(message);
      throw message;
    }
  }

  YAML::Node BaseConfig::getMapValue(string key, YAML::Node *rootNode) {
    vector<string> keyParts = Util::splitString(key, keySeparator);
    YAML::Node actualNode = _getValue(keyParts, *rootNode);
    try {
      return actualNode[keyParts.back()];
    }
    catch (YAML::BadConversion &e) {
      string message = "getMapValue: " + key;
      Util::debug(message);
      throw message;
    }
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