#include <vector>

namespace CCS {
  class MidiEventSubscriber {
    std::vector<int> subscribedMidiEventIds;

  public:
    virtual std::vector<int> getSubscribedMidiEventIds();
    virtual void onMidiEvent(int eventId, unsigned int dataByte);
    void subscribeToMidiEvents();
  };
}