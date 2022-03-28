#include "ActionsManager.h"
#include "ActionInvocation.h"
#include "Action.h"
#include "../Util.h"
#include "../Session.h"

namespace CCS {

  ActionsManager::ActionsManager() {
  }

  ActionsManager::~ActionsManager() {
  }

  void ActionsManager::registerAction(Action action) {
    actions.push_back(action);
  }

  void ActionsManager::registerProvider(ActionProvider *provider) {
    providers.push_back(provider);
  }

  void ActionsManager::invokeAction(
    Action action,
    vector<string> arguments,
    Session *session
  ) {
    action.invoke(arguments, session);
  }

  // Note that we do not accept arguments here, because they are encoded in the
  // rawAction string and are unpacket in ActionInvokation.
  void ActionsManager::invokeAction(std::string rawAction, Session *session) {
    auto invokation = new ActionInvokation(session, rawAction);
    try {
      Action action = getAction(invokation->providerId, invokation->actionId);
      action.invoke(invokation->arguments, session);
    }
    catch (...) {
      Util::error("Trying to invoke action that does not exist: " + rawAction);
    }
  }

  Action ActionsManager::getAction(std::string providerId, std::string actionId) {
    for (auto it = actions.begin(); it != actions.end(); ++it) {
      Action action = *it;
      if (action.getProviderId() == providerId && action.getActionId() == actionId) {
        return action;
      }
    }
    throw "Action not found";
  }

  ActionProvider *ActionsManager::getProvider(Action action) {
    return getProvider(action.getProviderId());
  }

  ActionProvider *ActionsManager::getProvider(std::string providerId) {
    for (auto it = providers.begin(); it != providers.end(); ++it) {
      ActionProvider *provider = *it;
      if (provider->getProviderId() == providerId) {
        return provider;
      }
    }
    throw "ActionProvider not found";
  }
}