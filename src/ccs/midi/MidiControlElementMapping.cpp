#include "MidiControlElementMapping.h"
#include <vector>
#include <iostream>
#include "../FxPlugins.h"
#include "yaml-cpp/yaml.h"
#include "../Page.h"
#include "../CcsException.h"
#include "../actions/CompositeAction.h"
#include "../config/MappingConfig.h"
#include "MidiControlElement.h"
#include "../Session.h"
#include "../Util.h"
#include "../../reaper-api/ReaperApi.h"
#include <chrono>

namespace CCS {

  using std::string;

  MidiControlElementMapping::MidiControlElementMapping(
    int midiEventId,
    string controlId,
    YAML::Node configRoot,
    MidiControlElement *controlElement,
    ReaperApi *api,
    Session *session,
    Page *page
  ) {
    try {
      m_midiEventId = midiEventId;
      m_config = new MappingConfig(&configRoot);
      m_controlElement = controlElement;
      m_controlId = controlId;
      m_controllerId = controlElement->getControllerId();
      m_controlType = controlElement->getType();
      m_onPressValue = controlElement->getOnPressValue();
      m_onReleaseValue = controlElement->getOnReleaseValue();
      m_api = api;
      m_session = session;
      m_page = page;

      m_actionTypes = getAvailableActionTypes(m_controlType);
      m_paramMapping = m_config->getValue("mapping");

      if (!m_paramMapping.empty()) {
        m_hasMapping = true;
        initializeMappingValues(m_paramMapping);

        m_inputType = m_config->getValue("mapping_type");
        if (m_inputType.empty()) {
          // Try to get it from the control element.
          m_controlElement->getTypeName(m_controlType);
        }
        else if (m_inputType == "radio_button") {
          m_radioGroupId = m_config->getValue("radio.group");
          m_page->registerRadioButtonMapping(this, m_radioGroupId);
        }

        // Fall back to absolute if nothing was specified.
        if (m_inputType.empty()) {
          m_inputType = "absolute";
        }

        if (m_inputType == "enum") {
          m_enumValues = m_session
            ->getPluginManager()
            ->getParamEnumValues(m_track, m_fxId, m_paramIdStr);
        }

        // Retrieve the current values on initialization.
        updateValuesFromReaper();
      }

      createActions();
    }
    catch (CcsException &e) {
      Util::error("Error create MidiControlElementMapping for " + m_controllerId + "." + m_controlId);
      Util::error(e.what());
    }
    catch (...) {
      Util::error("Error create MidiControlElementMapping for " + m_controllerId + "." + m_controlId);
    }
  }

  void MidiControlElementMapping::activate() {
    if (!m_hasMapping) {
      return;
    }
    auto subscriber = dynamic_cast<ReaperEventSubscriber *>(this);

    switch (m_mappingType) {
      case TRACK_MUTE:
      case TRACK_RECORD_ARM:
      case TRACK_SOLO:
      case TRACK_VOLUME:
        this->m_api->subscribeToControlSurfaceEvent(
          getControlSurfaceEventId(m_mappingType),
          m_track,
          subscriber
        );
        break;

      case FX_PARAMETER:
        this->m_api->subscribeToFxParameterChanged(
          m_track,
          m_fxId,
          m_paramId,
          subscriber
        );
        break;
    }
  }

  void MidiControlElementMapping::deactivate() {
    auto subscriber = dynamic_cast<ReaperEventSubscriber *>(this);
    this->m_api->unsubscribeFromFxParameterChanged(
      m_track,
      m_fxId,
      m_paramId,
      subscriber
    );
  }

