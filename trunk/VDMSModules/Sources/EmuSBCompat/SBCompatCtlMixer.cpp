#include "stdafx.h"

#include "SBCompatCtlMixer.h"


/////////////////////////////////////////////////////////////////////////////


// gets the bh most significant bits of x as a bh-bit value
// e.g. bh = 5: xXxxx??? -> 000xXxxx; bh = 1: x??????? -> 0000000x
#define B_HI(x,bh) (((x) >> (8 - (bh))) & ((1 << (bh)) - 1))
// gets the bl least significant bits of x as a bl-bit value
// e.g. bl = 5: ???xxxXx -> 000xxxXx; bl = 1: ???????x -> 0000000x
#define B_LO(x,bl) ((x) & ((1 << (bl)) - 1))
// merges two bit strings as follows: bl bits from lo become the bl least significant
// bits of the result; the rest of 8-bl bits are taken from hi's 8-bl least significant bits
// e.g. hi = ??xxxXxx, lo = ??????yY, bl = 2 -> xxxXxxyY
#define B_MK(hi,lo,bl) ((((hi) & ((1 << (8 - (bl))) - 1)) << (bl)) | ((lo) & ((1 << (bl)) - 1)))


/////////////////////////////////////////////////////////////////////////////


CSBCompatCtlMixer::CSBCompatCtlMixer(ISBMixerHWEmulationLayer* hwemu)
  : m_hwemu(hwemu), m_regIdx(0),
    m_IRQSelect((char)0xf0),  // bits 7..4 reserved: '1' when read
    m_DMASelect((char)0x00),  // bits 2, 4 reserved: '0' when read
    m_IRQStatus((char)0x10),  // bits 7..3 reserved: 10h for v4.04, 20h for v4.05, 80h for v4.12
    m_lastValue((char)0x00)   // TODO: default value?  Gets updatet on 'getValue' only?  On 'putValue' as well?
{
  ASSERT(m_hwemu != NULL);
}

CSBCompatCtlMixer::~CSBCompatCtlMixer(void)
{
}


/////////////////////////////////////////////////////////////////////////////


//
// Resets the mixer to its default state
//
void CSBCompatCtlMixer::reset(void) {
  // all registers are reset, except for
  // 0x80 (IRQSelect) and 0x81 (DMASelect)
  m_regIdx = 0;

  setMasterVol(0xc0, 0xc0, 0);
  setDACLevel(0xc0, 0xc0, 0);
  setLineLevel(0x00, 0x00, 0);
  setCDALevel(0x00, 0x00, 0);
  setMicLevel(0x00, 0);
  /* TODO: reset additional registers as they are implemented */
}

//
// Called when an address byte is output to the mixer's address port
//
void CSBCompatCtlMixer::setAddress(
    char address)
{
  m_regIdx = address & 0xff;
}

