#ifndef CCS_ACTIONS_H
#define CCS_ACTIONS_H

#include <vector>
#include <string>
#include "Action.h"
#include "ActionProvider.h"

namespace CCS {

  class Actions {
    std::vector<Action> actions;
    std::vector<ActionProvider*> providers;

  public:
    Actions();
    ~Actions();

    void registerAction(Action action);
    void registerProvider(ActionProvider* provider);

    void invokeAction(Action action);
    void invokeAction(std::string rawAction);
    Action getAction(std::string providerId, std::string actionId);
    ActionProvider* getProvider(Action action);
    ActionProvider* getProvider(std::string providerId);
  };
}

#endif
