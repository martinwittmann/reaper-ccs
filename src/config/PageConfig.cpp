#include <string>
#include "CcsConfig.h"

using std::string;

class PageConfig : public CcsConfig {
public:
    PageConfig(string baseDir) : CcsConfig(
      baseDir,
      true,
      "extends"
    ) {
      replaceVariables();
    }
};