#include <string>
#include<experimental/filesystem>
#include "yaml-cpp/yaml.h"
#include "CcsPage.cpp"
#include "CcsMidiController.cpp"

namespace fse = std::experimental::filesystem;
namespace fs = std::filesystem;
using std::string;

class CcsSession {
    string path;
    string pagesDir;
    string name;
    YAML::Node sessionConfig;
    std::vector<CcsPage*> pages;

public:
    CcsSession(string sessionPath) {
      path = sessionPath;
      pagesDir = path + fse::path::preferred_separator + "pages";
      sessionConfig = YAML::LoadFile(path + fse::path::preferred_separator + "session.yml");
      addSessionPages();
    }

    ~CcsSession() {
      for (auto it = pages.begin(); it != pages.end(); ++it) {
        delete *it;
      }
    }

    std::vector<string> getPageNames() {
      std::vector<string> result;
      for (const auto & entry: fse::directory_iterator(pagesDir)) {
        fse::path entry_path = fse::path(entry.path());
        if (is_directory(entry_path) || !isPageFile(entry_path.string())) {
          continue;
        }
        result.push_back(entry_path.filename());
      }
      return result;
    }

    bool isPageFile(fse::path path) {
      string filename = path.filename();
      filename.length();
      // Note that we ignore files starting with _ here. This allows us to use
      // those as templates to inherit from, like _default.yml.
      if (filename.starts_with(".") || filename.starts_with("_") || !filename.ends_with(".yml")) {
        return false;
      }
      string session_filename = path.string() + fse::path::preferred_separator + "session.yml";
      return fse::exists(fse::path(session_filename));
    }

    void addSessionPages() {
      sessionConfig = YAML::LoadFile(pagesDir);
      std::vector<string> pageNames = getPageNames();
      for (std::vector<string>::iterator it = pageNames.begin(); it != pageNames.end(); ++it) {
        string pagePath = pagesDir + fse::path::preferred_separator + *it;
        pages.push_back(new CcsPage(pagePath));
      }
    }
};