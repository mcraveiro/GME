
#ifndef MGA_MGAMETAASPECT_H
#define MGA_MGAMETAASPECT_H

#ifndef MGA_MGAMETABASE_H
#include "MgaMetaBase.h"
#endif

// --------------------------- CMgaMetaAspect

class ATL_NO_VTABLE CMgaMetaAspect : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaMetaAspect, &__uuidof(MgaMetaAspect)>,
	public IDispatchImpl<IMgaMetaAspect, &__uuidof(IMgaMetaAspect), &__uuidof(__MGAMetaLib)>,
	public CMgaMetaBase
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaAspect*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETAASPECT)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaAspect)

BEGIN_COM_MAP(CMgaMetaAspect)
	COM_INTERFACE_ENTRY(IMgaMetaAspect)
	COM_INTERFACE_ENTRY2(IMgaMetaBase, CMgaMetaBase)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaAspect)
END_COM_MAP()

DECLARE_MGAMETABASE()

public:
	STDMETHOD(get_ParentModel)(IMgaMetaModel **p)
	{ return ComGetPointerValue(GetUnknown(), ATTRID_ASPECTS_COLL, p); }

	STDMETHOD(get_Parts)(IMgaMetaParts **p)
	{ return ComGetCollectionValue<IMgaMetaPart>(GetUnknown(), ATTRID_PARTASPECT_PTR, p); }

	STDMETHOD(get_Attributes)(IMgaMetaAttributes **p)
	{ return ComGetSortedLinkCollValue<IMgaMetaAttribute>(GetUnknown(),
		ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p); }

// ------- Edit

public:
	STDMETHOD(CreatePart)(IMgaMetaRole *role, IMgaMetaPart **p);

	STDMETHOD(AddAttribute)(IMgaMetaAttribute *p)
	{ return ComAddLink(this, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR,
		ATTRID_ATTRLINK_ATTR_PTR, p); }

	STDMETHOD(RemoveAttribute)(IMgaMetaAttribute *p)
	{ return ComRemoveLink(this, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR,
		ATTRID_ATTRLINK_ATTR_PTR, p); }

// ------- Traverse

public:
	static void Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me);

};

#endif//MGA_MGAMETAASPECT_H
