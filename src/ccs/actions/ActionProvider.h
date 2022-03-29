#ifndef CCS_ACTION_PROVIDER_H
#define CCS_ACTION_PROVIDER_H

#include <string>
#include <vector>

namespace CCS {

  class Action;

  // We need to forward declare actions to not get into cyclic dependencies.
  class ActionsManager;

  class ActionProvider {
    std::string id;

  protected:
    ActionsManager *actionsManager;
    std::vector<Action*> m_providedActions;

  public:
    explicit ActionProvider(ActionsManager *actionsManager);
    ~ActionProvider();
    std::string getProviderId();
    void registerActionProvider(std::string providerId);
    virtual void actionCallback(std::string actionName, std::vector<std::string> arguments);

    bool isMacroAction(std::string rawAction);

    void provideAction(Action *action);
  };
}

#endif