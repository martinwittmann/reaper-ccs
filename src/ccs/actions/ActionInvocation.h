#ifndef CCS_ACTION_INVOCATION
#define CCS_ACTION_INVOCATION

#include <string>
#include <vector>

namespace CCS {

  class Session;

  class ActionInvokation {
    const char separator = ':';
    Session *session;

  public:
    std::string providerId;
    std::string actionId;
    std::vector<std::string> arguments;


    ActionInvokation(
      Session *session,
      std::string rawInvokation,
      std::string providerId = ""
    );

    std::string removePrefixSuffix(std::string input);
  };
}
#endif
