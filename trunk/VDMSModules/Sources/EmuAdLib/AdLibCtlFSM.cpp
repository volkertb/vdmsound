#include "stdafx.h"

#include "AdLibCtlFSM.h"

#define OPL_T1_PERIOD 80
#define OPL_T2_PERIOD 320

#define OPL_STATUS_IRQ_PENDING 0x80
#define OPL_STATUS_T1_EXPIRED  0x40
#define OPL_STATUS_T2_EXPIRED  0x20

#define OPL_CONTROL_RST_STATUS 0x80
#define OPL_CONTROL_T1_MASKED  0x40
#define OPL_CONTROL_T2_MASKED  0x20
#define OPL_CONTROL_T1_ENABLED 0x01
#define OPL_CONTROL_T2_ENABLED 0x02

CAdLibCtlFSM::CAdLibCtlFSM(IAdLibHWEmulationLayer* hwemu)
  : m_status(0), m_regIdx(0), m_hwemu(hwemu)
{
  _ASSERTE(m_hwemu != NULL);
}

CAdLibCtlFSM::~CAdLibCtlFSM(void)
{
}


/////////////////////////////////////////////////////////////////////////////


//
// Resets the OPL
//
void CAdLibCtlFSM::reset(void) {
  // Restart and unmask timer 1
  m_timer1.isEnabled = true;
  m_timer1.isMasked  = false;
  m_timer1.period    = OPL_T1_PERIOD * 256;

  // Restart and unmask timer 2
  m_timer2.isEnabled = true;
  m_timer2.isMasked  = false;
  m_timer2.period    = OPL_T2_PERIOD * 256;

  // Get the current time
  OPLTime_t tNow = m_hwemu->getTimeMicros();

  // Reset status flags and timers
  clearStatus();
  clearTimers(tNow);
}

//
// Called when the status is read from the OPL's status port
//
char CAdLibCtlFSM::getStatus(void) {
  // Get the current time
  OPLTime_t tNow = m_hwemu->getTimeMicros();

# ifdef _DEBUG
  char buf[1024];
  sprintf(buf, "Polling OPL status: timer 1 is %s/%s/%s, triggers every %0.3fms (%0.3fms since last roll-over); timer 2 is %s/%s/%s, triggers every %0.3fms (%0.3fms since last roll-over).", m_timer1.isMasked ? "masked" : "unmasked", m_timer1.isEnabled ? "enabled" : "disabled", m_timer1.isExpired ? "expired" : "not expired", (float)(m_timer1.period / 1000.0), (float)((m_timer1.period - (m_timer1.expiry - tNow)) / 1000.0), m_timer2.isMasked ? "masked" : "unmasked", m_timer2.isEnabled ? "enabled" : "disabled", m_timer2.isExpired ? "expired" : "not expired", (float)(m_timer2.period / 1000.0), (float)((m_timer2.period - (m_timer2.expiry - tNow)) / 1000.0));
  m_hwemu->logInformation(buf);
# endif

  return updateTimers(tNow);
}

//
// Called when a register is selected via the OPL's address port
//
void CAdLibCtlFSM::setAddress(char data) {
  m_regIdx = data & 0xff;
}

