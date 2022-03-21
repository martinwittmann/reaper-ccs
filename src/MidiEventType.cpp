#ifndef CCS_MIDI_EVENT_TYPE_H_
#define CCS_MIDI_EVENT_TYPE_H_
namespace CCS {
  struct MidiEventType {
    unsigned int status;
    unsigned int data1;
  };
}
#endif