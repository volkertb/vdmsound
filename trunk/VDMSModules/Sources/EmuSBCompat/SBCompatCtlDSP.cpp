#include "stdafx.h"

#include "SBConst.h"
#include "SBCompatCtlDSP.h"
#include "SBCompatCtlMixer.h"


/////////////////////////////////////////////////////////////////////////////


#undef MKWORD
#undef LOBYTE
#undef HIBYTE

#define MKWORD(bHi, bLo) ((((bHi) & 0xff) << 8) | ((bLo) & 0xff))
#define LOBYTE(word) ((word) & 0xff)
#define HIBYTE(word) (((word) >> 8) & 0xff)


/////////////////////////////////////////////////////////////////////////////


CSBCompatCtlDSP::CSBCompatCtlDSP(ISBDSPHWEmulationLayer* hwemu, CSBCompatCtlMixer* sbmix)
  : m_hwemu(hwemu), m_sbmix(sbmix),
    m_state(DSP_S_NORMAL),          // not in high-speed mode
    m_lastCommand((char)0x00),      // no last command
    m_isSpeakerEna(false),          // the speaker is disabled
    m_useTimeConstant(false),       // playback rate determined by sample rate, not time constant
    m_is8BitIRQPending(false),      // no 8-bit operation IRQ pending
    m_is16BitIRQPending(false),     // no 16-bit operation IRQs pending
    m_testRegister((char)0x00),     // no value in the test register
    m_E2Value((char)0xaa),          //
    m_E2Count(0)                    //
{
  _ASSERTE(m_hwemu != NULL);
  _ASSERTE(m_sbmix != NULL);
}

CSBCompatCtlDSP::~CSBCompatCtlDSP(void)
{
}


/////////////////////////////////////////////////////////////////////////////


//
// Unconditionally resets the DSP
//
void CSBCompatCtlDSP::reset(void) {
  // flush buffers
  flushInputBuffer();
  flushOutputBuffer();

  // reset other internal registers, except for:
  // - test register: must remain the same across resets
  m_state        = DSP_S_NORMAL;
  m_isSpeakerEna = false;
  m_lastCommand  = (char)0x00;

  m_E2Value      = (char)0xaa;
  m_E2Count      = 0;

  setNumSamples(0);
  setSampleRate(11025);

  stopAllDMA(true); // stop all DMA activity and wait until DREQ is deasserted
  ackAllIRQs();     // clear any pending IRQs
}

//
// Resets DSP on 1->0 level transition
//
void CSBCompatCtlDSP::reset(char data) {
  if ((data & 0x01) != 0) { // SB16 only looks at bit 0
    switch (m_state) {
      case DSP_S_NORMAL:
        m_hwemu->logInformation("DSP reset - reinitializing DSP");
        reset();  // reset the DSP
        break;

      case DSP_S_HIGHSPEED:
        m_hwemu->logInformation("DSP reset - exiting high-speed mode");
        stopAllDMA(true); // stop all DMA activity and wait until DREQ is deasserted
        ackAllIRQs();     // clear any pending IRQs
        break;
    }

    m_state = DSP_S_RESET;
  } else {
    flushOutputBuffer();        // flush the buffer
    m_bufOut.push((char)0xaa);  // acknowledge reset

    m_state = DSP_S_NORMAL;
  }
}

