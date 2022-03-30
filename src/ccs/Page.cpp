#include <string>
#include <map>
#include <experimental/filesystem>
#include "config/PageConfig.h"
#include "Page.h"
#include "globals.cpp"
#include "actions/ActionsManager.h"
#include "actions/ActionProvider.h"
#include "Variables.h"
#include "CcsException.h"

namespace CCS {

  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;
  using std::vector;

  Page::Page(
    string pagePath,
    ActionsManager *actionsManager,
    Session *session,
    ReaperApi *reaperApi
  ) : ActionProvider(actionsManager) {
    m_config = new PageConfig(pagePath);
    m_pageId = m_config->getValue("id");
    registerActionProvider(m_pageId);
    m_actionsManager = actionsManager;
    m_session = session;
    m_reaperApi = reaperApi;
    createActions();
    createMidiControlElementMappings();
  }

  Page::~Page() {
    delete m_config;
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

  void Page::setActive() {
    // Invoke the actions defined in "on_activate";
    try {
      vector<string> initActionItems = m_config->getListValues("on_activate");
      for (auto rawAction : initActionItems) {
        actionsManager->invokeAction(rawAction, m_session);
      }

      updateMidiControllerUI();
    }
    catch (CcsException &e) {
      Util::error("Error setting active page:");
      Util::error(e.what());
    }
  }

  void Page::createActions() {
    // Pages can also provide actions.
    // In the page configs these can be called with [page.action_id] and refer
    // to the current page / the current config file.
    // For simplicity the actionsManager will translate the action provider
    // page to the currently selected page id.
    // This means that we don't need to unregister page actions when changing
    // pages and also that it's possible to call actions from other pages.
    // TODO Do we need to prevent calling actions of other pages?
    //      By our defintion of page these would not make sense and most likely
    //      do something unintended.

    auto provider = dynamic_cast<ActionProvider*>(this);

    auto setStateAction = new Action(
      m_pageId,
      "set_state",
      provider
    );
    provideAction(setStateAction);

    // Get Actions from config.
    YAML::Node actionsNode = m_config->getMapValue("actions");
    YAML::Node variablesNode = m_config->getMapValue("variables");
    for (const auto &item: actionsNode) {
      auto actionId = item.first.as<string>();
      Action *action = new Action(
        m_pageId,
        actionId,
        item.second,
        m_actionsManager
      );
      provideAction(action);
    }
  }

  string Page::getPageId() {
    return m_pageId;
  }

  void Page::actionCallback(string actionName, vector<string> arguments) {
    if (actionName == "set_state") {
      m_state["_STATE." + arguments.at(0)] = arguments.at(1);
    }
  }

  std::map<int,MidiEventSubscriber*> Page::getSubscribedMidiEventIds() {
    std::map<int,MidiEventSubscriber*> result;
    for (const auto &mapping: m_controlElementMappings) {
      int eventId = mapping->getMidiEventId();
      auto subscriber = dynamic_cast<MidiEventSubscriber*>(mapping);
      result.insert(std::pair(eventId, subscriber));
    }
    return result;
  }

  void Page::createMidiControlElementMappings() {
    YAML::Node controlsNode = m_config->getMapValue("mappings.controls");

    for (const auto &controlConfig: controlsNode) {
      auto rawControlId = controlConfig.first.as<string>();
      auto controlIdParts = Util::splitString(rawControlId, '.');
      string midiControllerId = controlIdParts.at(0);
      string controlId = controlIdParts.at(1);

      MidiController *midiController = m_session->getMidiController(midiControllerId);
      MidiControlElement *controlEl = midiController->getMidiControlElement(controlId);
      int midiEventId = midiController->getMidiEventIdForControl(controlId);
      m_controlElementMappings.push_back(new MidiControlElementMapping(
        midiEventId,
        controlId,
        controlConfig.second,
        controlEl,
        m_reaperApi,
        m_session
      ));
    }
  }

  std::map<string, string> *Page::getState() {
    return &m_state;
  }

  void Page::updateMidiControllerUI() {
    for (const auto mapping : m_controlElementMappings) {
      mapping->updateControlElement();
    }
  }
}