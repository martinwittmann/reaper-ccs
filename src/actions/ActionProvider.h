#ifndef CCS_ACTION_PROVIDER_H
#define CCS_ACTION_PROVIDER_H

#include <string>
//#include "Actions.h"

namespace CCS {

  // We need to forward declare actions to not get into cyclic dependencies.
  class Actions;

  class ActionProvider {
    std::string id;
    Actions* manager;

    void registerAtManager();
  public:
    ActionProvider(std::string providerId, Actions* actionsManager);
    std::string getId();
  };
}

#endif