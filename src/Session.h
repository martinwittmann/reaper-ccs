#ifndef CCS_SESSION_H
#define CCS_SESSION_H

#include <string>
#include<experimental/filesystem>
#include "yaml-cpp/yaml.h"
#include "globals.cpp"
#include "config/SessionConfig.h"

#include "Page.h"
#include "MidiController.h"
#include "actions/Actions.h"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;

  class Session {
    string path;
    string pagesDir;
    string midiControllersDir;
    string name;
    SessionConfig* sessionConfig;
    std::vector<Page *> pages;
    std::vector<MidiController *> midiControllers;
    int activePage = 0;
    Actions* actions;
    midi_Output* output;

  public:
    Session(string sessionPath, Actions* actions, midi_Output* output);

    ~Session();

    static std::vector<string> getSessions(string sessionsDir);

    static bool isSessionFile(fse::path path);

    std::vector<string> getPageNames();

    std::vector<string> getMidiControllerNames();

    void loadSessionPages();

    void loadMidiControllers();

    void start();

    void setActivePage(int pageId);
  };
}

#endif
