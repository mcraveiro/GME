
#ifndef MGA_MGAMETAATTRIBUTE_H
#define MGA_MGAMETAATTRIBUTE_H

#ifndef MGA_MGAMETABASE_H
#include "MgaMetaBase.h"
#endif

// --------------------------- CMgaMetaAttribute

class ATL_NO_VTABLE CMgaMetaAttribute : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaMetaAttribute, &__uuidof(MgaMetaAttribute)>,
	public IDispatchImpl<IMgaMetaAttribute, &__uuidof(IMgaMetaAttribute), &__uuidof(__MGAMetaLib)>,
	public CMgaMetaBase
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaAttribute*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETAATTRIBUTE)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaAttribute)

BEGIN_COM_MAP(CMgaMetaAttribute)
	COM_INTERFACE_ENTRY(IMgaMetaAttribute)
	COM_INTERFACE_ENTRY2(IMgaMetaBase, CMgaMetaBase)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaAttribute)
END_COM_MAP()

DECLARE_MGAMETABASE();

public:
	STDMETHOD(get_DefinedIn)(IMgaMetaBase **p)
	{ return ComGetPointerValue(GetUnknown(), ATTRID_DEFATTR_PTR, p); }

	STDMETHOD(get_UsedIn)(IMgaMetaFCOs **p);

	STDMETHOD(get_ValueType)(attval_enum *p);
	STDMETHOD(get_DefaultValue)(VARIANT *p);
	STDMETHOD(get_Viewable)(VARIANT_BOOL *p);

	STDMETHOD(get_EnumItems)(IMgaMetaEnumItems **p)
	{ return ComGetSortedCollValue<IMgaMetaEnumItem>(GetUnknown(), ATTRID_ENUMITEMS_COLL, p); }

// ------- Edit

public:
	STDMETHOD(put_ValueType)(attval_enum p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_ATTVALTYPE, static_cast<long>(p)); }

	STDMETHOD(put_DefaultValue)(BSTR p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_VALUE, p); }

	STDMETHOD(put_Viewable)(VARIANT_BOOL p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_VIEWABLE, (p == VARIANT_FALSE)?0L:1L); }

	STDMETHOD(CreateEnumItem)(IMgaMetaEnumItem **p)
	{ return ComCreateMetaObj(GetUnknown(), METAID_METAENUMITEM, ATTRID_ENUMITEMS_COLL, p); }

// ------- Traverse

public:
	static void Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me);

};

#endif//MGA_MGAMETAATTRIBUTE_H
