#include <string>
#include<experimental/filesystem>
#include "yaml-cpp/yaml.h"
#include "globals.cpp"
#include "config/SessionConfig.h"
#include "Session.h"

#include "Page.cpp"
#include "MidiController.cpp"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;

  Session::Session(string sessionPath) {
    path = sessionPath;
    pagesDir = path + SEP + "pages";
    midiControllersDir = path + SEP + "controllers";
    sessionConfig = new SessionConfig(path + SEP + "session" + YAML_EXT);
    loadSessionPages();
    loadMidiControllers();
  }

  Session::~Session() {
    delete sessionConfig;
    for (auto it = pages.begin(); it != pages.end(); ++it) {
      delete *it;
    }
  }

  std::vector<string> Session::getSessions(string sessionsDir) {
    std::vector<string> result;
    for (const auto &entry: fse::directory_iterator(sessionsDir)) {
      fse::path entry_path = fse::path(entry.path());
      if (!is_directory(entry_path) || !Session::isSessionFile(entry_path.string())) {
        continue;
      }
      result.push_back(entry_path.filename());
    }
    return result;
  }

  bool Session::isSessionFile(fse::path path) {
    string filename = path.filename();
    if (filename.starts_with('.')) {
      return false;
    }
    string session_filename = path.string() + SEP + "session" + YAML_EXT;
    return fse::exists(fse::path(session_filename));
  }

  std::vector<string> Session::getPageNames() {
    std::vector<string> result;
    for (const auto &entry: fse::directory_iterator(pagesDir)) {
      fse::path entry_path = fse::path(entry.path());
      if (is_directory(entry_path) || !Page::isPageConfigFile(entry_path.string())) {
        continue;
      }
      result.push_back(entry_path.filename());
    }
    return result;
  }

  std::vector<string> Session::getMidiControllerNames() {
    std::vector<string> result;
    for (const auto &entry: fse::directory_iterator(midiControllersDir)) {
      fse::path entry_path = fse::path(entry.path());
      if (
        is_directory(entry_path) ||
        !MidiController::isMidiControllerConfigFile(entry_path.string())
        ) {
        continue;
      }
      result.push_back(entry_path.string());
    }
    return result;
  }

  void Session::loadSessionPages() {
    std::vector<string> pageNames = getPageNames();
    for (auto it = pageNames.begin(); it != pageNames.end(); ++it) {
      string pagePath = pagesDir + SEP + *it;
      pages.push_back(new Page(pagePath));
    }
  }

  void Session::loadMidiControllers() {
    std::vector<string> controllerNames = getMidiControllerNames();
    for (auto it = controllerNames.begin(); it != controllerNames.end(); ++it) {
      string pagePath = pagesDir + SEP + *it;
      pages.push_back(new Page(pagePath));
    }
  }
}