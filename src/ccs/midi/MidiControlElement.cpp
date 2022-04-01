#include <string>
#include "MidiEventType.h"
#include "../Util.h"
#include "MidiControlElement.h"
#include "MidiController.h"

namespace CCS {
  using std::string;

  /**
    *This represents a single control/ui element on a midi controller
    *like a knob, a button, a fader,...
    *Note that we do not use leds and other feedback elements here.
   */
  MidiControlElement::MidiControlElement(
    string controlId,
    short controlType,
    unsigned char status,
    unsigned char data1,
    MidiController *midiController,
    unsigned char onPressData2,
    unsigned char onReleaseData2
  ) {
    m_midiController = midiController;
    m_controllerId = m_midiController->getControllerId();
    m_controlId = controlId;
    m_controlType = controlType;
    m_statusByte = status;
    m_data1Byte = data1;
    m_onPressData2 = onPressData2;
    m_onReleaseData2 = onReleaseData2;
  }

  short MidiControlElement::getTypeByName(string type) {
    if (type == "button") return BUTTON;
    if (type == "absolute") return ABSOLUTE;
    if (type == "relative") return RELATIVE;
    return UNKNOWN_CONTROL_TYPE;
  }

  short MidiControlElement::getType() {
    return m_controlType;
  }

  MidiEventType MidiControlElement::getEventType() {
    return MidiEventType{m_statusByte, m_data1Byte};
  }

  string MidiControlElement::getControlId() {
    return m_controlId;
  }

  int MidiControlElement::getInputEventId() {
    return Util::getMidiEventId(m_statusByte, m_data1Byte);
  }

  short MidiControlElement::getOnPressValue() {
    return m_onPressData2;
  }

  short MidiControlElement::getOnReleaseValue() {
    return m_onReleaseData2;
  }
}