#ifndef CCS_ACTION_PROVIDER_H
#define CCS_ACTION_PROVIDER_H

#include <string>
#include <vector>

namespace CCS {

  // We need to forward declare actions to not get into cyclic dependencies.
  class Actions;

  class ActionProvider {
    std::string id;

  protected:
    Actions* actionsManager;

  public:
    explicit ActionProvider(Actions* actionsManager);
    std::string getId();
    void registerActionProvider(std::string providerId);
    virtual void actionCallback(std::string actionName, std::vector<std::string> arguments);

    // Note that createActions needs to be called from the derived class since
    // in many cases the actions created will depend on data loaded in its
    // constructors.
    // Since this constructor will be called before, only the derived class can
    // know, if/when to create its actions.
    virtual void createActions();

    bool isMacroAction(std::string rawAction);
  };
}

#endif