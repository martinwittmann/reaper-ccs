#include "MidiEventSubscriber.h"

namespace CCS {

  // This is supposed to be overriden by the derived class.
  std::map<int,MidiEventSubscriber*> MidiEventSubscriber::getSubscribedMidiEventIds() {
    std::map<int,MidiEventSubscriber*> result;
    return result;
  }

  void MidiEventSubscriber::onMidiEvent(int eventId, unsigned int dataByte) {}

  void MidiEventSubscriber::subscribeToMidiEvents() {
    subscribedMidiEventIds = getSubscribedMidiEventIds();
  }
}