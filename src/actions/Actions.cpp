#include "Actions.h"
#include "ActionInvocation.h"
#include "Action.h"

namespace CCS {

  Actions::Actions() {
  }

  Actions::~Actions() {
  }

  void Actions::registerAction(Action action) {
    actions.push_back(action);
  }

  void Actions::registerProvider(ActionProvider *provider) {
    providers.push_back(provider);
  }

  void Actions::invokeAction(Action action) {
    action.invoke();
  }

  void Actions::invokeAction(std::string rawAction) {
    auto invokation = new ActionInvokation(rawAction);
    Action action = getAction(invokation->providerId, invokation->actionId);
    invokeAction(action);
  }

  Action Actions::getAction(std::string providerId, std::string actionId) {
    for (auto it = actions.begin(); it != actions.end(); ++it) {
      Action action = *it;
      if (action.providerId == providerId && action.id == actionId) {
        return action;
      }
    }
    throw "Action not found";
  }

  ActionProvider* Actions::getProvider(Action action) {
    return getProvider(action.providerId);
  }

  ActionProvider* Actions::getProvider(std::string providerId) {
    for (auto it = providers.begin(); it != providers.end(); ++it) {
      ActionProvider* provider = *it;
      if (provider->getId() == providerId) {
        return provider;
      }
    }
    throw "ActionProvider not found";
  }
}