#include <string>
#include "CcsConfig.h"

using std::string;

class SessionConfig : public CcsConfig {
public:
    SessionConfig(string baseDir) : CcsConfig(baseDir, false) {
    }
};