//
// Called when a value is output to the mixer's value port
//
void CSBCompatCtlMixer::setValue(
    char value)
{
  char msgBuf[1024] = "";

# ifdef _DEBUG
  sprintf(msgBuf, "Setting MIXER register 0x%02x (value = 0x%02x)", m_regIdx, value & 0xff);
  m_hwemu->logInformation(msgBuf);
# endif

  switch (m_regIdx) {
    case 0x00:  /* reset */
      reset(value);
      return;

    case 0x02:  /* master volume */
    case 0x22:  /* master volume */
      setMasterVol(B_HI(value, 4), B_LO(value, 4), 8 - 4);  // highest 4 bits = "left", lowest 4 = "right"; only 4 out of 8 bits (8 - 4) used for representation
      return;

    case 0x04:  /* DAC level */
      setDACLevel(B_HI(value, 4), B_LO(value, 4), 8 - 4);
      return;

    /* TODO: implement 0x06, 0x34/0x35 (FM) */

    case 0x0a:  /* microphone level */
      setMicLevel(B_LO(value, 3), 8 - 3);
      return;

    case 0x0e:  /* output/stereo select */
      // TODO: implement
      return;

    case 0x28:  /* CD audio level */
      setCDALevel(B_HI(value, 4), B_LO(value, 4), 8 - 4);
      return;

    case 0x2e:  /* line in level */
      setLineLevel(B_HI(value, 4), B_LO(value, 4), 8 - 4);
      return;

    case 0x30:  /* master volume left */
      setMasterVol(B_HI(value, 5), -1, 8 - 5);
      return;

    case 0x31:  /* master volume right */
      setMasterVol(-1, B_HI(value, 5), 8 - 5);
      return;

    case 0x32:  /* DAC level left */
      setDACLevel(B_HI(value, 5), -1, 8 - 5);
      return;

    case 0x33:  /* DAC level right */
      setDACLevel(-1, B_HI(value, 5), 8 - 5);
      return;

    case 0x36:  /* CD audio level left */
      setCDALevel(B_HI(value, 5), -1, 8 - 5);
      return;

    case 0x37:  /* CD audio level right */
      setCDALevel(-1, B_HI(value, 5), 8 - 5);
      return;

    case 0x38:  /* line in level left */
      setLineLevel(B_HI(value, 5), -1, 8 - 5);
      return;

    case 0x39:  /* line in level right */
      setLineLevel(-1, B_HI(value, 5), 8 - 5);
      return;

    case 0x3a:  /* microphone level */
      setMicLevel(B_HI(value, 5), 8 - 5);
      return;

    /* TODO: implement 0x0c, 0x3b..0x47 (speaker, treble & bass, input) */

    case 0x80:  /* IRQ select */
      // TODO: implement: change the IRQ in controller using m_hwemu
      sprintf(msgBuf, "Attempted to programatically change IRQ (register = 0x%02x, value = 0x%02x)", m_regIdx, value & 0xff);
      m_hwemu->logWarning(msgBuf);
      return;

    case 0x81:  /* DMA select */
      // TODO: implement: change the DMA(s) in controller using m_hwemu
      sprintf(msgBuf, "Attempted to programatically change DMA channel(s) (register = 0x%02x, value = 0x%02x)", m_regIdx, value & 0xff);
      m_hwemu->logWarning(msgBuf);
      return;

    case 0x06:
    case 0x0c:
    case 0x26:
    case 0x34:
    case 0x35:
    case 0x3b:
    case 0x3c:
    case 0x3d:
    case 0x3e:
    case 0x3f:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
      // TODO: implement
      sprintf(msgBuf, "Attempted to write to unimplemented mixer register (register = 0x%02x, value = 0x%02x)", m_regIdx, value & 0xff);
      m_hwemu->logWarning(msgBuf);
      __dummy[m_regIdx] = value;
      return;

    case 0x01:
    case 0x82:
      sprintf(msgBuf, "Attempted to write to read-only mixer register (register = 0x%02x, value = 0x%02x)", m_regIdx, value & 0xff);
      m_hwemu->logWarning(msgBuf);
      return;

    default:
      sprintf(msgBuf, "Attempted to write to an unknown mixer register (register = 0x%02x, value = 0x%02x)", m_regIdx, value & 0xff);
      m_hwemu->logError(msgBuf);
      return;
  }
}

