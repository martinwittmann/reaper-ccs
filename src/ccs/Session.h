#ifndef CCS_SESSION_H
#define CCS_SESSION_H

#include <string>
#include <experimental/filesystem>
#include "yaml-cpp/yaml.h"
#include "globals.cpp"
#include "config/SessionConfig.h"
#include "../reaper/reaper_plugin.h"
#include "midi/MidiEventSubscriber.h"
#include "../reaper-api/ReaperApi.h"
#include "FxPlugins.h"
#include "actions/ActionProvider.h"

namespace CCS {
  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;

  class ActionsManager;
  class MidiController;
  class Page;

  class Session : public ActionProvider {
    string m_path;
    string m_pagesDir;
    string m_midiControllersDir;
    string m_pluginsDir;
    string m_name;
    SessionConfig *m_sessionConfig;
    std::vector<Page *> m_pages;
    std::vector<MidiController *> m_midiControllers;
    FxPlugins *m_pluginManager;
    Page *m_activePage;
    ActionsManager *m_actionsManager;
    midi_Output *m_output;
    ReaperApi *m_reaperApi;
    std::map<int,MidiEventSubscriber*> m_subscribedMidiEventIds;

  public:
    Session(
      string sessionPath,
      ActionsManager *actionsManager,
      midi_Output *output,
      ReaperApi *reaperApi
    );

    ~Session();
    static std::vector<string> getSessions(string sessionsDir);
    static bool isSessionFile(fse::path path);
    std::vector<string> getPageNames();
    std::vector<string> getMidiControllerNames();
    void loadSessionPages();
    void loadMidiControllers();
    void loadPage(string pageId);
    Page *getActivePage();
    std::map<int,MidiEventSubscriber*> *getSubscribedMidiEventIds();
    MidiController *getMidiController(std::string id);
    FxPlugins *getPluginManager();

    ActionsManager *getActionsManager();

    Page *getPage(string pageId);

    void actionCallback(std::string actionName, std::vector<std::string> arguments) override;
    void invokeAction(std::string, Session* session);
  };
}

#endif
