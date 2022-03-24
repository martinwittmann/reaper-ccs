#include <string>
#include "Session.h"
#include "config/GlobalConfig.h"
#include "globals.cpp"
#include "Ccs.h"
#include "actions/ActionsManager.h"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  using std::string;

  Ccs::Ccs(string baseDir, midi_Output* output) {
    sessionsDir = baseDir + "sessions";
    controllersDir = baseDir + "controllers";
    pluginsDir = baseDir + "plugins";
    this->output = output;
    config = new GlobalConfig(baseDir + "config" + YAML_EXT);
    actions = new Actions();

    sessions = Session::getSessions(sessionsDir);
    lastSession = config->getLastSessionId();
    currentSession = loadSession(lastSession, actions);
  }

  Ccs::~Ccs() {
    delete currentSession;
  }


  Session* Ccs::loadSession(string sessionId, Actions* actions) {
    string sessionPath = sessionsDir + SEP + sessionId;
    return new Session(sessionPath, actions, output);
  }
}