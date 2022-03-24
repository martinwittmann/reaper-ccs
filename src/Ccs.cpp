#include <string>
#include <vector>
#include "Session.h"
#include "config/GlobalConfig.h"
#include "globals.cpp"
#include "Ccs.h"
#include "Util.h"
#include "actions/ActionsManager.h"
#include <iostream>

namespace CCS {
  namespace fse = std::experimental::filesystem;
  using std::string;
  using std::vector;

  Ccs::Ccs(string baseDir, midi_Output* output) {
    sessionsDir = baseDir + "sessions";
    controllersDir = baseDir + "controllers";
    pluginsDir = baseDir + "plugins";
    this->output = output;
    config = new GlobalConfig(baseDir + "config" + YAML_EXT);
    actionsManager = new ActionsManager();

    sessions = Session::getSessions(sessionsDir);
    lastSession = config->getLastSessionId();
    currentSession = loadSession(lastSession, actionsManager);
    subscribedEventIds = currentSession->getSubscribedMidiEventIds();
  }

  Ccs::~Ccs() {
    delete currentSession;
  }


  Session* Ccs::loadSession(string sessionId, ActionsManager* actionsManager) {
    string sessionPath = sessionsDir + SEP + sessionId;
    return new Session(sessionPath, actionsManager, output);
  }

  void Ccs::onMidiEvent(MIDI_event_t* rawEvent) {
    std::cout << "\nMidi Event received:\n";
    std::cout << rawEvent->size << "\n";
    std::cout << rawEvent->midi_message[0] << " " << rawEvent->midi_message[1] << "\n";
    int eventId = Util::getMidiEventId(
      rawEvent->midi_message[0],
      rawEvent->midi_message[1]
    );
  }
}