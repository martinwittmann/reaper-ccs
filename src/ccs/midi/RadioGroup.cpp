#include "RadioGroup.h"
#include <string>;
#include "MidiControlElementMapping.h"

namespace CCS {

  using std::string;

  RadioGroup::RadioGroup(string id) {
    m_groupId = id;
  }

  void RadioGroup::registerMapping(MidiControlElementMapping *mapping) {
    if (mappingIsRegistered(mapping)) {
      return;
    }

    m_mappings.push_back(mapping);
  }

  bool RadioGroup::mappingIsRegistered(MidiControlElementMapping *newMapping) {
    for (auto mapping : m_mappings) {
      if (newMapping == mapping) {
        return true;
      }
    }
    return false;
  }

  string RadioGroup::getGroupId() {
    return m_groupId;
  }
}