//
// Called when a data byte is read from the MPU-401's data port
//
char CSBCompatCtlMixer::getValue(void) {
  int left, right;

  char msgBuf[1024] = "";

# ifdef _DEBUG
  sprintf(msgBuf, "Reading MIXER register 0x%02x", m_regIdx);
  m_hwemu->logInformation(msgBuf);
# endif

  switch (m_regIdx) {
    case 0x01:  /* status */
      // TODO: implement (get more information: last value or last index/address?!)
      sprintf(msgBuf, "Attempted to read from unimplemented status register (register = 0x%02x)", m_regIdx);
      m_hwemu->logWarning(msgBuf);
      return 0x00;

    case 0x02:  /* master volume */
    case 0x22:  /* master volume */
      getMasterVol(&left, &right, 8 - 4);
      return (char)B_MK(left, right, 4);

    case 0x04:  /* DAC level */
      getDACLevel(&left, &right, 8 - 4);
      return (char)B_MK(left, right, 4);

    /* TODO: implement 0x06, 0x34/0x35 (FM) */

    case 0x0a:  /* microphone level */
      getMicLevel(&right, 8 - 3);
      return (char)B_MK(0, right, 3);

    case 0x0e:  /* output/stereo select */
      // TODO: implement
      sprintf(msgBuf, "Attempted to read from unimplemented output stereo select register (register = 0x%02x)", m_regIdx);
      m_hwemu->logWarning(msgBuf);
      break;

    case 0x28:  /* CD audio level */
      getCDALevel(&left, &right, 8 - 4);
      return (char)B_MK(left, right, 4);

    case 0x2e:  /* line in level */
      getLineLevel(&left, &right, 8 - 4);
      return (char)B_MK(left, right, 4);

    case 0x30:  /* master volume left */
      getMasterVol(&left, NULL, 8 - 5);
      return (char)B_MK(left, 0, 8 - 5);

    case 0x31:  /* master volume right */
      getMasterVol(NULL, &right, 8 - 5);
      return (char)B_MK(right, 0, 8 - 5);

    case 0x32:  /* DAC level left */
      getDACLevel(&left, NULL, 8 - 5);
      return (char)B_MK(left, 0, 8 - 5);

    case 0x33:  /* DAC level right */
      getDACLevel(NULL, &right, 8 - 5);
      return (char)B_MK(right, 0, 8 - 5);

    case 0x36:  /* CD audio level left */
      getCDALevel(&left, NULL, 8 - 5);
      return (char)B_MK(left, 0, 8 - 5);

    case 0x37:  /* CD audio level right */
      getCDALevel(NULL, &right, 8 - 5);
      return (char)B_MK(right, 0, 8 - 5);

    case 0x38:  /* line in level left */
      getLineLevel(&left, NULL, 8 - 5);
      return (char)B_MK(left, 0, 8 - 5);

    case 0x39:  /* line in level right */
      getLineLevel(NULL, &right, 8 - 5);
      return (char)B_MK(right, 0, 8 - 5);

    case 0x3a:  /* microphone level */
      getMicLevel(&right, 8 - 5);
      return (char)B_MK(right, 0, 8 - 5);

    /* TODO: implement 0x0c, 0x3b..0x47 (speaker, treble & bass, input) */

    case 0x80:  /* IRQ select */
      return m_IRQSelect;

    case 0x81:  /* DMA select */
      return m_DMASelect;

    case 0x82:  /* IRQ status */
      return m_IRQStatus;

    case 0x06:
    case 0x0c:
    case 0x26:
    case 0x34:
    case 0x35:
    case 0x3b:
    case 0x3c:
    case 0x3d:
    case 0x3e:
    case 0x3f:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
      // TODO: implement
      sprintf(msgBuf, "Attempted to read from an unimplemented mixer register (register = 0x%02x, value = 0x%02x)", m_regIdx, __dummy[m_regIdx] & 0xff);
      m_hwemu->logWarning(msgBuf);
      return __dummy[m_regIdx];

    case 0x00:
      sprintf(msgBuf, "Attempted to read from a write-only mixer register (register = 0x%02x)", m_regIdx);
      m_hwemu->logWarning(msgBuf);
      break;

    default:
      sprintf(msgBuf, "Attempted to read from an unknown mixer register (register = 0x%02x)", m_regIdx);
      m_hwemu->logError(msgBuf);
      break;
  }

  return (char)0xff;
}

//
// Called when the card's IRQ is changed, either programatically
// (through mixer register 0x80), or internally (initialization)
//
void CSBCompatCtlMixer::setIRQSelect(
    int IRQ)                        // one of the supported IRQ's (2, 5, 7 or 10)
{
  switch (IRQ) {
    case 2:  m_IRQSelect = (m_IRQSelect & 0xf0) | 0x01; break;
    case 5:  m_IRQSelect = (m_IRQSelect & 0xf0) | 0x02; break;
    case 7:  m_IRQSelect = (m_IRQSelect & 0xf0) | 0x04; break;
    case 10: m_IRQSelect = (m_IRQSelect & 0xf0) | 0x08; break;
    default:
      ASSERT(false);
  }
}

