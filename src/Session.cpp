#include <string>
#include<experimental/filesystem>
#include "yaml-cpp/yaml.h"

#include "globals.cpp"
#include "config/SessionConfig.h"
#include "Session.h"
#include "Page.h"
#include "MidiController.h"
#include "actions/Actions.h"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;

  Session::Session(string sessionPath, Actions* actions) {
    path = sessionPath;
    pagesDir = path + SEP + "pages";
    midiControllersDir = fse::path(path)
      .parent_path()
      .parent_path()
      .append("controllers")
      .string();
    this->actions = actions;
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

  void Session::start() {
    setActivePage(0);
  }

  void Session::setActivePage(int pageId) {
    activePage = pageId;
    Page* page = pages.at(pageId);
    // TODO
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
    // Hardcoded for testing.
    const int deviceId = 0;
    std::vector<string> controllerNames = getMidiControllerNames();
    for (auto it = controllerNames.begin(); it != controllerNames.end(); ++it) {
      string controllerConfigFile = *it;
      midiControllers.push_back(
        new MidiController(controllerConfigFile, deviceId, actions)
      );
    }
  }
}