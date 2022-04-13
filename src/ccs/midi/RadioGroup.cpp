#include "RadioGroup.h"
#include <string>
#include <iostream>
#include "MidiControlElementMapping.h"
#include "../CcsException.h"
#include "../Util.h"

namespace CCS {

  using std::string;

  RadioGroup::RadioGroup(string id) {
    m_groupId = id;
  }

  void RadioGroup::registerMapping(
    string formattedValue,
    MidiControlElementMapping *mapping
  ) {
    if (mappingIsRegistered(formattedValue, mapping)) {
      return;
    }
    m_mappings.insert(std::pair(formattedValue, mapping));
  }

  bool RadioGroup::mappingIsRegistered(
    string formattedValue,
    MidiControlElementMapping *newMapping
  ) {
    for (auto it = m_mappings.begin(); it != m_mappings.end(); ++it) {
      string tmpValue = it->first;
      MidiControlElementMapping *mapping = it->second;
      if (tmpValue == formattedValue) {
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

  void RadioGroup::selectValue(string formattedValue) {
    bool valueFound = false;

    for (auto it = m_mappings.begin(); it != m_mappings.end(); ++it) {
      string tmpValue = it->first;
      MidiControlElementMapping *mapping = it->second;
      if (tmpValue == formattedValue) {
        mapping->selectThisRadioItem();
        valueFound = true;
      }
      else {
        mapping->unselectThisRadioItem();
      }
    }

    // There are fx plugins that seem to use different parameter values for
    // formatted values when setting the value and when changing it via the
    // UI and then getting the value.
    // This could be a reaper bug in TrackFX_GetFormattedParamValue or it
    // might be a bug in a plugin.
    // Since we can't correctly handle this case, we need to inform the
    // user about it, so that he can update the fx config file accordingly.
    if (!valueFound) {
      Util::error("Selecting unknown value in RadioGroup " + m_groupId + ": " + formattedValue);
      Util::error("This can happen if a plugin uses different values for setting and getting a parameter value. You should be able to fix this by updating this plugin's .yml file and setting the decimal value of the parameter's value you just changed to the value above (round to 2 decimal places).");
    }
  }
}
