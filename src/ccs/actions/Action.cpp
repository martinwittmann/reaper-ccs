#include <string>
#include <vector>
#include <functional>
#include "Action.h"
#include "../Variables.h"
#include "../midi/MidiController.h"
#include "../Page.h"
#include "../CcsException.h"
#include "../Util.h"
#include "yaml-cpp/yaml.h"
#include "CompositeAction.h"

namespace CCS {

  using std::string;
  using std::vector;

  // By providing a vector of sub actions we create a composite action that
  // simply invokes all sub actions when executed.
  Action::Action(
    string providerId,
    string actionId,
    YAML::Node node,
    ActionsManager *actionsManager
  ) {
    m_type = "composite";
    m_providerId = providerId;
    m_actionId = actionId;
    m_actionsManager = actionsManager;
    m_compositeAction = new CompositeAction(providerId + "." + actionId, node);
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

    if (m_type == "callback") {
      //Util::log("Executing callback action: " + m_actionId);

      // Apply the current state to all given arguments.
      std::map<string,string> *state = session->getActivePage()->getState();
      for (auto& argument: arguments) {
        argument = Variables::replaceVariables(argument, *state, "state");
      }

      m_actionProvider->actionCallback(m_actionId, arguments);
    }
    else if (m_type == "composite") {
      //string debargs;
      //for (auto arg : arguments) {
        //debargs += ":" + arg;
      //}
      //Util::log("Executing composite action: " + m_actionId + ": " + debargs);
      m_compositeAction->invoke(arguments, session);
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