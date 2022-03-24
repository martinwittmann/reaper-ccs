#ifndef CCS_PAGE_H
#define CCS_PAGE_H

#include <string>
#include <experimental/filesystem>
#include "yaml-cpp/yaml.h"
#include "Util.h"
#include "config/PageConfig.h"
#include "actions/Actions.h"
#include "actions/Action.h"

namespace CCS {

  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;

  class Page : public ActionProvider {
    PageConfig *config;
    Actions *actionsManager;
    vector<Action*> providedActions;
    string pageId;

  public:
    Page(string pagePath, Actions* actionsManager);

    ~Page();

    static bool isPageConfigFile(fse::path path);

    void setActive();

    void createActions() override;
    Action* createPageAction(string actionId, YAML::Node node);
    vector<string> getProcessedSubActions(vector<string> rawSubActions);
  };
}

#endif