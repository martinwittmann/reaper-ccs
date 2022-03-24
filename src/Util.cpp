#include <string>
#include <map>
#include <regex>
#include "yaml-cpp/yaml.h"
#include "boost/algorithm/string.hpp"
#include <regex>
#include "Util.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "boost/format.hpp"
#include "boost/format/group.hpp"
#include <iomanip>

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

  string Util::processString(string raw, map<string, string> variables, string pattern) {
    std::regex variables_regex = std::regex(pattern);
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
    // variables starting with the name of other variables.
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

  vector<string> Util::splitString(string &input, char delimiter) {
    vector<string> result;
    string tmp = "";
    for (char const &current: input) {
      if (current == delimiter) {
        result.push_back(tmp);
        tmp = "";
      }
      else {
        tmp += string(1, current);
      }
    }

    // Note that we add tmp unconditionally.
    // If it's not empty we need to add the last part, but if it is empty this
    // means that the last character was the delimiter and that we need to add
    // the empty item as expected.
    result.push_back(tmp);
    return result;

    // The implemenation copied from the internet seemed to introduce
    // non-deterministic behavior.
    // Let's stick with our own naive implementation for now.
    // TODO Use a good implementation for this.
  }

  string Util::regexReplace(string input, string pattern, string replacement) {
    std::regex regexExpression = std::regex(pattern);
    return std::regex_replace(input, regexExpression, replacement);
  }

  string Util::joinStrVector(vector<string> input, char separator) {
    string result;
    for (auto it = input.begin(); it != input.end(); ++it) {
      if (it != input.begin()) {
        result += std::string(1, separator);
      }
      result += *it;
    }
    return result;
  }

  string Util::removePrefixSuffix(string input) {
    string result = Util::regexReplace(input, "^[\"\\[]+", "");
    result = Util::regexReplace(result, "[\"\\]]+$", "");
    return result;
  }


  void Util::debug(std::string message) {
    std::cout << "[DEBUG] " + message + "\n";
  }

  void Util::error(std::string message) {
    std::cout << "[ERROR] " + message + "\n";
  }

  vector<unsigned char> Util::splitToBytes(string &input) {
    input = Util::regexReplace(input, "\\s+", "");
    vector<unsigned char> result;
    string tmp = "";
    for (int i = 0; i < input.length(); ++i) {
      if (i % 2 == 1) {
        string byte = input.substr(i - 1, 2);
        result.push_back(std::stoi(byte, 0, 16));
      }
    }
    return result;
  }

  string Util::strToHexBytes(std::string input) {
    string result;
    std::stringstream stream;
    for (int i = 0; i < input.length(); ++i) {
      if (i != 0) {
        result += " ";
      }
      stream << boost::format("%x") % int(input[i]);
      result += stream.str();
      // stream.clear() does not what I expected. See:
      // https://stackoverflow.com/questions/20731/how-do-you-clear-a-stringstream-variable
      stream.str(std::string());
    }
    return result;
  }

  void Util::debugMidiBuffer(std::vector<unsigned char>* buffer) {
    std::cout << "[MIDI OUTPUT]\n";
    for (auto it = buffer->begin(); it != buffer->end(); ++it) {
      if (it != buffer->begin()) {
        std::cout << " ";
      }
      std::cout << boost::format("%2x") % boost::io::group(std::setw(2), std::setfill('0'), int(*it)) ;
    }
    std::cout << "\n";
  }
}