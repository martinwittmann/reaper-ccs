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

}
