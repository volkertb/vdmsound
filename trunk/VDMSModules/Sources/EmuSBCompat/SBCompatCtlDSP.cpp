#include "stdafx.h"

#include "SBConst.h"
#include "SBCompatCtlDSP.h"


/////////////////////////////////////////////////////////////////////////////


#define MKWORD(bHi, bLo) ((((bHi) & 0xff) << 8) | ((bLo) & 0xff))


/////////////////////////////////////////////////////////////////////////////


CSBCompatCtlDSP::CSBCompatCtlDSP(ISBDSPHWEmulationLayer* hwemu)
  : m_hwemu(hwemu),
    m_isRstAsserted(false),         // the reset sequence was not yet engaged
    m_isSpeakerEna(false),          // the speaker is disabled
    m_useTimeConstant(false),       // playback rate determined by sample rate, not time constant
    m_lastCommand((char)0x00),      // no last command
    m_testRegister((char)0x00)      // no value in the test register
{
  ASSERT(m_hwemu != NULL);
}

CSBCompatCtlDSP::~CSBCompatCtlDSP(void)
{
}


/////////////////////////////////////////////////////////////////////////////


//
// Unconditionally resets the DSP
//
void CSBCompatCtlDSP::reset(void) {
  m_isRstAsserted = false;

  // flush buffers
  while (!m_bufOut.empty())
    m_bufOut.pop();
  m_bufIn.clear();

  // reset other internal registers, except for:
  // - test register: must remain the same across resets
  m_isSpeakerEna = false;
  m_lastCommand  = (char)0x00;

  setNumSamples(0);
  setNumChannels(1);
  setNumBits(8);
  setSampleRate(11025);

  /* TODO: stop DMA xfer? (ops. 0x90, 0x98, ... others ?) */
  /* TODO: account for different meanings of RESET when in high-speed DMA,
     MIDI UART mode, etc. */
}

//
// Resets DSP on 1->0 level transition
//
void CSBCompatCtlDSP::reset(char data) {
  if (data != 0) {
    m_isRstAsserted = true;
  } else if (m_isRstAsserted) {
    reset();
    m_bufOut.push((char)0xaa);  // acknowledge reset
  }
}

void CSBCompatCtlDSP::putCommand(char data) {
  // put data into incoming queue
  m_bufIn.push_back(data);

  // what command we are dealing with, and how long
  unsigned char cmd = m_bufIn[0];
  int commandLength = DSP_cmd_len[cmd & 0xff];

  if (commandLength <= 0) {
    char msgBuf[1024];
    sprintf(msgBuf, "Attempted to use undocumented DSP command 0x%02x", cmd & 0xff);
    m_hwemu->logWarning(msgBuf);
    m_bufIn.clear();    // flush buffer
    return;
  }

  if (commandLength < m_bufIn.size()) {
    char msgBuf[1024];
    sprintf(msgBuf, "DSP internal state inconsistency: command = 0x%02x, in buffer = %d bytes, expected %d bytes", cmd & 0xff, m_bufIn.size(), commandLength);
    m_hwemu->logWarning(msgBuf);
    m_bufIn.clear();    // flush buffer
    return;
  }

  if (commandLength  == commandLength) {
    if (processCommand(cmd))
      m_lastCommand = cmd;

    m_bufIn.clear();    // flush buffer
    return;
  }
}

char CSBCompatCtlDSP::getData(void) {
  if (!m_bufOut.empty()) {
    char data = m_bufOut.front();
    m_bufOut.pop();
    return data;
  } return (char)0xff;
}

char CSBCompatCtlDSP::getWrStatus(void) {
  // if there is still data in the output
  // buffer, then don't accept commands
  return m_bufOut.empty() ? 0x00 : 0x80;
}

char CSBCompatCtlDSP::getRdStatus(void) {
  // is there data in the output buffer ?
  return m_bufOut.empty() ? 0x00 : 0x80;
}

void CSBCompatCtlDSP::ack8bitIRQ(void) {
  // TODO: implement
}

void CSBCompatCtlDSP::ack16bitIRQ(void) {
  // TODO: implement
}


/////////////////////////////////////////////////////////////////////////////


