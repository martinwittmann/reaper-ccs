#include <string>
#include "CcsSession.cpp"
#include "CcsConfig.cpp"

namespace fse = std::experimental::filesystem;
const std::string sep = to_string(fse::path::preferred_separator);

class Ccs {

    string sessionsDir;
    string controllersDir;
    string pluginsDir;
    string lastSession;
    std::vector<string> sessions;

    CcsSession* currentSession;

    CcsConfig config;

public:
    Ccs(string baseDir) {
      sessionsDir = baseDir + sep + "sessions";
      controllersDir = baseDir + sep + "controllers";
      pluginsDir = baseDir + sep + "plugins";
      config = loadYamlFile(baseDir + "config.yml");

      sessions = getSessions();
      lastSession = config["last_session"].as<std::string>();
      currentSession = loadSession(lastSession);
    }

    ~Ccs() {
      delete currentSession;
    }

    std::vector<string> getSessions() {
      std::vector<string> result;
      for (const auto & entry: fse::directory_iterator(sessionsDir)) {
        fse::path entry_path = fse::path(entry.path());
        if (!is_directory(entry_path) || !isSessionDir(entry_path.string())) {
          continue;
        }
        result.push_back(entry_path.filename());
      }
      return result;
    }

    bool isSessionDir(fse::path path) {
      string filename = path.filename();
      if (filename.starts_with('.')) {
        return false;
      }
      string session_filename = path.string() + fse::path::preferred_separator + "session.yml";
      return fse::exists(fse::path(session_filename));
    }

    CcsSession* loadSession(string sessionId) {
      return new CcsSession(sessionsDir + fse::path::preferred_separator + sessionId);
    }
};