#ifndef CCS_MIDI_CONTROL_EVENT_H
#define CCS_MIDI_CONTROL_EVENT_H

#include <string>
#include <vector>

namespace CCS {

  class Session;

  using std::string;
  using std::vector;

  class MidiControlEvent {

    vector<string> subActions;
    // Each tuple consists of variable name, operator, value.
    vector<std::tuple<string, string, string>> conditions;

  public:
    MidiControlEvent(vector<string> subActions);
  };
}

#endif
