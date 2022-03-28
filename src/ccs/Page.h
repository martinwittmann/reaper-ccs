#ifndef CCS_PAGE_H
#define CCS_PAGE_H

#include <string>
#include <map>
#include <experimental/filesystem>
#include "yaml-cpp/yaml.h"
#include "Util.h"
#include "config/PageConfig.h"
#include "actions/ActionsManager.h"
#include "actions/ActionProvider.h"
#include "actions/Action.h"
#include "midi/MidiEventSubscriber.h"
#include "midi/MidiControlElementMapping.h"
#include "../reaper-api/ReaperApi.h"
#include "FxPlugins.h"

namespace CCS {

  class Session;

  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;
  using std::vector;

  class Action;

class Page : public ActionProvider {
    PageConfig *m_config;
    ActionsManager *m_actionsManager;
    Session *m_session;
    vector<Action*> m_providedActions;
    string m_pageId;
    // A map of string variables that can be set via a page action.
    // Example: [page.set_state:name:value]
    std::map<string,string> m_state;
    vector<MidiControlElementMapping*> m_controlElementMappings;
    ReaperApi *m_reaperApi;

  public:
    Page(
      string pagePath,
      ActionsManager *actionsManager,
      Session *session,
      ReaperApi *reaperApi
    );
    ~Page();
    static bool isPageConfigFile(fse::path path);
    void setActive();
    void createActions();
    Action *createPageAction(string actionId, YAML::Node node);
    vector<string> getProcessedSubActions(vector<string> rawSubActions);
    string getPageId();
    void actionCallback(string actionName, vector<string> arguments);
    void createMidiControlElementMappings();
    std::map<int,MidiEventSubscriber*> getSubscribedMidiEventIds();
  };
}

#endif