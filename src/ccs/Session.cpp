#include <string>
#include <set>
#include <experimental/filesystem>
#include "yaml-cpp/yaml.h"

#include "globals.cpp"
#include "config/SessionConfig.h"
#include "Session.h"
#include "Page.h"
#include "MidiController.h"
#include "actions/ActionsManager.h"
#include "reaper/ReaperApi.h"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;
  using std::vector;

  Session::Session(
    string sessionPath,
    ActionsManager* actionsManager,
    midi_Output* output,
    ReaperApi* reaperApi
  ) {
    path = sessionPath;
    pagesDir = path + SEP + "pages";
    midiControllersDir = fse::path(path)
      .parent_path()
      .parent_path()
      .append("controllers")
      .string();
    this->actionsManager = actionsManager;
    sessionConfig = new SessionConfig(path + SEP + "session" + YAML_EXT);
    this->output = output;
    this->reaperApi = reaperApi;

    loadMidiControllers();
    loadSessionPages();

    // TODO Find a better way to load a page.
    setActivePage(0);
  }

  Session::~Session() {
    delete sessionConfig;
    for (auto it = pages.begin(); it != pages.end(); ++it) {
      delete *it;
    }
  }

  void Session::setActivePage(int index) {
    Page* page = pages.at(index);
    activePage = page;
    // TODO implement changing page.
    // We need to call setAction after setting activePage here because the
    // call of setActive invokes the activation actions which depend on an
    // active page being set.
    page->setActive();
  }

  Page* Session::getActivePage() {
    return activePage;
  }

  vector<string> Session::getSessions(string sessionsDir) {
    vector<string> result;
    std::set<string> sortedSessions;
    for (const auto &entry: fse::directory_iterator(sessionsDir)) {
      fse::path entry_path = fse::path(entry.path());
      if (!is_directory(entry_path) || !Session::isSessionFile(entry_path.string())) {
        continue;
      }
      sortedSessions.insert(entry_path.filename());
    }

    for (auto sessionName : sortedSessions) {
      result.push_back(sessionName);
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

  vector<string> Session::getPageNames() {
    vector<string> result;
    std::set<string> sortedPages;
    for (const auto &entry: fse::directory_iterator(pagesDir)) {
      fse::path entry_path = fse::path(entry.path());
      if (is_directory(entry_path) || !Page::isPageConfigFile(entry_path.string())) {
        continue;
      }
      sortedPages.insert(entry_path.filename());
    }

    for (auto pageName : sortedPages) {
      result.push_back(pageName);
    }
    return result;
  }

  vector<string> Session::getMidiControllerNames() {
    vector<string> result;
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
    vector<string> pageNames = getPageNames();
    for (auto it = pageNames.begin(); it != pageNames.end(); ++it) {
      string pagePath = pagesDir + SEP + *it;
      pages.push_back(new Page(
        pagePath,
        actionsManager,
        this,
        reaperApi
      ));
    }
  }

  void Session::loadMidiControllers() {
    vector<string> controllerNames = getMidiControllerNames();
    for (auto it = controllerNames.begin(); it != controllerNames.end(); ++it) {
      string controllerConfigFile = *it;
      midiControllers.push_back(
        new MidiController(controllerConfigFile, output, actionsManager)
      );
    }
  }

  std::map<int,MidiEventSubscriber*> Session::getSubscribedMidiEventIds() {
    return activePage->getSubscribedMidiEventIds();
  }

  MidiController* Session::getMidiController(std::string id) {
    for (auto controller : midiControllers) {
      if (controller->getControllerId() == id) {
        return controller;
      }
    }
    throw "No midi controller with id: '" + id + "' found.";
  }
}