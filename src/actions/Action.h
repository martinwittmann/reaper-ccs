#ifndef CCS_ACTION_H
#define CCS_ACTION_H

#include <string>
#include <vector>
#include "Actions.h"

namespace CCS {

  using std::string;
  using std::vector;

  class Action {
    // A string identifier unique within the scope of the given provider id.
    string id;

    // This can be the id of a midi controller, "reaper" for reaper actions
    // (see ActionProviderReaper class) or "ccs" for actions ccs itself provides.
    string providerId;

    int type;

    vector<string> subActions;

    void (ActionProvider::*callback)(vector<string>) = NULL;
    vector<string> argumentNames;

    Actions* actionsManager;

    ActionProvider* actionProvider;

  public:
    // Available action types:
    // Actions that get a callback function.
    const static int CALLBACK = 0;
    // Actions can invoke other actions. This way users can combine multiple
    // actions into easy to use composite actions.
    const static int COMPOSITE = 1;

    // This creates an action of type COMPOSITE.
    Action(
      string providerId,
      string id,
      vector<string> argumentNames,
      vector<string> subActions,
      Actions* actionsManager
    );

    // This creates an action of type CALLBACK.
    Action(
      string providerId,
      string id,
      void (ActionProvider::*callback)(vector<string>),
      ActionProvider* actionProvider
    );

    void invoke(vector<string> arguments);
  };
}

#endif