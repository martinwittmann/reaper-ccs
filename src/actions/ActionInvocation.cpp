#include "ActionInvocation.h"
#include <string>
#include <vector>
#include "../Util.h"
#include "../Session.h"
#include "../Page.h"

using std::string;
using std::vector;

namespace CCS {

  ActionInvokation::ActionInvokation(
    Session* session,
    string rawInvokation,
    string providerId
  ) {
    this->session = session;
    string invokation = Util::removePrefixSuffix(rawInvokation);
    vector<string> parts = Util::splitString(invokation, separator);
    string actionId = parts.at(0);

    if (providerId.length() < 1) {
      char actionSeparator = '.';
      vector<string> actionParts = Util::splitString(actionId, actionSeparator);
      providerId = actionParts.at(0);
      actionId = actionParts.at(1);

      if (actionId == "page") {
        // Replace "page" with the currently active page id.
        Page* page = session->getActivePage();
        actionId = Util::regexReplace(actionId, "^\\[page\\.", "[" + page->getId() + ".");
        int a = 1;
      }
    }

    this->actionId = actionId;
    this->providerId = providerId;

    for (auto it = std::next(parts.begin()); it != parts.end(); ++it) {
      arguments.push_back(*it);
    }
  }
}
