#include "ActionInvocation.h"
#include <string>
#include <vector>
#include "../Util.h"

using std::string;
using std::vector;

namespace CCS {

  ActionInvokation::ActionInvokation(string rawInvokation, string providerId) {
    string invokation = Util::removePrefixSuffix(rawInvokation);
    vector<string> parts = Util::splitString(invokation, separator);
    string actionId = parts.at(0);

    if (providerId.length() < 1) {
      char actionSeparator = '.';
      vector<string> actionParts = Util::splitString(actionId, actionSeparator);
      providerId = actionParts.at(0);
      actionId = actionParts.at(1);
    }

    this->actionId = actionId;
    this->providerId = providerId;

    for (auto it = std::next(parts.begin()); it != parts.end(); ++it) {
      arguments.push_back(*it);
    }
  }
}
