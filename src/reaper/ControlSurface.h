#ifndef REAPER_CONTROL_SURFACE_H
#define REAPER_CONTROL_SURFACE_H

#include "ThreadedMidiOutput.h"
#include <string>
#include "csurf.h"

namespace Reaper {

  class ControlSurface : public IReaperControlSurface {

  protected:
    int m_midiInputDeviceId;
    int m_midiOutputDeviceId;
    midi_Input* m_midiInput;
    midi_Output* m_midiOutput;
    std::string m_controlSurfaceType;
    WDL_String m_controlSurfaceDescription;
    char m_configuration[1024];

  public:
    ControlSurface();
    ~ControlSurface();

    midi_Output *createThreadedMiDiOutput(midi_Output *output);
    void setupMidi(int inputDeviceId, int outputDeviceId, int* errorStats);
    void CloseNoReset() override;

    const char* GetTypeString() override;
    const char* GetDescString() override;
    const char* GetConfigString() override;
    void Run() override;
  };
}

#endif
