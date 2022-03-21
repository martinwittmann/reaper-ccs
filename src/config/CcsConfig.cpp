#include <string>
#include "yaml-cpp/yaml.h"
#include<experimental/filesystem>
#include "../CcsUtil.h"

namespace fse = std::experimental::filesystem;
namespace fs = std::filesystem;
using std::string;
using std::vector;

const string YAML_EXT = ".yml";

class CcsConfig {

private:
    string filename;
    YAML::Node yaml;
    bool allowExtendConfig;
    string extendKeyName;

public:
    CcsConfig(
      string filename,
      bool allowExtendConfig,
      string extendKeyName = "extend"
    ) {
      this->filename = filename;
      this->allowExtendConfig = allowExtendConfig;
      this->extendKeyName = extendKeyName;
      yaml = loadConfig(filename);
    }

    YAML::Node loadConfig(string filename) {
      fse::path path = fse::path(filename);
      YAML::Node tmpConfig = YAML::LoadFile(filename);
      if (!allowExtendConfig || !tmpConfig[extendKeyName].IsDefined()) {
        return tmpConfig;
      }

      string templateFilename = path.parent_path().string() + fse::path::preferred_separator + tmpConfig[extendKeyName].as<string>() + YAML_EXT;
      // Note that we're also using loadConfig for the template config to allow
      // multiple levels of extending/inheritance.
      YAML::Node result = loadConfig(templateFilename);
      mergeYaml(result, tmpConfig);

      return result;
    }

    map<string,string> getVariables(YAML::Node &variablesNode) {
      map<string,string> result;
      for (const auto& item : variablesNode) {
        auto key = item.first.as<string>();
        auto value = item.second.as<string>();
        result.insert(std::pair(key, value));
      }
      return result;
    }

    void replaceVariables() {
      YAML::Node variablesNode = yaml["variables"];
      auto variables = getVariables(variablesNode);
      replaceVariables(yaml, variables);
    }

    void replaceVariables(YAML::Node &yaml, map<string,string> variables) {
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
          /*
          for (YAML::iterator it = yaml.begin(); it != yaml.end(); ++it) {
            const YAML::Node &item = *it;
          }
          */
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
    void mergeYaml(YAML::Node target, YAML::Node const& source) {
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

    string getValue(string key, const char separator = '/') {
      vector<string> keyParts = CcsUtil::splitString(key, &separator);
      YAML::Node actualNode = _getValue(keyParts, yaml);
      //vector<string> ff = debugNode(actualNode);
      return actualNode[keyParts.back()].as<string>();
    }

    YAML::Node _getValue(vector<string> keyParts, YAML::Node node) {
      if (keyParts.size() == 1) {
        return node;
      }
      string firstKey = keyParts.at(0);
      keyParts.erase(keyParts.begin());
      return _getValue(keyParts, node[firstKey]);
    }

    vector<string> debugNode(YAML::Node node) {
      vector<string> result;
      for (const auto& item : node) {
        result.push_back(item.first.as<string>());
      }
      return result;
    }

    void mergeYamlNode(YAML::Node target, YAML::Node const& source) {
      mergeYaml(target, source);
    }
    void mergeYamlMap(YAML::Node target, YAML::Node const& source) {
      for (auto const& j : source) {
        mergeYamlNode(target[j.first.Scalar()], j.second);
      }
    }
    void mergeYamlSequences(YAML::Node target, YAML::Node const& source) {
      for (std::size_t i = 0; i != source.size(); ++i) {
        if (i < target.size()) {
          mergeYamlNode(target[i], source[i]);
        }
        else {
          target.push_back(YAML::Clone(source[i]));
        }
      }
    }
};
