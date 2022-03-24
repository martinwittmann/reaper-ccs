#ifndef CCS_ACTION_H
#define CCS_ACTION_H

#include <string>
#include <vector>
#include "ActionsManager.h"
#include "ActionProvider.h"

namespace CCS {

  using std::string;
  using std::vector;

  class Action {
    // A string identifier unique within the scope of the given provider id.
    string actionId;

    // This can be the id of a midi controller, "reaper" for reaper actions
    // (see ActionProviderReaper class) or "ccs" for actions ccs itself provides.
    string providerId;

    string type;

    vector<string> subActions;

    void (ActionProvider::*callback)(vector<string>) = NULL;
    vector<string> argumentNames;
    vector<string> argumentTypes;

    ActionsManager* actionsManager;

    ActionProvider* actionProvider;

  public:

    // This creates an action of type COMPOSITE.
    Action(
      string providerId,
      string actionId,
      vector<string> argumentNames,
      vector<string> argumentTypes,
      vector<string> subActions,
      ActionsManager* actionsManager
    );

    // This creates an action of type CALLBACK.
    Action(
      string providerId,
      string actionId,
      ActionProvider* actionProvider
    );

    void invoke(vector<string> arguments);

    string getId();
    string getProviderId();
  };
}

#endif