#include <string>
#include <vector>
#include <functional>
#include "Action.h"
#include "../Variables.h"
#include "../midi/MidiController.h"
#include "../Page.h"

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
    ActionsManager *actionsManager
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
    ActionProvider *actionProvider
  ) {
    this->type = "callback";
    this->providerId = providerId;
    this->actionId = actionId;
    this->actionProvider = actionProvider;
  }

  string Action::getActionId() {
    return actionId;
  }

  string Action::getProviderId() {
    return providerId;
  }

  void Action::invoke(vector<string> arguments, Session *session) {
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

        // We need to make sure that arguments retrieved from user created
        // configs are at least 2 characters long. Otherwise the sent messages
        // are incorrect.
        // TODO Are the values supposed to be base10 and converted?
        // Right now we're using them as is. This will break quite easily.
        if (argumentType == "byte" && !argumentValue.empty() && argumentValue.size() < 2) {
          argumentValue = "0" + argumentValue;
        }
        argumentVariables.insert(std::pair(argumentName, argumentValue));
      }
    }

    std::map<string,string> *state = session->getActivePage()->getState();

    if (type == "callback") {
      //Util::log("Executing callback action: " + actionId);
      for (auto& argument: arguments) {
        argument = Variables::replaceVariables(argument, argumentVariables);
        argument = Variables::replaceVariables(argument, *state, "state");
      }
      actionProvider->actionCallback(actionId, arguments);
    }
    else if (type == "composite") {
      //Util::log("Executing composite action: " + actionId);
      // Apply the arguments to every sub action.
      for (auto subAction: subActions) {
        string rawSubAction = subAction;
        rawSubAction = Variables::replaceVariables(rawSubAction, argumentVariables);
        rawSubAction = Variables::replaceVariables(rawSubAction, *state, "state");
        checkAction(rawSubAction);
        //Util::log("- " + rawSubAction);
        actionsManager->invokeAction(rawSubAction, session);
      }
      //Util::log("");
    }
  }

  void checkAction(string action) {
    string pattern = "\\$(_(STATE|ARGS)\\.)?[A-Z0-9_]+!?";
    if (Util::getUsedVariables(action, pattern).size() > 0) {
      string message = "Unkown variable in action: " + action;
      Util::error(message);
      throw message;
    }
  }
}