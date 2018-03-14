
#ifndef MGA_COREMETAOBJECT_H
#define MGA_COREMETAOBJECT_H

#include <list>//slist

class CCoreMetaProject;
class CCoreMetaAttribute;

// --------------------------- CCoreMetaObject

class ATL_NO_VTABLE CCoreMetaObject : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICoreMetaObject, &__uuidof(ICoreMetaObject), &__uuidof(__MGACoreLib)>,
	public ISupportErrorInfoImpl<&__uuidof(ICoreMetaObject)>
{
public:
	CCoreMetaObject();
	~CCoreMetaObject();


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(CCoreMetaObject)
	COM_INTERFACE_ENTRY(ICoreMetaObject)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ------- COM methods

public:
	STDMETHODIMP get_Project(ICoreMetaProject **p);
	STDMETHODIMP get_MetaID(metaid_type *p);
	STDMETHODIMP get_Token(BSTR *p);
	STDMETHODIMP get_Name(BSTR *p);
	STDMETHODIMP get_Attribute(attrid_type attrid, ICoreMetaAttribute **p);
	STDMETHODIMP GetAttributeDisp(attrid_type attrid, ICoreMetaAttribute **p) { return get_Attribute( attrid, p); }
	STDMETHODIMP get_Attributes(ICoreMetaAttributes **p);
	STDMETHODIMP get_ClassIDs(SAFEARRAY **p);
	STDMETHODIMP AddAttribute(attrid_type attrid, BSTR token, BSTR name, 
		valtype_type valtype, ICoreMetaAttribute **p);
	STDMETHODIMP AddClassID(guid_type classid);

// ------- Properties

public:
	typedef std::forward_list<CCoreMetaAttribute*> attributes_type;

	CCoreMetaProject *project;
	attributes_type attributes;
	std::vector<::GUID> classids;

	metaid_type metaid;
	std::string token;
	std::string name;

// ------- Methods

public:
	const attributes_type &GetAttributes() const { return attributes; }
	const std::vector<::GUID> &GetClassIDs() const { return classids; }
	metaid_type GetMetaID() const { return metaid; }

};

#endif//MGA_COREMETAOBJECT_H
