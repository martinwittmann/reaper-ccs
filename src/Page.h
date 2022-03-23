#ifndef CCS_PAGE_H
#define CCS_PAGE_H

#include <string>
#include <experimental/filesystem>
#include "yaml-cpp/yaml.h"
#include "Util.h"
#include "config/PageConfig.h"
#include "actions/Actions.h"

namespace CCS {

  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;

  class Page {
    PageConfig *config;
    Actions *actionManager;

  public:
    Page(string pagePath, Actions* actionManager);

    ~Page();

    static bool isPageConfigFile(fse::path path);

    void setActive();
  };
}

#endif