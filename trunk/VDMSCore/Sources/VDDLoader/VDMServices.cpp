// VDMServices.cpp : Implementation of CVDMServices
#include "stdafx.h"
#include "VDDLoader.h"
#include "VDMServices.h"

#include "Messages.h"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////

IVDMQUERYLib::IVDMRTEnvironmentPtr CVDMServices::m_env = NULL;

long CVDMServices::m_lInstanceCount = 0;
bool CVDMServices::m_isCommitted = false;
CIOPortMgr CVDMServices::m_ports;

VDD_IO_HANDLERS CVDMServices::m_hooks = {
  VDDPortINB,  NULL,NULL,NULL/*VDDPortINW,  VDDPortINSB,  VDDPortINSW*/,
  VDDPortOUTB, NULL,NULL,NULL/*VDDPortOUTW, VDDPortOUTSB, VDDPortOUTSW*/ };
CArray<VDD_IO_PORTRANGE,VDD_IO_PORTRANGE&> CVDMServices::m_ranges;

/////////////////////////////////////////////////////////////////////////////
// CVDMServices

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CVDMServices::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = 
  {
    &IID_IVDMBasicModule,
    &IID_IVDMBaseServices,
    &IID_IVDMIOServices,
    &IID_IVDMDMAServices
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

STDMETHODIMP CVDMServices::Init(IUnknown * configuration) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // Check if first instance
  if ((++m_lInstanceCount) > 1)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_ALREADYINIT, false), __uuidof(IVDMBasicModule), E_UNEXPECTED);

  if (configuration == NULL)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_POINTER, false, NULL, 0, false, _T("Init"), _T("configuration")), __uuidof(IVDMBasicModule), E_POINTER);

  // Will need this instance handle when dealing with the VDM
  m_hInstance = AfxGetInstanceHandle();

  // Grab a copy of the runtime environment (useful for logging, etc.)
  RTE_Set(m_env, configuration);

  // Reset the I/O handlers
  m_ports.removeAllHandlers();

  // Install the VDM process create/terminate/VDM block/resume callback procedures
  if (!VDDInstallUserHook(m_hInstance, VDDUserCreate, VDDUserTerminate, VDDUserBlock, VDDUserResume)) {
    DWORD lastError = GetLastError();
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_APIFAIL, false, NULL, 0, false, _T("Init"), _T("VDDInstallUserHook")), __uuidof(IVDMBasicModule), HRESULT_FROM_WIN32(lastError));
  }

  m_ranges.RemoveAll();       // make sure the array of port ranges to hook is empty

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("VDMServices initialized (hInstance = 0x%08x)"), m_hInstance));

  return S_OK;
}

