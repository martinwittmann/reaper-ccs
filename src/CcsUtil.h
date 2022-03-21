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
    static map<string,string> getVariables(YAML::Node &variablesNode);
    static bool _compareVariableLength(string var1, string var2);
    static string processString(string raw, map<string,string> variables);
    static void mergeYaml(YAML::Node target, YAML::Node const& source);
    static void _mergeYamlNode(YAML::Node target, YAML::Node const& source);
    static void _mergeYamlMap(YAML::Node target, YAML::Node const& source);
    static void _mergeYamlSequences(YAML::Node target, YAML::Node const& source);
};

#endif //PROJECTNAME_CCSUTIL_H
