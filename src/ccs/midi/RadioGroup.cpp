#include "RadioGroup.h"
#include <string>
#include "MidiControlElementMapping.h"
#include "../CcsException.h"

namespace CCS {

  using std::string;

  RadioGroup::RadioGroup(string id) {
    m_groupId = id;
  }

  void RadioGroup::registerMapping(
    double value,
    MidiControlElementMapping *mapping
  ) {
    if (mappingIsRegistered(value, mapping)) {
      return;
    }
    m_mappings.insert(std::pair(value, mapping));
  }

  bool RadioGroup::mappingIsRegistered(
    double value,
    MidiControlElementMapping *newMapping
  ) {
    for (auto it = m_mappings.begin(); it != m_mappings.end(); ++it) {
      double tmpValue = it->first;
      MidiControlElementMapping *mapping = it->second;
      if (tmpValue == value) {
        if (mapping == newMapping) {
          return true;
        }
        else {
          throw CcsException("RadioGroup::mappingIsRegistered(): Found mapping with same value, but different MidiControlElementMapping");
        }
      }
    }
    return false;
  }

  string RadioGroup::getGroupId() {
    return m_groupId;
  }

  void RadioGroup::selectValue(double value) {
    for (auto it = m_mappings.begin(); it != m_mappings.end(); ++it) {
      double tmpValue = it->first;
      MidiControlElementMapping *mapping = it->second;
      if (tmpValue == value) {
        mapping->selectThisRadioItem();
      }
      else {
        mapping->unselectThisRadioItem();
      }
    }
  }
}
