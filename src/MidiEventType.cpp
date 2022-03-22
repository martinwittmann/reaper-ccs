#ifndef CCS_MIDI_EVENT_TYPE_H
#define CCS_MIDI_EVENT_TYPE_H
namespace CCS {
  struct MidiEventType {
    unsigned int status;
    unsigned int data1;
  };
}
#endif