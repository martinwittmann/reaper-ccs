#ifndef CCS_PAGE_H
#define CCS_PAGE_H

#include <string>
#include <experimental/filesystem>
#include "yaml-cpp/yaml.h"
#include "Util.h"
#include "config/PageConfig.h"
#include "actions/ActionsManager.h"
#include "actions/ActionProvider.h"
#include "actions/Action.h"

namespace CCS {

  class Session;

  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;
  using std::vector;

  class Action;

  class Page : public ActionProvider {
    PageConfig *config;
    ActionsManager* actionsManager;
    Session* session;
    vector<Action*> providedActions;
    string pageId;

  public:
    Page(string pagePath, ActionsManager* actionsManager, Session* session);

    ~Page();

    static bool isPageConfigFile(fse::path path);

    void setActive();

    void createActions();
    Action* createPageAction(string actionId, YAML::Node node);
    vector<string> getProcessedSubActions(vector<string> rawSubActions);
    string getPageId();
  };
}

#endif