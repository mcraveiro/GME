
#ifndef MGA_MGAMETAENUMITEM_H
#define MGA_MGAMETAENUMITEM_H

// --------------------------- CMgaMetaEnumItem

class ATL_NO_VTABLE CMgaMetaEnumItem : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaMetaEnumItem, &__uuidof(MgaMetaEnumItem)>,
	public IDispatchImpl<IMgaMetaEnumItem, &__uuidof(IMgaMetaEnumItem), &__uuidof(__MGAMetaLib)>
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaEnumItem*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETAENUMITEM)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaEnumItem)

BEGIN_COM_MAP(CMgaMetaEnumItem)
	COM_INTERFACE_ENTRY(IMgaMetaEnumItem)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaEnumItem)
END_COM_MAP()

public:
	STDMETHOD(get_Parent)(IMgaMetaAttribute **p)
	{ return ComGetPointerValue(GetUnknown(), ATTRID_ENUMITEMS_COLL, p); }

	STDMETHOD(get_DisplayedName)(BSTR *p)
	{ return ComGetAttrValue(GetUnknown(), ATTRID_NAME, p); }

	STDMETHOD(get_Value)(BSTR *p)
	{ return ComGetAttrValue(GetUnknown(), ATTRID_VALUE, p); }

// ------- Edit

public:
	STDMETHOD(put_DisplayedName)(BSTR p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_NAME, p); }

	STDMETHOD(put_Value)(BSTR p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_VALUE, p); }
};

#endif//MGA_MGAMETAENUMITEM_H
