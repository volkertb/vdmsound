// TransferMgr.cpp : Implementation of CTransferMgr
#include "stdafx.h"
#include "DMAController.h"
#include "TransferMgr.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in a .mc file or something */
#define MSG_ERR_INTERFACE _T("The dependency module '%1' does not support the '%2' interface.%0")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_VDMSERVICES   L"VDMSrv"

/////////////////////////////////////////////////////////////////////////////

#define WM_DMA_START      (WM_USER + 0x100)
#define WM_DMA_STOP       (WM_USER + 0x101)

/////////////////////////////////////////////////////////////////////////////

#define DMA8_PAGE_MASK    0x00ff      // supposed to be 0x0f, but some games go beyond that
#define DMA16_PAGE_MASK   0x00ff      // 0x7f on pre-386 machines?

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

  // Initialize configuration and VDM services
  try {
    // Obtain the Query objects (for intialization purposes)
    Depends = configuration;    // Dependency query object

    // Obtain VDM Services instance (if available)
    if ((m_DMASrv = Depends->Get(INI_STR_VDMSERVICES)) == NULL) // DMA services
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_INTERFACE, /*false, NULL, 0, */false, (LPCTSTR)CString(INI_STR_VDMSERVICES), _T("IVDMDMAServices")), __uuidof(IVDMBasicModule), E_NOINTERFACE);
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();          // Propagate the error
  }

  // Create the DMA thread (manages DMA state, performs transfers, etc. asynchronously)
  DMAThread.Create(this, true);
  DMAThread.SetPriority(THREAD_PRIORITY_TIME_CRITICAL);  /* TODO: make configurable in VDMS.ini file */
  DMAThread.Resume();

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("TransferMgr initialized")));

  return S_OK;
}

STDMETHODIMP CTransferMgr::Destroy() {
  // Signal the DMA thread to quit
  if (DMAThread.GetThreadHandle() != NULL)
    DMAThread.Cancel();

  // Release all handlers
  for (int i = 0; i < NUM_DMA_CHANNELS; i++)
    channels[i].handler = NULL;

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

  if (channels[channel].handler != NULL)
		return E_INVALIDARG;

  /* TODO: enforce mutual exclusion when modifying anything in channels[] */

  channels[channel].handler = handler;
  channels[channel].isActive = false;     // no transfer request for this channel yet
  channels[channel].inProgress = false;   // no transfer actually initiated on this channel yet
  channels[channel].addr = 0x0000;        // address is not known
  channels[channel].count = 0xffff;       // count is not known

  return S_OK;
}

STDMETHODIMP CTransferMgr::RemoveDMAHandler(BYTE channel, IDMAHandler * handler) {
	if (handler == NULL)
		return E_POINTER;

  if (channel >= NUM_DMA_CHANNELS)
		return E_INVALIDARG;

  if (channels[channel].handler == NULL)
		return E_INVALIDARG;

  channels[channel].handler = NULL;

  return S_OK;
}

/* TODO: add "synchronous" option, in which initialize/abort functions only
   return after the first/last transfer & notification functions were called
   (pass a flag in the message's lParam, and wait on an event; the DMA thread
   will signal that event if told to do so in the lParam, at which point the
   initialize/abort function can return.  Suggested behaviour: for abort,
   receive message, DEASSERT DREQ, then signal the event; for init, receive,
   the message, ASSERT DREQ, then signal the event.
   Synchronous abort could be needed in a reset function, when no transfer/
   notification function calls are desired after the reset unless the
   transfer is explictly started (again).
   Synchronous init can be needed if DREQ must be asserted by the time the
   init function returns. */

STDMETHODIMP CTransferMgr::InitiateTransfer(BYTE channel, LONG synchronous) {
  if (channel >= NUM_DMA_CHANNELS)
		return E_INVALIDARG;

  if (channels[channel].handler == NULL)
		return E_INVALIDARG;

  DMAThread.PostMessage(WM_DMA_START, channel, NULL);
  
  return S_OK;
}

