#include <string>

class CcsAction {
public:
    CcsAction() {
      std::string actionTypes[] = {
        "send_midi_message",
        "ccs",
        "reaper"
      };
    }
};