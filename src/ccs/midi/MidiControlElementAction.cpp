#include "MidiControlElementAction.h"
#include <vector>
#include <string>
#include "yaml-cpp/yaml.h"
#include "../Util.h"
#include "../actions/ActionsManager.h"
#include "../Session.h"

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

    // We can iterate the same way over sequences and maps, so no check is needed.
    for (auto const &conditionRow: *conditions) {
      addCondition(conditionRow.first.as<string>(), conditionRow.second.as<string>());
    }
  }

  void MidiControlElementAction::addCondition(string key, string value) {
    string op;
    if (Util::regexMatch(key, "\\.is")) {
      op = "is";
    }
    else if (Util::regexMatch(key, "\\.isnot")) {
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

  bool MidiControlElementAction::conditionsAreMet(vector<string> variables) {
    for (auto condition : m_conditions) {
      if (!conditionIsMet(condition, variables)) {
        return false;
      }
    }
    return true;
  }

  bool MidiControlElementAction::conditionIsMet(
    midi_control_element_action_condition condition,
    map<string,string> variables
  ) {
    string leftSide = Util::processString(condition.variable, variables);
    if (condition.comparison_operator == "is" || condition.comparison_operator == "equals") {
      return
    }
  }

  void MidiControlElementAction::invoke(
    ActionsManager *manager,
    Session *session,
    vector<string> variables
  ) {
    if (!conditionsAreMet(variables)) {
      return;
    }

    for (auto action : m_actions) {
      manager->invokeAction(action, session);
    }
  }
}