STDMETHODIMP CTransferMgr::AbortTransfer(BYTE channel, LONG synchronous) {
  if (channel >= NUM_DMA_CHANNELS)
		return E_INVALIDARG;

  if (channels[channel].handler == NULL)
		return E_INVALIDARG;

  DMAThread.PostMessage(WM_DMA_STOP, channel, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IRunnable
/////////////////////////////////////////////////////////////////////////////

unsigned int CTransferMgr::Run(CThread& thread) {
  MSG message;

  static IDMAHANDLERSLib::TTYPE_T types[4] = { TRT_VERIFY, TRT_WRITE, TRT_READ, TRT_INVALID };
  static IDMAHANDLERSLib::TMODE_T modes[4] = { MOD_DEMAND, MOD_SINGLE, MOD_BLOCK, MOD_CASCADE };

  _ASSERTE(thread.GetThreadID() == DMAThread.GetThreadID());

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Transfer Manager thread created (handle = 0x%08x, ID = %d)"), (int)thread.GetThreadHandle(), (int)thread.GetThreadID()));

  do {
    bool isIdle = true;     // indicates whether any channel needs servicing

    // For every channel see if there is any registered handler,
    //  and if so then attempt to service it
    for (int DMAChannel = 0; DMAChannel < NUM_DMA_CHANNELS; DMAChannel++) {
      try {
        if (channels[DMAChannel].handler == NULL)
          continue;         // no one is registered with this channel, therefore don't touch it

        // Obtain the DMA information for the channel
        IVDMSERVICESLib::DMA_INFO_T DMAInfo;
        m_DMASrv->GetDMAState(DMAChannel, &DMAInfo);

        // Perpare some masks (used often)
        int DMAChMask = 0x01 << (DMAChannel & 0x03);
        int DREQMask  = 0x10 << (DMAChannel & 0x03);

        // Attempt to service channel
        if (channels[DMAChannel].isActive) {        // is this channel awaiting servicing?
          isIdle = false;   // at least one channel (this one) needs servicing

          DMAInfo.status |= DREQMask;               // set DREQ

          if ((DMAInfo.mask & DMAChMask) == 0) {    // is channel enabled?
            if (!channels[DMAChannel].inProgress) {
              channels[DMAChannel].addr = DMAInfo.addr;
              channels[DMAChannel].count = DMAInfo.count;
              channels[DMAChannel].inProgress = true;
            }

            bool isAutoInit = (DMAInfo.mode & 0x10) != 0;
            bool isDescending = ((DMAInfo.mode >> 5) & 0x01) != 0;
            ULONG physicalAddr = (DMAChannel < 4) ? (((DMAInfo.page & DMA8_PAGE_MASK) << 16) | (DMAInfo.addr & 0xffff)) : (((DMAInfo.page & DMA16_PAGE_MASK) << 17) | ((DMAInfo.addr & 0xffff) << 1));
            ULONG maxData = DMAInfo.count + 1;

            ULONG numData = channels[DMAChannel].handler->HandleTransfer(DMAChannel, types[DMAInfo.mode & 0x03], modes[(DMAInfo.mode >> 6) & 0x03], isAutoInit, physicalAddr, maxData, isDescending);

            _ASSERTE(numData <= DMAInfo.count + 1);

            if (numData > 0) {  // any change?
              if (numData > DMAInfo.count) {
                // Terminal count reached
                DMAInfo.status |= DMAChMask;  // set TC

                if (isAutoInit) {
                  // Reinitialize
                  DMAInfo.addr = channels[DMAChannel].addr;
                  DMAInfo.count = channels[DMAChannel].count;
                } else {
                  DMAInfo.addr = isDescending ? (DMAInfo.addr - (WORD)numData) : (DMAInfo.addr + (WORD)numData);
                  DMAInfo.count = 0xffff;

                  _ASSERTE(DMAInfo.addr == (WORD)(isDescending ? (channels[DMAChannel].addr - channels[DMAChannel].count - 1) : (channels[DMAChannel].addr + channels[DMAChannel].count + 1)));

                  channels[DMAChannel].isActive = false; // transfer done, disable (mask) channel
                }

                m_DMASrv->SetDMAState(DMAChannel, IVDMSERVICESLib::UPDATE_ALL, &DMAInfo);

                channels[DMAChannel].handler->HandleAfterTransfer(DMAChannel, numData, true);
              } else {
                // Terminal count NOT reached
                DMAInfo.addr = isDescending ? (DMAInfo.addr - (WORD)numData) : (DMAInfo.addr + (WORD)numData);
                DMAInfo.count -= (WORD)numData;

                m_DMASrv->SetDMAState(DMAChannel, IVDMSERVICESLib::UPDATE_ALL, &DMAInfo);

                channels[DMAChannel].handler->HandleAfterTransfer(DMAChannel, numData, false);
              }
            }
          } else {
            m_DMASrv->SetDMAState(DMAChannel, IVDMSERVICESLib::UPDATE_STATUS, &DMAInfo);
          }
        } else {
          DMAInfo.status &= (~DREQMask);            // clear DREQ
          m_DMASrv->SetDMAState(DMAChannel, IVDMSERVICESLib::UPDATE_STATUS, &DMAInfo);
        }
      } catch (_com_error& ce) {
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("TransferData: 0x%08x - %s"), ce.Error(), ce.ErrorMessage()));
      }
    }

    /* TODO: optimize use of SetDMAState (do not call it if the values did not change, e.g. must assert DREQ but DREQ is already asserted, etc.) */
    /* TODO: think about it (probably overkill with no advantage): parametrize (in VDMS.ini) amount of sleep between DMA servicing, and calibrate
       the Sleep() below (i.e. use timeGetTime() differences to deduct the time taken by all the other code in this loop) to reflect that value
         OR
       (much better) have feedback mechanism from DMA Handler about transfer (too fast/too slow) and adjust Sleep() accordingly by steering it up or down */

    if (thread.GetMessage(&message, isIdle)) {
      switch (message.message) {
        case WM_QUIT:
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Transfer Manager thread cancelled")));
          return 0;

        case WM_DMA_START:
          channels[message.wParam].isActive = true;
          channels[message.wParam].inProgress = false;
          break;

        case WM_DMA_STOP:
          channels[message.wParam].isActive = false;
          channels[message.wParam].inProgress = false;
          break;

        default:
          break;
      }
    } else {
      Sleep(15);    // whatever amount, but not too low for too high a priority!
    }
  } while (true);
}
