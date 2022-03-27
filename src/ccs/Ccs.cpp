#include <string>
#include <vector>
#include "Session.h"
#include "config/GlobalConfig.h"
#include "globals.cpp"
#include "Ccs.h"
#include "Util.h"
#include "actions/ActionsManager.h"
#include <iostream>
#include "reaper/ReaperApi.h"
#include "Test.cpp"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  using std::string;
  using std::vector;

  Ccs::Ccs(string baseDir, midi_Output* output) {
    resourceDir = baseDir;
    const char separator = fse::path::preferred_separator;
    ccsDir = resourceDir + separator + "ccs" + separator;

    sessionsDir = ccsDir + "sessions";
    controllersDir = ccsDir + "controllers";
    pluginsDir = ccsDir + "fx_plugins";

    this->output = output;
    config = new GlobalConfig(ccsDir + "config" + YAML_EXT);
    actionsManager = new ActionsManager();
    reaperApi = new ReaperApi();

    sessions = Session::getSessions(sessionsDir);
    lastSession = config->getLastSessionId();
    currentSession = loadSession(lastSession, actionsManager);
    subscribedMidiEventIds = currentSession->getSubscribedMidiEventIds();
    std::map<int,MidiEventSubscriber*>::iterator it;
    for (it = subscribedMidiEventIds.begin(); it != subscribedMidiEventIds.end(); ++it) {
      std::cout << "Subscribed to event id: " << it->first << "\n";
    }

    Test* test = new Test();
    auto subscriber = dynamic_cast<ReaperEventSubscriber*>(test);
    MediaTrack* track1 = reaperApi->getTrack(1);
    reaperApi->subscribeToFxParameterChanged(track1, 0, 3, subscriber);
  }

  Ccs::~Ccs() {
    delete currentSession;
  }


  Session* Ccs::loadSession(string sessionId, ActionsManager* actionsManager) {
    string sessionPath = sessionsDir + SEP + sessionId;
    return new Session(sessionPath, actionsManager, output, reaperApi);
  }

  void Ccs::onMidiEvent(MIDI_event_t* rawEvent) {
    int eventId = Util::getMidiEventId(
      rawEvent->midi_message[0],
      rawEvent->midi_message[1]
    );
    if (subscribedMidiEventIds.contains(eventId)) {
      MidiEventSubscriber* subscriber = subscribedMidiEventIds.at(eventId);
      if (subscriber) {
        subscriber->onMidiEvent(eventId, rawEvent->midi_message[2]);
      }
      else {
        std::cout << "Subscriber not found\n";
      }
    }
  }
}