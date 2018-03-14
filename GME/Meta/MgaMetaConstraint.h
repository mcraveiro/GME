
#ifndef MGA_MGAMETACONSTRAINT_H
#define MGA_MGAMETACONSTRAINT_H

// --------------------------- CMgaMetaConstraint

class ATL_NO_VTABLE CMgaMetaConstraint : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaMetaConstraint, &__uuidof(MgaMetaConstraint)>,
	public IDispatchImpl<IMgaMetaConstraint, &__uuidof(IMgaConstraint), &__uuidof(__MGAMetaLib)>
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaConstraint*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETACONSTRAINT)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaConstraint)

BEGIN_COM_MAP(CMgaMetaConstraint)
	COM_INTERFACE_ENTRY(IMgaMetaConstraint)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaConstraint)
END_COM_MAP()

public:
	STDMETHOD(get_Name)(BSTR *p)
	{ return ComGetAttrValue(GetUnknown(), ATTRID_NAME, p); }

	STDMETHOD(put_Name)(BSTR p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_NAME, p); }

	STDMETHOD(get_DisplayedName)(BSTR *p)
	{ return ComGetDisplayedName(GetUnknown(), ATTRID_DISPNAME, ATTRID_NAME, p); }

	STDMETHOD(put_DisplayedName)(BSTR p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_DISPNAME, p); }

	STDMETHOD(get_Expression)(BSTR *p)
	{ return ComGetAttrValue(GetUnknown(), ATTRID_CONSEXPR, p); }

	STDMETHOD(put_Expression)(BSTR p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_CONSEXPR, p); }

	STDMETHOD(get_EventMask)(unsigned long *p)
	{ return ComGetAttrValue(GetUnknown(), ATTRID_CONSMASK, (long *)p); }

	STDMETHOD(put_EventMask)(unsigned long p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_CONSMASK, (long)p); }

	STDMETHOD(get_Depth)(constraint_depth_enum *p);
	STDMETHOD(put_Depth)(constraint_depth_enum p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_CONSDEPTH, static_cast<long>(p)); }

	STDMETHOD(get_Priority)(long *p)
	{ return ComGetAttrValue(GetUnknown(), ATTRID_CONSPRIORITY, p); }

	STDMETHOD(put_Priority)(long p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_CONSPRIORITY, p); }

	STDMETHOD(get_Type)(constraint_type_enum *p);
	STDMETHOD(put_Type)(constraint_type_enum p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_CONSTYPE, static_cast<long>(p)); }

	STDMETHOD(Remove)()
	{ return ComDeleteObject(GetUnknown()); }

	STDMETHOD(SetDefinedForNamespace)( BSTR pStr);
	STDMETHOD(GetDefinedForNamespace)( BSTR *pStr);
};

#endif//MGA_MGAMETACONSTRAINT_H
