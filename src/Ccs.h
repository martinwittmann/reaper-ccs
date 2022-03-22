#ifndef CCS_CCS_H
#define CCS_CCS_H

#include <string>
#include "Session.h"
#include "config/GlobalConfig.h"
#include "globals.cpp"
#include "actions/Actions.h"

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

  public:
    Ccs(string baseDir);

    ~Ccs();

    Session *loadSession(string sessionId, Actions* actions);
  };
}

#endif