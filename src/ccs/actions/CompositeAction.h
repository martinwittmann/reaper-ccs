#ifndef CCS_COMPOSITE_ACTION_H
#define CCS_COMPOSITE_ACTION_H

#include <string>
#include <vector>
#include <map>
#include "yaml-cpp/yaml.h"

namespace CCS {

  class Session;
  class CompositeActionConfig;

  using std::string;
  using std::vector;
  using std::map;

  typedef struct {
    string variable;
    string comparison_operator;
    string value;
  } composite_action_condition;

  class CompositeAction {
    string m_id;
    CompositeActionConfig *m_config;
    vector<CompositeAction* > m_subActions;
    vector<composite_action_condition> m_conditions;

    vector<string> m_argumentTypes;

    bool m_isSimpleAction = false;
    // This will contain a string for a simple action. E.g.:
    // "[session.load_page:page1]"
    string m_simpleAction;

  public:
    vector<string> m_argumentNames;
    CompositeAction(string id, YAML::Node actionRoot);

    bool conditionsAreMet(map<string,string> variables);

    bool conditionIsMet(composite_action_condition condition, map<string,string> variables);

    void addCondition(string key, string value);

    void invoke(map<string,string> variables, Session *session);

    void invokeSimpleAction(map<string,string> arguments, Session *session);

    void invoke(vector <string> arguments, Session *session);

    void checkArguments(map<string,string> variables);
  };
}
#endif
