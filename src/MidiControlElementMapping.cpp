#include "MidiControlElementMapping.h"
#include <vector>
#include <iostream>
#include "Util.h"

namespace CCS {

  using std::string;

  MidiControlElementMapping::MidiControlElementMapping(
    int midiEventId,
    string rawControlId,
    YAML::Node config,
    MidiControlElement* midiControlElement
  ) {
    this->midiEventId = midiEventId;
    this->config = config;
    this->midiControlElement = midiControlElement;
    std::vector idParts = Util::splitString(rawControlId, '.');
    string controllerId = idParts.at(0);
    this->controllerId = controllerId;
    string controlId = idParts.at(1);
    this->controlId = controlId;
  }

  void MidiControlElementMapping::onMidiEvent(int eventId, unsigned char dataByte) {
    std::cout << "Midi event data: " << Util::formatHexByte(dataByte) << "\n";
  }

}