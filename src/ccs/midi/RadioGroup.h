#ifndef CCS_RADIO_GROUP_H
#define CCS_RADIO_GROUP_H

#include <string>
#include <map>

namespace CCS {

  class MidiControlElementMapping;

  class RadioGroup {
    std::string m_groupId;
    std::map<std::string, MidiControlElementMapping*> m_mappings;

  public:

    RadioGroup(std::string id);

    void registerMapping(std::string formattedValue, MidiControlElementMapping *mapping);

    bool mappingIsRegistered(std::string formattedValue, MidiControlElementMapping *mapping);

    std::string getGroupId();

    void selectValue(std::string value);
  };
}

#endif
