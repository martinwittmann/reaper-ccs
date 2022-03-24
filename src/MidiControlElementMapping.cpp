#include "MidiControlElementMapping.h"
#include <vector>
#include "Util.h"

namespace CCS {

  using std::string;

  MidiControlElementMapping::MidiControlElementMapping(
    string rawControlId,
    YAML::Node config,
    MidiController* midiController
  ) {
    this->config = config;
    this->midiController = midiController;
    std::vector idParts = Util::splitString(rawControlId, '.');
    controllerId = idParts.at(0);
    controlId = idParts.at(1);
  }

  int MidiControlElementMapping::getMidiEventId() {
    return midiController->getMidiEventIdForControl(controlId);
  }
}