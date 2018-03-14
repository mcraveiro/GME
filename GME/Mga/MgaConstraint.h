// MgaConstraint.h : Declaration of the CMgaConstraint

#ifndef __MGACONSTRAINT_H_
#define __MGACONSTRAINT_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMgaConstraint
class ATL_NO_VTABLE CMgaConstraint : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaConstraint, &__uuidof(MgaConstraint)>,
	public IDispatchImpl<IMgaConstraint, &__uuidof(IMgaConstraint), &__uuidof(__MGALib)>
{
public:
	CComBSTR name, expression;
	long eventmask;
	CMgaConstraint()
	{
	}
	static void GetConstraint(FCO *f, CoreObj &c, IMgaConstraint **cp);

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaConstraint)
	COM_INTERFACE_ENTRY(IMgaConstraint)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IMgaConstraint
public:
	STDMETHOD(get_Name)(BSTR * )							{ return E_MGA_NOT_IMPLEMENTED; };
//	STDMETHOD(get_Priority)(long *)							{ return E_MGA_NOT_IMPLEMENTED; };
	STDMETHOD(get_EventMask)(long *)						{ return E_MGA_NOT_IMPLEMENTED; };
	STDMETHOD(get_Expression)(BSTR * )						{ return E_MGA_NOT_IMPLEMENTED; };
	STDMETHOD(get_Status) (long *status)					{ return E_MGA_NOT_IMPLEMENTED; };
};

#endif //__MGACONSTRAINT_H_
