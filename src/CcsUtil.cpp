#include <string>

using std::string;

class CcsUtil {
public:
    static unsigned int hexToInt(string hex) {
      return std::stoul(hex, nullptr, 16);
    }
};