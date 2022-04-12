#ifndef CCS_RADIO_GROUP_H
#define CCS_RADIO_GROUP_H

#include <string>
#include <map>

namespace CCS {

  class MidiControlElementMapping;

  class RadioGroup {
    std::string m_groupId;
    std::map<double, MidiControlElementMapping*> m_mappings;

  public:

    RadioGroup(std::string id);

    void registerMapping(double value, MidiControlElementMapping *mapping);

    bool mappingIsRegistered(double value, MidiControlElementMapping *mapping);

    std::string getGroupId();

    void selectValue(double value);
  };
}

#endif
