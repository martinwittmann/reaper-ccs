#include "CompositeAction.h"
#include <string>
#include "../Util.h"
#include "../CcsException.h"
#include "../Variables.h"
#include "../Session.h"
#include "../Page.h"
#include "yaml-cpp/yaml.h"

namespace CCS {

  using std::string;

  CompositeAction::CompositeAction(string id, YAML::Node actionRoot) {
    m_id = id;
    m_config = new CompositeActionConfig(&actionRoot);

    if (actionRoot.Type() == YAML::NodeType::Scalar) {
      // This will be a simple action. No subactions or conditions.
      m_isSimpleAction = true;
      m_simpleAction = actionRoot.as<string>();
    }
    else if (actionRoot.Type() == YAML::NodeType::Sequence) {
      // This has a list of subactions.
      int i = 0;
      for (auto rawSubAction : actionRoot) {
        YAML::Node item = rawSubAction;
        CompositeAction *subAction = new CompositeAction(m_id + "." + string(m_id), item);
        m_subActions.push_back(subAction);
        i++;
      }
    }
    else if (actionRoot.Type() == YAML::NodeType::Map) {
      // Must be in the form:
      // conditions:
      //   VARIABLE.is: VALUE
      //   VAR2.isnot: VALUE2
      // actions:
      // - [subaction]
      // Subaction can again be a map with conditions and actions.
      if (!m_config->keyExists("actions")) {
        throw CcsException("Trying to create a CompositeAction from a yaml map that does not contain the actions key.");
      }

      // Set up conditions for this action.
      if (m_config->keyExists("conditions", &actionRoot)) {
        YAML::Node conditions = actionRoot["conditions"];

        if (conditions.Type() != YAML::NodeType::Map) {
          throw CcsException("CompositeAction(): Conditions must be a yaml map.");
        }

        for (auto const &conditionRow : conditions) {
          addCondition(conditionRow.first.as<string>(), conditionRow.second.as<string>());
        }
      }

      // Set up arguments.
      m_argumentNames = m_config->getListValues("arguments", &actionRoot);
      for (auto &argument : m_argumentNames) {
        if (argument.substr(argument.length() - 1) == "!") {
          argument = argument.substr(0, argument.length() - 1);
          m_argumentTypes.push_back("string");
        }
        else {
          m_argumentTypes.push_back("byte");
        }
      }

      // Set up the actual subactions.
      int i = 0;
      for (auto rawSubAction : actionRoot["actions"]) {
        YAML::Node item = rawSubAction;
        string tmpId = m_id + "." + std::to_string(i);
        auto subAction = new CompositeAction(tmpId, item);
        m_subActions.push_back(subAction);
        i++;
      }
    }
    else {
      throw CcsException("Trying to create CompositeAction from an invalid yaml node.");
    }

    if (!m_isSimpleAction && m_subActions.size() < 1) {
      Util::error("CompositeAction " + m_id + " does not have any subactions.");
    }
  }

  void CompositeAction::addCondition(string key, string value) {
    string op;
    if (Util::regexMatch(key, "\\.is$")) {
      op = "is";
    }
    else if (Util::regexMatch(key, "\\.isnot$")) {
      op = "isnot";
    }
    else {
      throw CcsException("CompositeAction::addCondition(): Unknown operator.");
    }

    string variable = Util::regexReplace(key, "\\.is(not)?$", "");
    m_conditions.push_back(composite_action_condition {
      variable,
      op,
      value
    });
  }

  bool CompositeAction::conditionsAreMet(std::map<string,string> variables) {
    for (auto condition : m_conditions) {
      if (!conditionIsMet(condition, variables)) {
        return false;
      }
    }
    return true;
  }

  bool CompositeAction::conditionIsMet(
    composite_action_condition condition,
    std::map<string,string> variables
  ) {
    string leftSide = Variables::replaceVariables(condition.variable, variables, "state");
    string rightSide = Variables::replaceVariables(condition.value, variables, "state");
    if (condition.comparison_operator == "is" || condition.comparison_operator == "equals") {
      return leftSide == rightSide;
    }
    else if (
      condition.comparison_operator == "isnot" ||
      condition.comparison_operator == "notis" ||
      condition.comparison_operator == "notequals" ||
      condition.comparison_operator == "equalsnot"
      ) {
      return leftSide != rightSide;
    }

    throw CcsException("Unknown condition operator");
  }

  void CompositeAction::invoke(vector<string> arguments, Session *session) {
    // We map the given arguments in the same order as the vector of
    // argument names we got in the constructor.
    std::map<string, string> argumentVariables;

    if (!m_argumentNames.empty()) {
      for (auto i = 0; i < arguments.size(); i++) {
        string argumentValue = arguments.at(i);

        /*
        if (i >= m_argumentNames.size()) {
          // We received more arguments than we expect.
          throw CcsException("Received unknown superfluous argument '" + argumentValue + "' for action " + m_id);
        }
        */
        string argumentType = m_argumentTypes.at(i);
        if (argumentType == "string") {
          argumentValue = Util::strToHexBytes(argumentValue) + " 00";
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

    invoke(argumentVariables, session);
  }

  void CompositeAction::invoke(std::map<string,string> variables, Session *session) {
    try {
      checkArguments(variables);

      // We need to inject the current state before invoking simple actions and
      // before testing conditions since they can depend on the current state.
      std::map<string,string> *state = session->getActivePage()->getState();
      variables.insert(state->begin(), state->end());

      if (m_isSimpleAction) {
        return invokeSimpleAction(variables, session);
      }

      if (!conditionsAreMet(variables)) {
        return;
      }

      for (auto subAction: m_subActions) {
        subAction->invoke(variables, session);
      }
    }
    catch (CcsException &e) {
      Util::error("Error invoking action:");
      Util::error(e.what());
    }
  }

  void CompositeAction::invokeSimpleAction(
    std::map<string,string> arguments,
    Session *session
  ) {
    string action = Variables::replaceVariables(m_simpleAction, arguments);
    session->invokeAction(action, session);
  }

  void CompositeAction::checkArguments(std::map<string,string> variables) {
    // TODO Implement a better check for variables.
    vector<string> arguments;
    for (auto variable : variables) {
      if (variable.first.substr(0, 6) == "_ARGS.") {
        string name = Util::regexReplace(variable.first, "^_ARGS.", "");
        arguments.push_back(name + ": " + variable.second);
      }
    }
    if (arguments.size() < m_argumentNames.size()) {
      string message = "Invoked composite action " + m_id + " with invalid arguments:\n";
      for (auto arg : arguments) {
        message += " - " + arg + "\n";
      }
      message += "\nExpected these arguments:\n";
      for (auto arg : m_argumentNames) {
        message += " - " + arg + "\n";
      }
      throw CcsException(message);
    }
  }

}