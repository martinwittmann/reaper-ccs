#include "ControlSurface.h"
#include "csurf.h"
#include "reaper_plugin_functions.h"

namespace Reaper {

  ControlSurface::ControlSurface() { }

  ControlSurface::~ControlSurface() {
    CloseNoReset();
  }

  midi_Output* ControlSurface::createThreadedMiDiOutput(midi_Output *output) {
    if (!output) {
      return output;
    }
    return new threadedMIDIOutput(output);
  }

  void ControlSurface::setupMidi(int inputDeviceId, int outputDeviceId, int* errorStats) {
    m_midiInputDeviceId = inputDeviceId;
    m_midiOutputDeviceId = outputDeviceId;

    // Create midi hardware access.
    if (m_midiInputDeviceId >= 0) {
      m_midiInput = CreateMIDIInput(m_midiInputDeviceId);
    }
    else {
      m_midiInput = NULL;
    }

    if (m_midiOutputDeviceId >= 0) {
      m_midiOutput = createThreadedMiDiOutput(CreateMIDIOutput(
        m_midiOutputDeviceId,
        false,
        NULL
      ));
    }
    else {
      m_midiOutput = NULL;
    }

    if (errorStats) {
      if (m_midiInputDeviceId >=0  && !m_midiInput) {
        *errorStats |= 1;
      }
      if (m_midiOutputDeviceId >=0  && !m_midiOutput) {
        *errorStats |= 2;
      }
    }

    if (m_midiInput) {
      m_midiInput->start();
    }
  }

  const char* ControlSurface::GetTypeString() {
    return m_controlSurfaceType.c_str();
  }

  const char* ControlSurface::GetDescString() {
    m_controlSurfaceDescription.SetFormatted(
      512,
      __LOCALIZE_VERFMT("Custom Control Surface", "csurf"),
      m_midiInput,
      m_midiOutput
    );
    return m_controlSurfaceDescription.Get();
  }
  const char* ControlSurface::GetConfigString() {
    // String of configuration data. We just store the midi device ids as integers.
    sprintf(m_configuration, "%d %d", m_midiInputDeviceId, m_midiOutputDeviceId);
    return m_configuration;
  }

  void ControlSurface::CloseNoReset() {
    if (m_midiOutput) {
      DELETE_ASYNC(m_midiOutput);
      m_midiOutput = nullptr;
    }
    if (m_midiInput) {
      DELETE_ASYNC(m_midiInput);
      m_midiInput = nullptr;
    }
  }

  void ControlSurface::Run() { }
}

