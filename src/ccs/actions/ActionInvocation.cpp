#include "ActionInvocation.h"
#include <string>
#include <vector>
#include "../Util.h"
#include "../Session.h"
#include "../Page.h"
#include "../CcsException.h"

using std::string;
using std::vector;

namespace CCS {

  ActionInvokation::ActionInvokation(
    Session *session,
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

      if (actionParts.size() != 2) {
        throw CcsException("Invalid action id: " + rawInvokation + "\n. The required format is: [action-provider].[action-id] - without the brackets.");
      }

      providerId = actionParts.at(0);
      actionId = actionParts.at(1);

      if (providerId == "page") {
        // Replace "page" with the currently active page id.
        Page *page = session->getActivePage();
        providerId = page->getPageId();
      }
    }

    this->actionId = actionId;
    this->providerId = providerId;

    for (auto it = std::next(parts.begin()); it != parts.end(); ++it) {
      arguments.push_back(*it);
    }
  }
}
