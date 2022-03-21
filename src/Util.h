#ifndef PROJECTNAME_CCSUTIL_H
#define PROJECTNAME_CCSUTIL_H

#include <string>
#include <map>
#include <regex>
#include "yaml-cpp/yaml.h"
#include "boost/algorithm/string.hpp"

namespace CCS {
  class Util {
    YAML::Node config;
  public:
    static unsigned int hexToInt(std::string hex);

    static bool _compareVariableLength(std::string var1, std::string var2);

    static std::string processString(std::string raw, std::map<std::string, std::string> variables);

    static void logError(std::string message);

    static std::vector<std::string> splitString(std::string &input, const char *delimiter);
  };
}

#endif //PROJECTNAME_CCSUTIL_H
