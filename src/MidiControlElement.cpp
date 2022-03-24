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
    string controlId,
    string typeName,
    unsigned char status,
    unsigned char data1,
    MidiController* midiController,
    unsigned char onPressData2,
    unsigned char onReleaseData2
  ) {
    this->controlId = controlId;
    type = getType(typeName);
    statusByte = status;
    data1Byte = data1;
    this->midiController = midiController;
    this->onPressData2 = onPressData2;
    this->onReleaseData2 = onReleaseData2;
  }

  int MidiControlElement::getType(string type) {
    if (type == "button") return BUTTON;
    if (type == "absolute") return ABSOLUTE;
    if (type == "relative") return RELATIVE;
    return UNKNOWN_CONTROL_TYPE;
  }

  MidiEventType MidiControlElement::getEventType() {
    return MidiEventType{statusByte, data1Byte};
  }

  int MidiControlElement::getInputEventId() {
    return Util::getMidiEventId(statusByte, data1Byte);
  }
}