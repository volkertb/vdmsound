#ifndef __MPU401CTLBUF_H_
#define __MPU401CTLBUF_H_

#include <Thread.h>

/* TODO: give the max. buffer length as a .INI setting */
// Individual size of the IN/OUT MIDI buffers
#define MAX_MIDI_BUF 65536

//
// This interface can receive MIDI messages, either for storage (in the case
//  of an in-bound FIFO, where MIDI messages are stored and will later be
//  retrieved, byte-by-byte), or for playback (in the case of an interface
//  that can directly connect with a synthesizer, where the messages have
//  already been assembled and are ready to be sent "on the wire").
//
class IMPU401MIDIConsumer {
  public:
	  virtual void putEvent(unsigned char status, unsigned char data1, unsigned char data2, unsigned char length) = 0;
	  virtual void putSysEx(const unsigned char * data, long length) = 0;
	  virtual void putRealTime(unsigned char data) = 0;
};

//
// This interface abstracts some of the hardware details in the MPU-401's
//  world, such as generating IRQs (on the PC side) and sending MIDI messages
//  to a synthesizer (on the "outside-world" side).
//
class IMPU401HWEmulationLayer
  : public IMPU401MIDIConsumer
{
  public:
    virtual void generateInterrupt(void) = 0;
    virtual void logError(const char* message) = 0;
    virtual void logWarning(const char* message) = 0;
    virtual void logInformation(const char* message) = 0;
};

//
//
//
class CMIDIInputBuffer
  : public IMPU401MIDIConsumer
{
  public:
    CMIDIInputBuffer(IMPU401HWEmulationLayer* hwemu);
    ~CMIDIInputBuffer(void);

  public:
	  void putEvent(unsigned char status, unsigned char data1, unsigned char data2, unsigned char length);
	  void putSysEx(const unsigned char * data, long length);
	  void putRealTime(unsigned char data);

  public:
    void reset(void);
    void putByte(unsigned char data);
    bool getByte(unsigned char* data);

    inline bool isEmpty(void)
      { return m_isEmptyBuf; };
    inline bool isFull(void)
      { return (m_buf.size() > MAX_MIDI_BUF); };

  protected:
    inline void _QUEUEPUT_PROLOGUE(void) {
      m_mutex.lock();
      if (isFull()) {
        std::ostringstream oss;
        oss << std::setbase(10) << "MIDI-in buffer is full (" << m_buf.size() << "  bytes), flushing.";
        m_hwemu->logError(oss.str().c_str());
        while (!m_buf.empty()) m_buf.pop();
      }
    }

    inline void _QUEUEPUT_EPILOGUE(void) {
      m_isEmptyBuf = false;
      if (!m_IRQPending) {
        m_hwemu->generateInterrupt();
        m_IRQPending = true;
      }
      m_mutex.unlock();
    }

    inline void _QUEUEGET_PROLOGUE(void) {
      m_mutex.lock();
      m_isEmptyBuf = (m_buf.size() <= 1);
    }

    inline void _QUEUEGET_EPILOGUE(void) {
      m_IRQPending = false;
      m_mutex.unlock();
    }

  protected:
    CCriticalSection m_mutex;
    std::queue<unsigned char> m_buf;
    bool m_isEmptyBuf;
    bool m_IRQPending;

  protected:
    IMPU401HWEmulationLayer* m_hwemu;
};


//
//
//
class CMIDIOutputBuffer {
  public:
    CMIDIOutputBuffer(IMPU401HWEmulationLayer* hwemu);
    ~CMIDIOutputBuffer(void);

  public:
    void reset(void);
    void putByte(unsigned char data);

    inline bool isEmpty(void)
      { return m_buf.empty(); };
    inline bool isFull(void)
      { return (m_buf.size() > MAX_MIDI_BUF); };

  protected:
    std::vector<unsigned char> m_buf;
    unsigned char m_currentEvent;

  protected:
    IMPU401HWEmulationLayer* m_hwemu;
};

#endif //__MPU401CTLBUF_H_