  void MidiControlElementMapping::createActions() {
    for (auto eventType: m_actionTypes) {
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
    } else if (
      controlElementType == MidiControlElement::RELATIVE ||
      controlElementType == MidiControlElement::ABSOLUTE
      ) {
      result.push_back("on_change");
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

  std::map<string, string> MidiControlElementMapping::getActionVariables() {
    std::map<string, string> result;
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
    deactivate();
  }

  void MidiControlElementMapping::toggleValue() {
    if (m_value == m_maxValue) {
      m_value = m_minValue;
    } else {
      m_value = m_maxValue;
    }
  }

  void MidiControlElementMapping::onMidiEvent(int eventId, unsigned char data2) {

    switch (m_controlType) {
      case MidiControlElement::BUTTON:
        if (data2 == m_onPressValue) {
          if (m_hasMapping) {
            if (m_inputType == "toggle") {
              toggleValue();
            } else if (m_inputType == "enum") {
              setNextEnumValue();
            }
          }
          invokeActions("on_press");
        } else if (data2 == m_onReleaseValue) {
          invokeActions("on_release");
        }
        break;

      case MidiControlElement::ABSOLUTE:
        if (m_hasMapping) {
          switch (m_mappingType) {
            case TRACK_VOLUME:
            case FX_PARAMETER:
              m_value = Util::getParamValueFrom7Bit(data2, m_minValue, m_maxValue);
              break;

            case TRACK_MUTE:
            case TRACK_RECORD_ARM:
            case TRACK_SOLO:
              if (data2 >= 63) {
                m_value = 1.0;
              } else {
                m_value = 0.0;
              }
          }
          invokeActions("on_change");
        }
        break;

      case MidiControlElement::RELATIVE:
        if (m_hasMapping) {
          double diff;

          switch (m_mappingType) {
            case TRACK_VOLUME:
            case FX_PARAMETER:
              diff = getRelativeValueDiff(data2);
              // To give users better / finer control over parameters we only apply half
              // of the diff we're getting from the controller. Otherwise the distance
              // between min and max is not that big.
              m_value += diff / 2;

              if (m_value < m_minValue) {
                m_value = m_minValue;
              } else if (m_value > m_maxValue) {
                m_value = m_maxValue;
              }
              break;

            case TRACK_MUTE:
            case TRACK_RECORD_ARM:
            case TRACK_SOLO:
              if (data2 >= 63) {
                m_value = 1.0;
              } else {
                m_value = 0.0;
              }
          }

          invokeActions("on_change");
        }
        break;
    }

    if (m_hasMapping) {
      switch (m_mappingType) {
        case TRACK_VOLUME:
          m_api->setTrackVolume(m_track, m_value);
          break;

        case TRACK_MUTE:
          m_api->setTrackMute(m_track, m_value == m_maxValue);
          break;

        case TRACK_SOLO:
          m_api->setTrackSolo(m_track, m_value == m_maxValue);
          break;

        case TRACK_RECORD_ARM:
          m_api->setTrackRecordArm(m_track, m_value == m_maxValue);
          break;

        case FX_PARAMETER:
          m_api->setFxParameterValue(m_track, m_fxId, m_paramId, m_value);
          break;
      }
    }
  }

  double MidiControlElementMapping::getRelativeValueDiff(unsigned char rawDiff) {
    char diff;
    if (rawDiff > 63) {
      diff = rawDiff - 128;
    } else if (rawDiff < 63) {
      diff = rawDiff;
    };
    return Util::getParamValueFrom7Bit(diff, m_minValue, m_maxValue);
  }

  void MidiControlElementMapping::addValueDiff(char rawDiff) {
    double diff;
    if (rawDiff > 63) {
      rawDiff = rawDiff - 128;
    } else if (rawDiff < 63) {
      rawDiff = rawDiff;
    }

    diff = Util::getParamValueFrom7Bit(rawDiff, m_minValue, m_maxValue);

    // To give users better / finer control over parameters we only apply half
    // of the diff we're getting from the controller. Otherwise the distance
    // between min and max is not that big.
    m_value += diff / 2;

    if (m_value < m_minValue) {
      m_value = m_minValue;
    } else if (m_value > m_maxValue) {
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
    if (m_hasMapping && m_mappingType == FX_PARAMETER) {
      invokeOnValueChangeAction(true);
    }
  }

  void MidiControlElementMapping::onTrackVolumeChanged(double volume) {
    bool forceUpdate = false;
    m_value = Util::sliderToVolume(volume);
    if (m_value < 0.01) {
      m_value = 0.00;
      forceUpdate = true;
    } else if (m_value > 0.99) {
      m_value = 1.00;
      forceUpdate = true;
    }
    m_formattedValue = Util::roundDouble(m_value * 10.0);
    if (m_hasMapping && m_mappingType == TRACK_VOLUME) {
      invokeOnValueChangeAction(forceUpdate);
    }
  }

  void MidiControlElementMapping::onTrackMuteChanged(bool mute) {
    m_value = mute ? 1.0 : 0.0;
    m_formattedValue = mute ? "MUTE" : "";
    if (m_hasMapping && m_mappingType == TRACK_MUTE) {
      invokeOnValueChangeAction();
    }
  }

  void MidiControlElementMapping::onTrackSoloChanged(bool solo) {
    m_value = solo ? 1.0 : 0.0;
    m_formattedValue = solo ? "SOLO" : "";
    if (m_hasMapping && m_mappingType == TRACK_SOLO) {
      invokeOnValueChangeAction();
    }
  }

  void MidiControlElementMapping::onTrackRecordArmChanged(bool recordArm) {
    m_value = recordArm ? 1.0 : 0.0;
    m_formattedValue = recordArm ? "REC" : "";
    if (m_hasMapping && m_mappingType == TRACK_RECORD_ARM) {
      invokeOnValueChangeAction();
    }
  }

  void MidiControlElementMapping::initializeMappingValues(string rawMapping) {
    std::vector<string> parts = Util::splitString(rawMapping, '.');
    string first = Util::toLower(parts.at(0));
    if (Util::regexMatch(first, "^track")) {
      string trackPart = Util::regexReplace(first, "[^0-9]+", "");
      int trackId = stoi(trackPart);
      m_track = m_api->getTrack(trackId);

      if (Util::regexMatch(parts.at(1), "^fx")) {
        m_mappingType = FX_PARAMETER;

        string fxIdPart = parts.at(1);
        fxIdPart = Util::regexReplace(fxIdPart, "[^0-9]+", "");
        // We're using 1-based indices. There is no special fx index 0 in reaper.
        // Index 0 is just the first fx plugin.
        m_fxId = stoi(fxIdPart) - 1;

        m_paramIdStr = parts.at(2);
        m_paramId = m_session
          ->getPluginManager()
          ->getParamId(m_track, m_fxId, m_paramIdStr);
      }
      else if (Util::regexMatch(parts.at(1), "volume")) {
        m_mappingType = TRACK_VOLUME;
        m_minValue = 0;
        m_maxValue = 1;
      }
      else if (Util::regexMatch(parts.at(1), "mute")) {
        m_mappingType = TRACK_MUTE;
        m_minValue = 0;
        m_maxValue = 1;
      }
      else if (Util::regexMatch(parts.at(1), "solo")) {
        m_mappingType = TRACK_SOLO;
        m_minValue = 0;
        m_maxValue = 1;
      }
      else if (Util::regexMatch(parts.at(1), "record_arm")) {
        m_mappingType = TRACK_RECORD_ARM;
        m_minValue = 0;
        m_maxValue = 1;
      }
    }
  }

  void MidiControlElementMapping::setNextEnumValue() {
    auto it = m_enumValues.find(m_formattedValue);
    it++;
    if (it == m_enumValues.end()) {
      // We were at the end already.
      m_value = m_enumValues.begin()->second;
    } else {
      m_value = it->second;
    }
  }

  void MidiControlElementMapping::updateControlElement() {
    if (m_hasMapping) {
      switch (m_mappingType) {
        case TRACK_VOLUME:
        case TRACK_MUTE:
        case TRACK_SOLO:
        case TRACK_RECORD_ARM:
        case FX_PARAMETER:
          invokeActions("on_value_change");
          break;
      }
    }
  }

  void MidiControlElementMapping::updateValuesFromReaper() {
    switch (m_mappingType) {

      case FX_PARAMETER:
        updateFxParamValuesFromReaper();
        break;

      case TRACK_VOLUME:
        m_value = m_api->getTrackVolume(m_track);
        m_formattedValue = Util::roundDouble(m_value * 10.0);
        Util::log(m_controlId + ": " + std::to_string(m_value));
        break;
    }
  }

  void MidiControlElementMapping::updateFxParamValuesFromReaper() {
    const auto[value, minValue, maxValue, midValue] = m_api->getParamValueEx(
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

  int MidiControlElementMapping::getControlSurfaceEventId(int mappingType) {
    switch (mappingType) {
      case TRACK_MUTE:
        return ReaperApi::ON_TRACK_MUTE_CHANGED;

      case TRACK_RECORD_ARM:
        return ReaperApi::ON_TRACK_RECORD_ARM_CHANGED;

      case TRACK_SOLO:
        return ReaperApi::ON_TRACK_SOLO_CHANGED;

      case TRACK_VOLUME:
        return ReaperApi::ON_TRACK_VOLUME_CHANGED;

      default:
        return -1;
    }
  }

  void MidiControlElementMapping::invokeOnValueChangeAction(bool forceUpdate) {
    // Throttle updating the controller ui to maximum 100 times per second to
    // limit the used midi bandwidth. Also allow forcing the update through.
    auto diff = std::chrono::high_resolution_clock::now() - m_lastOnValueChangeAction;
    if (forceUpdate || diff > std::chrono::milliseconds(10)) {
      m_page->invokeBeforeValueChangesAction();
      invokeActions("on_value_change");
      m_page->invokeAfterValueChangesAction();
      m_lastOnValueChangeAction = std::chrono::high_resolution_clock::now();
    }
  }

}