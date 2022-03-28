#include "MidiControlElementAction.h"
#include <vector>
#include <map>
#include <string>
#include "yaml-cpp/yaml.h"
#include "../Util.h"
#include "../actions/ActionsManager.h"
#include "../Session.h"
#include <iostream>
#include "../Page.h"

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
      throw "MidiControlElementAction(): Conditions must be a yaml map.";
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
      throw "MidiControlElementAction::addCondition(): Unknown operator";
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
    string leftSide = Util::processString(condition.variable, variables, "\\$(_STATE\\.)?[A-Z0-9_]+!?");
    string rightSide = Util::processString(condition.value, variables, "\\$(_STATE\\.)?[A-Z0-9_]+!?");
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

    throw "Unknown condition operator";
  }

  void MidiControlElementAction::invoke(
    Session *session,
    string value
  ) {
    std::map<string,string> variables;
    variables.insert(std::pair("VALUE", value));

    for (auto &item : *session->getActivePage()->getState()) {
      variables.insert(std::pair("_STATE." + item.first, item.second));
    }

    if (!conditionsAreMet(variables)) {
      return;
    }

    for (auto rawAction : m_actions) {
      string action = Util::processString(rawAction, variables, "\\$(_STATE\\.)?[A-Z0-9_]+!?");
      session->getActionsManager()->invokeAction(action, session);
    }
  }
}
