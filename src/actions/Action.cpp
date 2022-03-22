#include <string>
#include <vector>
#include "Action.h"
#include "../Variables.h"

namespace CCS {

  using std::string;
  using std::vector;

  // By providing a vector of sub actions we create a composite action that
  // simply invokes all sub actions when executed.
  Action::Action(
    string providerId,
    string id,
    vector<string> argumentNames,
    vector<string> subActions,
    Actions* actionsManager
  ) {
    this->type = Action::COMPOSITE;
    this->providerId = providerId;
    this->id = id;
    this->argumentNames = argumentNames;
    this->subActions = subActions;
    this->actionsManager = actionsManager;
  }

  Action::Action(
    string providerId,
    string id,
    void (ActionProvider::*callback)(vector<string>),
    ActionProvider* actionProvider
  ) {
    this->type = Action::CALLBACK;
    this->providerId = providerId;
    this->id = id;
    this->callback = callback;
    this->actionProvider = actionProvider;
  }

  void Action::invoke(vector<string> arguments) {

    switch (type) {
      case Action::CALLBACK:
        // See https://opensource.com/article/21/2/ccc-method-pointers for some
        // help with pointers to a method.
        (actionProvider->*callback)(arguments);
        break;

      case Action::COMPOSITE:
        // TODO Implement substitution of arguments before invoking the subactions.
        std::map<string,string> variables = Variables::getVariables(
          argumentNames,
          arguments
        );
        for (auto it = subActions.begin(); it != subActions.end(); ++it) {
          string rawAction = *it;
          rawAction = Variables::replaceVariables(rawAction, variables);
          actionsManager->invokeAction(rawAction);
        }
        break;
    }
  }
}