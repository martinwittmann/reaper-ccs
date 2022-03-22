#include <string>
#include <map>
#include <regex>
#include "yaml-cpp/yaml.h"
#include "boost/algorithm/string.hpp"
#include <regex>
#include "Util.h"
#include <iostream>
#include <vector>

namespace CCS {

  using std::string;
  using std::vector;
  using std::map;
  using std::stoul;

  unsigned int Util::hexToInt(string hex) {
    return stoul(hex, nullptr, 16);
  }

  bool Util::_compareVariableLength(string var1, string var2) {
    return var1.length() > var2.length();
  }

  string Util::processString(string raw, map<string, string> variables) {
    std::regex variables_regex = std::regex("\\$[A-Z0-9_]+");
    // Find all variables used in this string.
    auto matches = std::sregex_iterator(
      raw.begin(),
      raw.end(),
      variables_regex
    );
    vector<string> used_variables;
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
    std::sort(used_variables.begin(), used_variables.end(), Util::_compareVariableLength);
    string result = raw;

    for (auto it = used_variables.begin(); it != used_variables.end(); ++it) {
      string variable = *it;
      // We need to remove the $ when looking up the value.
      string variableName = variable.substr(1);
      map<string, string>::iterator variableIt = variables.find(variableName);
      if (variableIt != variables.end()) {
        string value = variables.at(variableName);
        boost::replace_all(result, variable, value);
      }
    }
    return result;
  }

  void Util::logError(string message) {
    std::cout << "[ERROR] " << message << "\n";
  }

  vector<string> Util::splitString(string &input, const char *delimiter) {
    vector<string> result;
    char *token = strtok(const_cast<char *>(input.c_str()), delimiter);
    while (token != nullptr) {
      string part = string(token);
      result.push_back(part);
      token = strtok(nullptr, delimiter);
    }
    return result;
  }

  string Util::regexReplace(string input, string pattern, string replacement) {
    std::regex regexExpression = std::regex(pattern);
    return std::regex_replace(input, regexExpression, replacement);
  }

  string Util::joinStrVector(vector<string> input, char separator) {
    string result;
    for (auto it = input.begin(); it != input.end(); ++it) {
      if (it != input.begin()) {
        result += std::string(separator);
      }
      result += *it;
    }
    return result;
  }
}