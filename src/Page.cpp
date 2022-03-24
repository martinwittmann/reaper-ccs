#include <string>
#include <map>
#include <experimental/filesystem>
#include "config/PageConfig.h"
#include "Page.h"
#include "globals.cpp"
#include "actions/ActionsManager.h"
#include "actions/ActionProvider.h"
#include "Variables.h"

namespace CCS {

  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;

  Page::Page(string pagePath, ActionsManager* actionsManager, Session* session)
    : ActionProvider(actionsManager) {
    config = new PageConfig(pagePath);
    pageId = config->getValue("id");
    registerActionProvider(pageId);
    this->actionsManager = actionsManager;
    this->session = session;
    createActions();
  }

  Page::~Page() {
    delete config;
    for (auto &action : providedActions) {
      delete action;
    }
    providedActions.clear();
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
    vector<string> initActionItems = config->getListValues("on_activate");
    for (auto rawAction : initActionItems) {
      actionsManager->invokeAction(rawAction, session);
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
      pageId,
      "set_state",
      provider
    );
    providedActions.push_back(setStateAction);
    actionsManager->registerAction(*setStateAction);

    // Get Actions from config.
    YAML::Node actionsNode = config->getMapValue("actions");
    YAML::Node variablesNode = config->getMapValue("variables");
    std::map<string,string> variables = Variables::getVariables(variablesNode);
    for (const auto &item: actionsNode) {
      auto actionId = item.first.as<string>();
      Action* action = createPageAction(
        actionId,
        item.second
      );
      providedActions.push_back(action);
      actionsManager->registerAction(*action);
    }
  }

  Action* Page::createPageAction(string actionId, YAML::Node node) {
    vector<string> rawSubActions = config->getListValues("message", &node);
    vector<string> processedSubActions = getProcessedSubActions(rawSubActions);
    vector<string> argumentNames = config->getListValues("arguments", &node);
    vector<string> argumentTypes;

    for (auto &argument : argumentNames) {
      if (argument.substr(argument.length() - 1) == "!") {
        argument = argument.substr(0, argument.length() - 1);
        argumentTypes.push_back("string");
      }
      else {
        argumentTypes.push_back("byte");
      }
    }

    return new Action(
      pageId,
      actionId,
      argumentNames,
      argumentTypes,
      processedSubActions,
      actionsManager
    );
  }

  vector<string> Page::getProcessedSubActions(vector<string> rawSubActions) {
    vector<string> result;

    vector<string>midiMessages;
    for (auto rawSubAction: rawSubActions) {
      result.push_back(Util::regexReplace(rawSubAction, "^\\[page\\.", "[" + pageId + "."));
    }
    return result;
  }

  string Page::getPageId() {
    return pageId;
  }

  void Page::actionCallback(std::string actionName, std::vector<std::string> arguments) {
    if (actionName == "set_state") {
      state[arguments.at(0)] = arguments.at(1);
    }
  }

  void Page::onMidiEvent(int eventId, unsigned int dataByte) {
  }


  vector<int> Page::getSubscribedMidiEventIds() {
    vector<int> result;
    for (const auto &mapping: controlElementMappings) {
      result.push_back(mapping->getMidiEventId());
    }
    return result;
  }

  void Page::createMidiControlElementMappings() {
    YAML::Node controlsNode = config->getMapValue("mappings.controls");
    auto subscriber = dynamic_cast<MidiEventSubscriber*>(this);

    for (const auto &controlConfig: controlsNode) {
      auto controlId = controlConfig.first.as<string>();
      auto controlParts = Util::splitString(controlId, '.');
      string midiControllerId = controlParts.at(0);
      MidiController* midiController = session->getMidiController(midiControllerId);
      controlElementMappings.push_back(new MidiControlElementMapping(
        controlId,
        controlConfig.second,
        midiController,
        subscriber
      ));
    }
  }
}