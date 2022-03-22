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

  public:
    explicit ActionProvider(Actions* actionsManager);
    std::string getId();
    void registerActionProvider(std::string providerId);
  };
}

#endif