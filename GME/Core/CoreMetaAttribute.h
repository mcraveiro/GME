
#ifndef MGA_COREMETAATTRIBUTE_H
#define MGA_COREMETAATTRIBUTE_H

class CCoreMetaObject;

// --------------------------- CCoreMetaAttribute

class ATL_NO_VTABLE CCoreMetaAttribute : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICoreMetaAttribute, &__uuidof(ICoreMetaAttribute), &__uuidof(__MGACoreLib)>,
	public ISupportErrorInfoImpl<&__uuidof(ICoreMetaAttribute)>
{
public:
	CCoreMetaAttribute();

DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(CCoreMetaAttribute)
	COM_INTERFACE_ENTRY(ICoreMetaAttribute)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ------- COM methods

public:
	STDMETHODIMP get_Object(ICoreMetaObject **p);
	STDMETHODIMP get_AttrID(attrid_type *p);
	STDMETHODIMP get_Token(BSTR *p);
	STDMETHODIMP get_Name(BSTR *p);
	STDMETHODIMP get_ValueType(valtype_type *p);
	STDMETHODIMP get_LockAttrID(attrid_type *p);
	STDMETHODIMP get_MaxSize(long *p);

// ------- Properties

public:
	CCoreMetaObject *object;
	attrid_type attrid;
	valtype_type valtype;
	std::wstring token;
	std::wstring name;

// ------- Methods

public:
	attrid_type GetAttrID() const { return attrid; }
	valtype_type GetValType() const { return valtype; }
};

#endif//MGA_COREMETAATTRIBUTE_H
