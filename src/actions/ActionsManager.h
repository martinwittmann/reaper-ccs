#ifndef CCS_ACTIONS_H
#define CCS_ACTIONS_H

#include <vector>
#include <string>
#include "ActionProvider.h"
#include "Action.h"

namespace CCS {

  class Action;

  class ActionsManager {
    std::vector<Action> actions;
    std::vector<ActionProvider*> providers;

  public:
    ActionsManager();
    ~ActionsManager();

    void registerAction(Action action);
    void registerProvider(ActionProvider* provider);

    void invokeAction(Action action, std::vector<std::string> arguments);
    void invokeAction(std::string rawAction);
    Action getAction(std::string providerId, std::string actionId);
    ActionProvider* getProvider(Action action);
    ActionProvider* getProvider(std::string providerId);
  };
}

#endif
