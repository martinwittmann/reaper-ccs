#include "MidiEventSubscriber.h"

namespace CCS {

  std::vector<int> MidiEventSubscriber::getSubscribedMidiEventIds() {
    std::vector<int> result;
    return result;
  }

  void MidiEventSubscriber::onMidiEvent(int eventId, unsigned int dataByte) {}

  void MidiEventSubscriber::subscribeToMidiEvents() {
    subscribedMidiEventIds = getSubscribedMidiEventIds();
  }
}