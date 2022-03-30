#include "MidiControlElementMapping.h"
#include <vector>
#include <iostream>
#include "../FxPlugins.h"
#include "yaml-cpp/yaml.h"
#include "../Page.h"
#include "../CcsException.h"
#include "../actions/CompositeAction.h"

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
      initializeMappingValues(m_paramMapping);

      m_mappingType = m_config->getValue("mapping_type");
      if (m_mappingType == "enum") {
        m_enumValues = m_session
          ->getPluginManager()
          ->getParamEnumValues(m_track, m_fxId, m_paramIdStr);
      }

      auto subscriber = dynamic_cast<ReaperEventSubscriber*>(this);
      this->m_api->subscribeToFxParameterChanged(
        m_track,
        m_fxId,
        m_paramId,
        subscriber
      );

      // Retrieve the current values on initialization.
      updateValuesFromReaper();
    }

    createActions();
  }

  void MidiControlElementMapping::createActions() {
    for (auto eventType : m_actionTypes) {
      const YAML::Node actionConfig = m_config->getNode(eventType);
      if (!actionConfig) {
        continue;
      }
      string actionId = m_controllerId + "." + m_controlId + "." + eventType;
      m_actions.insert(std::pair(eventType, new CompositeAction(actionId, actionConfig)));
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

  void MidiControlElementMapping::invokeActions(string actionType) {
    if (!m_actions.contains(actionType)) {
      return;
    }
    CompositeAction *action = m_actions.at(actionType);
    auto variables = getActionVariables();
    action->invoke(variables, m_session);
  }

  std::map<string,string> MidiControlElementMapping::getActionVariables() {
    std::map<string,string> result;
    string valueStr = Util::byteToHex(Util::get7BitValue(
      m_value,
      m_minValue,
      m_maxValue
    ));
    result.insert(std::pair("VALUE", valueStr));

    string formattedStr = Util::compactString(m_formattedValue);
    result.insert(std::pair("FORMATTED_VALUE", formattedStr));

    string minValueStr = Util::byteToHex(Util::get7BitValue(
      m_maxValue,
      m_minValue,
      m_maxValue
    ));
    result.insert(std::pair("MIN_VALUE", minValueStr));

    string maxValueStr = Util::byteToHex(Util::get7BitValue(
      m_maxValue,
      m_minValue,
      m_maxValue
    ));
    result.insert(std::pair("MAX_VALUE", maxValueStr));

    for (auto &item: *m_session->getActivePage()->getState()) {
      result.insert(std::pair(item.first, item.second));
    }

    return result;
  }

  MidiControlElementMapping::~MidiControlElementMapping() {
    delete m_config;
  }

  void MidiControlElementMapping::toggleValue() {
    if (m_value == m_maxValue) {
      m_value = m_minValue;
    }
    else {
      m_value = m_maxValue;
    }
  }

  void MidiControlElementMapping::onMidiEvent(int eventId, unsigned char data2) {
    switch (m_controlType) {
      case MidiControlElement::BUTTON:
        if (data2 == m_onPressValue) {
          if (m_hasMappedFxParam) {
            if (m_mappingType == "toggle") {
              toggleValue();
            }
            else if (m_mappingType == "enum") {
              setNextEnumValue();
            }
          }
          invokeActions("on_press");
        }
        else if (data2 == m_onReleaseValue) {
          invokeActions("on_release");
        }
        break;

      case MidiControlElement::ABSOLUTE:
        m_value = Util::getParamValueFrom7Bit(data2, m_minValue, m_maxValue);
        break;

      case MidiControlElement::RELATIVE:
        addValueDiff(data2);
        break;
    }

    if (m_hasMappedFxParam) {
      m_api->setFxParameterValue(m_track, m_fxId, m_paramId, m_value);
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

    diff = Util::getParamValueFrom7Bit(rawDiff, m_minValue, m_maxValue);

    // To give users better / finer control over parameters we only apply half
    // of the diff we're getting from the controller. Otherwise the distance
    // between min and max is not that big.
    m_value += diff / 2;

    if (m_value < m_minValue) {
      m_value = m_minValue;
    }
    else if (m_value > m_maxValue) {
      m_value = m_maxValue;
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
    int trackId = stoi(trackPart);
    m_track = m_api->getTrack(trackId);

    string fxIdPart = parts.at(1);
    fxIdPart = Util::regexReplace(fxIdPart, "[^0-9]+", "");
    // Also 1-based indices, but there is not special fx index 0 in reaper.
    m_fxId = stoi(fxIdPart) - 1;

    m_paramIdStr = parts.at(2);
    m_paramId = m_session
      ->getPluginManager()
      ->getParamId(m_track, m_fxId, m_paramIdStr);
  }

  void MidiControlElementMapping::setNextEnumValue() {
    auto it = m_enumValues.find(m_formattedValue);
    it++;
    if (it == m_enumValues.end()) {
      // We were at the end already.
      m_value = m_enumValues.begin()->second;
    }
    else {
      m_value = it->second;
    }
  }

  void MidiControlElementMapping::updateControlElement() {
    if (m_hasMappedFxParam) {
      //Util::log(m_paramIdStr + ": " + std::to_string(m_value));
      invokeActions("on_value_change");
    }
  }

  void MidiControlElementMapping::updateValuesFromReaper() {
    const auto [value, minValue, maxValue, midValue] = m_api->getParamValueEx(
      m_track,
      m_fxId,
      m_paramId
    );
    m_value = value;
    m_minValue = minValue;
    m_maxValue = maxValue;
    m_midValue = midValue;

    m_formattedValue = m_api->getFormattedParamValue(
      m_track,
      m_fxId,
      m_paramId
    );
  }
}