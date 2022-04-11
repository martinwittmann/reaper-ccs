#ifndef CCS_RADIO_GROUP_H
#define CCS_RADIO_GROUP_H

#include <string>
#include <vector>

namespace CCS {

  class MidiControlElementMapping;

  class RadioGroup {
    std::string m_groupId;
    std::vector<MidiControlElementMapping*> m_mappings;

  public:

    RadioGroup(std::string id);

    void registerMapping(MidiControlElementMapping *mapping);

    bool mappingIsRegistered(MidiControlElementMapping *mapping);

    std::string getGroupId();
  };
}

#endif