STDMETHODIMP CVDMServices::Destroy() {
  // Unloading old instance
  m_lInstanceCount--;
  // Check if first instance
  if (m_lInstanceCount > 0)
    return S_OK;              // Don't need to do anything more if we were not the first instance
  if (m_lInstanceCount < 0) {
    m_lInstanceCount = 0;     // Initialization did not occur (1)?
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_NOTINIT, false), __uuidof(IVDMBasicModule), E_UNEXPECTED);
  }

  // Uninstall the I/O hooks (if ever installed)
  if (m_isCommitted) {
    VDDDeInstallIOHook(m_hInstance, m_ranges.GetSize(), m_ranges.GetData());
  }

  // Uninstall the VDM process create/terminate/VDM block/resume callback procedures
  if (!VDDDeInstallUserHook(m_hInstance)) {
    DWORD lastError = GetLastError();
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_APIFAIL, false, NULL, 0, false, _T("Destroy"), _T("VDDDeInstallUserHook")), __uuidof(IVDMBasicModule), HRESULT_FROM_WIN32(lastError));
  }

  // Reset the I/O handlers
  m_ports.removeAllHandlers();

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("VDMServices released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IVDMBaseServices
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CVDMServices::GetRegister(REGISTER_T reg, ULONG * value) {
  if (value == NULL)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_POINTER, false, NULL, 0, false, _T("GetRegister"), _T("value")), __uuidof(IVDMBaseServices), E_POINTER);

  switch (reg) {
    case REG_EAX: *value = getEAX(); return S_OK;
    case REG_AX:  *value = getAX();  return S_OK;
    case REG_AL:  *value = getAL();  return S_OK;
    case REG_AH:  *value = getAH();  return S_OK;

    case REG_EBX: *value = getEBX(); return S_OK;
    case REG_BX:  *value = getBX();  return S_OK;
    case REG_BL:  *value = getBL();  return S_OK;
    case REG_BH:  *value = getBH();  return S_OK;

    case REG_ECX: *value = getECX(); return S_OK;
    case REG_CX:  *value = getCX();  return S_OK;
    case REG_CL:  *value = getCL();  return S_OK;
    case REG_CH:  *value = getCH();  return S_OK;

    case REG_EDX: *value = getEDX(); return S_OK;
    case REG_DX:  *value = getDX();  return S_OK;
    case REG_DL:  *value = getDL();  return S_OK;
    case REG_DH:  *value = getDH();  return S_OK;

    case REG_SS:  *value = getSS();  return S_OK;
    case REG_ESP: *value = getESP(); return S_OK;
    case REG_SP:  *value = getSP();  return S_OK;
    case REG_EBP: *value = getEBP(); return S_OK;
    case REG_BP:  *value = getBP();  return S_OK;

    case REG_DS:  *value = getDS();  return S_OK;
    case REG_ESI: *value = getESI(); return S_OK;
    case REG_SI:  *value = getSI();  return S_OK;

    case REG_ES:  *value = getES();  return S_OK;
    case REG_EDI: *value = getEDI(); return S_OK;
    case REG_DI:  *value = getDI();  return S_OK;

    case REG_CS:  *value = getCS();  return S_OK;
    case REG_EIP: *value = getEIP(); return S_OK;
    case REG_IP:  *value = getIP();  return S_OK;

    default:
      *value = 0;
      return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("GetRegister"), _T("reg"), (int)reg), __uuidof(IVDMBaseServices), E_INVALIDARG);
  }
}

STDMETHODIMP CVDMServices::SetRegister(REGISTER_T reg, ULONG value) {

# pragma warning ( push )
# pragma warning ( disable : 4244 )

  switch (reg) {
    case REG_EAX: setEAX(value); return S_OK;
    case REG_AX:  setAX(value);  return S_OK;
    case REG_AL:  setAL(value);  return S_OK;
    case REG_AH:  setAH(value);  return S_OK;

    case REG_EBX: setEBX(value); return S_OK;
    case REG_BX:  setBX(value);  return S_OK;
    case REG_BL:  setBL(value);  return S_OK;
    case REG_BH:  setBH(value);  return S_OK;

    case REG_ECX: setECX(value); return S_OK;
    case REG_CX:  setCX(value);  return S_OK;
    case REG_CL:  setCL(value);  return S_OK;
    case REG_CH:  setCH(value);  return S_OK;

    case REG_EDX: setEDX(value); return S_OK;
    case REG_DX:  setDX(value);  return S_OK;
    case REG_DL:  setDL(value);  return S_OK;
    case REG_DH:  setDH(value);  return S_OK;

    case REG_SS:  setSS(value);  return S_OK;
    case REG_ESP: setESP(value); return S_OK;
    case REG_SP:  setSP(value);  return S_OK;
    case REG_EBP: setEBP(value); return S_OK;
    case REG_BP:  setBP(value);  return S_OK;

    case REG_DS:  setDS(value);  return S_OK;
    case REG_ESI: setESI(value); return S_OK;
    case REG_SI:  setSI(value);  return S_OK;

    case REG_ES:  setES(value);  return S_OK;
    case REG_EDI: setEDI(value); return S_OK;
    case REG_DI:  setDI(value);  return S_OK;

    case REG_CS:  setCS(value);  return S_OK;
    case REG_IP:  setIP(value);  return S_OK;

    case REG_EIP:
    default:
      return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("SetRegister"), _T("reg"), (int)reg), __uuidof(IVDMBaseServices), E_INVALIDARG);
  }

# pragma warning ( pop )

}

