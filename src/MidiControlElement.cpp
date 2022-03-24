#include <string>
#include "MidiEventType.cpp"
#include "MidiControlElement.h"
#include "Util.h"

namespace CCS {
  using std::string;

  /**
   * This represents a single control/ui element on a midi controller
   * like a knob, a button, a fader,...
   * Note that we do not use leds and other feedback elements here.
   */
  MidiControlElement::MidiControlElement(
    string typeName,
    unsigned int status,
    unsigned int data1
  ) {
    type = MidiControlElement::getType(typeName);
    statusByte = status;
    data1Byte = data1;
  }

  int MidiControlElement::getType(string type) {
    if (type == "button") return BUTTON;
    if (type == "fader") return FADER;
    if (type == "encoder_absolute") return ENCODER_ABSOLUTE;
    if (type == "encoder_relative") return ENCODER_RELATIVE;
    return UNKNOWN_CONTROL_TYPE;
  }

  MidiEventType MidiControlElement::getEventType() {
    return MidiEventType{statusByte, data1Byte};
  }

  int MidiControlElement::getInputEventId() {
    return Util::getMidiEventId(statusByte, data1Byte);
  }
}