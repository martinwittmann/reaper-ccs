#ifndef CCS_MIDI_CONTROL_ELEMENT_ACTION_H
#define CCS_MIDI_CONTROL_ELEMENT_ACTION_H

#include <vector>
#include <string>
#include <map>
#include "yaml-cpp/yaml.h"
#include "../actions/ActionsManager.h"
#include "../Session.h"

namespace CCS {
  typedef struct {
    std::string variable;
    std::string comparison_operator;
    std::string value;
  } midi_control_element_action_condition;

  class MidiControlElementAction {
    std::vector<std::string> m_actions;
    std::vector<midi_control_element_action_condition> m_conditions;

  public:
    explicit MidiControlElementAction(std::string action);
    explicit MidiControlElementAction(std::vector<std::string> actions);
    MidiControlElementAction(
      std::vector<std::string> actions,
      YAML::Node *conditions
    );

    void addCondition(std::string key, std::string value);

    void invoke(
      ActionsManager *manager,
      Session *session,
      std::vector<string> variables
    );

    bool conditionsAreMet(vector <string> variables);

    bool conditionIsMet(midi_control_element_action_condition, vector <string> variables);

    bool conditionIsMet(midi_control_element_action_condition condition, map <string, string> variables);
  };
}
#endif