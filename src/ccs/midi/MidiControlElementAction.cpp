#include "MidiControlElementAction.h"
#include <vector>
#include <map>
#include <string>
#include "yaml-cpp/yaml.h"
#include "../Util.h"
#include "../actions/ActionsManager.h"
#include "../Session.h"
#include "../Page.h"
#include "../Variables.h"
#include "../CcsException.h"

namespace CCS {
  using std::string;
  using std::vector;

  MidiControlElementAction::MidiControlElementAction(string action) {
    m_actions.push_back(action);
  }

  MidiControlElementAction::MidiControlElementAction(vector<string> actions) {
    m_actions = actions;
  }

  MidiControlElementAction::MidiControlElementAction(
    std::vector<string> actions,
    YAML::Node *conditions
  ) : MidiControlElementAction(actions) {
    if (conditions->Type() == YAML::NodeType::Null) {
      // Nothing to do.
      return;
    }

    if (conditions->Type() != YAML::NodeType::Map) {
      throw CcsException("MidiControlElementAction(): Conditions must be a yaml map.");
    }

    for (auto const &conditionRow: *conditions) {
      addCondition(conditionRow.first.as<string>(), conditionRow.second.as<string>());
    }
  }

  void MidiControlElementAction::addCondition(string key, string value) {
    string op;
    if (Util::regexMatch(key, "is$")) {
      op = "is";
    }
    else if (Util::regexMatch(key, "isnot$")) {
      op = "isnot";
    }
    else {
      throw CcsException("midicontrolelementaction::addcondition(): unknown operator");
    }

    string variable = Util::regexReplace(key, "\\.is(not)?$", "");
    m_conditions.push_back(midi_control_element_action_condition {
      variable,
      op,
      value
    });
  }

  bool MidiControlElementAction::conditionsAreMet(std::map<string,string> variables) {
    for (auto condition : m_conditions) {
      if (!conditionIsMet(condition, variables)) {
        return false;
      }
    }
    return true;
  }

  bool MidiControlElementAction::conditionIsMet(
    midi_control_element_action_condition condition,
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

  void MidiControlElementAction::invoke(
    Session *session,
    const string &value,
    const string &minValue,
    const string &maxValue,
    const string &formattedValue
  ) {
    std::map<string,string> variables;
    variables.insert(std::pair("VALUE", value));
    variables.insert(std::pair("FORMATTED_VALUE", formattedValue));
    variables.insert(std::pair("MIN_VALUE", minValue));
    variables.insert(std::pair("MAX_VALUE", maxValue));

    // Putting together stat before testing conditions, since conditions will
    // depend on state variables.
    for (auto &item : *session->getActivePage()->getState()) {
      variables.insert(std::pair(item.first, item.second));
    }

    if (!conditionsAreMet(variables)) {
      return;
    }

    for (auto rawAction : m_actions) {
      string action = Variables::replaceVariables(rawAction, variables, "state");
      session->getActionsManager()->invokeAction(action, session);
    }
  }
}
