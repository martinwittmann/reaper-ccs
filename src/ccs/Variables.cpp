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
      string value;
      if (item.second && item.second.Type() != YAML::NodeType::Null) {
        value = item.second.as<string>();
      }
      result.insert(std::pair(key, value));
    }
    return result;
  }

  string Variables::replaceVariables(
    string input,
    map<string,string> variables,
    string type
  ) {
    if (variables.empty()) {
      return input;
    }

    if (type == "state") {
      return Util::processString(input, variables, "\\$(_STATE\\.)?[A-Z0-9_]+!?");
    }
    else {
      return Util::processString(input, variables, "\\$(_ARGS\\.)?[A-Z0-9_]+!?");
    }
  }

  void Variables::replaceVariables(YAML::Node &yaml, map <string, string> variables) {
    if (variables.empty()) {
      return;
    }
    switch (yaml.Type()) {
      case YAML::NodeType::Scalar:
        // Actually replace variables.
        yaml = Util::processString(yaml.as<string>(), variables);
        break;
      case YAML::NodeType::Map:
        // For maps we simply recurse.
        for (const auto &item: yaml) {
          auto node = item.second;
          if (node && node.Type() != YAML::NodeType::Null) {
            replaceVariables(node, variables);
          }
        }break;
      case YAML::NodeType::Sequence:
        for (const auto &item: yaml) {
          auto node = item;
          replaceVariables(node, variables);
        }
        break;
      default:
        string message = "Variables::replaceVariables: Unknown source node type";
        Util::error(message);
        throw message;
    }
  }
}