#ifndef CCS_PAGE_H
#define CCS_PAGE_H

#include <string>
#include <map>
#include <experimental/filesystem>
#include "yaml-cpp/yaml.h"
#include "actions/ActionProvider.h"

namespace CCS {

  class Session;
  class Util;
  class PageConfig;
  class ActionsManager;
  class ReaperApi;
  class FxPlugins;
  class MidiControlElementMapping;
  class Action;
  class MidiEventSubscriber;
  class CompositeAction;

  namespace fse = std::experimental::filesystem;
  namespace fs = std::filesystem;
  using std::string;
  using std::vector;

  class Action;
  class RadioGroup;

class Page : public ActionProvider {
    PageConfig *m_config = nullptr;
    ActionsManager *m_actionsManager = nullptr;
    Session *m_session = nullptr;
    string m_pageId;
    // A map of string variables that can be set via a page action.
    // Example: [page.set_state:name:value]
    std::map<string,string> m_state;
    vector<MidiControlElementMapping*> m_controlElementMappings;
    ReaperApi *m_reaperApi = nullptr;
    CompositeAction *m_activateAction = nullptr;
    CompositeAction *m_beforeValueChangesAction = nullptr;
    CompositeAction *m_afterValueChangesAction = nullptr;
    std::map<int,MidiEventSubscriber*> m_subscribedMidiEventIds;
    std::vector<RadioGroup *> m_radioGroups;

  public:
    Page(
      string pagePath,
      ActionsManager *actionsManager,
      Session *session,
      ReaperApi *reaperApi
    );
    ~Page();
    static bool isPageConfigFile(fse::path path);
    void setActive();
    void createActions();
    string getPageId();
    void actionCallback(string actionName, vector<string> arguments);
    void createMidiControlElementMappings();
    void createSubscribedMidiEventIds();
    std::map<int,MidiEventSubscriber*> getSubscribedMidiEventIds();
    std::map<string,string> *getState();

  void updateMidiControllerUI();

  void invokeBeforeValueChangesAction();

  void invokeAfterValueChangesAction();

  void setInactive();

  void registerRadioButtonMapping(std::string value, MidiControlElementMapping *mapping, string groupId);

  bool radioGroupExists(string groupId);

  void registerRadioGroup(string groupId);

  RadioGroup *getRadioGroup(string groupId);
};
}

#endif