
#ifndef MGA_MGAMETAATOM_H
#define MGA_MGAMETAATOM_H

#ifndef MGA_MGAMETAFCO_H
#include "MgaMetaFCO.h"
#endif

// --------------------------- CMgaMetaAtom

class ATL_NO_VTABLE CMgaMetaAtom : 
	public CComCoClass<CMgaMetaAtom, &__uuidof(MgaMetaAtom)>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IMgaMetaAtom, &__uuidof(IMgaMetaAtom), &__uuidof(__MGAMetaLib)>,
	public CMgaMetaFCO
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaAtom*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETAATOM)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaAtom)

BEGIN_COM_MAP(CMgaMetaAtom)
	COM_INTERFACE_ENTRY(IMgaMetaAtom)
	COM_INTERFACE_ENTRY(IMgaMetaFCO)
	COM_INTERFACE_ENTRY2(IMgaMetaBase, CMgaMetaBase)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaAtom)
END_COM_MAP()

DECLARE_MGAMETAFCO()

public:
};

#endif//MGA_MGAMETAATOM_H
