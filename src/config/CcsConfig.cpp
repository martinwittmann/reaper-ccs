#include <string>
#include "yaml-cpp/yaml.h"
#include <experimental/filesystem>
#include "../globals.cpp"
#include "../CcsUtil.h"
#include "CcsConfig.h"

namespace fse = std::experimental::filesystem;
namespace fs = std::filesystem;

using std::string;
using std::vector;

CcsConfig::CcsConfig(string filename, bool allowExtendConfig, string extendKeyName) {
  this->filename = filename;
  this->allowExtendConfig = allowExtendConfig;
  this->extendKeyName = extendKeyName;
  yaml = loadConfig(filename);
}

YAML::Node CcsConfig::loadConfig(string filename) {
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

map<string,string> CcsConfig::getVariables(YAML::Node &variablesNode) {
  map<string,string> result;
  for (const auto& item : variablesNode) {
    auto key = item.first.as<string>();
    auto value = item.second.as<string>();
    result.insert(std::pair(key, value));
  }
  return result;
}

void CcsConfig::replaceVariables() {
  YAML::Node variablesNode = yaml["variables"];
  auto variables = getVariables(variablesNode);
  replaceVariables(yaml, variables);
}

void CcsConfig::replaceVariables(YAML::Node &yaml, map<string,string> variables) {
  switch (yaml.Type()) {
    case YAML::NodeType::Scalar:
      // Actually replace variables.
      yaml = CcsUtil::processString(yaml.as<string>(), variables);
      break;
    case YAML::NodeType::Map:
      // For maps we simply recurse.
      for (const auto& item : yaml) {
        auto node = item.second;
        string dd = item.first.as<string>();
        replaceVariables(node, variables);
      }
      break;
    case YAML::NodeType::Sequence:
      for (const auto& item : yaml) {
        auto node = item;
        replaceVariables(node, variables);
      }
      break;
    default:
      CcsUtil::logError("CcsConfig::mergeYaml: Unknown source node type");
  }
}

// Copied from https://github.com/coryan/jaybeams/blob/master/jb/merge_yaml.cpp
void CcsConfig::mergeYaml(YAML::Node target, YAML::Node const& source) {
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
      CcsUtil::logError("CcsConfig::mergeYaml: Unknown source node type");
  }
}

bool CcsConfig::keyExists(string key) {
  vector<string> keyParts = CcsUtil::splitString(key, &keySeparator);
  YAML::Node actualNode = _getValue(keyParts, yaml);
  return actualNode[keyParts.back()];
}

bool CcsConfig::keyExists(string key, YAML::Node* rootNode) {
  vector<string> keyParts = CcsUtil::splitString(key, &keySeparator);
  YAML::Node actualNode = _getValue(keyParts, *rootNode);
  return actualNode[keyParts.back()];
}

string CcsConfig::getValue(string key) {
  vector<string> keyParts = CcsUtil::splitString(key, &keySeparator);
  YAML::Node actualNode = _getValue(keyParts, yaml);
  return actualNode[keyParts.back()].as<string>();
}

string CcsConfig::getValue(string key, YAML::Node* rootNode) {
  vector<string> keyParts = CcsUtil::splitString(key, &keySeparator);
  YAML::Node actualNode = _getValue(keyParts, *rootNode);
  return actualNode[keyParts.back()].as<string>();
}

vector<string> CcsConfig::getListValues(string key) {
  vector<string> keyParts = CcsUtil::splitString(key, &keySeparator);
  YAML::Node actualNode = _getValue(keyParts, yaml);
  return actualNode[keyParts.back()].as<vector<string>>();
}

vector<string> CcsConfig::getListValues(string key, YAML::Node* rootNode) {
  vector<string> keyParts = CcsUtil::splitString(key, &keySeparator);
  YAML::Node actualNode = _getValue(keyParts, *rootNode);
  return actualNode[keyParts.back()].as<vector<string>>();
}

YAML::Node CcsConfig::getMapValue(string key) {
  vector<string> keyParts = CcsUtil::splitString(key, &keySeparator);
  YAML::Node actualNode = _getValue(keyParts, yaml);
  return actualNode[keyParts.back()];
}

YAML::Node CcsConfig::getMapValue(string key, YAML::Node* rootNode) {
  vector<string> keyParts = CcsUtil::splitString(key, &keySeparator);
  YAML::Node actualNode = _getValue(keyParts, *rootNode);
  return actualNode[keyParts.back()];
}

YAML::Node CcsConfig::_getValue(vector<string> keyParts, YAML::Node node) {
  if (keyParts.size() == 1) {
    return node;
  }
  string firstKey = keyParts.at(0);
  keyParts.erase(keyParts.begin());
  return _getValue(keyParts, node[firstKey]);
}

vector<string> CcsConfig::debugNode(YAML::Node node) {
  vector<string> result;
  for (const auto& item : node) {
    result.push_back(item.first.as<string>());
  }
  return result;
}

void CcsConfig::mergeYamlNode(YAML::Node target, YAML::Node const& source) {
  mergeYaml(target, source);
}
void CcsConfig::mergeYamlMap(YAML::Node target, YAML::Node const& source) {
  for (auto const& j : source) {
    mergeYamlNode(target[j.first.Scalar()], j.second);
  }
}
void CcsConfig::mergeYamlSequences(YAML::Node target, YAML::Node const& source) {
  for (std::size_t i = 0; i != source.size(); ++i) {
    if (i < target.size()) {
      mergeYamlNode(target[i], source[i]);
    }
    else {
      target.push_back(YAML::Clone(source[i]));
    }
  }
}
