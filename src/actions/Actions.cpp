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

  void Actions::invokeAction(Action action, vector<string> arguments) {
    action.invoke(arguments);
  }

  // Note that we do not accept arguments here, because they are encoded in the
  // rawAction string and are unpacket in ActionInvokation.
  void Actions::invokeAction(std::string rawAction) {
    auto invokation = new ActionInvokation(rawAction);
    Action action = getAction(invokation->providerId, invokation->actionId);
    action.invoke(invokation->arguments);
  }

  Action Actions::getAction(std::string providerId, std::string actionId) {
    int a = 1;
    for (auto it = actions.begin(); it != actions.end(); ++it) {
      Action action = *it;
      if (action.getProviderId() == providerId && action.getId() == actionId) {
        return action;
      }
    }
    throw "Action not found";
  }

  ActionProvider* Actions::getProvider(Action action) {
    return getProvider(action.getProviderId());
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