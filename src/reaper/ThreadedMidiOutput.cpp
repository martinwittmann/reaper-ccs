#include "ThreadedMidiOutput.h"
#include "reaper_plugin.h"
#include "../WDL/setthreadname.h"

namespace Reaper {

  threadedMIDIOutput::threadedMIDIOutput(midi_Output *out) {
    m_output = out;
    m_quit = 0;
    unsigned id;
    m_hThread = (HANDLE)_beginthreadex(
      NULL,
      0,
      threadProc,
      this,
      0,
      &id
    );
  }

  threadedMIDIOutput::~threadedMIDIOutput() {
    if (m_hThread) {
      m_quit = 1;
      WaitForSingleObject(m_hThread,INFINITE);
      CloseHandle(m_hThread);
      m_hThread = 0;
      Sleep(30);
    }

    if (m_output) {
      m_output->Destroy();
    }
    m_empty.Empty(true);
    m_full.Empty(true);
  }

  void threadedMIDIOutput::Destroy() {
    HANDLE thread = m_hThread;
    if (!thread) {
      delete this;
    }
    else {
      m_hThread = NULL;
      m_quit = 2;

      // thread will delete self
      WaitForSingleObject(thread,100);
      CloseHandle(thread);
    }
  }

  // Frame_offset can be <0 for "instant" if supported
  void threadedMIDIOutput::SendMsg(MIDI_event_t *msg, int frame_offset) {
    if (!msg) {
      return;
    }

    WDL_HeapBuf *b = NULL;
    if (m_empty.GetSize()) {
      m_mutex.Enter();
      b = m_empty.Get(m_empty.GetSize() - 1);
      m_empty.Delete(m_empty.GetSize() - 1);
      m_mutex.Leave();
    }
    if (!b && m_empty.GetSize() + m_full.GetSize() < 500)
      b = new WDL_HeapBuf(256);

    if (b) {
      int sz = msg->size;
      if (sz < 3) {
        sz = 3;
      }
      int len = msg->midi_message + sz - (unsigned char *)msg;
      memcpy(b->Resize(len,false),msg,len);
      m_mutex.Enter();
      m_full.Add(b);
      m_mutex.Leave();
    }
  }

  // Frame_offset can be <0 for "instant" if supported
  void threadedMIDIOutput::Send(
    unsigned char status,
    unsigned char d1,
    unsigned char d2,
    int frame_offset
  ) {
    MIDI_event_t evt = {
      0,
      3,
      status,
      d1,
      d2
    };
    SendMsg(&evt, frame_offset);
  }

  unsigned WINAPI threadedMIDIOutput::threadProc(LPVOID p) {
    WDL_SetThreadName("reaper/cs_midio");
    WDL_HeapBuf *lastbuf = NULL;
    threadedMIDIOutput *_this = (threadedMIDIOutput*)p;
    unsigned int scnt = 0;
    for (;;) {
      if (_this->m_full.GetSize() || lastbuf) {
        _this->m_mutex.Enter();
        if (lastbuf) {
          _this->m_empty.Add(lastbuf);
        }
        lastbuf = _this->m_full.Get(0);
        _this->m_full.Delete(0);
        _this->m_mutex.Leave();

        if (lastbuf) {
          _this->m_output->SendMsg((MIDI_event_t*)lastbuf->Get(),-1);
        }
        scnt = 0;
      }
      else {
        Sleep(1);
        if (_this->m_quit&&scnt++>3) {
          // Only quit once all messages have been sent.
          break;
        }
      }
    }
    delete lastbuf;
    if (_this->m_quit == 2) {
      delete _this;
    }
    return 0;
  }
}
