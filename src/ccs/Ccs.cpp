#include <string>
#include <vector>
#include "Session.h"
#include "config/GlobalConfig.h"
#include "globals.cpp"
#include "Ccs.h"
#include "Util.h"
#include "../reaper-api/ReaperApi.h"
#include "actions/ActionsManager.h"
#include "midi/MidiEventSubscriber.h"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  using std::string;
  using std::vector;

  Ccs::Ccs(string baseDir, midi_Output *output) {
    m_resourceDir = baseDir;
    const char separator = fse::path::preferred_separator;
    m_ccsDir = m_resourceDir + separator + "ccs" + separator;

    m_sessionsDir = m_ccsDir + "sessions";
    m_controllersDir = m_ccsDir + "controllers";
    m_pluginsDir = m_ccsDir + "fx_plugins";

    m_output = output;
    m_config = new GlobalConfig(m_ccsDir + "config" + YAML_EXT);
    m_actionsManager = new ActionsManager();
    reaperApi = new ReaperApi();

    m_sessions = Session::getSessions(m_sessionsDir);
    m_lastSession = m_config->getLastSessionId();
    m_currentSession = loadSession(m_lastSession, m_actionsManager);
    m_subscribedMidiEventIds = m_currentSession->getSubscribedMidiEventIds();
    std::map<int,MidiEventSubscriber*>::iterator it;
  }

  Ccs::~Ccs() {
    delete m_currentSession;
  }


  Session *Ccs::loadSession(string sessionId, ActionsManager *actionsManager) {
    string sessionPath = m_sessionsDir + SEP + sessionId;
    return new Session(sessionPath, actionsManager, m_output, reaperApi);
  }

  void Ccs::onMidiEvent(MIDI_event_t *rawEvent) {
    int eventId = Util::getMidiEventId(
      rawEvent->midi_message[0],
      rawEvent->midi_message[1]
    );
    if (m_subscribedMidiEventIds->contains(eventId)) {
      MidiEventSubscriber *subscriber = m_subscribedMidiEventIds->at(eventId);
      if (subscriber) {
        subscriber->onMidiEvent(eventId, rawEvent->midi_message[2]);
      }
    }
  }
}