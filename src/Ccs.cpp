#include <string>
#include "Session.cpp"
#include "config/GlobalConfig.h"
#include "globals.cpp"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  using std::string;

  class Ccs {
    string sessionsDir;
    string controllersDir;
    string pluginsDir;
    string lastSession;
    std::vector<string> sessions;
    CcsSession *currentSession;
    GlobalConfig *config;

  public:
    Ccs(string baseDir) {
      sessionsDir = baseDir + SEP + "sessions";
      controllersDir = baseDir + SEP + "controllers";
      pluginsDir = baseDir + SEP + "plugins";
      config = new GlobalConfig(baseDir + "config" + YAML_EXT);
      sessions = CcsSession::getSessions(sessionsDir);
      lastSession = config->getLastSessionId();
      currentSession = loadSession(lastSession);
    }

    ~Ccs() {
      delete currentSession;
    }


    CcsSession *loadSession(string sessionId) {
      return new CcsSession(sessionsDir + SEP + sessionId);
    }
  };
}