void CSBCompatCtlDSP::putCommand(char data) {
  /* TODO: in the future: handle UART MIDI through here */

  unsigned char cmd;
  int commandLength;

  switch (m_state) {
    case DSP_S_NORMAL:
      // put data into incoming queue
      m_bufIn.push_back(data);

      // what command we are dealing with, and how long
      cmd = m_bufIn[0];
      commandLength = DSP_cmd_len[cmd & 0xff];

      if (commandLength <= 0) {
        char msgBuf[1024];
        sprintf(msgBuf, "Attempted to use undocumented DSP command 0x%02x", cmd & 0xff);
        m_hwemu->logWarning(msgBuf);
        flushInputBuffer(); // flush buffer
        return;
      }

      if (commandLength < m_bufIn.size()) {
        char msgBuf[1024];
        sprintf(msgBuf, "DSP internal state inconsistency: command = 0x%02x, in buffer = %d bytes, expected %d bytes", cmd & 0xff, m_bufIn.size(), commandLength);
        m_hwemu->logWarning(msgBuf);
        flushInputBuffer(); // flush buffer
        return;
      }

      if (commandLength == m_bufIn.size()) {

#       ifdef _DEBUG
        char msgBuf[1024];
        sprintf(msgBuf, "Processing DSP command 0x%02x (%d bytes)", cmd & 0xff, commandLength);
        for (int i = 1; i < commandLength; i++) sprintf(msgBuf + strlen(msgBuf), " %02x", m_bufIn[i] & 0xff);
        m_hwemu->logInformation(msgBuf);
#       endif

        if (processCommand(cmd))
          m_lastCommand = cmd;

        flushInputBuffer(); // flush buffer
        return;
      } else {
        return;             // command not complete yet
      }

    case DSP_S_HIGHSPEED:
      // During high-speed DMA, any byte that comes in here is ignored
      // or treated like a sample byte, but *not* like a DSP command
      return;

    case DSP_S_RESET:
      // While reset is asserted, any byte that comes in here is ignored
      return;

    default:
      m_hwemu->logError("DSP command - invalid state, reverting to NORMAL");
      m_state = DSP_S_NORMAL;
  }
}

char CSBCompatCtlDSP::getData(void) {
  /* TODO: if UART MIDI mode supported, might need to multiplex between
     this and a thread-safe FIFO, or make this FIFO thread-safe -- MIDI
     data would come in asynchronously! */

  // Always remember the last value that we had in the output FIFO; if the
  //   FIFO is empty, output this last value (that is what the SB is doing)
  static char lastReply = (char)0xff;

  switch (m_state) {
    case DSP_S_NORMAL:
      if (!m_bufOut.empty()) {
        char reply = m_bufOut.front();
        m_bufOut.pop();

        lastReply = reply;  // remember this value

#       ifdef _DEBUG
        char msgBuf[1024];
        sprintf(msgBuf, "Replying to last DSP command: 0x%02x (%d bytes left in buffer)", lastReply & 0xff, m_bufOut.size());
        m_hwemu->logInformation(msgBuf);
#       endif

        return reply;
      } else {
        return lastReply;
      }

    case DSP_S_HIGHSPEED:
      // During high-speed DMA ignore any port read requests
      return lastReply;

    case DSP_S_RESET:
      // While reset is asserted ignore any port read requests
      return lastReply;

    default:
      m_hwemu->logError("DSP get data - invalid state, reverting to NORMAL");
      m_state = DSP_S_NORMAL;
      return lastReply;
  }
}

char CSBCompatCtlDSP::getWrStatus(void) {
  switch (m_state) {
    case DSP_S_NORMAL:
      return (char)MK_SB_WR_STATUS(true);   // always accept commands

    case DSP_S_HIGHSPEED:
      return (char)MK_SB_WR_STATUS(false);  // don't accept commands

    case DSP_S_RESET:
      return (char)MK_SB_WR_STATUS(false);  // don't accept commands

    default:
      m_hwemu->logError("DSP get WRITE status - invalid state, reverting to NORMAL");
      m_state = DSP_S_NORMAL;
      return (char)MK_SB_WR_STATUS(false);  // don't accept commands yet
  }
}

char CSBCompatCtlDSP::getRdStatus(void) {
  switch (m_state) {
    case DSP_S_NORMAL:
      // is there data in the output buffer ?
      return (char)MK_SB_RD_STATUS(!m_bufOut.empty());

    case DSP_S_HIGHSPEED:
      return (char)MK_SB_RD_STATUS(false);  // no data is available

    case DSP_S_RESET:
      return (char)MK_SB_RD_STATUS(false);  // no data is available

    default:
      m_hwemu->logError("DSP get READ status - invalid state, reverting to NORMAL");
      m_state = DSP_S_NORMAL;
      return (char)MK_SB_RD_STATUS(false);  // no data is available yet
  }
}

