#include "CompositeAction.h"
#include <string>
#include "../Util.h"
#include "../CcsException.h"
#include "../Variables.h"
#include "../Session.h"
#include "yaml-cpp/yaml.h"

namespace CCS {

  using std::string;

  CompositeAction::CompositeAction(YAML::Node actionRoot) {
    m_config = new CompositeActionConfig(&actionRoot);

    if (actionRoot.Type() == YAML::NodeType::Scalar) {
      // This will be a simple action. No subactions or conditions.
      m_isSimpleAction = true;
      m_simpleAction = actionRoot.as<string>();
    }
    else if (actionRoot.Type() == YAML::NodeType::Sequence) {
      // This has a list of subactions.
      for (auto rawSubAction : actionRoot) {
        YAML::Node item = rawSubAction;
        CompositeAction *subAction = new CompositeAction(item);
        m_subActions.push_back(subAction);
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

      if (m_config->keyExists("conditions", &actionRoot)) {
        YAML::Node conditions = actionRoot["conditions"];

        if (conditions.Type() != YAML::NodeType::Map) {
          throw CcsException("CompositeAction(): Conditions must be a yaml map.");
        }

        for (auto const &conditionRow : conditions) {
          addCondition(conditionRow.first.as<string>(), conditionRow.second.as<string>());
        }
      }

      for (auto rawSubAction : actionRoot["actions"]) {
        YAML::Node item = rawSubAction;
        auto subAction = new CompositeAction(item);
        m_subActions.push_back(subAction);
      }
    }
    else {
      throw CcsException("Trying to create CompositeAction from an invalid yaml node.");
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

  void CompositeAction::invoke(std::map<string, string> variables, Session *session) {
    try {
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
    std::map<string,string> variables,
    Session *session
  ) {
    string action = Variables::replaceVariables(m_simpleAction, variables, "state");
    session->invokeAction(action, session);
  }

}