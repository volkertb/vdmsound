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
    enum msgType { MSG_INFO, MSG_WARNING, MSG_ERROR };

  public:
    virtual void generateInterrupt(void) = 0;
    virtual void logMessage(msgType type, const char* message) = 0;
};

//
//
//
class CMIDIInputBuffer
  : public IMPU401MIDIConsumer
{
  public:
    CMIDIInputBuffer(IMPU401HWEmulationLayer* _hwemu);
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
      { return isEmptyBuf; };
    inline bool isFull(void)
      { return (buf.size() > MAX_MIDI_BUF); };

  protected:
    inline void _QUEUEPUT_PROLOGUE(void) {
      mutex.lock();
      if (isFull()) {
        std::ostringstream oss;
        oss << std::setbase(10) << "MIDI-in buffer is full (" << buf.size() << "  bytes), flushing.";
        hwemu->logMessage(IMPU401HWEmulationLayer::MSG_ERROR, oss.str().c_str());
        while (!buf.empty()) buf.pop();
      }
    }

    inline void _QUEUEPUT_EPILOGUE(void) {
      isEmptyBuf = false;
      if (!IRQPending) {
        hwemu->generateInterrupt();
        IRQPending = true;
      }
      mutex.unlock();
    }

    inline void _QUEUEGET_PROLOGUE(void) {
      mutex.lock();
      isEmptyBuf = (buf.size() <= 1);
    }

    inline void _QUEUEGET_EPILOGUE(void) {
      IRQPending = false;
      mutex.unlock();
    }

  protected:
    CCriticalSection mutex;
    std::queue<unsigned char> buf;
    bool isEmptyBuf;
    bool IRQPending;

  protected:
    IMPU401HWEmulationLayer* hwemu;
};


//
//
//
class CMIDIOutputBuffer {
  public:
    CMIDIOutputBuffer(IMPU401HWEmulationLayer* _hwemu);
    ~CMIDIOutputBuffer(void);

  public:
    void reset(void);
    void putByte(unsigned char data);

    inline bool isEmpty(void)
      { return buf.empty(); };
    inline bool isFull(void)
      { return (buf.size() > MAX_MIDI_BUF); };

  protected:
    std::vector<unsigned char> buf;
    unsigned char currentEvent;

  protected:
    IMPU401HWEmulationLayer* hwemu;
};

#endif //__MPU401CTLBUF_H_
