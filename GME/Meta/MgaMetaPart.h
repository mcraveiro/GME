
#ifndef MGA_MGAMETAPART_H
#define MGA_MGAMETAPART_H

#ifndef MGA_MGAMETABASE_H
#include "MgaMetaBase.h"
#endif

// --------------------------- CMgaMetaPart

class ATL_NO_VTABLE CMgaMetaPart : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaMetaPart, &__uuidof(MgaMetaPart)>,
	public IDispatchImpl<IMgaMetaPart, &__uuidof(IMgaMetaPart), &__uuidof(__MGAMetaLib)>,
	public CMgaMetaBase
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaPart*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETAPART)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaPart)

BEGIN_COM_MAP(CMgaMetaPart)
	COM_INTERFACE_ENTRY(IMgaMetaPart)
	COM_INTERFACE_ENTRY2(IMgaMetaBase, CMgaMetaBase)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaPart)
END_COM_MAP()

DECLARE_MGAMETAREGNODES()

public:
	STDMETHOD(get_Role)(IMgaMetaRole **p)
	{ return ComGetPointerValue(GetUnknown(), ATTRID_PARTROLE_PTR, p); }

	STDMETHOD(get_ParentAspect)(IMgaMetaAspect **p) 
	{ return ComGetPointerValue(GetUnknown(), ATTRID_PARTASPECT_PTR, p); }

	STDMETHOD(get_IsPrimary)(VARIANT_BOOL *p);
	STDMETHOD(get_IsLinked)(VARIANT_BOOL *p);

	STDMETHOD(get_KindAspect)(BSTR *p)
	{ return ComGetAttrValue(GetUnknown(), ATTRID_KINDASPECT, p); }

// ------- Base

public:
	STDMETHOD(get_MetaRef)(metaref_type *p)
	{
		CHECK_OUT(p);
		*p = metaref;
		return S_OK;
	}
	STDMETHOD(put_MetaRef)(metaref_type p) 
	{ HRESULT hr = PutMetaRef_(p); 
		if(hr == S_OK) hr = ComPutAttrValue(GetUnknown(), ATTRID_METAREF, p); 
		return hr; 
	} 

	STDMETHOD(get_MetaProject)(IMgaMetaProject **p) 
	{
		CHECK_OUT(p);
		CopyTo(metaproject, p);
		return S_OK;
	}

	STDMETHOD(get_Name)(BSTR *p)
	{ /*return E_NOTIMPL;*/
		HRESULT hr;
		CHECK_OUT(p);
		CComPtr<IMgaMetaRole> pRole;
		hr = ((IMgaMetaPart *) this)->get_Role(&pRole);
		if (SUCCEEDED(hr)) {
			hr = pRole->get_Name(p);
		}
		return hr;
	}

	STDMETHOD(put_Name)(BSTR p)
	{ return E_NOTIMPL; }

	STDMETHOD(get_DisplayedName)(BSTR *p)
	{ return E_NOTIMPL; }

	STDMETHOD(put_DisplayedName)(BSTR p)
	{ return E_NOTIMPL; }

	STDMETHOD(get_ObjType)(objtype_enum *p)
	{ return ComGetObjType(GetUnknown(), p); }

	STDMETHOD(get_Constraints)(IMgaMetaConstraints **p) 
	{ return E_NOTIMPL; }

	STDMETHOD(CreateConstraint)(IMgaMetaConstraint **p) 
	{ return E_NOTIMPL; }

	STDMETHOD(Delete)()
	{ return ComDeleteObject(GetUnknown()); }

// ------- Edit

public:
	STDMETHOD(put_IsPrimary)(VARIANT_BOOL p);
	STDMETHOD(put_IsLinked)(VARIANT_BOOL p);

	STDMETHOD(put_KindAspect)(BSTR p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_KINDASPECT, p); }

// ------- Traverse

public:
	static void Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me);

};

#endif//MGA_MGAMETAPART_H
