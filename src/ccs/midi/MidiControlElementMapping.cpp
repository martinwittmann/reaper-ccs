#include "MidiControlElementMapping.h"
#include <vector>
#include <iostream>
#include "../FxPlugins.h"
#include "../../reaper/reaper_plugin_functions.h"
#include "yaml-cpp/yaml.h"
#include "MidiControlElementAction.h"
#include "../Page.h"

namespace CCS {

  using std::string;

  MidiControlElementMapping::MidiControlElementMapping(
    int midiEventId,
    string controlId,
    YAML::Node configRoot,
    MidiControlElement *controlElement,
    ReaperApi *api,
    Session *session
  ) {
    m_midiEventId = midiEventId;
    m_config = new MappingConfig(&configRoot);
    m_controlElement = controlElement;
    m_controlId = controlId;
    m_controlType = controlElement->getType();
    m_onPressValue = controlElement->getOnPressValue();
    m_onReleaseValue = controlElement->getOnReleaseValue();
    m_api = api;
    m_session = session;

    m_paramMapping = m_config->getValue("mapping");
    if (!m_paramMapping.empty()) {
      m_hasMappedFxParam = true;
      setMappingValues(m_paramMapping);

      auto subscriber = dynamic_cast<ReaperEventSubscriber*>(this);
      this->m_api->subscribeToFxParameterChanged(
        m_mappedTrack,
        m_mappedFxId,
        m_mappedParamId,
        subscriber
      );
    }

    createActions();
  }

  void MidiControlElementMapping::createActions() {
    const YAML::Node actions = m_config->getNode("actions");
    if (!actions) {
      return;
    }

    for (auto &rawAction : actions) {
      YAML::Node item = rawAction;
      if (item.Type() == YAML::NodeType::Scalar) {
        // Only a single action was provided.
        m_actions.push_back(new MidiControlElementAction(item.as<string>()));
      }
      else {
        if (m_config->keyExists("conditions", &item)) {
          YAML::Node conditions = item["conditions"];
          m_config->getListValues("actions", &item);
          m_actions.push_back(new MidiControlElementAction(
            m_config->getListValues("actions", &item),
            &conditions
          ));
        }
        else {
          m_actions.push_back(new MidiControlElementAction(
            m_config->getListValues("actions", &item)
          ));
        }
      }
    }
  }

  void MidiControlElementMapping::invokeActions() {
    for (auto action : m_actions) {
      action->invoke(
        m_session,
        Util::byteToHex(m_value)
      );
    }
  }

  MidiControlElementMapping::~MidiControlElementMapping() {
    delete m_config;
  }

  void MidiControlElementMapping::onMidiEvent(int eventId, unsigned char data2) {
    switch (m_controlType) {
      case MidiControlElement::BUTTON:
        if (data2 == m_onPressValue) {
          m_value = data2;
          //onButtonPress(eventId, data2);
        }
        else if (data2 == m_onReleaseValue) {
          m_value = data2;
          //onButtonRelease(eventId, data2);
        }
        break;

      case MidiControlElement::ABSOLUTE:
        m_value = data2;
        //onChange(eventId, data2);
        break;

      case MidiControlElement::RELATIVE:
        char diff = 64 - data2;
        m_value += diff;
        //onChange(eventId, data2);
        break;
    }
  }

  int MidiControlElementMapping::getMidiEventId() {
    return m_midiEventId;
  }

  void MidiControlElementMapping::onFxParameterChanged(
    MediaTrack *track,
    int fxId,
    int paramId,
    double value,
    double minValue,
    double maxValue
  ) {
    m_value = Util::get7BitValue(value, minValue, maxValue);
    std::cout << "changed " << paramId << "\n";
    if (m_hasMappedFxParam) {
      invokeActions();
    }
  }

  void MidiControlElementMapping::setMappingValues(string rawMapping) {
    std::vector<string> parts = Util::splitString(rawMapping, '.');
    string trackPart = parts.at(0);
    trackPart = Util::regexReplace(trackPart, "[^0-9]+", "");
    // Note that in reaper, track 0 is the master track. Since we're using
    // 1-based indices in the configs we can simply convert this to int.
    // ???
    // For some reason using the 1-based index does not work, but the
    // zero-based index does?!
    int trackId = stoi(trackPart) - 1;
    m_mappedTrack = GetTrack(0, trackId);

    string fxIdPart = parts.at(1);
    fxIdPart = Util::regexReplace(fxIdPart, "[^0-9]+", "");
    // Also 1-based indices, but there is not special fx index 0 in reaper.
    m_mappedFxId = stoi(fxIdPart) - 1;

    string paramIdPart = parts.at(2);
    m_mappedParamId = m_session
      ->getPluginManager()
      ->getParamId(m_mappedTrack, m_mappedFxId, paramIdPart);
  }
}