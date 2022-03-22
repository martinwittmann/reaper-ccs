#include "Variables.h"
#include "Util.h"

namespace CCS {

  using std::string;
  using std::map;
  using std::vector;

  map <string, string> Variables::getVariables(YAML::Node &variablesNode) {
    map <string, string> result;
    for (const auto &item: variablesNode) {
      auto key = item.first.as<string>();
      auto value = item.second.as<string>();
      result.insert(std::pair(key, value));
    }
    return result;
  }

  string Variables::replaceVariables(string input, map<string,string> variables) {
    return Util::processString(input, variables);
  }

  void Variables::replaceVariables(YAML::Node &yaml, map <string, string> variables) {
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
        Util::logError("Variables::replaceVariables: Unknown source node type");
    }
  }
}