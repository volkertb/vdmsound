// TransferMgr.cpp : Implementation of CTransferMgr
#include "stdafx.h"
#include "DMAController.h"
#include "TransferMgr.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in a .mc file or something */
#define MSG_ERR_INTERFACE _T("The dependency module '%1' does not support the '%2' interface.%0")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_VDMSERVICES   L"VDMSrv"
#define INI_STR_MINPERIOD     L"minDMAPeriod"
#define INI_STR_MAXPERIOD     L"maxDMAPeriod"

/////////////////////////////////////////////////////////////////////////////

#define UM_DMA_START      (WM_USER + 0x100)
#define UM_DMA_STOP       (WM_USER + 0x101)

/////////////////////////////////////////////////////////////////////////////

#define DMA8_PAGE_MASK    0x00ff      // supposed to be 0x0f, but some games go beyond that
#define DMA16_PAGE_MASK   0x00ff      // 0x7f on pre-386 machines?

#define DMA_BOOST         1.30        // how much to boost DMA performance when requested to
#define RECOVERY_TIME     3000        // how many milliseconds should elapse before the DMA activity period can recuperate the increase obtained in a boost

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

#include <mmsystem.h>
#pragma comment ( lib , "winmm.lib" )

/////////////////////////////////////////////////////////////////////////////
// CTransferMgr

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CTransferMgr::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
    &IID_IVDMBasicModule,
    &IID_IDMAController
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// IVDMBasicModule
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CTransferMgr::Init(IUnknown * configuration) {
  if (configuration == NULL)
    return E_POINTER;

  // Grab a copy of the runtime environment (useful for logging, etc.)
  RTE_Set(m_env, configuration);

  // Obtain the Query objects (for intialization purposes)
  IVDMQUERYLib::IVDMQueryDependenciesPtr Depends;   // Dependency query object
  IVDMQUERYLib::IVDMQueryConfigurationPtr Config;   // Configuration query object

  // Initialize configuration and VDM services
  try {
    // Obtain the Query objects (for intialization purposes)
    Depends = configuration;    // Dependency query object
    Config  = configuration;    // Configuration query object

    // Obtain VDM Services instance (if available)
    if ((m_DMASrv = Depends->Get(INI_STR_VDMSERVICES)) == NULL) // DMA services
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_INTERFACE, /*false, NULL, 0, */false, (LPCTSTR)CString(INI_STR_VDMSERVICES), _T("IVDMDMAServices")), __uuidof(IVDMBasicModule), E_NOINTERFACE);

    // Try to obtain the DMA engine settings, use defaults if none specified
    m_minPeriod = CFG_Get(Config, INI_STR_MINPERIOD,  5, 10, false);
    m_maxPeriod = CFG_Get(Config, INI_STR_MAXPERIOD, 20, 10, false);
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();          // Propagate the error
  }

  // Create the DMA thread (manages DMA state, performs transfers, etc. asynchronously)
  m_DMAThread.Create(this, true);
  m_DMAThread.SetPriority(THREAD_PRIORITY_TIME_CRITICAL);  /* TODO: make configurable in VDMS.ini file ? */
  m_DMAThread.Resume();

  // Set the DMA activity period
  m_period = m_maxPeriod;
  m_recoveryRate = 1.00;

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("TransferMgr initialized (minPeriod = %dms, maxPeriod = %dms)"), (int)m_minPeriod, (int)m_maxPeriod));

  return S_OK;
}