//
// Called when a data byte is output to the OPL's data port
//
void CAdLibCtlFSM::putData(char data) {
  char buf[1024];

# ifdef _DEBUG
  sprintf(buf, "OPL write 0x%02x to register 0x%02x", data & 0xff, m_regIdx & 0xff);
  m_hwemu->logInformation(buf);
# endif

  switch (m_regIdx) {
    case 0x02:    // Timer 1 count
      m_timer1.period = OPL_T1_PERIOD * (256 - (data & 0xff));
      sprintf(buf, "Reprogramming OPL timer 1, counter value = %d (T = %5.3fms).", data & 0xff, (float)(m_timer1.period/1000.0));
      m_hwemu->logInformation(buf);
      return;

    case 0x03:    // Timer 2 count
      m_timer2.period = OPL_T2_PERIOD * (256 - (data & 0xff));
      sprintf(buf, "Reprogramming OPL timer 2, counter value = %d (T = %5.3fms).", data & 0xff, (float)(m_timer2.period/1000.0));
      m_hwemu->logInformation(buf);
      return;

    case 0x04:    // IRQ-Reset / Mask / Start
      if (data & 0x80) {
        // IRQ-Reset bit. Resets timer status flags.
        // All other bits are ignored when this bit is set.
        m_hwemu->logInformation("Resetting OPL status flags.");
        clearStatus();
        return;
      } else {
        // Get the current time
        OPLTime_t tNow = m_hwemu->getTimeMicros();

        // Catch up with timer emulation
        updateTimers(tNow);

        // Mask/unmask timers
        m_timer1.isMasked = ((data & OPL_CONTROL_T1_MASKED) != 0);
        m_timer2.isMasked = ((data & OPL_CONTROL_T2_MASKED) != 0);

        // Clear expiration flags for masked timers
        clearStatus(m_timer1.isMasked, m_timer2.isMasked);

        // Enable/disable timers
        bool old_T1_isEnabled = m_timer1.isEnabled;
        bool old_T2_isEnabled = m_timer2.isEnabled;

        m_timer1.isEnabled = ((data & OPL_CONTROL_T1_ENABLED) != 0);
        m_timer2.isEnabled = ((data & OPL_CONTROL_T2_ENABLED) != 0);

        // If previously-disabled timers were just enabled, resynchronize
        clearTimers(tNow, m_timer1.isEnabled && !old_T1_isEnabled, m_timer2.isEnabled && !old_T2_isEnabled);

        sprintf(buf, "Reprogramming OPL timers (timer 1 is %s and %s, timer 2 is %s and %s).", m_timer1.isEnabled ? "enabled" : "disabled", m_timer1.isMasked ? "masked" : "unmasked", m_timer2.isEnabled ? "enabled" : "disabled", m_timer2.isMasked ? "masked" : "unmasked");
        m_hwemu->logInformation(buf);

        return;
      } 

    default:
      m_hwemu->setOPLReg(m_regIdx, data & 0xff);
      return;
  }
}



/////////////////////////////////////////////////////////////////////////////



//
// Selectively clear the status (timer expiration and/or IRQ pending) flags
//
void CAdLibCtlFSM::clearStatus(bool T1, bool T2) {
  if (T1) m_status &= ~OPL_STATUS_T1_EXPIRED;
  if (T2) m_status &= ~OPL_STATUS_T2_EXPIRED;
  if ((m_status & (OPL_STATUS_T1_EXPIRED | OPL_STATUS_T2_EXPIRED)) == 0) m_status &= ~OPL_STATUS_IRQ_PENDING;
}

//
// Selectively reset the timers (start over the countdown)
//
void CAdLibCtlFSM::clearTimers(OPLTime_t tNow, bool T1, bool T2) {
  if (T1) m_timer1.expiry = tNow + m_timer1.period;
  if (T2) m_timer2.expiry = tNow + m_timer2.period;
}

//
// Check which timers have expired (if any) and update the status flags
//
char CAdLibCtlFSM::updateTimers(OPLTime_t tNow) {
  // Check if Timer 1 has expired
  if ((m_timer1.isEnabled) && (tNow >= m_timer1.expiry)) {
    // Compute the next expiration time
    m_timer1.expiry = tNow + m_timer1.period - ((tNow - m_timer1.expiry) % m_timer1.period);
    // If the timer is not masked then mark it as expired and flag IRQ pending
    if (!m_timer1.isMasked) m_status |= (OPL_STATUS_IRQ_PENDING | OPL_STATUS_T1_EXPIRED);
  }

  // Check if Timer 2 has expired
  if ((m_timer2.isEnabled) && (tNow >= m_timer2.expiry)) {
    // Mark the timer as expired and compute the next expiration time
    m_timer2.expiry = tNow + m_timer2.period - ((tNow - m_timer2.expiry) % m_timer2.period);
    // If the timer is not masked then mark it as expired and flag IRQ pending
    if (!m_timer2.isMasked) m_status |= (OPL_STATUS_IRQ_PENDING | OPL_STATUS_T2_EXPIRED);
  }

  return m_status;
}
