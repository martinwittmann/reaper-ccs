#include <string>
#include <vector>
#include <functional>
#include "Action.h"
#include "../Variables.h"
#include "../MidiController.h"

namespace CCS {

  using std::string;
  using std::vector;

  // By providing a vector of sub actions we create a composite action that
  // simply invokes all sub actions when executed.
  Action::Action(
    string providerId,
    string actionId,
    vector<string> argumentNames,
    vector<string> argumentTypes,
    vector<string> subActions,
    Actions* actionsManager
  ) {
    this->type = "composite";
    this->providerId = providerId;
    this->actionId = actionId;
    this->argumentNames = argumentNames;
    this->argumentTypes = argumentTypes;
    this->subActions = subActions;
    this->actionsManager = actionsManager;
  }

  Action::Action(
    string providerId,
    string actionId,
    ActionProvider* actionProvider
  ) {
    this->type = "callback";
    this->providerId = providerId;
    this->actionId = actionId;
    this->callback = callback;
    this->actionProvider = actionProvider;
  }

  string Action::getId() {
    return actionId;
  }

  string Action::getProviderId() {
    return providerId;
  }

  void Action::invoke(vector<string> arguments) {
    // We map the given arguments in the same order as the vector of
    // argument names we got in the constructor.
    std::map<string, string> argumentVariables;
    if (!argumentNames.empty()) {
      for (auto i = 0; i < arguments.size(); i++) {
        string argumentValue = arguments.at(i);
        string argumentType = argumentTypes.at(i);
        if (argumentType == "string") {
          argumentValue = Util::strToHexBytes(argumentValue);
        }
        string argumentName = "_ARGS." + argumentNames.at(i);
        argumentVariables.insert(std::pair(argumentName, argumentValue));
      }
    }

    if (type == "callback") {
      for (auto& argument: arguments) {
        argument = Variables::replaceVariables(argument, argumentVariables);
      }
      actionProvider->actionCallback(actionId, arguments);
    }
    else if (type == "composite") {

      // Apply the arguments to every sub action.
      for (auto subAction: subActions) {
        string rawSubAction = subAction;
        rawSubAction = Variables::replaceVariables(rawSubAction, argumentVariables);
        actionsManager->invokeAction(rawSubAction);
      }
    }
  }
}