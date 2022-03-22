#ifndef CCS_ACTION_H
#define CCS_ACTION_H

#include <string>

namespace CCS {

  class Action {
  public:
    const static int CCS = 0;
    const static int SEND_MIDI = 0;
    const static int REAPER = 0;

    // A string identifier unique within the scope of the given provider id.
    std::string id;

    // This can be the id of a midi controller, "reaper" for reaper actions
    // (see ActionProviderReaper class) or "ccs" for actions ccs itself provides.
    std::string providerId;

    Action(std::string providerId, std::string id);
  };
}

#endif