STDMETHODIMP CVDMServices::GetFlag(FLAG_T flag, ULONG * value) {
  if (value == NULL)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_POINTER, false, NULL, 0, false, _T("GetFlag"), _T("value")), __uuidof(IVDMBaseServices), E_POINTER);

  switch (flag) {
    case FLAG_CARRY:  *value = getCF(); return S_OK;
    case FLAG_PARITY: *value = getPF(); return S_OK;
    case FLAG_AUX:    *value = getAF(); return S_OK;
    case FLAG_ZERO:   *value = getZF(); return S_OK;
    case FLAG_SIGN:   *value = getSF(); return S_OK;
    case FLAG_INT:    *value = getIF(); return S_OK;
    case FLAG_DIR:    *value = getDF(); return S_OK;
    case FLAG_OFLOW:  *value = getOF(); return S_OK;

    case FLAG_TRAP:
    default:
      *value = 0;
      return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("GetFlag"), _T("flag"), (int)flag), __uuidof(IVDMBaseServices), E_INVALIDARG);
  }
}

STDMETHODIMP CVDMServices::SetFlag(FLAG_T flag, ULONG value) {
  switch (flag) {
    case FLAG_CARRY:  setCF(value); return S_OK;
    case FLAG_PARITY: setPF(value); return S_OK;
    case FLAG_AUX:    setAF(value); return S_OK;
    case FLAG_ZERO:   setZF(value); return S_OK;
    case FLAG_SIGN:   setSF(value); return S_OK;
    case FLAG_INT:    setIF(value); return S_OK;
    case FLAG_DIR:    setDF(value); return S_OK;
    case FLAG_OFLOW:  setOF(value); return S_OK;

    case FLAG_TRAP:
    default:
      return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("SetFlag"), _T("flag"), (int)flag), __uuidof(IVDMBaseServices), E_INVALIDARG);
  }
}

STDMETHODIMP CVDMServices::GetMemory(WORD segment, ULONG offset, ADDRMODE_T mode, BYTE * buffer, ULONG length) {
  if (buffer == NULL)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_POINTER, false, NULL, 0, false, _T("GetMemory"), _T("buffer")), __uuidof(IVDMBaseServices), E_POINTER);

  if (length < 1)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("GetMemory"), _T("length"), (int)length), __uuidof(IVDMBaseServices), E_INVALIDARG);

  PBYTE pSrc;

  switch (mode) {
    case ADDR_PM:
      memset(buffer, 0, length);
      return E_NOTIMPL;

    case ADDR_V86:
      pSrc = GetVDMPointer(MAKELONG(segment, offset), length, FALSE);
      memcpy(buffer, pSrc, length);
      FreeVDMPointer(MAKELONG(segment, offset), length, pSrc, FALSE);
      return S_OK;

    default:
      memset(buffer, 0, length);
      return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("GetMemory"), _T("mode"), (int)mode), __uuidof(IVDMBaseServices), E_INVALIDARG);
  }
}

STDMETHODIMP CVDMServices::SetMemory(WORD segment, ULONG offset, ADDRMODE_T mode, BYTE * buffer, ULONG length) {
  if (buffer == NULL)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_POINTER, false, NULL, 0, false, _T("SetMemory"), _T("buffer")), __uuidof(IVDMBaseServices), E_POINTER);

  if (length < 1)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("SetMemory"), _T("length"), (int)length), __uuidof(IVDMBaseServices), E_INVALIDARG);

  PBYTE pDest;

  switch (mode) {
    case ADDR_PM:
      return E_NOTIMPL;

    case ADDR_V86:
      pDest = GetVDMPointer(MAKELONG(segment, offset), length, FALSE);
      memcpy(pDest, buffer, length);
      FreeVDMPointer(MAKELONG(segment, offset), length, pDest, FALSE);
      return S_OK;

    default:
      return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("SetMemory"), _T("mode"), (int)mode), __uuidof(IVDMBaseServices), E_INVALIDARG);
  }
}

STDMETHODIMP CVDMServices::SimulateInterrupt(INTERRUPT_T type, BYTE line, USHORT count) {
  switch (type) {
    case INT_MASTER:
      VDDSimulateInterrupt(ICA_MASTER, line, count);
      return S_OK;

    case INT_SLAVE:
      VDDSimulateInterrupt(ICA_SLAVE, line, count);
      return S_OK;

    default:
      return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("SimulateInterrupt"), _T("type"), (int)type), __uuidof(IVDMBaseServices), E_INVALIDARG);
  }
}

