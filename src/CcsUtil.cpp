#include <string>
#include <map>
#include <regex>
#include "yaml-cpp/yaml.h"
#include "boost/algorithm/string.hpp"
#include "CcsUtil.h"
#include <iostream>
#include <vector>

using std::string;
using std::map;

unsigned int CcsUtil::hexToInt(string hex) {
  return std::stoul(hex, nullptr, 16);
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
    string variableName = variable.substr(1);
    map<string,string>::iterator variableIt = variables.find(variableName);
    if (variableIt != variables.end()) {
      string value = variables.at(variableName);
      boost::replace_all(result, variable, value);
    }
  }
  return result;
}

void CcsUtil::logError(string message) {
  std::cout << "[ERROR] " << message << "\n";
}

std::vector<string> CcsUtil::splitString(string &input, const char* delimiter) {
  std::vector<string> result;
  char *token = strtok(const_cast<char*>(input.c_str()), delimiter);
  while (token != nullptr) {
    string part = std::string(token);
    result.push_back(part);
    token = strtok(nullptr, delimiter);
  }
  return result;
}