#include <string>
#include <set>
#include <experimental/filesystem>
#include "yaml-cpp/yaml.h"

#include "globals.cpp"
#include "config/SessionConfig.h"
#include "Session.h"
#include "Page.h"
#include "midi/MidiController.h"
#include "actions/ActionsManager.h"
#include "../reaper-api/ReaperApi.h"
#include "CcsException.h"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;
  using std::vector;

  Session::Session(
    string sessionPath,
    ActionsManager *actionsManager,
    midi_Output *output,
    ReaperApi *reaperApi
  ) : ActionProvider(actionsManager) {
    m_path = sessionPath;
    m_pagesDir = m_path + SEP + "pages";
    m_midiControllersDir = fse::path(m_path)
      .parent_path()
      .parent_path()
      .append("controllers")
      .string();

    m_pluginsDir = fse::path(m_path)
      .parent_path()
      .parent_path()
      .append("fx_plugins")
      .string();

    m_pluginManager = new FxPlugins(m_pluginsDir);
    m_actionsManager = actionsManager;
    m_sessionConfig = new SessionConfig(m_path + SEP + "session" + YAML_EXT);
    m_output = output;
    m_reaperApi = reaperApi;

    registerActionProvider("session");

    auto provider = dynamic_cast<ActionProvider*>(this);
    auto loadPageAction = new Action(
      "session",
      "load_page",
      provider
    );
    provideAction(loadPageAction);

    auto debugAction = new Action(
      "session",
      "debug",
      provider
    );
    provideAction(debugAction);

    loadMidiControllers();
    loadSessionPages();

    // We always start out on the first page we know.
    loadPage(m_pages.at(0)->getPageId());
  }

  Session::~Session() {
    delete m_sessionConfig;
    for (auto it = m_pages.begin(); it != m_pages.end(); ++it) {
      delete *it;
    }
  }

  Page *Session::getPage(string pageId) {
    for (auto page : m_pages) {
      if (page->getPageId() == pageId) {
        return page;
      }
    }
    throw CcsException("Trying to get unknown page: " + pageId);
  }

  void Session::loadPage(string pageId) {
    try {
      Page *page = getPage(pageId);
      m_activePage = page;

      // We need to call setActive after setting activePage here because the
      // call of setActive invokes the on_activate actions which depend on an
      // active page being set.
      page->setActive();
    }
    catch (CcsException &e) {
      Util::error("Exception in Session::loadPage(" + pageId + "):");
      Util::error(e.what());
    }
  }

  Page *Session::getActivePage() {
    return m_activePage;
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
    for (const auto &entry: fse::directory_iterator(m_pagesDir)) {
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
    for (const auto &entry: fse::directory_iterator(m_midiControllersDir)) {
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
      string pagePath = m_pagesDir + SEP + *it;
      m_pages.push_back(new Page(
        pagePath,
        m_actionsManager,
        this,
        m_reaperApi
      ));
    }
  }

  void Session::loadMidiControllers() {
    vector<string> controllerNames = getMidiControllerNames();
    for (auto it = controllerNames.begin(); it != controllerNames.end(); ++it) {
      string controllerConfigFile = *it;
      m_midiControllers.push_back(
        new MidiController(controllerConfigFile, m_output, m_actionsManager)
      );
    }
  }

  std::map<int,MidiEventSubscriber*> Session::getSubscribedMidiEventIds() {
    return m_activePage->getSubscribedMidiEventIds();
  }

  MidiController *Session::getMidiController(std::string id) {
    for (auto controller : m_midiControllers) {
      if (controller->getControllerId() == id) {
        return controller;
      }
    }
    throw CcsException("No midi controller with id: '" + id + "' found.");
  }

  FxPlugins *Session::getPluginManager() {
    return m_pluginManager;
  };

  ActionsManager *Session::getActionsManager() {
    return m_actionsManager;
  }

  void Session::actionCallback(std::string actionName, std::vector<std::string> arguments) {
    if (actionName == "load_page") {
      loadPage(arguments.at(0));
    }
    else if (actionName == "debug") {
      Util::log("[DEBUG]");
      for (auto argument : arguments) {
        Util::log("  " + argument);
      }
      Util::log("");
    }
  }

  void Session::invokeAction(std::string action, Session* session) {
    getActionsManager()->invokeAction(action, session);
  };
}