STDMETHODIMP CTransferMgr::Destroy() {
  // Signal the DMA thread to quit
  if (m_DMAThread.GetThreadHandle() != NULL)
    m_DMAThread.Cancel();

  // Release all handlers
  for (int i = 0; i < NUM_DMA_CHANNELS; i++)
    m_channels[i].handler = NULL;

  // Release the VDM Services module
  m_DMASrv = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("TransferMgr released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IDMAController
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CTransferMgr::AddDMAHandler(BYTE channel, IDMAHandler * handler) {
	if (handler == NULL)
		return E_POINTER;

  if (channel >= NUM_DMA_CHANNELS)
		return E_INVALIDARG;

  if (m_channels[channel].handler != NULL)
		return E_INVALIDARG;

  m_channels[channel].handler = handler;
  m_channels[channel].isActive = false;     // no transfer request for this channel yet
  m_channels[channel].inProgress = false;   // no transfer actually initiated on this channel yet
  m_channels[channel].addr = 0x0000;        // address is not known
  m_channels[channel].count = 0xffff;       // count is not known

  return S_OK;
}

STDMETHODIMP CTransferMgr::RemoveDMAHandler(BYTE channel, IDMAHandler * handler) {
	if (handler == NULL)
		return E_POINTER;

  if (channel >= NUM_DMA_CHANNELS)
		return E_INVALIDARG;

  if (m_channels[channel].handler == NULL)
		return E_INVALIDARG;

  AbortTransfer(channel, true);
  m_channels[channel].handler = NULL;

  return S_OK;
}

STDMETHODIMP CTransferMgr::InitiateTransfer(BYTE channel, LONG synchronous) {
  if (channel >= NUM_DMA_CHANNELS)
		return E_INVALIDARG;

  if (m_channels[channel].handler == NULL)
		return E_INVALIDARG;

  if (m_DMAThread.PostMessage(UM_DMA_START, (WPARAM)channel, (LPARAM)synchronous)) {
    if (synchronous != FALSE)
      m_event.Lock();     // wait for transfer initiation acknowledgement
  } else {
    DWORD lastError = GetLastError();
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("InitiateTransfer: Error encountered while posting message\n0x%08x - %s"), lastError, (LPCTSTR)FormatMessage(lastError)));
  }

  return S_OK;
}

STDMETHODIMP CTransferMgr::AbortTransfer(BYTE channel, LONG synchronous) {
  if (channel >= NUM_DMA_CHANNELS)
		return E_INVALIDARG;

  if (m_channels[channel].handler == NULL)
		return E_INVALIDARG;

  if (m_DMAThread.PostMessage(UM_DMA_STOP, (WPARAM)channel, (LPARAM)synchronous)) {
    if (synchronous != FALSE)
      m_event.Lock();     // wait for transfer abortion acknowledgement
  } else {
    DWORD lastError = GetLastError();
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("AbortTransfer: Error encountered while posting message\n0x%08x - %s"), lastError, (LPCTSTR)FormatMessage(lastError)));
  }

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IRunnable
/////////////////////////////////////////////////////////////////////////////

