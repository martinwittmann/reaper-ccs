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
    short controlType,
    unsigned char status,
    unsigned char data1,
    MidiController* midiController,
    unsigned char onPressData2,
    unsigned char onReleaseData2
  ) {
    this->controlId = controlId;
    this->controlType = controlType;
    statusByte = status;
    data1Byte = data1;
    this->midiController = midiController;
    this->onPressData2 = onPressData2;
    this->onReleaseData2 = onReleaseData2;
  }

  short MidiControlElement::getTypeByName(string type) {
    if (type == "button") return BUTTON;
    if (type == "absolute") return ABSOLUTE;
    if (type == "relative") return RELATIVE;
    return UNKNOWN_CONTROL_TYPE;
  }

  short MidiControlElement::getType() {
    return controlType;
  }

  MidiEventType MidiControlElement::getEventType() {
    return MidiEventType{statusByte, data1Byte};
  }

  string MidiControlElement::getControlId() {
    return controlId;
  }

  int MidiControlElement::getInputEventId() {
    return Util::getMidiEventId(statusByte, data1Byte);
  }

  short MidiControlElement::getOnPressValue() {
    return onPressData2;
  }

  short MidiControlElement::getOnReleaseValue() {
    return onReleaseData2;
  }

  void MidiControlElement::onChange(std::vector<int> b) {

  }

  void MidiControlElement::onButtonPress(std::vector<int> b) {

  }

  void MidiControlElement::onButtonRelease(std::vector<int> b) {

  }
}