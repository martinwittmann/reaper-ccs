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

    static bool isPageConfigFile(fse::path path) {
      string filename = path.filename();
      filename.length();
      // Note that we ignore files starting with _ here. This allows us to use
      // those as templates to inherit from, like _default.yml.
      if (filename.starts_with(".") || filename.starts_with("_") || !filename.ends_with(YAML_EXT)) {
        return false;
      }
      return is_regular_file(path);
    }
};