//
// Called when the card's DMA(s) are changed, either programatically
// (through mixer register 0x81), or internally (initialization)
//
void CSBCompatCtlMixer::setDMASelect(
    int DMA8,                       // DMA channel to use for 8-bit transfers (-1 if none)
    int DMA16)                      // DMA channel to use for 16-bit transfers (-1 if none)
{
  ASSERT((DMA8 == -1) || ((DMA8 >= 0) && (DMA8 <= 3) && (DMA8 != 2)));
  ASSERT((DMA8 == -1) || ((DMA16 >= 5) && (DMA16 <= 7)));

  int dma8mask  = (DMA8 < 0)  ? 0 : (1 << DMA8);
  int dma16mask = (DMA16 < 0) ? 0 : (1 << DMA16);

  m_DMASelect = (m_DMASelect & (~0xeb)) | ((dma8mask | dma16mask) & 0xeb);
}

//
// Called right before an IRQ is generated, for the ISR to know why
// it was called, or after an IRQ acknowledge (to clear the status)
//
void CSBCompatCtlMixer::setIRQStatus(
    IRQSource_t source,             // on whose behalf the interrupt was generated
    bool isPending)                 // was the interrupt just generated (pending), or acknowledged (must clear)?
{
  ASSERT((source >= 0) && (source <= 2));

  int mask = 1 << source;

  if (isPending) {
    m_IRQStatus |= mask;            // set the bit
  } else {
    m_IRQStatus &= (~mask);         // clear the bit
  }
}


/////////////////////////////////////////////////////////////////////////////


void CSBCompatCtlMixer::reset(
    char data)
{
  reset();
}

void CSBCompatCtlMixer::setMasterVol(
    int left,                       // left volume (down-scaled); ignored if -1
    int right,                      // right volume (down-scaled); ignored if -1
    int scale)                      // by how many bits the left/right are down-scaled
{
  if (left > -1) m_masterL = left << scale;
  if (right > -1) m_masterR = right << scale;
}

void CSBCompatCtlMixer::setDACLevel(int left, int right, int scale) {
  if (left > -1) m_dacL = left << scale;
  if (right > -1) m_dacR = right << scale;
}

void CSBCompatCtlMixer::setLineLevel(int left, int right, int scale) {
  if (left > -1) m_lineL = left << scale;
  if (right > -1) m_lineR = right << scale;
}

void CSBCompatCtlMixer::setCDALevel(int left, int right, int scale) {
  if (left > -1) m_cdaL = left << scale;
  if (right > -1) m_cdaR = right << scale;
}

void CSBCompatCtlMixer::setMicLevel(int level, int scale) {
  m_mic = level << scale;
}

void CSBCompatCtlMixer::getMasterVol(
    int *left,                      // left volume (down-scaled); ignored if NULL
    int *right,                     // right volume (down-scaled); ignored if NULL
    int scale)                      // by how many bits the results should be down-scaled
{
  if (left) *left = m_masterL >> scale;
  if (right) *right = m_masterR >> scale;
}

void CSBCompatCtlMixer::getDACLevel(int* left, int* right, int scale) {
  if (left) *left = m_dacL >> scale;
  if (right) *right = m_dacR >> scale;
}

void CSBCompatCtlMixer::getLineLevel(int* left, int* right, int scale) {
  if (left) *left = m_lineL >> scale;
  if (right) *right = m_lineR >> scale;
}

void CSBCompatCtlMixer::getCDALevel(int* left, int* right, int scale) {
  if (left) *left = m_cdaL >> scale;
  if (right) *right = m_cdaR >> scale;
}

void CSBCompatCtlMixer::getMicLevel(int* level, int scale) {
  *level = m_mic >> scale;
}
