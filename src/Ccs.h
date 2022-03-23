#ifndef CCS_CCS_H
#define CCS_CCS_H

#include <string>
#include "Session.h"
#include "config/GlobalConfig.h"
#include "globals.cpp"
#include "actions/Actions.h"
#include "sdk/reaper_plugin.h"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  using std::string;

  class Ccs {
    string sessionsDir;
    string controllersDir;
    string pluginsDir;
    string lastSession;
    std::vector<string> sessions;
    Session *currentSession;
    GlobalConfig *config;
    Actions* actions;
    midi_Output* output;

  public:
    Ccs(string baseDir, midi_Output* output);

    ~Ccs();

    Session *loadSession(string sessionId, Actions* actions);
  };
}

#endif