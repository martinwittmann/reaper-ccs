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

      m_paramMapping = m_config->getValue("mapping");


      if (!m_paramMapping.empty()) {
        m_hasMapping = true;
        initializeMappingValues(m_paramMapping);

        m_mappingType = m_config->getValue("mapping_type");

        // Fall back to absolute if nothing was specified.
        if (m_mappingType.empty()) {
          m_mappingType = "absolute";
        }

        if (m_config->keyExists("relative_speed_factor")) {
          m_relativeSpeedFactor = std::stod(m_config->getValue("relative_speed_factor"));
        }

        if (m_mappingType == "enum_button" || m_mappingType == "radio_button") {
          if (m_mappingTarget == FX_PARAMETER) {
            m_enumValues = m_session
              ->getPluginManager()
              ->getParamEnumValues(m_track, m_fxId, m_paramIdStr);
          }

          if (m_mappingType == "radio_button") {
            m_radioFormattedValue = m_config->getValue("radio.value");

            if (m_mappingTarget == FX_PARAMETER) {
              // We only use m_radioValue for enum fx params.
              m_radioValue = m_enumValues.at(m_radioFormattedValue);
            }
          }
        }
      }

      // Mapping type might be empty if no mapping is used, but that's ok.
      m_actionTypes = getAvailableActionTypes(m_mappingType, m_controlType);
      createActions();

      // Retrieve the current values on initialization.
      updateValuesFromReaper();
    }
    catch (CcsException &e) {
      Util::error("Error creating MidiControlElementMapping for " + m_controllerId + "." + m_controlId);
      Util::error(e.what());
    }
    catch (...) {
      Util::error("Error creating MidiControlElementMapping for " + m_controllerId + "." + m_controlId);
    }
  }

  void MidiControlElementMapping::activate() {
    if (!m_hasMapping || !m_trackExists) {
      return;
    }
    auto subscriber = dynamic_cast<ReaperEventSubscriber *>(this);

    this->m_api->subscribeToControlSurfaceEvent(
      ReaperApi::ON_TRACK_LIST_CHANGED,
      m_track,
      subscriber
    );

    switch (m_mappingTarget) {
      case TRACK_MUTE:
      case TRACK_RECORD_ARM:
      case TRACK_SOLO:
      case TRACK_VOLUME:
        this->m_api->subscribeToControlSurfaceEvent(
          getControlSurfaceEventId(m_mappingTarget),
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

      case FX_PRESET:
        this->m_api->subscribeToFxPresetChanged(
          m_track,
          m_fxId,
          subscriber
        );
        break;
    }
  }

  void MidiControlElementMapping::deactivate() {
    auto subscriber = dynamic_cast<ReaperEventSubscriber *>(this);

    // We're not unsubscribing from onTracklistChanged here because we want to
    // be updated if our track becomes available again and reactivate.

    this->m_api->unsubscribeFromControlSurfaceEvent(
      getControlSurfaceEventId(m_mappingTarget),
      m_track,
      subscriber
    );

    this->m_api->unsubscribeFromFxParameterChanged(
      m_track,
      m_fxId,
      m_paramId,
      subscriber
    );

    this->m_api->unsubscribeFromFxPresetChanged(
      m_track,
      m_fxId,
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

  vector<string> MidiControlElementMapping::getAvailableActionTypes(
    string mappingType,
    short controlElementType
  ) {
    vector<string> result = {"on_value_change"};

    if (
      controlElementType == MidiControlElement::BUTTON ||
      controlElementType == MidiControlElement::PAD
    ) {
      result.push_back("on_press");
      result.push_back("on_release");

      if (mappingType == "radio_button") {
        result.push_back("radio.on_selected");
        result.push_back("radio.on_unselected");
      }
    }
    else if (
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

  double MidiControlElementMapping::getToggledValue() {
    double result;
    if (m_value == m_maxValue) {
      result = m_minValue;
    }
    else {
      result = m_maxValue;
    }
    return result;
  }

  void MidiControlElementMapping::onMidiEvent(int eventId, unsigned char data2) {

    switch (m_controlType) {
      case MidiControlElement::BUTTON:
        if (data2 == m_onPressValue) {
          onButtonPress();
        }
        else if (data2 == m_onReleaseValue) {
          onButtonRelease();
        }
        break;

      case MidiControlElement::ABSOLUTE:
        onAbsoluteMidiValue(data2);
        break;

      case MidiControlElement::RELATIVE:
        char diff;
        if (data2 > 63) {
          diff = data2 - 128;
        }
        else if (data2 < 63) {
          diff = data2;
        };
        onRelativeMidiValue(diff);
        break;

      case MidiControlElement::PAD:
        // Don't invoke actions for really soft presses.
        if (data2 > 8) {
          // For now we treat pads like regular buttons.
          onButtonPress();
        }
        break;
    }
  }

  int MidiControlElementMapping::getMidiEventId() {
    if (m_mappingType == "radio_button") {
      int a = 1;
    }
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
    if (m_trackExists && m_hasMapping) {
      // For radio_buttons we update all buttons in this group via the known
      // on_selected/on_unselected actions, before invoking the regular
      // on_value_changed actions.
      if (m_mappingType == "radio_button") {
        if(m_formattedValue == m_radioFormattedValue) {
          selectThisRadioItem();
        }
        else {
          unselectThisRadioItem();
        }
      }
      invokeOnValueChangeAction(true);
    }
  }

  void MidiControlElementMapping::onFxPresetChanged(
    MediaTrack *track,
    int fxId,
    int presetIndex,
    string presetName
  ) {
    m_value = double(presetIndex);
    m_formattedValue = presetName;
    if (m_trackExists && m_hasMapping) {
      // For radio_buttons we update all buttons in this group via the known
      // on_selected/on_unselected actions, before invoking the regular
      // on_value_changed actions.
      if (m_mappingType == "radio_button") {
        if(m_formattedValue == m_radioFormattedValue) {
          selectThisRadioItem();
        }
        else {
          unselectThisRadioItem();
        }
      }
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
    if (m_trackExists && m_hasMapping && m_mappingTarget == TRACK_VOLUME) {
      invokeOnValueChangeAction(forceUpdate);
    }
  }

  void MidiControlElementMapping::onTrackMuteChanged(bool mute) {
    m_value = mute ? 1.0 : 0.0;
    m_formattedValue = mute ? "MUTE" : "";
    if (m_trackExists && m_hasMapping && m_mappingTarget == TRACK_MUTE) {
      invokeOnValueChangeAction();
    }
  }

  void MidiControlElementMapping::onTrackSoloChanged(bool solo) {
    m_value = solo ? 1.0 : 0.0;
    m_formattedValue = solo ? "SOLO" : "";
    if (m_trackExists && m_hasMapping && m_mappingTarget == TRACK_SOLO) {
      invokeOnValueChangeAction();
    }
  }

  void MidiControlElementMapping::onTrackRecordArmChanged(bool recordArm) {
    m_value = recordArm ? 1.0 : 0.0;
    m_formattedValue = recordArm ? "REC" : "";
    if (m_trackExists && m_hasMapping && m_mappingTarget == TRACK_RECORD_ARM) {
      invokeOnValueChangeAction();
    }
  }

  void MidiControlElementMapping::onTrackListChanged(int numTracks) {
    updateTrack();
  }

  void MidiControlElementMapping::initializeMappingValues(string rawMapping) {
    std::vector<string> parts = Util::splitString(rawMapping, '.');
    string first = Util::toLower(parts.at(0));
    if (Util::regexMatch(first, "^track")) {
      string trackPart = Util::regexReplace(first, "[^0-9]+", "");
      m_trackId = stoi(trackPart);
      updateTrack();

      if (!trackIsAvailable()) {
        return;
      }

      if (Util::regexMatch(parts.at(1), "^fx")) {

        if (Util::regexMatch(parts.at(1), "^fx[0-9]+_preset$")) {
          string rawName = Util::regexReplace(parts.at(1), "_preset$", "");
          m_fxId = m_api->getFxIdByGenericName(rawName);
          m_mappingTarget = FX_PRESET;
        }
        else {
          m_mappingTarget = FX_PARAMETER;

          string paramIdStr = parts.at(2);
          m_paramIdStr = paramIdStr;
          m_fxId = m_api->getFxIdByGenericName(parts.at(1));
          m_paramId = m_session
            ->getPluginManager()
            ->getParamId(m_track, m_fxId, m_paramIdStr);
        }
      }
      else if (Util::regexMatch(parts.at(1), "volume")) {
        m_mappingTarget = TRACK_VOLUME;
        m_minValue = 0;
        m_maxValue = 1;
      }
      else if (Util::regexMatch(parts.at(1), "mute")) {
        m_mappingTarget = TRACK_MUTE;
        m_minValue = 0;
        m_maxValue = 1;
      }
      else if (Util::regexMatch(parts.at(1), "solo")) {
        m_mappingTarget = TRACK_SOLO;
        m_minValue = 0;
        m_maxValue = 1;
      }
      else if (Util::regexMatch(parts.at(1), "record_arm")) {
        m_mappingTarget = TRACK_RECORD_ARM;
        m_minValue = 0;
        m_maxValue = 1;
      }
    }
  }

  double MidiControlElementMapping::getNextEnumValue() {
    auto it = m_enumValues.find(m_formattedValue);
    it++;
    if (it == m_enumValues.end()) {
      // We were at the end already, get the first value.
      return m_enumValues.begin()->second;
    } else {
      return it->second;
    }
  }

  double MidiControlElementMapping::getPreviousEnumValue() {
    auto it = m_enumValues.find(m_formattedValue);
    if (it == m_enumValues.begin()) {
      // We were at the end already, get the first value.
      return m_enumValues.begin()->second;
    }
    else {
      it--;
      return it->second;
    }
  }

  void MidiControlElementMapping::updateControlElement() {
    if (m_hasMapping) {
      switch (m_mappingTarget) {
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
    if (!m_hasMapping) {
      return;
    }

    switch (m_mappingTarget) {

      case FX_PARAMETER:
        updateFxParamValuesFromReaper();
        break;

      case TRACK_VOLUME:
        m_value = m_api->getTrackVolume(m_track);
        m_formattedValue = Util::roundDouble(m_value * 10.0);
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

  void MidiControlElementMapping::selectThisRadioItem() {
    invokeActions("radio.on_selected");
  }

  void MidiControlElementMapping::unselectThisRadioItem() {
    invokeActions("radio.on_unselected");
  }

  void MidiControlElementMapping::onButtonPress() {
    invokeActions("on_press");

    if (m_hasMapping) {
      if (m_mappingType == "toggle") {
        applyChangesFromUserInput(getToggledValue());
      }
      else if (m_mappingType == "enum_button") {
        applyChangesFromUserInput(getNextEnumValue());
      }
      else if (m_mappingType == "radio_button") {

        if (m_mappingTarget == FX_PARAMETER) {
          applyChangesFromUserInput(m_radioValue);
        }
        else if (m_mappingTarget == FX_PRESET) {
          m_formattedValue = m_radioFormattedValue;
          m_api->loadFxPreset(m_track, m_fxId, m_radioFormattedValue);
        }
      }
    }
  }

  void MidiControlElementMapping::onButtonRelease() {
    invokeActions("on_release");
    // We do not apply changes for toggle buttons on release, since this would
    // undo changes made on press.
  }

  void MidiControlElementMapping::applyChangesFromUserInput(double newValue) {
    if (m_hasMapping) {
      switch (m_mappingTarget) {
        case TRACK_VOLUME:
          m_api->setTrackVolume(m_track, newValue);
          break;

        case TRACK_MUTE:
          m_api->setTrackMute(m_track, newValue == m_maxValue);
          break;

        case TRACK_SOLO:
          m_api->setTrackSolo(m_track, newValue == m_maxValue);
          break;

        case TRACK_RECORD_ARM:
          m_api->setTrackRecordArm(m_track, newValue == m_maxValue);
          break;

        case FX_PARAMETER:
          m_api->setFxParameterValue(m_track, m_fxId, m_paramId, newValue);
          break;
      }
      m_value = newValue;
    }
  }

  void MidiControlElementMapping::onAbsoluteMidiValue(unsigned char data2) {
    invokeActions("on_change");

    if (m_hasMapping) {
      double newValue;

      switch (m_mappingTarget) {
        case TRACK_VOLUME:
        case FX_PARAMETER:
          newValue = Util::getParamValueFrom7Bit(data2, m_minValue, m_maxValue);
          applyChangesFromUserInput(newValue);
          break;


        case TRACK_MUTE:
        case TRACK_RECORD_ARM:
        case TRACK_SOLO:
          if (data2 >= 63) {
            newValue = 1.0;
          } else {
            newValue = 0.0;
          }
          applyChangesFromUserInput(newValue);
      }
    }
  }

  void MidiControlElementMapping::onRelativeMidiValue(char diff) {
    invokeActions("on_change");

    if (m_hasMapping) {
      double newValue;
      double valueDiff;

      switch (m_mappingTarget) {
        case TRACK_VOLUME:
        case FX_PARAMETER:
          valueDiff = Util::getParamValueFrom7Bit(diff, m_minValue, m_maxValue);

          if (m_mappingType == "toggle") {
            if (valueDiff > 0) {
              applyChangesFromUserInput(m_maxValue);
            }
            else {
              applyChangesFromUserInput(m_minValue);
            }
          }
          else if (m_mappingType == "enum_button") {
            if (valueDiff > 0) {
              applyChangesFromUserInput(getNextEnumValue());
            }
            else {
              applyChangesFromUserInput(getPreviousEnumValue());
            }
          }
          else {
            // We're allowing configs to set a factor for each control.
            newValue = m_value + (valueDiff * m_relativeSpeedFactor);

            if (newValue < m_minValue) {
              newValue = m_minValue;
            }
            else if (newValue > m_maxValue) {
              newValue = m_maxValue;
            }
            applyChangesFromUserInput(newValue);
          }
          break;

        case FX_PRESET:
          if (valueDiff > 0) {
            m_api->loadNextFxPreset(m_track, m_fxId);
          }
          else {
            m_api->loadPreviousFxPreset(m_track, m_fxId);
          }
          break;
      }
    }
  }

  bool MidiControlElementMapping::trackIsAvailable() {
    return m_trackId >= 0 && m_track != nullptr && m_trackExists;
  }

  void MidiControlElementMapping::updateTrack() {
    m_track = m_api->getTrack(m_trackId);
    bool trackExisted = m_trackExists;
    m_trackExists = m_track != nullptr;
    if (trackExisted && !m_trackExists) {
      deactivate();
    }
    else if (!trackExisted && m_trackExists) {
      activate();
    }
  }
}