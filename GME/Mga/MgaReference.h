// MgaReference.h : Declaration of the CMgaReference

#ifndef __MGAREFERENCE_H_
#define __MGAREFERENCE_H_

#include "resource.h"       // main symbols
#include "MgaFCO.h"

/////////////////////////////////////////////////////////////////////////////
// CMgaReference
class ATL_NO_VTABLE CMgaReference : 
	public CComCoClass<CMgaReference, &__uuidof(MgaReference)>,
	public IMgaFCOImpl< 
		CComObjectRootEx<CComSingleThreadModel>,
	    IDispatchImpl<IMgaReference, &__uuidof(IMgaReference), &__uuidof(__MGALib)> >,
	public ISupportErrorInfoImpl<&__uuidof(IMgaReference)>   {
public:
	CMgaReference()
	{
	}


DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaReference)
	COM_INTERFACE_ENTRY(IMgaReference)
	COM_INTERFACE_ENTRY_IID(__uuidof(IMgaObject),IMgaReference)
	COM_INTERFACE_ENTRY_IID(__uuidof(IMgaFCO),IMgaReference)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IID(IID_ISupportErrorInfo, IMyErrorInfoBase)
END_COM_MAP()

// IMgaReference
public:
	STDMETHOD(get_Referred)(IMgaFCO * *pVal)			{ return inFCO->get_Referred(pVal); }
	STDMETHOD(put_Referred)(IMgaFCO * newVal)			{ return inFCO->put_Referred(newVal); }
	STDMETHOD(get_RefAspect)(IMgaMetaAspect **pVal)		{ return inFCO->get_RefAspect(pVal); }
	STDMETHOD(put_RefAspect)(IMgaMetaAspect * newVal)	{ return inFCO->put_RefAspect(newVal); }
	STDMETHOD(get_UsedByConns)(IMgaConnPoints **pVal)	{ return inFCO->get_UsedByConns(pVal); }
	STDMETHOD(CompareToBase)(short *status)				{ return inFCO->RefCompareToBase(status); }
    STDMETHOD(RevertToBase)()							{ return inFCO->RefRevertToBase(); }
	STDMETHOD(ClearRef)()					 		    { return inFCO->put_Referred( 0); }
};

class putreftask : public DeriveTreeTask {
	bool Do(CoreObj self, std::vector<CoreObj> *peers = NULL);
public:
	// isext is true if the relation is sure to be external, false if not known
	putreftask(bool isext)  {
		masterattr = ATTRID_MASTEROBJ;
		if (isext) {
			endreserve = 1;
		}
	};
};

#endif //__MGAREFERENCE_H_