STDMETHODIMP CVDMServices::TerminateVDM() {
  VDDTerminateVDM();
  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IVDMIOServices
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CVDMServices::AddIOHook(WORD basePort, WORD portRange, OPERATIONS_T inOps, OPERATIONS_T outOps, IIOHandler * handler) {
  if (handler == NULL)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_POINTER, false, NULL, 0, false, _T("AddIOHook"), _T("handler")), __uuidof(IVDMIOServices), E_POINTER);

  if (portRange < 1)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("AddIOHook"), _T("portRange"), (int)portRange), __uuidof(IVDMIOServices), E_INVALIDARG);

  if (basePort + portRange > m_ports.numPorts)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("AddIOHook"), _T("basePort + portRange"), (int)(basePort + portRange)), __uuidof(IVDMIOServices), E_INVALIDARG);

  if (m_isCommitted)
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("An attempt was made to hook the I/O port range 0x%03x ... 0x%03x *after* all hooks were registered with the NT VDM!"), (int)basePort, (int)(basePort + portRange - 1)));

  if (!m_ports.addHandler(basePort, portRange, inOps, outOps, handler))
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_IOADDCONFLICT, false, NULL, 0, false, (int)basePort, (int)(basePort + portRange - 1)), __uuidof(IVDMIOServices), E_INVALIDARG);

  return S_OK;
}

STDMETHODIMP CVDMServices::RemoveIOHook(WORD basePort, WORD portRange, IIOHandler * handler) {
  if (handler == NULL)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_POINTER, false, NULL, 0, false, _T("RemoveIOHook"), _T("handler")), __uuidof(IVDMIOServices), E_POINTER);

  if (portRange < 1)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("RemoveIOHook"), _T("portRange"), (int)portRange), __uuidof(IVDMIOServices), E_INVALIDARG);

  if (basePort + portRange > m_ports.numPorts)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_INVALIDARG, false, NULL, 0, false, _T("AddIOHook"), _T("basePort + portRange"), (int)(basePort + portRange)), __uuidof(IVDMIOServices), E_INVALIDARG);

  if (!m_ports.removeHandler(basePort, portRange, handler))
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_IODELCONFLICT, false, NULL, 0, false, (int)basePort, (int)(basePort + portRange - 1)), __uuidof(IVDMIOServices), E_INVALIDARG);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IVDMDMAServices
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CVDMServices::GetDMAState(USHORT channel, DMA_INFO_T * DMAInfo) {
  if (DMAInfo == NULL)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_POINTER, false, NULL, 0, false, _T("GetDMAState"), _T("DMAInfo")), __uuidof(IVDMDMAServices), E_POINTER);

  VDD_DMA_INFO info;

  if (!VDDQueryDMA(m_hInstance, channel, &info)) {
    DWORD lastError = GetLastError();
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_APIFAIL, false, NULL, 0, false, _T("GetDMAState"), _T("VDDQueryDMA")), __uuidof(IVDMDMAServices), HRESULT_FROM_WIN32(lastError));
  }

  DMAInfo->page   = info.page;
  DMAInfo->addr   = info.addr;
  DMAInfo->count  = info.count;
  DMAInfo->status = info.status;
  DMAInfo->mode   = info.mode;
  DMAInfo->mask   = info.mask;

  return S_OK;
}

STDMETHODIMP CVDMServices::SetDMAState(USHORT channel, DMA_INFO_SEL_T flags, DMA_INFO_T * DMAInfo) {
  if (DMAInfo == NULL)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_POINTER, false, NULL, 0, false, _T("SetDMAState"), _T("DMAInfo")), __uuidof(IVDMDMAServices), E_POINTER);

  WORD vddFlags = 0;
  VDD_DMA_INFO info;

  if ((flags & UPDATE_PAGE) != 0) {
    vddFlags |= VDD_DMA_PAGE;
    info.page = DMAInfo->page;
  }

  if ((flags & UPDATE_ADDR) != 0) {
    vddFlags |= VDD_DMA_ADDR;
    info.addr = DMAInfo->addr;
  }

  if ((flags & UPDATE_COUNT) != 0) {
    vddFlags |= VDD_DMA_COUNT;
    info.count = DMAInfo->count;
  }

  if ((flags & UPDATE_STATUS) != 0) {
    vddFlags |= VDD_DMA_STATUS;
    info.status = DMAInfo->status;
  }

  if (!VDDSetDMA(m_hInstance, channel, vddFlags, &info)) {
    DWORD lastError = GetLastError();
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_APIFAIL, false, NULL, 0, false, _T("SetDMAState"), _T("VDDSetDMA")), __uuidof(IVDMDMAServices), HRESULT_FROM_WIN32(lastError));
  }

  return S_OK;
}