void CSBCompatCtlDSP::ack8BitIRQ(void) {
  if (m_is8BitIRQPending) {
    m_is8BitIRQPending = false;
    m_hwemu->logInformation("IRQ acknowledged (8-bit)");
  }

  m_sbmix->setIRQStatus(CSBCompatCtlMixer::DSP_8BIT, m_is8BitIRQPending);
}

void CSBCompatCtlDSP::ack16BitIRQ(void) {
  if (m_is16BitIRQPending) {
    m_is16BitIRQPending = false;
    m_hwemu->logInformation("IRQ acknowledged (16-bit)");
  }

  m_sbmix->setIRQStatus(CSBCompatCtlMixer::DSP_16BIT, m_is16BitIRQPending);
}

void CSBCompatCtlDSP::set8BitIRQ(void) {
  if (m_is8BitIRQPending) {
    m_hwemu->logWarning("set8BitIRQ(): an IRQ is already pending");
  } else {
    m_is8BitIRQPending = true;
  }

  m_sbmix->setIRQStatus(CSBCompatCtlMixer::DSP_8BIT, m_is8BitIRQPending);
  m_hwemu->generateInterrupt();
}

void CSBCompatCtlDSP::set16BitIRQ(void) {
  if (m_is16BitIRQPending) {
    m_hwemu->logWarning("set16BitIRQ(): an IRQ is already pending");
  } else {
    m_is16BitIRQPending = true;
  }

  m_sbmix->setIRQStatus(CSBCompatCtlMixer::DSP_16BIT, m_is16BitIRQPending);
  m_hwemu->generateInterrupt();
}

bool CSBCompatCtlDSP::get8BitIRQ(void) {
  return m_is8BitIRQPending;
}

bool CSBCompatCtlDSP::get16BitIRQ(void) {
  return m_is16BitIRQPending;
}


/////////////////////////////////////////////////////////////////////////////


void CSBCompatCtlDSP::stopAllDMA(bool isSynchronous) {
  // stop all pending DMA operations
  m_hwemu->stopTransfer(ISBDSPHWEmulationLayer::TT_RECORD, isSynchronous);
  m_hwemu->stopTransfer(ISBDSPHWEmulationLayer::TT_PLAYBACK, isSynchronous);
}

void CSBCompatCtlDSP::ackAllIRQs(void) {
  // acknowledge any pending IRQs
  if (m_is8BitIRQPending) ack8BitIRQ();
  if (m_is16BitIRQPending) ack16BitIRQ();
}

