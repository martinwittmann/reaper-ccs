#include <string>
#include "Session.h"
#include "config/GlobalConfig.h"
#include "globals.cpp"
#include "Ccs.h"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  using std::string;

  Ccs::Ccs(string baseDir) {
    sessionsDir = baseDir + SEP + "sessions";
    controllersDir = baseDir + SEP + "controllers";
    pluginsDir = baseDir + SEP + "plugins";
    config = new GlobalConfig(baseDir + "config" + YAML_EXT);
    sessions = Session::getSessions(sessionsDir);
    lastSession = config->getLastSessionId();
    currentSession = loadSession(lastSession);
  }

  Ccs::~Ccs() {
    delete currentSession;
  }


  Session* Ccs::loadSession(string sessionId) {
    return new Session(sessionsDir + SEP + sessionId);
  }
}