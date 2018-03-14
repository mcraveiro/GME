
#ifndef MGA_MGAMETAROLE_H
#define MGA_MGAMETAROLE_H

#ifndef MGA_MGAMETABASE_H
#include "MgaMetaBase.h"
#endif

// --------------------------- CMgaMetaRole

class ATL_NO_VTABLE CMgaMetaRole : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaMetaRole, &__uuidof(MgaMetaRole)>,
	public IDispatchImpl<IMgaMetaRole, &__uuidof(IMgaMetaRole), &__uuidof(__MGAMetaLib)>,
	public CMgaMetaBase,
	public ISupportErrorInfo
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaRole*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETAROLE)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaRole)

BEGIN_COM_MAP(CMgaMetaRole)
	COM_INTERFACE_ENTRY(IMgaMetaRole)
	COM_INTERFACE_ENTRY2(IMgaMetaBase, CMgaMetaBase)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaRole)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

DECLARE_MGAMETABASE()

	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		return (
			IsEqualGUID(riid, __uuidof(IMgaMetaRole))
			|| IsEqualGUID(riid, __uuidof(IMgaMetaBase))
			|| IsEqualGUID(riid, __uuidof(IDispatch))
			) ? S_OK : S_FALSE;
	}

public:
	STDMETHOD(get_ParentModel)(IMgaMetaModel **p)
	{ return ComGetPointerValue(GetUnknown(), ATTRID_ROLES_COLL, p); }

	STDMETHOD(get_Parts)(IMgaMetaParts **p)
	{ return ComGetCollectionValue<IMgaMetaPart>(
		GetUnknown(), ATTRID_PARTROLE_PTR, p); }

	STDMETHOD(get_Kind)(IMgaMetaFCO **p)
	{ return ComGetPointerValue(GetUnknown(), ATTRID_KIND_PTR, p); }

// ------- Edit

public:

};

#endif//MGA_MGAMETAROLE_H
