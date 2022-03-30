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
#include "../reaper/reaper_plugin_functions.h"
#include <fstream>

namespace CCS {

  using std::string;
  using std::vector;
  using std::map;
  using std::stoul;

  unsigned char Util::hexToInt(string hex) {
    return stoul(hex, nullptr, 16);
  }

  bool Util::_compareVariableLength(string var1, string var2) {
    return var1.length() > var2.length();
  }

  vector<string> Util::getUsedVariables(string input, string pattern) {
    std::regex variables_regex = std::regex(pattern);
    vector<string> result;
    // Find all variables used in this string.
    auto matches = std::sregex_iterator(
      input.begin(),
      input.end(),
      variables_regex
    );
    for (std::sregex_iterator it = matches; it != std::sregex_iterator(); ++it) {
      std::smatch match = *it;
      result.push_back(it->str());
    }
    return result;
  }

  string Util::processString(string raw, map<string, string> variables, string pattern) {
    vector<string> used_variables = Util::getUsedVariables(raw, pattern);

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

  bool Util::regexMatch(string input, string pattern) {
    std::regex regex = std::regex(pattern);
    // We need to use regex_search, since regex_match only returns true if the
    // whole input string matches.
    return std::regex_search(input, regex);
  }

  string Util::cleanId(string input) {
    boost::algorithm::to_lower(input);
    string result = Util::regexReplace(input, "[^a-z0-9]+", "_");
    result = Util::regexReplace(result, "_+", "_");
    result = Util::regexReplace(result, "^_", "");
    result = Util::regexReplace(result, "_$", "");
    return result;
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
      stream << boost::format("%2x") % boost::io::group(std::setw(2), std::setfill('0'), int(input[i]));
      result += stream.str();
      // stream.clear() does not what I expected. See:
      // https://stackoverflow.com/questions/20731/how-do-you-clear-a-stringstream-variable
      stream.str(std::string());
    }
    return result;
  }

  string Util::formatHexByte(unsigned char byte) {
    std::stringstream stream;
    stream << boost::format("%2x") % boost::io::group(std::setw(2), std::setfill('0'), int(byte));
    return stream.str();
  }

  int Util::getMidiEventId(unsigned char statusByte, unsigned char data1Byte) {
    // We create a larger integer by simply shifting the status byte 8 bits to
    // the left and then adding the data1 bytes to the 8 rightmost bits.
    int result = statusByte;
    result = result << 8;
    result = result | data1Byte;
    return result;
  }

  short Util::get7BitValue(double value, double minValue, double maxValue) {
    double range = maxValue - minValue;
    int steps = 127;
    double stepSize = range / steps;
    short result = value / stepSize;
    return result;
  }

  double Util::getParamValueFrom7Bit(short value, double minValue, double maxValue) {
    double range = maxValue - minValue;
    int steps = 127;
    double stepSize = range / steps;
    return double(value) * stepSize;
  }

  std::string Util::byteToHex(unsigned char byte) {
    return Util::formatHexByte(byte);
  }

  void Util::log(string message, bool lineBreak) {
    std::cout << message;
    if (lineBreak) {
      std::cout << "\n";
    }
  }

  void Util::log(int message, bool lineBreak) {
    Util::log(std::to_string(message), lineBreak);
  }

  void Util::log(char message, bool lineBreak) {
    Util::log(std::to_string(message), lineBreak);
  }

  void Util::log(short message, bool lineBreak) {
    Util::log(std::to_string(message), lineBreak);
  }

  void Util::log(double message, bool lineBreak) {
    Util::log(std::to_string(message), lineBreak);
  }

  void Util::error(string message, bool lineBreak) {
    message = "CCS: " + message;
    if (lineBreak) {
      message += "\n";
    }
    ShowConsoleMsg(message.c_str());
  }

  void Util::debugYaml(YAML::Node node) {
    YAML::Emitter emitter;
    emitter << node;
    string filename = "/tmp/witti.yml";
    std::ofstream file(filename);
    file << emitter.c_str();
  }
}