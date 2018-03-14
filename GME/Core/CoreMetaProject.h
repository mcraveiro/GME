
#ifndef MGA_COREMETAPROJECT_H
#define MGA_COREMETAPROJECT_H

#include <list>//slist

class CCoreMetaObject;

// --------------------------- CCoreMetaProject

class ATL_NO_VTABLE CCoreMetaProject : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCoreMetaProject, &__uuidof(CoreMetaProject)>,
	public IDispatchImpl<ICoreMetaProject, &__uuidof(ICoreMetaProject), &__uuidof(__MGACoreLib)>,
	public ISupportErrorInfoImpl<&__uuidof(ICoreMetaProject)>
{
public:
	CCoreMetaProject();
	~CCoreMetaProject();

DECLARE_REGISTRY_RESOURCEID(IDR_COREMETAPROJECT)

BEGIN_COM_MAP(CCoreMetaProject)
	COM_INTERFACE_ENTRY(ICoreMetaProject)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ------- COM methods

public:
	STDMETHODIMP get_GUID(guid_type *p);
	STDMETHODIMP put_GUID(guid_type p);
	STDMETHODIMP get_Token(BSTR *p);
	STDMETHODIMP put_Token(BSTR p);
	STDMETHODIMP get_Name(BSTR *p);
	STDMETHODIMP put_Name(BSTR p);
	STDMETHODIMP get_Object(metaid_type metaid, ICoreMetaObject **p);
	STDMETHODIMP GetObjectDisp(metaid_type metaid, ICoreMetaObject **p) { return get_Object( metaid, p); }
	STDMETHODIMP get_Objects(ICoreMetaObjects **p);
	STDMETHODIMP AddObject(metaid_type metaid, BSTR token, BSTR name, ICoreMetaObject **p);

// ------- Methods

public:
	// returns NULL if not found
	CCoreMetaObject *GetObject(metaid_type metaid) NOTHROW;

// ------- Properties

public:

	typedef std::forward_list<CCoreMetaObject*> objects_type;
	typedef objects_type::iterator objects_iterator;

	objects_type objects;
	std::string name;
	std::string token;
	::GUID guid;
};

#ifndef _ATL_DEBUG_INTERFACES
inline IUnknown *CastToUnknown(CCoreMetaProject *p) { return (IUnknown*)(ICoreMetaProject*)p; }
inline CCoreMetaProject *CastToMetaProject(IUnknown *p) { return (CCoreMetaProject*)(ICoreMetaProject*)p; }
#else
inline IUnknown *CastToUnknown(CCoreMetaProject *p) { return (IUnknown*)(ICoreMetaProject*)p; }
inline CCoreMetaProject *CastToMetaProject(IUnknown *p) { return (CCoreMetaProject*)(ICoreMetaProject*)((ATL::_QIThunk *)(p))->m_pUnk; }
#endif

#endif//MGA_COREMETAPROJECT_H