STDMETHODIMP CVDMServices::PerformDMATransfer(USHORT channel, BYTE * buffer, ULONG length, ULONG * transferred) {
  if (buffer == NULL)
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_E_POINTER, false, NULL, 0, false, _T("PerformDMATransfer"), _T("buffer")), __uuidof(IVDMDMAServices), E_POINTER);

  DWORD numBytes = VDDRequestDMA(m_hInstance, channel, buffer, length);
  DWORD lastError = GetLastError();

  if ((numBytes == 0) && (lastError != ERROR_SUCCESS))
    return AtlReportError(GetObjectCLSID(), FormatMessage(MSG_ERR_APIFAIL, false, NULL, 0, false, _T("PerformDMATransfer"), _T("VDDRequestDMA")), __uuidof(IVDMDMAServices), HRESULT_FROM_WIN32(lastError));

  if (transferred != NULL)
    *transferred = numBytes;

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// ** PRIVATE ** VDD user hook functions
/////////////////////////////////////////////////////////////////////////////

VOID CALLBACK CVDMServices::VDDUserCreate(USHORT DosPDB) {
//MessageBox(FormatMessage(_T("Created DOS process: PDB = 0x%1!08x!"), false, (int)DosPDB),
//           FormatMessage(_T("VDDUserCreate")),
//           MB_OK, MB_ICONINFORMATION);

  if (!m_isCommitted) {
    // Push MFC state (needed by AfxGetInstanceHandle())
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // Install the VDD hooks
    m_ports.getPortRanges(m_ranges);
    if (!VDDInstallIOHook(AfxGetInstanceHandle(), m_ranges.GetSize(), m_ranges.GetData(), &m_hooks)) {
      DWORD lastError = GetLastError();
      CString portsStr = StringFromPortRanges(m_ranges.GetSize(), m_ranges.GetData());
      if (MessageBox(FormatMessage(MSG_ERR_IOHOOKINST, false, AfxGetInstanceHandle(), 0, false, HRESULT_FROM_WIN32(lastError), (LPCTSTR)FormatMessage(lastError), (LPCTSTR)portsStr),
                     LoadString(IDS_MBT_VDDERROR),
                     MB_YESNO|MB_DEFBUTTON2, MB_ICONERROR) == IDNO)
      {
        VDDTerminateVDM();    // terminate the VDM
      }
    } else {
      m_isCommitted = true;
    }
  }
}

VOID CALLBACK CVDMServices::VDDUserTerminate(USHORT DosPDB) {
//MessageBox(FormatMessage(_T("Terminated DOS process: PDB = 0x%1!08x!"), false, (int)DosPDB),
//           FormatMessage(_T("VDDUserTerminate")),
//           MB_OK, MB_ICONINFORMATION);
}

VOID CALLBACK CVDMServices::VDDUserBlock(VOID) {
//MessageBox(FormatMessage(_T("VDM blocked")),
//           FormatMessage(_T("VDDUserBlock")),
//           MB_OK, MB_ICONINFORMATION);
}

VOID CALLBACK CVDMServices::VDDUserResume(VOID) {
//MessageBox(FormatMessage(_T("VDM resumed")),
//           FormatMessage(_T("VDDUserResume")),
//           MB_OK, MB_ICONINFORMATION);
}



/////////////////////////////////////////////////////////////////////////////
//
// ** PURELY LOCAL STUFF **
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// VDD IO hook functions
/////////////////////////////////////////////////////////////////////////////

VOID CALLBACK CVDMServices::VDDPortINB(WORD iPort, BYTE * data) {
  try {
    m_ports.PortINB(iPort, data);
  } catch (_com_error& ce) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: 0x%08x - %s"), _T("VDDPortINB"), ce.Error(), ce.ErrorMessage()));
  } catch (...) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: Unhandled exception"), _T("VDDPortINB")));
  }
}

VOID CALLBACK CVDMServices::VDDPortINW(WORD iPort, WORD * data) {
  try {
    m_ports.PortINW(iPort, data);
  } catch (_com_error& ce) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: 0x%08x - %s"), _T("VDDPortINW"), ce.Error(), ce.ErrorMessage()));
  } catch (...) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: Unhandled exception"), _T("VDDPortINW")));
  }
}

