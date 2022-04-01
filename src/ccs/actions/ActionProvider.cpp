#include "ActionProvider.h"
#include "ActionsManager.h"
#include "Action.h"
#include <string>

namespace CCS {

  using std::string;
  using std::vector;

  // Note that we do not set the provider's id here yet.
  // The reason for this is that we want instances of the MidiController class
  // to inherit from ActionProvider and that the controller's id is read inside
  // its constructor from MidiControllerConfig.
  // Since base class constructors are being called before the actual constructor
  // simply do not have the controller's id yet, when instantiating ActionProvider.
  // We work around the issue by moving setting the provider id into
  // registerActionProvider.
  // I know, this is a side-effect situation, but until I find a cleaner way
  // we're sticking with this.
  ActionProvider::ActionProvider(ActionsManager *actionsManager) {
    m_actionsManager = actionsManager;
  }

  ActionProvider::~ActionProvider() {
    for (auto &action : m_providedActions) {
      delete action;
    }
    m_providedActions.clear();
  }

  void ActionProvider::registerActionProvider(string providerId) {
    m_actionProviderId = providerId;
    m_actionsManager->registerProvider(this);
  }

  string ActionProvider::getProviderId() {
    return m_actionProviderId;
  }

  void ActionProvider::actionCallback(string actionName, vector<string> arguments) {}

  bool ActionProvider::isMacroAction(string rawAction) {
    // If the string contains brackets we think it's a macro action because we
    // define that macros/subactions are being called by using
    // [action_provider.action_id:argument1:argument2:...]
    return rawAction.find("[") != -1;
  }

  void ActionProvider::provideAction(Action *action) {
    m_providedActions.push_back(action);
    m_actionsManager->registerAction(action);
  }
}