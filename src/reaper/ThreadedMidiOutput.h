
#ifndef THREADED_MIDI_OUTPUT_H
#define THREADED_MIDI_OUTPUT_H

#include "reaper_plugin.h"
#include "../WDL/mutex.h"
#include "../WDL/ptrlist.h"

namespace Reaper {

  class threadedMIDIOutput : public midi_Output {
  public:
    WDL_Mutex m_mutex;
    WDL_PtrList<WDL_HeapBuf> m_full,m_empty;

    HANDLE m_hThread;
    int m_quit; // set to 1 to finish, 2 to finish+delete self
    midi_Output *m_output;

    threadedMIDIOutput(midi_Output* output);
    virtual ~threadedMIDIOutput();
    virtual void Destroy();
    virtual void SendMsg(MIDI_event_t *msg, int frame_offset);
    virtual void Send(
      unsigned char status,
      unsigned char d1,
      unsigned char d2,
      int frame_offset
    );
    static unsigned WINAPI threadProc(LPVOID p);
  };
}

#endif
