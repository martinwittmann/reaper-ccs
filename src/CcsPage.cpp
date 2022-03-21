#include <string>
#include<experimental/filesystem>
#include "yaml-cpp/yaml.h"
#include "CcsUtil.h"
#include "config/PageConfig.cpp"

namespace fse = std::experimental::filesystem;
namespace fs = std::filesystem;
using std::string;

class CcsPage {
    PageConfig* config;

public:
    CcsPage(string pagePath) {
      config = new PageConfig(pagePath);
      //string hui = config->getValue("mappings/controls/slmk3.encoder2/label");
      string hui = config->getValue("on_activate/1");
      int dd = 1;
    }

    ~CcsPage() {
      delete config;
    }
};