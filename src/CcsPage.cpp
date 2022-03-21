#include <string>
#include<experimental/filesystem>
#include "yaml-cpp/yaml.h"
#include "CcsUtil.h"

namespace fse = std::experimental::filesystem;
namespace fs = std::filesystem;
using std::string;

class CcsPage {
    YAML::Node config;

    YAML::Node loadConfig(string filename) {
      fse::path path = fse::path(filename);
      YAML::Node config = YAML::LoadFile(filename);
      if (!config["extend"].IsDefined()) {
        return config;
      }

      string templateFilename = path.parent_path().string() + fse::path::preferred_separator + config["extend"].as<string>() + ".yml";
      YAML::Node result = YAML::LoadFile(templateFilename);
      CcsUtil::mergeYaml(result, config);

      // TODO Read all variables and replace all variable usages in the whole config.
      return result;
    }
public:
    CcsPage(string pagePath) {
      config = loadConfig(pagePath);
      string hui = config["mappings"]["controls"]["button1"]["label"].as<string>();
      int dd = 1;
    }
};