unsigned int CTransferMgr::Run(CThread& thread) {
  MSG message;

  static IDMAHANDLERSLib::TTYPE_T types[4] = { TRT_VERIFY, TRT_WRITE, TRT_READ, TRT_INVALID };
  static IDMAHANDLERSLib::TMODE_T modes[4] = { MOD_DEMAND, MOD_SINGLE, MOD_BLOCK, MOD_CASCADE };

  _ASSERTE(thread.GetThreadID() == m_DMAThread.GetThreadID());

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Transfer Manager thread created (handle = 0x%08x, ID = %d)"), (int)thread.GetThreadHandle(), (int)thread.GetThreadID()));

  bool notifyLo = false,
       notifyHi = false;  // indicates whether a one-time notification is needed regarding some aspect of DMA transfer management

  bool needsAck = false;  // indicates whether someone initiated/aborted a transaction and is waiting for an acknowledgement

  do {
    bool isIdle = true;   // indicates whether any channel(s) need servicing
    bool isSlow = false;  // indicates that the activity frequency should be increased

    // For every channel see if there is any registered handler,
    //  and if so then attempt to service it
    for (int DMAChannel = 0; DMAChannel < NUM_DMA_CHANNELS; DMAChannel++) {
      try {
        if (m_channels[DMAChannel].handler == NULL)
          continue;       // no one is registered with this channel, therefore don't touch it

        // Obtain the DMA information for the channel
        IVDMSERVICESLib::DMA_INFO_T DMAInfo;
        m_DMASrv->GetDMAState(DMAChannel, &DMAInfo);

        // Perpare some masks (used often)
        int DMAChMask = 0x01 << (DMAChannel & 0x03);
        int DREQMask  = 0x10 << (DMAChannel & 0x03);

        // Attempt to service channel
        if (m_channels[DMAChannel].isActive) {      // is this channel awaiting servicing?
          isIdle = false; // at least one channel (this one) needs servicing

#         if _DEBUG
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Polling (active) DMA channel %d: page/offset = %04x/%04x, count = %04x (%d) ; status = %02x, mode = %02x, mask = %02x (%s)"), DMAChannel, DMAInfo.page & 0xffff, DMAInfo.addr & 0xffff, DMAInfo.count & 0xffff, DMAInfo.count & 0xffff, DMAInfo.status & 0xff, DMAInfo.mode & 0xff, DMAInfo.mask & 0xff, (DMAInfo.mask & DMAChMask) != 0 ? _T("masked") : _T("not masked")));
#         endif

          DMAInfo.status |= DREQMask;               // set DREQ

          if ((DMAInfo.mask & DMAChMask) == 0) {    // is channel enabled?
            if (!m_channels[DMAChannel].inProgress) {
              m_channels[DMAChannel].addr = DMAInfo.addr;
              m_channels[DMAChannel].count = DMAInfo.count;
              m_channels[DMAChannel].inProgress = true;
            }

            bool isAutoInit = (DMAInfo.mode & 0x10) != 0;
            bool isDescending = ((DMAInfo.mode >> 5) & 0x01) != 0;
            ULONG physicalAddr = (DMAChannel < 4) ? (((DMAInfo.page & DMA8_PAGE_MASK) << 16) | (DMAInfo.addr & 0xffff)) : (((DMAInfo.page & DMA16_PAGE_MASK) << 16) | ((DMAInfo.addr & 0x7fff) << 1));
            ULONG numData = 0;
            ULONG maxData = DMAInfo.count + 1ul;

            if (m_channels[DMAChannel].handler->HandleTransfer(DMAChannel, types[DMAInfo.mode & 0x03], modes[(DMAInfo.mode >> 6) & 0x03], isAutoInit, physicalAddr, maxData, isDescending, &numData)) {
              isSlow = true;    // remember to boost the DMA activity frequency, this handler is not too happy with it as it is now
            }

            _ASSERTE(numData <= DMAInfo.count + 1ul);

            if (numData > 0) {  // any change?
              if (numData > DMAInfo.count) {
                // Terminal count reached
                DMAInfo.status |= DMAChMask;        // set TC

                if (isAutoInit) {
                  // Reinitialize
                  DMAInfo.addr = m_channels[DMAChannel].addr;
                  DMAInfo.count = m_channels[DMAChannel].count;
                } else {
                  DMAInfo.status &= (~DREQMask);    // transfer done => clear DREQ
                  DMAInfo.addr = isDescending ? (DMAInfo.addr - (WORD)numData) : (DMAInfo.addr + (WORD)numData);
                  DMAInfo.count = 0xffff;

                  _ASSERTE(DMAInfo.addr == (WORD)(isDescending ? (m_channels[DMAChannel].addr - m_channels[DMAChannel].count - 1) : (m_channels[DMAChannel].addr + m_channels[DMAChannel].count + 1)));

                  m_channels[DMAChannel].isActive = false;  // transfer done, disable (mask) channel
                }

                m_DMASrv->SetDMAState(DMAChannel, IVDMSERVICESLib::UPDATE_ALL, &DMAInfo);

                m_channels[DMAChannel].handler->HandleAfterTransfer(DMAChannel, numData, true);
              } else {
                // Terminal count NOT reached
                DMAInfo.addr = isDescending ? (DMAInfo.addr - (WORD)numData) : (DMAInfo.addr + (WORD)numData);
                DMAInfo.count -= (WORD)numData;

                m_DMASrv->SetDMAState(DMAChannel, IVDMSERVICESLib::UPDATE_ALL, &DMAInfo);

                m_channels[DMAChannel].handler->HandleAfterTransfer(DMAChannel, numData, false);
              }
            } else {
              DMAInfo.status &= (~DREQMask);        // temporarily inactive => clear DREQ
              m_DMASrv->SetDMAState(DMAChannel, IVDMSERVICESLib::UPDATE_STATUS, &DMAInfo);
            }
          } else {
            m_DMASrv->SetDMAState(DMAChannel, IVDMSERVICESLib::UPDATE_STATUS, &DMAInfo);
          }
        } else {
#         if _DEBUG
          if ((DMAInfo.status & DREQMask) != 0) RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Deasserting DREQ on (now inactive) DMA channel %d"), DMAChannel));
#         endif

          DMAInfo.status &= (~DREQMask);            // clear DREQ
          m_DMASrv->SetDMAState(DMAChannel, IVDMSERVICESLib::UPDATE_STATUS, &DMAInfo);
        }
      } catch (_com_error& ce) {
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("TransferData: 0x%08x - %s"), ce.Error(), ce.ErrorMessage()));
      }
    }

    /* TODO: optimize use of SetDMAState (do not call it if the values did not change, e.g. must assert DREQ but DREQ is already asserted, etc.) */

    // If someone is waiting for an acknowledgement following a start
    //   or stop operation (by now DREQ is set properly), do so.
    if (needsAck) {
      needsAck = false;
      m_event.SetEvent();
    }

    // If a handler needs more frequent DMA servicing, boost the DMA activity frequency,
    //   otherwise slowly bring it down to the minimum possible value for minimal overhead
    if (isSlow) {
      double lastPeriod = m_period;
      m_period = max(m_minPeriod, m_period * (1.0 / DMA_BOOST));

      if (((int)m_period == (int)lastPeriod) &&     // did not change, so we probably hit the lower limit
          ((int)m_period == (int)m_minPeriod))
      {
        if (notifyLo) RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Unable to further boost DMA processing rate, lower bound already met (lower bound = %dms, last period = %0.2fms, current period = %0.2fms"), (int)m_minPeriod, lastPeriod, m_period));
        notifyLo = false;   // notified of exceptional condition once, don't do it again if the condition persists
      } else {
        m_recoveryRate = (double)(RECOVERY_TIME - m_period) / (RECOVERY_TIME - DMA_BOOST * m_period);
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Boosting DMA processing rate by %0.1f%% at DMA handler's request (period decreased from %0.2fms to %0.2fms), post-boost recovery rate updated to %0.3f%%"), 100.0 * (lastPeriod/m_period - 1.0), lastPeriod, m_period, (m_recoveryRate - 1.00) * 100.0));
        notifyLo = ((m_period / lastPeriod) < ((1.0 / DMA_BOOST) + 0.05));  // if the system is changing significantly, assume we left the exceptional state, so notify as soon as it arises again (if ever)
      }
    } else {
      double lastPeriod = m_period;
      m_period = min(m_maxPeriod, m_period * m_recoveryRate);

      if (((int)m_period == (int)lastPeriod) &&     // did not change, so we probably hit the upper limit
          ((int)m_period == (int)m_maxPeriod))
      {
        if (notifyHi) RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("DMA processing rate recovered to its normal value (upper bound = %dms, last period = %0.2fms, current period = %0.2fms"), (int)m_maxPeriod, lastPeriod, m_period));
        notifyHi = false;   // notified of this condition once, don't do it again if the condition persists
      } else {
        notifyHi = true;    // left the condition, so notify as soon as it arises again
      }
    }

    // Check if any start/stop requests are pending; if no channels are
    //   active, block until a request is received, otherwise continue
    if (thread.GetMessage(&message, isIdle)) {      // blocks if isIdle==true
      switch (message.message) {
        case WM_QUIT:
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Transfer Manager thread cancelled")));
          return 0;

        case UM_DMA_START:
          _ASSERTE(message.wParam < NUM_DMA_CHANNELS);

#         if _DEBUG
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Received DMA start request (%s) on %s channel %d"), message.lParam != FALSE ? _T("synchronous") : _T("asynchronous"), m_channels[message.wParam].isActive ? _T("active") : _T("inactive"), (int)message.wParam));
#         endif

          needsAck = (message.lParam != FALSE);     // remember to signal back after the DMA is programmed to reflect a STARTED transaction
          m_channels[message.wParam].isActive = true;
          m_channels[message.wParam].inProgress = false;
          break;

        case UM_DMA_STOP:
          _ASSERTE(message.wParam < NUM_DMA_CHANNELS);

#         if _DEBUG
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Received DMA stop request (%s) on %s channel %d"), message.lParam != FALSE ? _T("synchronous") : _T("asynchronous"), m_channels[message.wParam].isActive ? _T("active") : _T("inactive"), (int)message.wParam));
#         endif

          needsAck = (message.lParam != FALSE);     // remember to signal back after the DMA is programmed to reflect a STOPPED transaction
          m_channels[message.wParam].isActive = false;
          m_channels[message.wParam].inProgress = false;
          break;

        default:
          break;
      }
    } else {
      Sleep((DWORD)m_period);  // calibrated based on handler's performance feedback
    }
  } while (true);
}
