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
  : m_regIdx(0), m_status(0), m_hwemu(hwemu)
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
  // Get the current time
  OPLTime_t t_now = m_hwemu->getTimeMicros();

  // Clear the status register
  m_status = 0x00;

  // Restart and unmask timer 1
  m_timer1.isEnabled = true;
  m_timer1.isMasked  = false;
  m_timer1.timeout   = OPL_T1_PERIOD * 256;
  m_timer1.timebase  = t_now;

  // Restart and unmask timer 2
  m_timer2.isEnabled = true;
  m_timer2.isMasked  = false;
  m_timer2.timeout   = OPL_T2_PERIOD * 256;
  m_timer2.timebase  = t_now;
}

//
// Called when the status is read from the OPL's status port
//
char CAdLibCtlFSM::getStatus(void) {
  // Get the current time
  OPLTime_t t_now = m_hwemu->getTimeMicros();

# ifdef _DEBUG
  char buf[1024];
  sprintf(buf, "Polling OPL status: timer 1 is %s and %s, triggers every %0.3fms (%0.3fms since last roll-over); timer 2 is %s and %s, triggers every %0.3fms (%0.3fms since last roll-over).", m_timer1.isEnabled ? "enabled" : "disabled", m_timer1.isMasked ? "masked" : "unmasked", (float)(m_timer1.timeout / 1000.0), (float)((t_now - m_timer1.timebase) / 1000.0), m_timer2.isEnabled ? "enabled" : "disabled", m_timer2.isMasked ? "masked" : "unmasked", (float)(m_timer2.timeout / 1000.0), (float)((t_now - m_timer2.timebase) / 1000.0));
  m_hwemu->logInformation(buf);
# endif

  // Check if Timer 1 has expired
  if ((m_timer1.isEnabled) && ((t_now > (m_timer1.timebase + m_timer1.timeout)) || (t_now < m_timer1.timebase))) {
    // Compute the most recent moment when the counter rolled over (expired)
    m_timer1.timebase = t_now - ((t_now - m_timer1.timebase) % m_timer1.timeout);
    // If the timer is not masked, set the corresponding status bit(s)
    if (!m_timer1.isMasked) m_status |= OPL_STATUS_T1_EXPIRED;
  }

  // Check if Timer 2 has expired
  if ((m_timer2.isEnabled) && ((t_now > (m_timer2.timebase + m_timer2.timeout)) || (t_now < m_timer2.timebase))) {
    // Compute the most recent moment when the counter rolled over (expired)
    m_timer2.timebase = t_now - ((t_now - m_timer2.timebase) % m_timer2.timeout);
    // If the timer is not masked, set the corresponding status bit(s)
    if (!m_timer2.isMasked) m_status |= OPL_STATUS_T2_EXPIRED;
  }

  // If either timer expired while unmasked, set the IRQ bit; clear the IRQ bit otherwise
  if ((m_status & (OPL_STATUS_T1_EXPIRED | OPL_STATUS_T2_EXPIRED)) != 0) {
    m_status |= OPL_STATUS_IRQ_PENDING;
  } else {
    m_status &= ~OPL_STATUS_IRQ_PENDING ;
  }

  return m_status;
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
      m_timer1.timeout = OPL_T1_PERIOD * (256 - (data & 0xff));
      sprintf(buf, "Reprogramming OPL timer 1, counter value = %d.", data & 0xff);
      m_hwemu->logInformation(buf);
      return;

    case 0x03:    // Timer 2 count
      m_timer2.timeout = OPL_T2_PERIOD * (256 - (data & 0xff));
      sprintf(buf, "Reprogramming OPL timer 2, counter value = %d.", data & 0xff);
      m_hwemu->logInformation(buf);
      return;

    case 0x04:    // IRQ-Reset / Mask / Start
      if (data & 0x80) {
        // IRQ-Reset bit. Resets timer and IRQ flags in status register.
        // All other bits are ignored when this bit is set.
        m_status = 0x00;
        m_hwemu->logInformation("Resetting OPL status flags.");
        return;
      } else {
        // Get the current time
        OPLTime_t t_now = m_hwemu->getTimeMicros();

        // Mask/unmask timers
        m_timer1.isMasked = ((data & OPL_CONTROL_T1_MASKED) != 0);
        m_timer2.isMasked = ((data & OPL_CONTROL_T2_MASKED) != 0);

        if (m_timer1.isMasked)
          m_status &= ~OPL_STATUS_T1_EXPIRED;

        if (m_timer2.isMasked)
          m_status &= ~OPL_STATUS_T2_EXPIRED;

        // Enable/disable timers
        bool old_T1_isEnabled = m_timer1.isEnabled;
        bool old_T2_isEnabled = m_timer2.isEnabled;

        m_timer1.isEnabled = ((data & OPL_CONTROL_T1_ENABLED) != 0);
        m_timer2.isEnabled = ((data & OPL_CONTROL_T2_ENABLED) != 0);

        // If previously-disabled timers were just enabled, resynchronize
        if (m_timer1.isEnabled && !old_T1_isEnabled)
          m_timer1.timebase = t_now;

        if (m_timer2.isEnabled && !old_T2_isEnabled)
          m_timer2.timebase = t_now;

        sprintf(buf, "Reprogramming OPL timers (timer 1 is %s and %s, timer 2 is %s and %s).", m_timer1.isEnabled ? "enabled" : "disabled", m_timer1.isMasked ? "masked" : "unmasked", m_timer2.isEnabled ? "enabled" : "disabled", m_timer2.isMasked ? "masked" : "unmasked");
        m_hwemu->logInformation(buf);

        return;
      } 

    default:
      m_hwemu->setOPLReg(m_regIdx, data & 0xff);
      return;
  }
}
