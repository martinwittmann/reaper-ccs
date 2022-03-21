#include <string>

namespace CCS {

  class Action {
  public:
    Action() {
      std::string actionTypes[] = {
        "send_midi_message",
        "ccs",
        "reaper"
      };
    }
  };
}