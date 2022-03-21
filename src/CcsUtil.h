//
// Created by martin on 21.03.22.
//

#ifndef PROJECTNAME_CCSUTIL_H
#define PROJECTNAME_CCSUTIL_H

#include <string>
#include <map>
#include <regex>
#include "yaml-cpp/yaml.h"
#include "boost/algorithm/string.hpp"

using std::string;
using std::map;

class CcsUtil {
    YAML::Node config;
public:
    static unsigned int hexToInt(string hex);
    static bool _compareVariableLength(string var1, string var2);
    static string processString(string raw, map<string,string> variables);
    static void logError(string message);
    static std::vector<string> splitString(string &input, const char* delimiter);
};

#endif //PROJECTNAME_CCSUTIL_H