VOID CALLBACK CVDMServices::VDDPortINSB(WORD iPort, BYTE * data, WORD count) {
  try {
    m_ports.PortINSB(iPort, data, count);
  } catch (_com_error& ce) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: 0x%08x - %s"), _T("VDDPortINSB"), ce.Error(), ce.ErrorMessage()));
  } catch (...) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: Unhandled exception"), _T("VDDPortINSB")));
  }
}

VOID CALLBACK CVDMServices::VDDPortINSW(WORD iPort, WORD * data, WORD count) {
  try {
    m_ports.PortINSW(iPort, data, count);
  } catch (_com_error& ce) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: 0x%08x - %s"), _T("VDDPortINSW"), ce.Error(), ce.ErrorMessage()));
  } catch (...) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: Unhandled exception"), _T("VDDPortINSW")));
  }
}



/////////////////////////////////////////////////////////////////////////////



VOID CALLBACK CVDMServices::VDDPortOUTB(WORD oPort, BYTE data) {
  try {
    m_ports.PortOUTB(oPort, data);
  } catch (_com_error& ce) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: 0x%08x - %s"), _T("VDDPortOUTB"), ce.Error(), ce.ErrorMessage()));
  } catch (...) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: Unhandled exception"), _T("VDDPortOUTB")));
  }
}

VOID CALLBACK CVDMServices::VDDPortOUTW(WORD oPort, WORD data) {
  try {
    m_ports.PortOUTW(oPort, data);
  } catch (_com_error& ce) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: 0x%08x - %s"), _T("VDDPortOUTW"), ce.Error(), ce.ErrorMessage()));
  } catch (...) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: Unhandled exception"), _T("VDDPortOUTW")));
  }
}

VOID CALLBACK CVDMServices::VDDPortOUTSB(WORD oPort, BYTE * data, WORD count) {
  try {
    m_ports.PortOUTSB(oPort, data, count);
  } catch (_com_error& ce) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: 0x%08x - %s"), _T("VDDPortOUTSB"), ce.Error(), ce.ErrorMessage()));
  } catch (...) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: Unhandled exception"), _T("VDDPortOUTSB")));
  }
}

VOID CALLBACK CVDMServices::VDDPortOUTSW(WORD oPort, WORD * data, WORD count) {
  try {
    m_ports.PortOUTSW(oPort, data, count);
  } catch (_com_error& ce) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: 0x%08x - %s"), _T("VDDPortOUTSW"), ce.Error(), ce.ErrorMessage()));
  } catch (...) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("%s: Unhandled exception"), _T("VDDPortOUTSW")));
  }
}



/////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////

//
// Pretty-prints a list of port ranges for display and/or logging
//
CString CVDMServices::StringFromPortRanges(
    int numPorts,
    const VDD_IO_PORTRANGE* ports)
{
  CString retVal;

  if (numPorts < 1)
    return CString(_T("( empty list )"));

  for (int i = 0; i < numPorts; i++) {
    CString portStr;

    if (ports[i].First == ports[i].Last) {
      portStr.Format(_T("%03x"), ports[i].First);
    } else {
      portStr.Format(_T("%03x...%03x"), ports[i].First, ports[i].Last);
    }

    if (i < (numPorts - 1)) {
      retVal += portStr + ", ";
    } else {
      retVal += portStr;
    }
  }

  return retVal;
}



/////////////////////////////////////////////////////////////////////////////



/*
STDMETHODIMP CVDMServices::AddMemHook(ULONG baseAddr, ULONG addrRange, IMemHandler * handler) {
  // TODO: implement
  return E_NOTIMPL;
}

STDMETHODIMP CVDMServices::RemoveMemHook(ULONG baseAddr, ULONG addrRange) {
  // TODO: implement
  return E_NOTIMPL;
}

STDMETHODIMP CVDMServices::AllocMem(ULONG address, ULONG size) {
  if (VDDAllocMem(m_hInstance, (LPVOID)address, size)) {
    return S_OK;
  } else {
    return HRESULT_FROM_WIN32(GetLastError());
  }
}

STDMETHODIMP CVDMServices::FreeMem(ULONG address, ULONG size) {
  if (VDDFreeMem(m_hInstance, (LPVOID)address, size)) {
    return S_OK;
  } else {
    return HRESULT_FROM_WIN32(GetLastError());
  }
}
*/
