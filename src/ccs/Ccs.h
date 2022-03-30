#ifndef CCS_CCS_H
#define CCS_CCS_H

#include <string>
#include <map>
#include "Session.h"
#include "config/GlobalConfig.h"
#include "globals.cpp"
#include "actions/ActionsManager.h"
#include "../reaper/reaper_plugin.h"
#include "midi/MidiEventSubscriber.h"
#include "../reaper-api/ReaperApi.h"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  using std::string;

  class Ccs {
    string m_resourceDir;
    string m_ccsDir;
    string m_sessionsDir;
    string m_controllersDir;
    string m_pluginsDir;
    string m_lastSession;
    std::vector<string> m_sessions;
    Session *m_currentSession;
    GlobalConfig *m_config;
    ActionsManager *m_actionsManager;
    midi_Output *m_output;
    std::map<int,MidiEventSubscriber*>* m_subscribedMidiEventIds;
    std::vector<string> m_subscribedReaperEvents;

  public:
    ReaperApi *reaperApi;
    Ccs(string baseDir, midi_Output *output);
    ~Ccs();
    Session *loadSession(string sessionId, ActionsManager *actionsManager);
    void onMidiEvent(MIDI_event_t *rawEvent);
  };
}

#endif