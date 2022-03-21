#include <string>
#include<experimental/filesystem>
#include "config/PageConfig.h"
#include "Page.h"
#include "globals.cpp"

namespace CCS {

  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;

  Page::Page(string pagePath) {
    config = new PageConfig(pagePath);
  }

  Page::~Page() {
    delete config;
  }

  bool Page::isPageConfigFile(fse::path path) {
    string filename = path.filename();
    filename.length();
    // Note that we ignore files starting with _ here. This allows us to use
    // those as templates to inherit from, like _default.yml.
    if (filename.starts_with(".") || filename.starts_with("_") || !filename.ends_with(YAML_EXT)) {
      return false;
    }
    return is_regular_file(path);
  }
}