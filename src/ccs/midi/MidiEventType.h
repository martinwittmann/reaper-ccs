#ifndef CCS_MIDI_EVENT_TYPE_H
#define CCS_MIDI_EVENT_TYPE_H
namespace CCS {
  struct MidiEventType {
    unsigned char status;
    unsigned char data1;
  };
}
#endif