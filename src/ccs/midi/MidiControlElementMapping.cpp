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
        Util::byteToHex(Util::get7BitValue(m_value, m_mappedMinValue, m_mappedMaxValue)),
        m_formattedValue
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
          m_value = Util::getParamValueFrom7Bit(data2, m_mappedMinValue, m_mappedMaxValue);
          //onButtonPress(eventId, data2);
        }
        else if (data2 == m_onReleaseValue) {
          m_value = Util::getParamValueFrom7Bit(data2, m_mappedMinValue, m_mappedMaxValue);
          //onButtonRelease(eventId, data2);
        }
        break;

      case MidiControlElement::ABSOLUTE:
        m_value = Util::getParamValueFrom7Bit(data2, m_mappedMinValue, m_mappedMaxValue);
        m_controlElement->onChange(data2);
        break;

      case MidiControlElement::RELATIVE:
        /*
        char diff = 0;
        if (data2 > 63) {
          diff = data2 - 128;
        }
        else if (data2 < 63) {
          diff = data2;
        }
        */
        addValueDiff(data2);
        m_controlElement->onChange(data2);
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
      invokeActions();
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