bool CSBCompatCtlDSP::processCommand(unsigned char command) {
  _ASSERTE(m_bufIn.size() > 0);
  _ASSERTE(m_bufIn[0] == command);

  int i;
  char msgBuf[1024] = "";

  /* TODO: interface with mixer for establishing mono/stereo output, filters, etc.; let DSP have a
     ptr. to the mixer, and have the mixer expose public fn's for querying setero, filters, etc. */

  switch (command) {
    case 0x04:  /* 004h : DSP Status (Obsolete) */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x04 (DSP status)");
      flushOutputBuffer();  // flush any previous replies
      m_bufOut.push(0xff);  // output reply
      return false;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    case 0x10:  /* 010h : Direct DAC, 8-bit */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x10 (Direct DAC)");
      return false;

    case 0x14:  /* 014h : DMA DAC, 8-bit */
    case 0x91:  /* 091h : DMA DAC, 8-bit (High Speed) */
      /* TODO: set state to DSP_S_HIGHSPEED for cmd. 0x91 (implement when DMA single-cycle terminal count
         notification is functional, to automatically switch out of high-speed at the end of transfer) */
      if ((command == 0x91) && (m_hwemu->getDSPVersion() < 0x0400))  // high-speed not supported on v4.xx+
        m_hwemu->logWarning("Attempted to use partially unimplemented DSP command 0x91 (DMA DAC 8-bit high-speed)");

      m_hwemu->startTransfer(
          ISBDSPHWEmulationLayer::TT_PLAYBACK,
          getNumChannels(),
          getSampleRate(),
          8,
          MKWORD(m_bufIn[2], m_bufIn[1]) + 1,
          ISBDSPHWEmulationLayer::CODEC_PCM,
          false);
      return true;

    case 0x16:  /* 016h : DMA DAC, 2-bit ADPCM */
    case 0x17:  /* 017h : DMA DAC, 2-bit ADPCM Reference */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP commands 0x16/0x17 (DMA DAC 2-bit ADPCM)");
      return false;

    case 0x1c:  /* 01Ch : Auto-Initialize DMA DAC, 8-bit */
    case 0x90:  /* 090h : Auto-Initialize DMA DAC, 8-bit (High Speed) */
      if ((command == 0x90) && (m_hwemu->getDSPVersion() < 0x0400))  // high-speed not supported on v4.xx+
        m_state = DSP_S_HIGHSPEED;  // set state to high-speed; can only get out by resetting the DSP

      m_hwemu->startTransfer(
          ISBDSPHWEmulationLayer::TT_PLAYBACK,
          getNumChannels(),
          getSampleRate(),
          8,
          getNumSamples(),
          ISBDSPHWEmulationLayer::CODEC_PCM,
          true);
      return true;

    case 0x1f:  /* 01Fh : Auto-Initialize DMA DAC, 2-bit ADPCM Reference */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x1f (A/I DMA DAC, 2-bit ADPCM ref.)");
      return false;

    case 0x20:  /* 020h : Direct ADC, 8-bit */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x20 (Direct ADC)");
      flushOutputBuffer();  // flush any previous replies
      m_bufOut.push(0xff);  // output reply
      return false;

    case 0x24:  /* 024h : DMA ADC, 8-bit */
    case 0x99:  /* 024h : DMA ADC, 8-bit (High Speed) */
      /* TODO: set state to DSP_S_HIGHSPEED for cmd. 0x91 (implement when DMA single-cycle terminal count
         notification is functional, to automatically switch out of high-speed at the end of transfer) */
      if ((command == 0x99) && (m_hwemu->getDSPVersion() < 0x0400))  // high-speed not supported on v4.xx+
        m_hwemu->logWarning("Attempted to use partially unimplemented DSP command 0x99 (DMA ADC 8-bit high-speed)");

      m_hwemu->startTransfer(
          ISBDSPHWEmulationLayer::TT_RECORD,
          1,                /* TODO: check for stereo recording (SBPro or cmd. 0xa0/0xa8?) */
          getSampleRate(),  /* TODO: have separate rate for playback/recording */
          8,
          MKWORD(m_bufIn[2], m_bufIn[1]) + 1,
          ISBDSPHWEmulationLayer::CODEC_PCM,
          false);
      return true;

    case 0x28:  /* 028h : Direct ADC, 8-bit (Burst) */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x28 (Direct ADC, burst)");
      return false;

    case 0x2c:  /* 02Ch : Auto-Initialize DMA ADC, 8-bit */
    case 0x98:  /* 098h : Auto-Initialize DMA ADC, 8-bit (High Speed) */
      if ((command == 0x98) && (m_hwemu->getDSPVersion() < 0x0400))  // high-speed not supported on v4.xx+
        m_state = DSP_S_HIGHSPEED;  // set state to high-speed; can only get out by resetting the DSP

      m_hwemu->startTransfer(
          ISBDSPHWEmulationLayer::TT_RECORD,
          1,                /* TODO: check for stereo recording (SBPro or cmd. 0xa0/0xa8?) */
          getSampleRate(),  /* TODO: have separate rate for playback/recording */
          8,
          getNumSamples(),
          ISBDSPHWEmulationLayer::CODEC_PCM,
          true);
      return true;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    case 0x30:  /* 030h : MIDI Read Poll */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x30 (MIDI read poll)");
      flushOutputBuffer();  // flush any previous replies
      m_bufOut.push(0xff);  // output reply
      return false;

    case 0x31:  /* 031h : MIDI Read Interrupt */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x31 (MIDI read interrupt)");
      return false;

    case 0x32:  /* 032h : MIDI Read Timestamp Poll */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x32 (MIDI read timestamp poll)");
      flushOutputBuffer();  // flush any previous replies
      m_bufOut.push(0xff);  // output reply
      m_bufOut.push(0xff);
      m_bufOut.push(0xff);
      m_bufOut.push(0xff);
      return false;

    case 0x33:  /* 033h : MIDI Read Timestamp Interrupt */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x33 (MIDI read interrupt poll)");
      return false;

    case 0x34:  /* 034h : MIDI Read Poll + Write Poll (UART) */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x34 (MIDI read poll + write poll)");
      return false;

    case 0x35:  /* 035h : MIDI Read Interrupt + Write Poll (UART) */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x35 (MIDI read interrupt + write poll)");
      return false;

    case 0x36:  /* 036h : MIDI Read Timestamp Poll + Write Poll (UART) */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x36 (MIDI read timestamp poll + write poll)");
      return false;

    case 0x37:  /* 037h : MIDI Read Timestamp Interrupt + Write Poll (UART) */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x37 (MIDI read timestamp interrupt + write poll)");
      return false;

    case 0x38:  /* 038h : MIDI Write Poll */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x38 (MIDI write poll)");
      return false;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    case 0x40:  /* 040h : Set Time Constant */
      setTimeConstant(m_bufIn[1]);  /* TODO: confirm: independent of 16-bit/8-bit (i.e. *sample rate*, not byte rate) */
      return true;

    case 0x41:  /* 041h : Set Output Sample Rate */
      /* TODO: inquire: LSL6 (16-bit sound over 8-bit DMA) treats sample rate as "byte rate" (not 16 bit-samples rate) -- BECAUSE LSL6 does 16 bit over 8-bit DMA?! */
      setSampleRate(MKWORD(m_bufIn[1], m_bufIn[2]));  // sample rate independent from number of channels
      return true;

    case 0x42:  /* 042h : Set Input Sample Rate */
      // TODO: implement (same holds as for DSP cmd. 0x41)
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x42 (Input sample rate)");
      setSampleRate(MKWORD(m_bufIn[1], m_bufIn[2]));  // sample rate independent from number of channels
      return false;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    case 0x45:  /* 045h : Continue Auto-Initialize DMA, 8-bit */
    case 0x47:  /* 047h : Continue Auto-Initialize DMA, 16-bit */
    case 0xd4:  /* 0D4h : Continue DMA Operation, 8-bit */
    case 0xd6:  /* 0D6h : Continue DMA Operation, 16-bit */
      // TODO: do we need to discriminate between 8-bit and 16-bit (i.e. expect
      //  matching 8-bit or 16-bit halt/resume calls), or A/I and S.C. ?
      m_hwemu->resumeTransfer(ISBDSPHWEmulationLayer::TT_PLAYBACK);
      return true;

    case 0x48:  /* 048h : Set DMA Block Size */
      /* TODO: take care (in the future) of ADPCM */
      setNumSamples(MKWORD(m_bufIn[2], m_bufIn[1]) + 1);  /* TODO: how about 16 bit (seems 8-bit only)? how about stereo?! */
      return true;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    case 0x74:  /* 074h : DMA DAC, 4-bit ADPCM */
    case 0x75:  /* 075h : DMA DAC, 4-bit ADPCM Reference */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP commands 0x74/0x75 (DMA DAC, 4-bit ADPCM)");
      return false;

    case 0x76:  /* 076h : DMA DAC, 2.6-bit ADPCM */
    case 0x77:  /* 077h : DMA DAC, 2.6-bit ADPCM Reference */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP commands 0x76/0x77 (DMA DAC, 2.6-bit ADPCM)");
      return false;

    case 0x7d:  /* 07Dh : Auto-Initialize DMA DAC, 4-bit ADPCM Reference */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x7d (A/I DMA DAC, 4-bit ADPCM)");
      return false;

    case 0x7f:  /* 07Fh : Auto-Initialize DMA DAC, 2.6-bit ADPCM Reference */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x7f (A/I DMA DAC, 2.6-bit ADPCM)");
      return false;

    case 0x80:  /* 080h : Silence DAC */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x80 (Silence DAC)");
      return false;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    case 0xa0:  /* 0A0h : Disable Stereo Input Mode */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0xa0 (Disable stereo input)");
      return false;

    case 0xa8:  /* 0A8h : Enable Stereo Input Mode */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0xa8 (Enable stereo input)");
      return false;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    case 0xb0:  /* 0B*h, 0C*h : Generic DAC/ADC DMA (16-bit, 8-bit) */
    case 0xb2:
    case 0xb4:
    case 0xb6:
    case 0xc0:
    case 0xc2:
    case 0xc4:
    case 0xc6:
      m_hwemu->startTransfer(
          (command & 0x08) == 0 ?                       // D/A (playback) vs. A/D (record)
            ISBDSPHWEmulationLayer::TT_PLAYBACK : ISBDSPHWEmulationLayer::TT_RECORD,
          (m_bufIn[1] & 0x20) == 0 ? 1 : 2,             // mono vs. stereo
          getSampleRate(),                              // asmples per channel per second
          (command & 0x10) == 0 ? 8 : 16,               // 8- vs. 16-bit
          MKWORD(m_bufIn[3], m_bufIn[2]) + 1,           // samples between interrupts /* TODO: How about stereo? */
          (m_bufIn[1] & 0x10) == 0 ?                    // unsigned vs. signed quantites
            ISBDSPHWEmulationLayer::CODEC_PCM : ISBDSPHWEmulationLayer::CODEC_PCM_SIGNED,
          (command & 0x04) != 0);                       // autoinit vs. single-cycle
      return true;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    case 0xd0:  /* 0D0h : Halt DMA Operation, 8-bit */
    case 0xd5:  /* 0D5h : Halt DMA Operation, 16-bit */
      m_hwemu->pauseTransfer(ISBDSPHWEmulationLayer::TT_PLAYBACK);
      return true;

    case 0xd1:  /* 0D1h : Enable Speaker */
      m_isSpeakerEna = true;
      return true;

    case 0xd3:  /* 0D3h : Disable Speaker */
      m_isSpeakerEna = false;
      return true;

    case 0xd8:  /* 0D8h : Speaker Status */
      flushOutputBuffer();  // flush any previous replies
      m_bufOut.push(m_isSpeakerEna ? 0xff : 0x00);  // output reply
      return true;

    case 0xd9:  /* 0D9h : Exit Auto-Initialize DMA Operation, 16-bit */
    case 0xda:  /* 0DAh : Exit Auto-Initialize DMA Operation, 8-bit */
      m_hwemu->stopTransfer(ISBDSPHWEmulationLayer::TT_PLAYBACK);
      return true;

    case 0xe0:  /* 0E0h : DSP Identification */
      flushOutputBuffer();  // flush any previous replies
      m_bufOut.push(~m_bufIn[1]); // output reply
      return true;

    case 0xe1:  /* 0E1h : DSP Version */
      /* TODO: maybe make this configurable?! */
      flushOutputBuffer();  // flush any previous replies
      m_bufOut.push(HIBYTE(m_hwemu->getDSPVersion()));  // output reply: major version
      m_bufOut.push(LOBYTE(m_hwemu->getDSPVersion()));  // output reply: minor version
      return true;

    case 0xe2:  /* 0E2h : ??? */
      for (i = 0; i < 8; i++)
        if ((m_bufIn[1] >> i) & 0x01) m_E2Value += E2_incr_table[m_E2Count % 4][i];

      m_E2Value += E2_incr_table[m_E2Count % 4][8];
      m_E2Count++;

      m_hwemu->startTransfer(ISBDSPHWEmulationLayer::TT_E2CMD, m_E2Value, true);

      return true;

    case 0xe3:  /* 0E3h : DSP Copyright */
      flushOutputBuffer();  // flush any previous replies

      // Enqueue the copyright string, including the terminating '\0'
      for (i = 0; i <= strlen(getCopyright()); i++)
        m_bufOut.push(getCopyright()[i]);

      return true;

    case 0xe4:  /* 0E4h : Write Test Register */
      m_testRegister = m_bufIn[1];
      return true;

    case 0xe8:  /* 0E8h : Read Test Register */
      flushOutputBuffer();  // flush any previous replies
      m_bufOut.push(m_testRegister);  // output reply
      return true;

    case 0xf0:  /* 0F0h : Sine Generator */
      // TODO: implement
      m_hwemu->logError("Attempted to use partially implemented DSP command 0xf0 (Sine generator)");
      setTimeConstant(0xc0);

      if (m_hwemu->getDSPVersion() < 0x0400)
        m_isSpeakerEna = true;

      return true;

    case 0xf1:  /* 0F1h : DSP Auxiliary Status (Obsolete) */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0xf1 (DSP aux. status, obsolete)");
      flushOutputBuffer();  // flush any previous replies
      m_bufOut.push(0xff);  // output reply
      return false;

    case 0xf2:  /* 0F2h : IRQ Request, 8-bit */
      set8BitIRQ();         // generate a 8-bit IRQ
      return true;

    case 0xf3:  /* 0F3h : IRQ Request, 16-bit */
      set16BitIRQ();        // generate a 16-bit IRQ
      return true;

    case 0xfb:  /* 0FBh : DSP Status */
      /* TODO: implement -- IMPORTANT */
      m_hwemu->logError("Attempted to use unimplemented DSP command 0xfb (DSP status)");
      flushOutputBuffer();  // flush any previous replies
      m_bufOut.push(0xff);  // output reply
      return false;

    case 0xfc:  /* 0FCh : DSP Auxiliary Status */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0xfc (DSP aux. status)");
      flushOutputBuffer();  // flush any previous replies
      m_bufOut.push(0xff);  // output reply
      return false;

    case 0xfd:  /* 0FDh : DSP Command Status */
      flushOutputBuffer();  // flush any previous replies
      m_bufOut.push(m_lastCommand); // output reply
      return true;

    default:
      sprintf(msgBuf, "Attempted to use undocumented DSP command 0x%02x; CHECK DSP_cmd_len TABLE!", command & 0xff);
      m_hwemu->logError(msgBuf);
  }

  return false;
}

const char* CSBCompatCtlDSP::getCopyright(void) {
  return "COPYRIGHT (C) CREATIVE TECHNOLOGY LTD, 1992.";
}

/* TODO: ** BUG! ** if stereo/mono hard-coded in command (not taken from
   mixer) and time-constant is used, the wrong pitch will probably be used!!!
   Inquire if, in samplerate=fn(timeconstant,#channels), the # of channels
   is evaluated (1) at the time the TC is set, or (2) when the DSP command
   is executed, and if (2), then if only the mixer #channels is used, or 
   if for commands bxh/cxh stereo/mono is also used */

int CSBCompatCtlDSP::getNumChannels(void) {
  if (m_hwemu->getDSPVersion() < 0x0400) {
    return (m_sbmix->isStereoOutput() ? 2 : 1);
  } else {
    return 1;
  }
}
