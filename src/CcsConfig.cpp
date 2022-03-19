#include <string>
#include "yaml-cpp/yaml.h"
#include<experimental/filesystem>
#include "CcsSession.cpp"

namespace fse = std::experimental::filesystem;
namespace fs = std::filesystem;
using std::string;

class CcsConfig {

private:
    string baseDir;
    string sessionsDir;
    string controllersDir;
    string pluginsDir;
    string lastSession;
    std::vector<string> sessions;

    CcsSession* currentSession;

    YAML::Node config;

public:
    CcsConfig(string initBaseDir) {
      baseDir = initBaseDir;
      sessionsDir = baseDir + fse::path::preferred_separator + "sessions";
      controllersDir = baseDir + fse::path::preferred_separator + "controllers";
      pluginsDir = baseDir + fse::path::preferred_separator + "plugins";
      config = loadYamlFile(baseDir + "config.yml");

      sessions = getSessions();
      lastSession = config["last_session"].as<std::string>();
      currentSession = loadSession(lastSession);
    }

    ~CcsConfig() {
      delete currentSession;
    }

    YAML::Node loadYamlFile(string filename) {
      return YAML::LoadFile(filename);
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
