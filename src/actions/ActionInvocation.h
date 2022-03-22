#ifndef CCS_ACTION_INVOCATION
#define CCS_ACTION_INVOCATION

#include <string>
#include <vector>

namespace CCS {
  class ActionInvokation {
    const char separator = ':';

  public:
    std::string providerId;
    std::string actionId;
    std::vector<std::string> arguments;


    ActionInvokation(std::string rawInvokation, std::string providerId = "");

    std::string removePrefixSuffix(std::string input);
  };
}
#endif
