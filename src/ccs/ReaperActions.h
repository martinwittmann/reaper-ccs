#ifndef CCS_REAPER_ACTIONS_H
#define CCS_REAPER_ACTIONS_H
#include "actions/ActionProvider.h"
#include "../reaper-api/ReaperApi.h"

namespace CCS {

  class ReaperApi;

  // This class only serves as action provider for global reaper actions.
  class ReaperActions : public ActionProvider {
    ReaperApi *m_api;
  public:
    ReaperActions(ActionsManager *actionsManager, CCS::ReaperApi *reaperApi);

    void createActions();

    void actionCallback(string actionName, vector <string> arguments);
  };

}
#endif
