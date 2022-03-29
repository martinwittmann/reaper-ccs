#include <string>
#include <vector>
#include <functional>
#include "Action.h"
#include "../Variables.h"
#include "../midi/MidiController.h"
#include "../Page.h"
#include "../CcsException.h"
#include "../Util.h"

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
    m_type = "composite";
    m_providerId = providerId;
    m_actionId = actionId;
    m_argumentNames = argumentNames;
    m_argumentTypes = argumentTypes;
    m_subActions = subActions;
    m_actionsManager = actionsManager;

    if (m_subActions.size() < 1) {
      Util::error("Action " + m_providerId + "." + m_actionId + " does not have any subactions.");
    }
  }

  Action::Action(
    string providerId,
    string actionId,
    ActionProvider *actionProvider
  ) {
    m_type = "callback";
    m_providerId = providerId;
    m_actionId = actionId;
    m_actionProvider = actionProvider;
  }

  string Action::getActionId() {
    return m_actionId;
  }

  string Action::getProviderId() {
    return m_providerId;
  }

  void Action::invoke(vector<string> arguments, Session *session) {
    // We map the given arguments in the same order as the vector of
    // argument names we got in the constructor.
    std::map<string, string> argumentVariables;
    if (!m_argumentNames.empty()) {
      for (auto i = 0; i < arguments.size(); i++) {
        string argumentValue = arguments.at(i);

        if (i >= m_argumentNames.size()) {
          // We received more arguments than we expect.
          throw CcsException("Received unknown superfluous argument '" + argumentValue + "' for action " + m_providerId + "." + m_actionId);
        }
        string argumentType = m_argumentTypes.at(i);
        if (argumentType == "string") {
          argumentValue = Util::strToHexBytes(argumentValue);
        }
        string argumentName = "_ARGS." + m_argumentNames.at(i);

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

    if (m_type == "callback") {
      Util::log("Executing callback action: " + m_actionId);
      for (auto& argument: arguments) {
        argument = Variables::replaceVariables(argument, argumentVariables);
        argument = Variables::replaceVariables(argument, *state, "state");
      }
      m_actionProvider->actionCallback(m_actionId, arguments);
    }
    else if (m_type == "composite") {
      Util::log("Executing composite action: " + m_actionId);
      // Apply the arguments to every sub action.
      for (auto rawSubAction: m_subActions) {
        string subAction = rawSubAction;
        subAction = Variables::replaceVariables(subAction, argumentVariables);
        subAction = Variables::replaceVariables(subAction, *state, "state");
        checkAction(subAction);
        Util::log("- " + subAction);
        m_actionsManager->invokeAction(subAction, session);
      }
      //Util::log("");
    }
  }

  void Action::checkAction(string action) {
    string pattern = "\\$(_(STATE|ARGS)\\.)?[A-Z0-9_]+!?";
    if (Util::getUsedVariables(action, pattern).size() > 0) {
      string message = "Unkown variable in action: " + action;
      throw CcsException(message);
    }
  }
}