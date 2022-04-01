#ifndef CCS_MIDI_EVENT_SUBSCRIBER
#define CCS_MIDI_EVENT_SUBSCRIBER

#include <vector>
#include <map>

namespace CCS {
  class MidiEventSubscriber {
    std::map<int,MidiEventSubscriber*> subscribedMidiEventIds;

  public:
    virtual std::map<int,MidiEventSubscriber*> getSubscribedMidiEventIds();
    virtual void onMidiEvent(int eventId, unsigned char dataByte);
    void subscribeToMidiEvents();
  };
}

#endif