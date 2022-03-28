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
    string resourceDir;
    string ccsDir;
    string sessionsDir;
    string controllersDir;
    string pluginsDir;
    string lastSession;
    std::vector<string> sessions;
    Session *currentSession;
    GlobalConfig *config;
    ActionsManager *actionsManager;
    midi_Output *output;
    std::map<int,MidiEventSubscriber*> subscribedMidiEventIds;
    std::vector<string> subscribedReaperEvents;

  public:
    ReaperApi *reaperApi;
    Ccs(string baseDir, midi_Output *output);
    ~Ccs();
    Session *loadSession(string sessionId, ActionsManager *actionsManager);
    void onMidiEvent(MIDI_event_t *rawEvent);
  };
}

#endif