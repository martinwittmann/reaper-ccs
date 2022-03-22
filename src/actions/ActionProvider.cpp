#include "ActionProvider.h"
#include "Actions.h"
#include <string>

namespace CCS {

  ActionProvider::ActionProvider(std::string providerId, Actions* actionsManager) {
    id = providerId;
    this->manager = actionsManager;
  }

  void ActionProvider::registerAtManager() {
    manager->registerProvider(this);
  }

  std::string ActionProvider::getId() {
    return id;
  }
}