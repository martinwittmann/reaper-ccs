#include <string>
#include <map>
#include <regex>
#include "yaml-cpp/yaml.h"
#include "boost/algorithm/string.hpp"
#include "CcsUtil.h"

using std::string;
using std::map;

unsigned int CcsUtil::hexToInt(string hex) {
  return std::stoul(hex, nullptr, 16);
}

map<string,string> CcsUtil::getVariables(YAML::Node &variablesNode) {
  map<string,string> result;
  for (const auto& item : variablesNode) {
    auto key = item.first.as<string>();
    auto value = item.second.as<string>();
    result.insert(std::pair(key, value));
  }
  return result;
}

bool CcsUtil::_compareVariableLength(string var1, string var2) {
  return var1.length() > var2.length();
}

string CcsUtil::processString(string raw, map<string,string> variables) {
  std::regex variables_regex = std::regex("\\$[A-Z0-9_]+");
  // Find all variables used in this string.
  auto matches = std::sregex_iterator(
    raw.begin(),
    raw.end(),
    variables_regex
  );
  std::vector<string> used_variables;
  for (std::sregex_iterator it = matches; it != std::sregex_iterator(); ++it) {
    std::smatch match = *it;
    used_variables.push_back(it->str());
  }

  if (used_variables.empty()) {
    // There's nothing to do if no variables are used.
    return raw;
  }

  // We need replace longer variables first because there could be
  // variables starting the the name of other variables.
  // Example: $LABEL = "123", $LABEL_BUTTON = "Click";
  // If we would handle $LABEL before $LABEL_BUTTON we would do something
  // the user does not expect and mess up the string.
  std::sort(used_variables.begin(), used_variables.end(), CcsUtil::_compareVariableLength);
  string result = raw;

  for (auto it = used_variables.begin(); it != used_variables.end(); ++it) {
    string variable = *it;
    // We need to remove the $ when looking up the value.
    string value1 = variables.at(variable.substr(1));
    boost::replace_all(result, variable, value1);
  }
  return result;
}

// Copied from https://github.com/coryan/jaybeams/blob/master/jb/merge_yaml.cpp
void CcsUtil::mergeYaml(YAML::Node target, YAML::Node const& source) {
  switch (source.Type()) {
    case YAML::NodeType::Scalar:
      target = source.Scalar();
      break;
    case YAML::NodeType::Map:
      CcsUtil::_mergeYamlMap(target, source);
      break;
    case YAML::NodeType::Sequence:
      CcsUtil::_mergeYamlSequences(target, source);
      break;
  }
}

void CcsUtil::_mergeYamlNode(YAML::Node target, YAML::Node const& source) {
  CcsUtil::mergeYaml(target, source);
}

void CcsUtil::_mergeYamlMap(YAML::Node target, YAML::Node const& source) {
  for (auto const& j : source) {
    CcsUtil::_mergeYamlNode(target[j.first.Scalar()], j.second);
  }
}

void CcsUtil::_mergeYamlSequences(YAML::Node target, YAML::Node const& source) {
  for (std::size_t i = 0; i != source.size(); ++i) {
    if (i < target.size()) {
      CcsUtil::_mergeYamlNode(target[i], source[i]);
    }
    else {
      target.push_back(YAML::Clone(source[i]));
    }
  }
}