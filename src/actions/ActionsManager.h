#ifndef CCS_ACTIONS_H
#define CCS_ACTIONS_H

#include <vector>
#include <string>

namespace CCS {


  class Session;
  class Action;
  class ActionProvider;

  class ActionsManager {
    std::vector<Action> actions;
    std::vector<ActionProvider*> providers;

  public:
    ActionsManager();
    ~ActionsManager();

    void registerAction(Action action);
    void registerProvider(ActionProvider* provider);

    void invokeAction(Action action, std::vector<std::string> arguments, Session* session);
    void invokeAction(std::string rawAction, Session* session);
    Action getAction(std::string providerId, std::string actionId);
    ActionProvider* getProvider(Action action);
    ActionProvider* getProvider(std::string providerId);
  };
}

#endif
