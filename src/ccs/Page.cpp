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
#include "actions/CompositeAction.h"
#include "actions/Action.h"
#include "midi/MidiController.h"
#include "Session.h"
#include "Util.h"
#include "midi/MidiControlElementMapping.h"

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

    // This needs to be called after setting up the mappings because these are
    // needed to determine the subscribed midi event ids.
    createSubscribedMidiEventIds();

    if (m_config->keyExists("on_activate")) {
      m_activateAction = new CompositeAction(
        m_pageId + ".on_activate",
        m_config->getMapValue("on_activate")
      );
    }

    // This action will be invoked, before any mapped control's on_change actions.
    if (m_config->keyExists("mappings.before_value_changes")) {
      m_beforeValueChangesAction = new CompositeAction(
        m_pageId + ".before_value_changes",
        m_config->getMapValue("mappings.before_value_changes")
      );
    }

    // This action will be invoked, after any mapped control's on_change actions.
    if (m_config->keyExists("mappings.after_value_changes")) {
      m_afterValueChangesAction = new CompositeAction(
        m_pageId + ".after_value_changes",
        m_config->getMapValue("mappings.after_value_changes")
      );
    }

    // We initialize the state of each page with its own page id set and any
    // values from "initial_state".
    m_state.insert(std::pair("_STATE.CURRENT_PAGE", m_pageId));
    YAML::Node initialState = m_config->getMapValue("initial_state");
    for (auto node : initialState) {
      m_state.insert(std::pair(
        "_STATE." + node.first.as<string>(),
          node.second.as<string>()
      ));
    }
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
      std::map<string,string> *state = getState();
      if (m_activateAction) {
        m_activateAction->invoke(*state, m_session);
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

    auto updateUiAction = new Action(
      m_pageId,
      "update_controller_ui",
      provider
    );
    provideAction(updateUiAction);

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
    else if (actionName == "update_controller_ui") {
      updateMidiControllerUI();
    }
  }

  void Page::createSubscribedMidiEventIds() {
    m_subscribedMidiEventIds.clear();
    for (const auto &mapping: m_controlElementMappings) {
      int eventId = mapping->getMidiEventId();
      auto subscriber = dynamic_cast<MidiEventSubscriber*>(mapping);
      m_subscribedMidiEventIds.insert(std::pair(eventId, subscriber));
    }
  }

  std::map<int,MidiEventSubscriber*> Page::getSubscribedMidiEventIds() {
    return m_subscribedMidiEventIds;
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
        m_session,
        this
      ));
    }
  }

  std::map<string, string> *Page::getState() {
    return &m_state;
  }

  void Page::updateMidiControllerUI() {
    std::map<string,string> *state = getState();
    if (m_beforeValueChangesAction) {
      m_beforeValueChangesAction->invoke(*state, m_session);
    }
    for (const auto mapping : m_controlElementMappings) {
      mapping->updateControlElement();
    }
    if (m_afterValueChangesAction) {
      m_beforeValueChangesAction->invoke(*state, m_session);
    }
  }

  void Page::invokeBeforeValueChangesAction() {
    std::map<string,string> *state = getState();
    m_beforeValueChangesAction->invoke(*state, m_session);
  }

  void Page::invokeAfterValueChangesAction() {
    std::map<string,string> *state = getState();
    m_afterValueChangesAction->invoke(*state, m_session);
  }
}