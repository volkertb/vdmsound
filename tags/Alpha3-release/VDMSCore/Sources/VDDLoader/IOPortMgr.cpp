#include "stdafx.h"

/* TODO: figure a nicer way of including "IOPortMgr.h" without blowing everything up @ compilation */
#  include "VDDLoader.h"
#  include "VDMServices.h"

/////////////////////////////////////////////////////////////////////////////
//
//  CIOPortMgr
//
/////////////////////////////////////////////////////////////////////////////

CIOPortMgr::CIOPortMgr(void)
  : numPorts(PORTMAP_SIZE)
{
  removeAllHandlers();
}

CIOPortMgr::~CIOPortMgr(void) {
}



/////////////////////////////////////////////////////////////////////////////
// Handler management functions
/////////////////////////////////////////////////////////////////////////////

//
//
//
bool CIOPortMgr::addHandler(
    WORD basePort,
    WORD portRange,
    OPERATIONS_T inOps,
    OPERATIONS_T outOps,
    IIOHandler * handler)
{
  int handlerIdx, portIdx;
  bool handlerExists = false;

  // Scan through handlers list to see if handler is already known
  for (handlerIdx = 0; handlerIdx < handlers.GetSize(); handlerIdx++) {
    if (handlers.ElementAt(handlerIdx).handler == handler) {
      handlerExists = true;
      break;
    }
  }

  // Add handler to list if necessary
  if (!handlerExists) {
    handlers.Add(IOHandlerInfo(handler));
    _ASSERTE(handlers.ElementAt(handlerIdx).handler == handler);
  }

  // Verify that there is no conflict with one or more other handlers
  for (portIdx = basePort; portIdx < basePort + portRange; portIdx++) {
    if ((portMap[portIdx].handlerIdx >= 0) && (portMap[portIdx].handlerIdx != handlerIdx)) {
      return false;
    }
  }

  // Assign handler's ID (index) to relevant port(s)
  for (portIdx = basePort; portIdx < basePort + portRange; portIdx++) {
    addHandler(portIdx, handlerIdx, inOps, outOps);
  }

  return true;
}

//
//
//
bool CIOPortMgr::removeHandler(
    WORD basePort,
    WORD portRange,
    IIOHandler * handler)
{
  int handlerIdx, portIdx;
  bool handlerExists = false;

  // Scan through handlers list to see if handler is known
  for (handlerIdx = 0; handlerIdx < handlers.GetSize(); handlerIdx++) {
    if (handlers.ElementAt(handlerIdx).handler == handler) {
      handlerExists = true;
      break;
    }
  }

  if (!handlerExists)
    return false;     // handler was never registered with us!

  // Verify that there is no conflict with one or more other handlers
  for (portIdx = basePort; portIdx < basePort + portRange; portIdx++) {
    if ((portMap[portIdx].handlerIdx >= 0) && (portMap[portIdx].handlerIdx != handlerIdx)) {
      return false;
    }
  }

  // Remove handler's ID (index) from relevant port(s)
  for (portIdx = basePort; portIdx < basePort + portRange; portIdx++) {
    removeHandler(portIdx);
  }

  return true;
}

//
//
//
void CIOPortMgr::removeAllHandlers(void) {
  for (int portIdx = 0; portIdx < (sizeof(portMap) / sizeof(portMap[0])); portIdx++)
    removeHandler(portIdx);

  handlers.RemoveAll();
}



/////////////////////////////////////////////////////////////////////////////
// VDD-specific functions
/////////////////////////////////////////////////////////////////////////////

//
//
//
void CIOPortMgr::getPortRanges(
    CArray<VDD_IO_PORTRANGE,VDD_IO_PORTRANGE&>& ranges)
{
  VDD_IO_PORTRANGE range;
  int lastHandlerIdx = -1;

  for (int portIdx = 0; portIdx < PORTMAP_SIZE; lastHandlerIdx = portMap[portIdx++].handlerIdx) {
    // entering a hooked range
    if ((lastHandlerIdx < 0) && (portMap[portIdx].handlerIdx >= 0)) {
      range.First = portIdx;
      continue;
    }
    // leaving a hooked range
    if ((lastHandlerIdx >= 0) && (portMap[portIdx].handlerIdx < 0)) {
      range.Last = portIdx - 1;
      ranges.Add(range);
      continue;
    }
  }
}



/////////////////////////////////////////////////////////////////////////////
// Handler dispatch functions
/////////////////////////////////////////////////////////////////////////////

void CIOPortMgr::PortINB(WORD iPort, BYTE * data) {
  // Map the trapped instruction into the closest available handler routine
  switch (portMap[iPort].inMap[0]) {  // SINGLE BYTE (inMap[0]) instruction
    case IOPortInfo::OP_xxB:
      ASSERT(portMap[iPort].handlerIdx >= 0);
      handlers.ElementAt(portMap[iPort].handlerIdx).handler->HandleINB(iPort, data);
      return;

    default:
      *data = (BYTE)(-1);
      return;
  }
}

void CIOPortMgr::PortINW(WORD iPort, WORD * data) {
  ASSERT(FALSE);
}

void CIOPortMgr::PortINSB(WORD iPort, BYTE * data, WORD count) {
  ASSERT(FALSE);
}

void CIOPortMgr::PortINSW(WORD iPort, WORD * data, WORD count) {
  ASSERT(FALSE);
}


void CIOPortMgr::PortOUTB(WORD oPort, BYTE data) {
  switch (portMap[oPort].outMap[0]) {
    case IOPortInfo::OP_xxB:
      ASSERT(portMap[oPort].handlerIdx >= 0);
      handlers.ElementAt(portMap[oPort].handlerIdx).handler->HandleOUTB(oPort, data);
      return;

    default:
      return;
  }
}

void CIOPortMgr::PortOUTW(WORD oPort, WORD data) {
  ASSERT(FALSE);
}

void CIOPortMgr::PortOUTSB(WORD oPort, BYTE * data, WORD count) {
  ASSERT(FALSE);
}

void CIOPortMgr::PortOUTSW(WORD oPort, WORD * data, WORD count) {
  ASSERT(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////

void CIOPortMgr::addHandler(
    int portIdx,
    int handlerIdx,
    OPERATIONS_T inOps,
    OPERATIONS_T outOps)
{
  portMap[portIdx].handlerIdx = handlerIdx;

  /* TODO: now all I/O events map directly to their equivalent
     handler; no translation is made between, for instance, the INSB
     instruction and the INB handler.  Must allow for at least certain
     translations */

  portMap[portIdx].inMap[0] = IOPortInfo::OP_xxB;
  portMap[portIdx].inMap[1] = IOPortInfo::OP_xxW;
  portMap[portIdx].inMap[2] = IOPortInfo::OP_xxSB;
  portMap[portIdx].inMap[3] = IOPortInfo::OP_xxSW;

  portMap[portIdx].outMap[0] = IOPortInfo::OP_xxB;
  portMap[portIdx].outMap[1] = IOPortInfo::OP_xxW;
  portMap[portIdx].outMap[2] = IOPortInfo::OP_xxSB;
  portMap[portIdx].outMap[3] = IOPortInfo::OP_xxSW;
}

void CIOPortMgr::removeHandler(
    int portIdx)
{
  portMap[portIdx].handlerIdx = -1;
  memset(portMap[portIdx].inMap,  -1, sizeof(portMap[portIdx].inMap));
  memset(portMap[portIdx].outMap, -1, sizeof(portMap[portIdx].outMap));
}
