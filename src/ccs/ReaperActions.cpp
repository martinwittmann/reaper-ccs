#include "ReaperActions.h"
#include "../reaper-api/ReaperApi.h"
#include "actions/Action.h"

namespace CCS {
  ReaperActions::ReaperActions(
    ActionsManager *actionsManager,
    ReaperApi *reaperApi
  ) : ActionProvider(actionsManager) {

    m_actionsManager = actionsManager;
    m_api = reaperApi;
    registerActionProvider("reaper");
    createActions();
  }


  void ReaperActions::createActions() {
    vector<string> actionNames = {
      "play",
      "pause",
      "stop",
      "record",
      "toggle_repeat",
      "rewind",
      "fast_forward",
    };

    auto provider = dynamic_cast<ActionProvider*>(this);
    for (auto actionName : actionNames) {
      auto action = new Action(
        "reaper",
        actionName,
        provider
      );
      provideAction(action);
    }
  }

  void ReaperActions::actionCallback(std::string actionName, std::vector<std::string> arguments) {
    if (actionName == "play") {
      m_api->play();
    }
    else if (actionName == "pause") {
      m_api->pause();
    }
    else if (actionName == "stop") {
      m_api->stop();
    }
    else if (actionName == "record") {
      m_api->record();
    }
    else if (actionName == "toggle_repeat") {
      m_api->toggleRepeat();
    }
    else if (actionName == "rewind") {
      m_api->rewind();
    }
    else if (actionName == "fast_forward") {
      m_api->fastForward();
    }
  }
}
