#ifndef __VDMUTIL_H_
#define __VDMUTIL_H_

#pragma warning ( disable : 4192 )
#import <IVDMQuery.tlb>

void AFXAPI RTE_Set(IVDMQUERYLib::IVDMRTEnvironmentPtr& environment, IUnknown * configuration);
void AFXAPI RTE_RecordLogEntry(IVDMQUERYLib::IVDMRTEnvironmentPtr& environment, IVDMQUERYLib::LOGENTRY_T type, LPCTSTR message);

IUnknown* AFXAPI DEP_Get(IVDMQUERYLib::IVDMQueryDependenciesPtr& dependencies, _bstr_t name, IUnknown* defaultValue, bool isSilent = false);

int AFXAPI CFG_Get(IVDMQUERYLib::IVDMQueryConfigurationPtr& configuration, _bstr_t name, int defaultValue, int base = 10, bool isSilent = false);
_bstr_t AFXAPI CFG_Get(IVDMQUERYLib::IVDMQueryConfigurationPtr& configuration, _bstr_t name, _bstr_t defaultValue, bool isSilent = false);

#endif //__VDMUTIL_H_
