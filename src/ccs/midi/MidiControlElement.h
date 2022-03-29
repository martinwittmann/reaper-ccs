#ifndef CCS_MIDI_CONTROL_ELEMENT_H
#define CCS_MIDI_CONTROL_ELEMENT_H

#include <string>
#include <vector>
#include "MidiEventType.cpp"

namespace CCS {
  using std::string;

  class MidiController;

  /**
    *This represents a single control/ui element on a midi controller
    *like a knob, a button, a fader,...
    *Note that we do not use leds and other feedback elements here.
   */
  class MidiControlElement {
    string controlId;
    short controlType;
    // The status byte of the midi message received for this control element.
    // We assume that the status stays the same, no matter which action this
    // element triggers.
    // For example when pushing and releasing a button we expect the same status
    // and data1 bytes assuming a midi cc status and the corresponding cc number
    // as data1.
    // For relative encoders we also expect status and data1 to stay the same
    // the the received data2 byte to contain the direction and speed information.
    unsigned char statusByte;
    unsigned char data1Byte;
    unsigned char onPressData2;
    unsigned char onReleaseData2;
    MidiController *midiController;

  public:
    const static int UNKNOWN_CONTROL_TYPE = -1;
    const static int BUTTON = 0;
    const static int ABSOLUTE = 1;
    const static int RELATIVE = 2;

    MidiControlElement(
      string controlId,
      short controlType,
      unsigned char status,
      unsigned char data1,
      MidiController *midiController,
      unsigned char onPressData2 = 0x7F,
      unsigned char onReleaseData2 = 0x00
    );

    short getType();
    string getControlId();
    static short getTypeByName(string type);
    MidiEventType getEventType();
    int getInputEventId();
    short getOnPressValue();
    short getOnReleaseValue();
  };
}

#endif