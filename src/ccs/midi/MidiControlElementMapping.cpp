#include "MidiControlElementMapping.h"
#include <vector>
#include <iostream>
#include "../FxPlugins.h"
#include "../../reaper/reaper_plugin_functions.h"
#include "yaml-cpp/yaml.h"
#include "MidiControlElementAction.h"
#include "../Page.h"
#include "../CcsException.h"

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

    m_actionTypes = getAvailableActionTypes(m_controlType);
    m_paramMapping = m_config->getValue("mapping");

    if (!m_paramMapping.empty()) {
      m_hasMappedFxParam = true;


      m_mappingType = m_config->getValue("mapping_type");
      initializeMappingValues(m_paramMapping);

      auto subscriber = dynamic_cast<ReaperEventSubscriber*>(this);
      this->m_api->subscribeToFxParameterChanged(
        m_mappedTrack,
        m_mappedFxId,
        m_mappedParamId,
        subscriber
      );

      double rawValue = TrackFX_GetParamEx(
        m_mappedTrack,
        m_mappedFxId,
        m_mappedParamId,
        &m_mappedMinValue,
        &m_mappedMaxValue,
        &m_mappedMidValue
      );
      m_value = rawValue;
    }

    createActions();
  }

  void MidiControlElementMapping::createActions() {
    for (auto eventType : m_actionTypes) {
      m_actions.insert(std::pair(eventType, getActions(eventType)));
    }
  }

  vector<string> MidiControlElementMapping::getAvailableActionTypes(short controlElementType) {
    vector<string> result = {"on_value_change"};

    if (controlElementType == MidiControlElement::BUTTON) {
      result.push_back("on_press");
      result.push_back("on_release");
    }

    return result;
  }

  vector<MidiControlElementAction*> MidiControlElementMapping::getActions(string eventKey) {
    vector<MidiControlElementAction*> result;
    const YAML::Node actions = m_config->getNode(eventKey);
    if (!actions) {
      return result;
    }

    for (auto &rawAction : actions) {
      YAML::Node item = rawAction;
      if (item.Type() == YAML::NodeType::Scalar) {
        // Only a single action was provided.
        result.push_back(new MidiControlElementAction(item.as<string>()));
      }
      else {
        if (m_config->keyExists("conditions", &item)) {
          YAML::Node conditions = item["conditions"];
          m_config->getListValues("actions", &item);
          result.push_back(new MidiControlElementAction(
            m_config->getListValues("actions", &item),
            &conditions
          ));
        }
        else {
          result.push_back(new MidiControlElementAction(
            m_config->getListValues("actions", &item)
          ));
        }
      }
    }

    return result;
  }

  void MidiControlElementMapping::invokeActions(string actionType) {
    vector<MidiControlElementAction*> actions;

    for (auto type : m_actionTypes) {
      if (type == actionType) {
        actions = m_actions.at(actionType);
        break;
      }
    }

    for (auto action : actions) {
      action->invoke(
        m_session,
        Util::byteToHex(Util::get7BitValue(m_value, m_mappedMinValue, m_mappedMaxValue)),
        Util::byteToHex(Util::get7BitValue(m_mappedMinValue, m_mappedMinValue, m_mappedMaxValue)),
        Util::byteToHex(Util::get7BitValue(m_mappedMaxValue, m_mappedMinValue, m_mappedMaxValue)),
        m_formattedValue
      );
    }
  }

  MidiControlElementMapping::~MidiControlElementMapping() {
    delete m_config;
  }

  void MidiControlElementMapping::toggleValue() {
    if (m_value == m_mappedMaxValue) {
      m_value = m_mappedMinValue;
    }
    else {
      m_value = m_mappedMaxValue;
    }
  }

  void MidiControlElementMapping::onMidiEvent(int eventId, unsigned char data2) {
    switch (m_controlType) {
      case MidiControlElement::BUTTON:
        if (data2 == m_onPressValue) {
          if (m_hasMappedFxParam && m_mappingType == "toggle") {
            toggleValue();
          }
          invokeActions("on_press");
        }
        else if (data2 == m_onReleaseValue) {
          invokeActions("on_release");
        }
        break;

      case MidiControlElement::ABSOLUTE:
        m_value = Util::getParamValueFrom7Bit(data2, m_mappedMinValue, m_mappedMaxValue);
        break;

      case MidiControlElement::RELATIVE:
        addValueDiff(data2);
        break;
    }

    if (m_hasMappedFxParam) {
      TrackFX_SetParam(m_mappedTrack, m_mappedFxId, m_mappedParamId, m_value);
    }
  }

  void MidiControlElementMapping::addValueDiff(char rawDiff) {
    double diff;
    if (rawDiff > 63) {
      rawDiff = rawDiff - 128;
    }
    else if (rawDiff < 63) {
      rawDiff = rawDiff;
    }

    diff = Util::getParamValueFrom7Bit(rawDiff, m_mappedMinValue, m_mappedMaxValue);

    // To give users better / finer control over parameters we only apply half
    // of the diff we're getting from the controller. Otherwise the distance
    // between min and max is not that big.
    m_value += diff / 2;

    if (m_value < m_mappedMinValue) {
      m_value = m_mappedMinValue;
    }
    else if (m_value > m_mappedMaxValue) {
      m_value = m_mappedMaxValue;
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
    double maxValue,
    string formattedValue
  ) {
    m_value = value;
    m_formattedValue = formattedValue;
    if (m_hasMappedFxParam) {
      invokeActions("on_value_change");
    }
  }

  void MidiControlElementMapping::initializeMappingValues(string rawMapping) {
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