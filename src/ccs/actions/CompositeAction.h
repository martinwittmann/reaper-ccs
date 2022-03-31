#ifndef CCS_COMPOSITE_ACTION_H
#define CCS_COMPOSITE_ACTION_H

#include <string>
#include <vector>
#include <map>
#include "../Session.h"
#include "yaml-cpp/yaml.h"
#include "../config/CompositeActionConfig.h"

namespace CCS {
  typedef struct {
    std::string variable;
    std::string comparison_operator;
    std::string value;
  } composite_action_condition;

  class CompositeAction {
    std::string m_id;
    CompositeActionConfig *m_config;
    std::vector<CompositeAction* > m_subActions;
    std::vector<composite_action_condition> m_conditions;

    std::vector<std::string> m_argumentTypes;

    bool m_isSimpleAction = false;
    // This will contain a string for a simple action. E.g.:
    // "[session.load_page:page1]"
    std::string m_simpleAction;

  public:
    std::vector<std::string> m_argumentNames;
    CompositeAction(std::string id, YAML::Node actionRoot);

    bool conditionsAreMet(std::map<string,string> variables);

    bool conditionIsMet(composite_action_condition condition, std::map<string,string> variables);

    void addCondition(std::string key, std::string value);

    void invoke(std::map<string,string> variables, Session *session);

    void invokeSimpleAction(std::map<string,string> arguments, Session *session);

    void invoke(vector <string> arguments, Session *session);

    void checkArguments(std::map<string,string> variables);
  };
}
#endif
