
#ifndef MGA_MGAMETAMODEL_H
#define MGA_MGAMETAMODEL_H

#ifndef MGA_MGAMETAFCO_H
#include "MgaMetaFCO.h"
#endif

// --------------------------- CMgaMetaModel

class ATL_NO_VTABLE CMgaMetaModel : 
	public CComCoClass<CMgaMetaModel, &__uuidof(MgaMetaModel)>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IMgaMetaModel, &__uuidof(IMgaMetaModel), &__uuidof(__MGAMetaLib)>,
	public CMgaMetaFCO,
	public ISupportErrorInfo
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaModel*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETAMODEL)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaModel)

BEGIN_COM_MAP(CMgaMetaModel)
	COM_INTERFACE_ENTRY(IMgaMetaModel)
	COM_INTERFACE_ENTRY(IMgaMetaFCO)
	COM_INTERFACE_ENTRY2(IMgaMetaBase, CMgaMetaBase)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaModel)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

DECLARE_MGAMETAFCO()

	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		return (
			IsEqualGUID(riid, __uuidof(IMgaMetaModel))
			|| IsEqualGUID(riid, __uuidof(IMgaMetaFCO))
			|| IsEqualGUID(riid, __uuidof(IMgaMetaBase))
			|| IsEqualGUID(riid, __uuidof(IDispatch))
			) ? S_OK : S_FALSE;
	}

public:
	STDMETHOD(get_DefinedFCOs)(IMgaMetaFCOs **p)
	{   return ComGetSortedCollValue<IMgaMetaFCO>(GetUnknown(), ATTRID_DEFFCO_PTR, p); }

	STDMETHOD(get_DefinedFCOByName)(BSTR name, VARIANT_BOOL inscope, IMgaMetaFCO **p);

	STDMETHOD(GetDefinedFCOByNameDisp)(BSTR name, VARIANT_BOOL inscope, IMgaMetaFCO **p)
	{ return get_DefinedFCOByName( name, inscope, p); }

	STDMETHOD(get_Roles)(IMgaMetaRoles **p)
	{ return ComGetSortedCollValue<IMgaMetaRole>(GetUnknown(), ATTRID_ROLES_COLL, p); }

	STDMETHOD(get_RoleByName)(BSTR name, IMgaMetaRole **p);

	STDMETHOD(GetRoleByNameDisp)(BSTR name, IMgaMetaRole **p)
	{ return get_RoleByName( name, p); }

	STDMETHOD(get_Aspects)(IMgaMetaAspects **p)
	{ return ComGetSortedCollValue<IMgaMetaAspect>(GetUnknown(), ATTRID_ASPECTS_COLL, p); }

	STDMETHOD(get_AspectByName)(BSTR name, IMgaMetaAspect **p);

	STDMETHOD(GetAspectByNameDisp)(BSTR name, IMgaMetaAspect **p)
	{ return get_AspectByName( name, p); }

	STDMETHOD(LegalConnectionRoles)(BSTR paths, IMgaMetaRoles **p);
	STDMETHOD(LegalReferenceRoles)(BSTR path, IMgaMetaRoles **p);
	STDMETHOD(LegalSetRoles)(BSTR path, IMgaMetaRoles **p);
	STDMETHOD(LegalRoles)(IMgaMetaFCO *kind, IMgaMetaRoles **p);

// ------- Edit

public:
	STDMETHOD(DefineModel)(IMgaMetaModel **p)
	{ return ComDefineBase(this, METAID_METAMODEL, ATTRID_DEFFCO_PTR, p); }

	STDMETHOD(DefineAtom)(IMgaMetaAtom **p)
	{ return ComDefineBase(this, METAID_METAATOM, ATTRID_DEFFCO_PTR, p); }

	STDMETHOD(DefineReference)(IMgaMetaReference **p)
	{ return ComDefineBase(this, METAID_METAREFERENCE, ATTRID_DEFFCO_PTR, p); }

	STDMETHOD(DefineSet)(IMgaMetaSet **p)
	{ return ComDefineBase(this, METAID_METASET, ATTRID_DEFFCO_PTR, p); }

	STDMETHOD(DefineConnection)(IMgaMetaConnection **p)
	{ return ComDefineBase(this, METAID_METACONNECTION, ATTRID_DEFFCO_PTR, p); }

	STDMETHOD(CreateRole)(IMgaMetaFCO *kind, IMgaMetaRole **p);

	STDMETHOD(CreateAspect)(IMgaMetaAspect **p)
	{ return ComDefineBase(this, METAID_METAASPECT, ATTRID_ASPECTS_COLL, p); }

// ------- Traverse

public:
	static void Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me);

};

#endif//MGA_MGAMETAMODEL_H
