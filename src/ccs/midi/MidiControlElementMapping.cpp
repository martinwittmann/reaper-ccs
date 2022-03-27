#include "MidiControlElementMapping.h"
#include <vector>
#include <iostream>

namespace CCS {

  using std::string;

  MidiControlElementMapping::MidiControlElementMapping(
    int midiEventId,
    string controlId,
    YAML::Node configRoot,
    MidiControlElement* controlElement,
    ReaperApi* api
  ) {
    this->midiEventId = midiEventId;
    config = new MappingConfig(&configRoot);
    this->controlElement = controlElement;
    this->controlId = controlId;
    controlType = controlElement->getType();
    onPressValue = controlElement->getOnPressValue();
    onReleaseValue = controlElement->getOnReleaseValue();
    this->api = api;

    if (controlId == "encoder1") {
      /*
      auto subscriber = dynamic_cast<ReaperEventSubscriber*>(this);
      MediaTrack* track = this->api->getTrack(1);
      this->api->subscribeToFxParameterChanged(
        track,
        0,
        0,
        subscriber
      );
      */
    }
  }

  MidiControlElementMapping::~MidiControlElementMapping() {
    delete config;
  }

  void MidiControlElementMapping::onMidiEvent(int eventId, unsigned char data2) {
    std::cout << "Midi event data: " << Util::formatHexByte(data2) << "\n";
    switch (controlType) {
      case MidiControlElement::BUTTON:
        if (data2 == onPressValue) {
          value = data2;
          //onButtonPress(eventId, data2);
        }
        else if (data2 == onReleaseValue) {
          value = data2;
          //onButtonRelease(eventId, data2);
        }
        break;

      case MidiControlElement::ABSOLUTE:
        value = data2;
        //onChange(eventId, data2);
        break;

      case MidiControlElement::RELATIVE:
        std::cout << 64 - data2 << "\n";
        char diff = 64 - data2;
        value += diff;
        //onChange(eventId, data2);
        break;
    }
  }

  int MidiControlElementMapping::getMidiEventId() {
    return midiEventId;
  }

  void MidiControlElementMapping::onFxParameterChanged(
    MediaTrack *track,
    int fxId,
    int paramId,
    double value
    ) {
    std::cout << "Fx param changed:\n";
    std::cout << fxId << " " << paramId << " " << value << "\n";
  }
}