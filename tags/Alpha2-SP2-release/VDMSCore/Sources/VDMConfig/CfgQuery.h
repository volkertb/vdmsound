// CfgQuery.h : Declaration of the CCfgQuery

#ifndef __CFGQUERY_H_
#define __CFGQUERY_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMQuery.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

/////////////////////////////////////////////////////////////////////////////

struct CfgEnvironment;

/////////////////////////////////////////////////////////////////////////////
// CCfgQuery
class ATL_NO_VTABLE CCfgQuery : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CCfgQuery, &CLSID_CfgQuery>,
	public ISupportErrorInfo,
  public IVDMQueryDependencies,
  public IVDMQueryConfiguration,
  public IVDMRTEnvironment
{
public:
	CCfgQuery();
	~CCfgQuery();

DECLARE_REGISTRY_RESOURCEID(IDR_CFGQUERY)
DECLARE_NOT_AGGREGATABLE(CCfgQuery)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCfgQuery)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IVDMQueryDependencies)
  COM_INTERFACE_ENTRY(IVDMQueryConfiguration)
  COM_INTERFACE_ENTRY(IVDMRTEnvironment)
END_COM_MAP()

  void CCfgQuery::Init(const CfgEnvironment& env);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IVDMQueryDependencies
public:
  STDMETHOD(Get)(BSTR name, IUnknown ** dependency);

// IVDMQueryDependencies
public:
  STDMETHOD(Get)(BSTR name, BSTR * value);

// IVDMRTEnvironment
public:
  STDMETHOD(RecordLogEntry)(LOGENTRY_T type, BSTR message);

protected:
  bool getDbgValue(const std::string& key, std::string& value, const std::string& defaultValue);
  CString FormatTime(DWORD millis);

protected:
  CfgEnvironment* m_pEnv;
  std::string m_logFile;
  int m_logLevel;
};

#endif //__CFGQUERY_H_
