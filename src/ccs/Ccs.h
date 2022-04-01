#ifndef CCS_CCS_H
#define CCS_CCS_H

#include <string>
#include <map>
#include "globals.cpp"
#include "../reaper/reaper_plugin.h"

namespace CCS {

  class Session;
  class GlobalConfig;
  class ActionsManager;
  class MidiEventSubscriber;
  class ReaperApi;

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
    Session *m_currentSession = nullptr;
    GlobalConfig *m_config = nullptr;
    ActionsManager *m_actionsManager = nullptr;
    midi_Output *m_output = nullptr;
    std::map<int,MidiEventSubscriber*>* m_subscribedMidiEventIds;
    std::vector<string> m_subscribedReaperEvents;

  public:
    ReaperApi *reaperApi = nullptr;
    Ccs(string baseDir, midi_Output *output);
    ~Ccs();
    Session *loadSession(string sessionId, ActionsManager *actionsManager);
    void onMidiEvent(MIDI_event_t *rawEvent);
  };
}

#endif