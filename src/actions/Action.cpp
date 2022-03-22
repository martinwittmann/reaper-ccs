#include <string>
#include "Action.h"

namespace CCS {

  Action::Action(std::string providerId, std::string id) {
    this->providerId = providerId;
    this->id = id;
  }
}