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
    YAML::Node config;
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
      config = loadConfig(filename);
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

    void replaceVariables(YAML::Node &config, map<string,string> variables) {
      switch (config.Type()) {
        case YAML::NodeType::Scalar:
          // Actually replace variables.
          config = CcsUtil::processString(config.as<string>(), variables);
          break;
        case YAML::NodeType::Map:
          // For maps we simply recurse.
          for (const auto& item : config) {
            auto node = item.second;
            replaceVariables(node, variables);
          }
          break;
        case YAML::NodeType::Sequence:
          for (const auto& item : config) {
            auto node = item.second;
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
      YAML::Node* actualNode = _getValue(keyParts, &config);
      return "sdf";
      //return actualNode->as<string>();
    }

    YAML::Node* _getValue(vector<string> keyParts, YAML::Node *node) {
      return node;
      /*
      if (keyParts.size() == 1) {
        return node;
      }
      string firstKey = keyParts.at(0);
      keyParts.erase(keyParts.begin());
      // TODO Find a cleaner way of doing this.
      YAML::Node hui = *node;
      auto newNode = hui[firstKey];
      return _getValue(keyParts, &newNode);
      */
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