bool CSBCompatCtlDSP::processCommand(unsigned char command) {
  ASSERT(m_bufIn.size() > 0);
  ASSERT(m_bufIn[0] == command);

  int i;
  char msgBuf[1024] = "";

  switch (command) {
    case 0x04:  /* 004h : DSP Status (Obsolete) */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x04 (DSP status)");
      m_bufOut.push(0xff);
      return false;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    case 0x10:  /* 010h : Direct DAC, 8-bit */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x10 (Direct DAC)");
      return false;

    case 0x14:  /* 014h : DMA DAC, 8-bit */
    case 0x91:  /* 091h : DMA DAC, 8-bit (High Speed) */
      setNumBits(8);
      setNumSamples(MKWORD(m_bufIn[2], m_bufIn[1]) + 1);  /* TODO: how about stereo? */
//    DSPPlay(false);
      return true;

    case 0x16:  /* 016h : DMA DAC, 2-bit ADPCM */
    case 0x17:  /* 017h : DMA DAC, 2-bit ADPCM Reference */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP commands 0x16/0x17 (DMA DAC 2-bit ADPCM)");
      return false;

    case 0x1c:  /* 01Ch : Auto-Initialize DMA DAC, 8-bit */
    case 0x90:  /* 090h : Auto-Initialize DMA DAC, 8-bit (High Speed) */
      // TODO: account for high-speed by blocking DSP command interpretation until reset?
      setNumBits(8);
//    DSPPlay(true);
      if (command == 0x90)
        m_hwemu->logWarning("Attempted to use partially unimplemented DSP command 0x90 (A/I DMA DAC 8-bit high-speed)");
      return true;

    case 0x1f:  /* 01Fh : Auto-Initialize DMA DAC, 2-bit ADPCM Reference */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x1f (A/I DMA DAC, 2-bit ADPCM ref.)");
      return false;

    case 0x20:  /* 020h : Direct ADC, 8-bit */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x20 (Direct ADC)");
      m_bufOut.push(0xff);
      return false;

    case 0x24:  /* 024h : DMA ADC, 8-bit */
    case 0x99:  /* 024h : DMA ADC, 8-bit (High Speed) */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP commands 0x24/0x99 (DMA ADC, 8-bit [+/- high speed])");
      return false;

    case 0x28:  /* 028h : Direct ADC, 8-bit (Burst) */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x28 (Direct ADC, burst)");
      return false;

    case 0x2c:  /* 02Ch : Auto-Initialize DMA ADC, 8-bit */
    case 0x98:  /* 098h : Auto-Initialize DMA ADC, 8-bit (High Speed) */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP commands 0x2c/0x98 (A/I DMA ADC, 8-bit [+/- high speed])");
      return false;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    case 0x30:  /* 030h : MIDI Read Poll */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x30 (MIDI read poll)");
      m_bufOut.push(0xff);
      return false;

    case 0x31:  /* 031h : MIDI Read Interrupt */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x31 (MIDI read interrupt)");
      return false;

    case 0x32:  /* 032h : MIDI Read Timestamp Poll */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x32 (MIDI read timestamp poll)");
      m_bufOut.push(0xff);
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
      setSampleRate(MKWORD(m_bufIn[1], m_bufIn[2])); 
      return true;

    case 0x42:  /* 042h : Set Input Sample Rate */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0x42 (Input sample rate)");
      return false;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    case 0x45:  /* 045h : Continue Auto-Initialize DMA, 8-bit */
    case 0x47:  /* 047h : Continue Auto-Initialize DMA, 16-bit */
//    DSPResume();
      return true;

    case 0x48:  /* 048h : Set DMA Block Size */
      /* TODO: take care (in the future) of ADPCM */
      setNumBits(8);    /* TODO: verify that, indeed, this cannot be 16 bit; how about stereo?! */
      setNumSamples(MKWORD(m_bufIn[2], m_bufIn[1]));
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
      switch (command & 0xf0) {
        case 0xb0: setNumBits(16); break;
        case 0xc0: setNumBits(8); break;
      }

      if (true) {
        int mode = m_bufIn[1];

        bool isRecord = ((command & 0x08) == 0x08);     // A/D (record) vs. D/A (playback)
        bool autoInit = ((command & 0x04) == 0x04);     // autoinit vs. single-cycle
        bool isFIFO   = ((command & 0x02) == 0x02);     // not really used

        bool isStereo = ((command & 0x20) == 0x20);     // stereo vs. mono
        bool isSigned = ((command & 0x10) == 0x10);     // signed vs. unsigned quantites

        if (isStereo) {
          setNumChannels(2);
        } else {
          setNumChannels(1);
        }

        setNumSamples(MKWORD(m_bufIn[3], m_bufIn[2]) + 1); /* TODO: How about stereo? */
//      DSPPlay(autoInit, true, isSigned ? SB16_CODEC_SIGNED_PCM : SB16_CODEC_UNSIGNED_PCM);
      } return true;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    case 0xd0:  /* 0D0h : Halt DMA Operation, 8-bit */
//    DSPPause();
      return true;

    case 0xd1:  /* 0D1h : Enable Speaker */
      m_isSpeakerEna = true;
      return true;

    case 0xd3:  /* 0D3h : Disable Speaker */
      m_isSpeakerEna = false;
      return true;

    case 0xd4:  /* 0D4h : Continue DMA Operation, 8-bit */
//    DSPResume();
      return true;

    case 0xd5:  /* 0D5h : Halt DMA Operation, 16-bit */
//    DSPPause();
      return true;

    case 0xd6:  /* 0D6h : Continue DMA Operation, 16-bit */
//    DSPResume();
      return true;

    case 0xd8:  /* 0D8h : Speaker Status */
      m_bufOut.push(m_isSpeakerEna ? 0xff : 0x00);
      return true;

    case 0xd9:  /* 0D9h : Exit Auto-Initialize DMA Operation, 16-bit */
    case 0xda:  /* 0DAh : Exit Auto-Initialize DMA Operation, 8-bit */
//    DSPStop();
      return true;

    case 0xe0:  /* 0E0h : DSP Identification */
      m_bufOut.push(~m_bufIn[1]);
      return true;

    case 0xe1:  /* 0E1h : DSP Version */
      /* TODO: maybe make this configurable?! */
      m_bufOut.push(0x04);  // major
      m_bufOut.push(0x05);  // minor
      return true;

    case 0xe3:  /* 0E3h : DSP Copyright */
      for (i = 0; i <= strlen(getCopyright()); i++)
        m_bufOut.push(getCopyright()[i]);
      return true;

    case 0xe4:  /* 0E4h : Write Test Register */
      m_testRegister = m_bufIn[1];
      return true;

    case 0xe8:  /* 0E8h : Read Test Register */
      m_bufOut.push(m_testRegister);
      break;

    case 0xf0:  /* 0F0h : Sine Generator */
      // TODO: implement
      m_hwemu->logError("Attempted to use partially implemented DSP command 0xf0 (Sine generator)");
      m_isSpeakerEna = true;
      return true;

    case 0xf1:  /* 0F1h : DSP Auxiliary Status (Obsolete) */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0xf1 (DSP aux. status, obsolete)");
      m_bufOut.push(0xff);
      return false;

    case 0xf2:  /* 0F2h : IRQ Request, 8-bit */
    case 0xf3:  /* 0F3h : IRQ Request, 16-bit */
//    notify(SB16_NOT_INTERRUPT, NULL);     // NULL: not auto-init
      return true;

    case 0xfb:  /* 0FBh : DSP Status */
      /* TODO: implement -- IMPORTANT */
      m_hwemu->logError("Attempted to use unimplemented DSP command 0xfb (DSP status)");
      m_bufOut.push(0xff);
      return false;

    case 0xfc:  /* 0FCh : DSP Auxiliary Status */
      // TODO: implement
      m_hwemu->logError("Attempted to use unimplemented DSP command 0xfc (DSP aux. status)");
      m_bufOut.push(0xff);
      return false;

    case 0xfd:  /* 0FDh : DSP Command Status */
      /* TODO: inquire: exclude self ?! */
      m_bufOut.push(m_lastCommand);
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
