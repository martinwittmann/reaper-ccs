#ifndef CCS_UTIL_H
#define CCS_UTIL_H

#include <string>
#include <map>
#include <regex>
#include "yaml-cpp/yaml.h"
#include "boost/algorithm/string.hpp"

namespace CCS {
  class Util {
    YAML::Node config;
  public:
    static unsigned char hexToInt(std::string hex);

    static bool _compareVariableLength(std::string var1, std::string var2);

    static std::string processString(
      std::string raw,
      std::map<std::string,std::string> variables,
      std::string pattern = "\\$(_ARGS\\.)?[A-Z0-9_]+!?"
    );

    static std::vector<std::string> splitString(std::string &input, char delimiter);

    static std::string regexReplace(std::string input, std::string pattern, std::string replacement);

    static bool regexMatch(std::string input, std::string pattern);

    static std::string cleanId(std::string input);

    static std::string joinStrVector(std::vector<std::string> input, char separator);

    static std::string removePrefixSuffix(std::string input);

    static std::vector<unsigned char> splitToBytes(std::string &input);

    static std::string strToHexBytes(std::string input);

    static int getMidiEventId(unsigned char statusByte, unsigned char data1Byte);

    static std::string formatHexByte(unsigned char byte);

    static std::string byteToHex(unsigned char byte);

    static short get7BitValue(double value, double minValue, double maxValue);

    static double getParamValueFrom7Bit(short value, double minValue, double maxValue);

    static void log(std::string message, bool lineBreak = true);
    static void log(char message, bool lineBreak = true);
    static void log(short message, bool lineBreak = true);
    static void log(int message, bool lineBreak = true);
    static void log(double message, bool lineBreak = true);

    static std::vector<std::string> getUsedVariables(std::string input, std::string pattern);

    static void error(std::string message, bool lineBreak = true);

    static void debugYaml(YAML::Node node);

    static std::string compactString(std::string input);

    static std::string toLower(std::string input);

    static double volumeToSlider(double raw);

    static double sliderToVolume(double raw);

    static std::string roundDouble(double value, int precision = 2);

    static std::pair<unsigned char, unsigned char> getStatusAndData1(int eventId);
  };
}

#endif
