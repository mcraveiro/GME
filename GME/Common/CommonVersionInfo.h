#ifndef COMMONVERSIONINFO_H
#define COMMONVERSIONINFO_H

#include "CommonError.h"

class ATL_NO_VTABLE IGMEVersionInfoImpl : public IGMEVersionInfo {
public:
	    BEGIN_INTERFACE
		STDMETHOD(get_version(GMEInterfaceVersion *pVal)) {
			//AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
			if(!pVal) return E_POINTER;
			*pVal = GMEInterfaceVersion_Current;
			return S_OK;
		}
};


#endif // COMMONVERSIONINFO_H
