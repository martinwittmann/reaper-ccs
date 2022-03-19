#include <string>

using std::string;

/**
 * This represents a single control/ui element on a midi controller
 * like a knob, a button, a fader,...
 * Note that we do not use leds and other feedback elements here.
 */
class CcsMidiControlElement {
    int type;
    // The status byte of the midi message received for this control element.
    // We assume that the status stays the same, no matter which action this
    // element triggers.
    // For example when pushing and releasing a button we expect the same status
    // and data1 bytes assuming a midi cc status and the corresponding cc number
    // as data1.
    // For relative encoders we also expect status and data1 to stay the same
    // the the received data2 byte to contain the direction and speed information.
    unsigned int statusByte;
    unsigned int data1Byte;


public:
    const static int UNKNOWN_CONTROL_TYPE = -1;
    const static int BUTTON = 0;
    const static int ENCODER_ABSOLUTE = 1;
    const static int ENCODER_RELATIVE = 2;
    const static int FADER = 3;

    CcsMidiControlElement(int elementType, unsigned int status, unsigned int data1) {
      type = elementType;
      statusByte = status;
      data1Byte = data1;
    }

    static int getType(string type) {
      if (type == "button") return BUTTON;
      if (type == "fader") return FADER;
      if (type == "encoder_absolute") return ENCODER_ABSOLUTE;
      if (type == "encoder_relative") return ENCODER_RELATIVE;
      return UNKNOWN_CONTROL_TYPE;
    }
};