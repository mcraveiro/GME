
#ifndef MGA_MGAMETAREFERENCE_H
#define MGA_MGAMETAREFERENCE_H

#ifndef MGA_MGAMETAFCO_H
#include "MgaMetaFCO.h"
#endif

// --------------------------- CMgaMetaReference

class ATL_NO_VTABLE CMgaMetaReference : 
	public CComCoClass<CMgaMetaReference, &__uuidof(MgaMetaReference)>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IMgaMetaReference, &__uuidof(IMgaMetaReference), &__uuidof(__MGAMetaLib)>,
	public CMgaMetaFCO
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaReference*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETAREFERENCE)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaReference)

BEGIN_COM_MAP(CMgaMetaReference)
	COM_INTERFACE_ENTRY(IMgaMetaReference)
	COM_INTERFACE_ENTRY(IMgaMetaFCO)
	COM_INTERFACE_ENTRY2(IMgaMetaBase, CMgaMetaBase)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaReference)
END_COM_MAP()

DECLARE_MGAMETAFCO()

public:
	STDMETHOD(get_RefSpec)(IMgaMetaPointerSpec **p)
	{ return ::QueryInterface(GetUnknown(), p); }

	STDMETHOD(CheckPath)(BSTR path, VARIANT_BOOL *p);

// ------- Traverse

public:
	static void Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me);

};

#endif//MGA_MGAMETAREFERENCE_H
