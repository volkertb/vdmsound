#ifndef __IOPORTMGR_H_
#define __IOPORTMGR_H_

/////////////////////////////////////////////////////////////////////////////

#define PORTMAP_SIZE 65536

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMHandlers.tlb>

/////////////////////////////////////////////////////////////////////////////

class CIOPortMgr {
  protected:
    struct IOPortInfo {
      enum opTypes {
        OP_INVALID = -1,
        OP_xxB     = 0,
        OP_xxW     = 1,
        OP_xxSB    = 2,
        OP_xxSW    = 3
      };

      int handlerIdx;
      char inMap[4];
      char outMap[4];
    };

    struct IOHandlerInfo {

      IOHandlerInfo(void)
        { };
      IOHandlerInfo(IIOHandler* _handler)
        : handler(_handler)
        { };
      IOHandlerInfo(const IOHandlerInfo& src)
        : handler(src.handler)
        { };
      IOHandlerInfo& operator=(const IOHandlerInfo& src) {
        handler = src.handler;
        return *this;
      }

      IVDMHANDLERSLib::IIOHandlerPtr handler;
    };

  public:
    CIOPortMgr(void);
    virtual ~CIOPortMgr(void);

  public:
    bool addHandler(WORD basePort, WORD portRange, OPERATIONS_T inOps, OPERATIONS_T outOps, IIOHandler * handler);
    bool removeHandler(WORD basePort, WORD portRange, IIOHandler * handler);
    void removeAllHandlers(void);

  public:
    void getPortRanges(CArray<VDD_IO_PORTRANGE,VDD_IO_PORTRANGE&>& ranges);

  public:
    void PortINB(WORD iPort, BYTE * data);
    void PortINW(WORD iPort, WORD * data);
    void PortINSB(WORD iPort, BYTE * data, WORD count);
    void PortINSW(WORD iPort, WORD * data, WORD count);

    void PortOUTB(WORD oPort, BYTE data);
    void PortOUTW(WORD oPort, WORD data);
    void PortOUTSB(WORD oPort, BYTE * data, WORD count);
    void PortOUTSW(WORD oPort, WORD * data, WORD count);

  public:
    const int numPorts;

  protected:
    void addHandler(int portIdx, int handlerIdx, OPERATIONS_T inOps, OPERATIONS_T outOps);
    void removeHandler(int portIdx);

  protected:
    IOPortInfo portMap[PORTMAP_SIZE];
    CArray<IOHandlerInfo,IOHandlerInfo&> handlers;
};

#endif //__IOPORTMGR_H_
