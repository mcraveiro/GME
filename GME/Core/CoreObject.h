
#ifndef MGA_COREOBJECT_H
#define MGA_COREOBJECT_H

#include "CoreTransactionItem.h"
#include "CoreMetaObject.h"

#include <list>
//#include <slist>

const locking_type PUT_DELETE_DONE_LOCK = 55;

class CCoreProject;
class CCoreAttribute;
class CCoreLockAttribute;
class CCoreMetaObject;

// --------------------------- CCoreObject

class ATL_NO_VTABLE CCoreObject : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICoreObject, &__uuidof(ICoreObject), &__uuidof(__MGACoreLib)>,
	public CCoreFinalTrItemImpl<&__uuidof(ICoreObject)>
{
public:
	CCoreObject();
	virtual ~CCoreObject();

#ifdef DEBUG
	char footprint[4];
#endif

	static CComObjPtr<CCoreObject> Create(CCoreProject *project, metaid_type metaid, objid_type objid);

DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(CCoreObject)
	COM_INTERFACE_ENTRY(ICoreObject)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_FUNC_BLIND(0, &CCoreObject::AggregatedInterfaceLookup)
END_COM_MAP()

	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		// TODO: go through all the aggregates and look for that interface?
		return true ? S_OK : S_FALSE;
	}

	static HRESULT WINAPI AggregatedInterfaceLookup(void *pvThis, REFIID riid, LPVOID *ppv, DWORD_PTR dw);		

// ------- COM methods

public:
	STDMETHODIMP get_Project(ICoreProject **p);
	STDMETHODIMP get_MetaObject(ICoreMetaObject **p);
	STDMETHODIMP get_ObjID(objid_type *p);
	STDMETHODIMP get_Attribute(attrid_type attrid, ICoreAttribute **p);
	STDMETHODIMP GetAttributeDisp(attrid_type attrid, ICoreAttribute **p) { return get_Attribute( attrid, p); }
	STDMETHODIMP get_Attributes(ICoreAttributes **p);
	STDMETHODIMP get_AttributeValue(attrid_type attrid, VARIANT *p);
	STDMETHODIMP GetAttributeValueDisp(attrid_type attrid, VARIANT *p) { return get_AttributeValue( attrid, p); }
	STDMETHODIMP put_AttributeValue(attrid_type attrid, VARIANT p);
	STDMETHODIMP SetAttributeValueDisp(attrid_type attrid, VARIANT p) { return put_AttributeValue( attrid, p); }
	STDMETHODIMP get_LoadedAttrValue(attrid_type attrid, VARIANT *p);
	STDMETHODIMP GetLoadedAttrValueDisp(attrid_type attrid, VARIANT *p) { return get_LoadedAttrValue( attrid, p); }
	STDMETHODIMP get_PreviousAttrValue(attrid_type attrid, VARIANT *p);
	STDMETHODIMP GetPreviousAttrValueDisp(attrid_type attrid, VARIANT *p) { return get_PreviousAttrValue( attrid, p); }
	STDMETHODIMP get_PeerLockValue(attrid_type attrid, locking_type *p);
	STDMETHODIMP GetPeerLockValueDisp(attrid_type attrid, locking_type *p) { return get_PeerLockValue( attrid, p); }
	STDMETHODIMP SearchCollection(attrid_type coll_attrid, attrid_type search_attrid,
		VARIANT search_value, ICoreObject **p);
	STDMETHODIMP get_IsDeleted(VARIANT_BOOL *p);
	STDMETHODIMP Delete();
	STDMETHODIMP Clone(ICoreObject **p);

// ------- Properties
	
protected:
	CComObjPtr<CCoreProject> project;
	CComObjPtr<CCoreMetaObject> metaobject;
	objid_type objid;

	typedef core::forward_list<CCoreAttribute*> attributes_type;
	typedef attributes_type::iterator attributes_iterator;
	attributes_type attributes;

	typedef core::forward_list< CComObjPtr<IUnknown> > aggregates_type;
	typedef aggregates_type::iterator aggregates_iterator;
	aggregates_type aggregates;

	typedef unsigned char status_type;
	status_type status;

// ------- Creation 

public:
	void CreateAttributes();
	void CreateAggregates();
	void FillAfterCreateObject();

// ------- Methods

public:
	objid_type GetObjID() const NOTHROW { return objid; }
	CCoreProject *GetProject() const NOTHROW { ASSERT( project ); return project; }
	CCoreMetaObject *GetMetaObject() const NOTHROW { ASSERT( metaobject ); return metaobject; }
	metaid_type GetMetaID() const NOTHROW { ASSERT( metaobject ); return metaobject->GetMetaID(); }

	CCoreAttribute *FindAttribute(attrid_type attrid) const NOTHROW;

	void RegisterAttribute(CCoreAttribute *attribute) NOTHROW;
	void UnregisterAttribute(CCoreAttribute *attribute) NOTHROW;

	template<class Functor, class UnwindFunctor>
	void GetAttributes(CCoreLockAttribute *lockattribute, Functor& f, UnwindFunctor& uf);

	void LoadAttributes(CCoreLockAttribute *lockattribute);
	void UnloadAttributes(CCoreLockAttribute *lockattribute) NOTHROW;

// ------- Status

#define COREOBJECT_DIRTY				0x0001

public:
	void SetStatusFlag(status_type flags) NOTHROW { status |= flags; }
	void ResetStatusFlag(status_type flags) NOTHROW { status &= ~flags; }
	bool GetStatusFlag(status_type flag) const NOTHROW { return (status & flag) != 0; }

public:
	bool InTransaction() const NOTHROW;
	bool InWriteTransaction() const NOTHROW;
	bool IsZombie() const NOTHROW { return project == NULL; }
	void SetZombie() NOTHROW;

	bool IsDirty() const NOTHROW { return GetStatusFlag(COREOBJECT_DIRTY); }
	void SetDirty() NOTHROW { SetStatusFlag(COREOBJECT_DIRTY); }
	void ResetDirty() NOTHROW { ResetStatusFlag(COREOBJECT_DIRTY); }

	bool HasEmptyPointers() const NOTHROW;
	bool HasEmptyPointersAndLocks() const NOTHROW;

// ------- Debug

#ifdef _DEBUG
public:
	void Dump();
#endif

// ------- FinalTrItem

public:
	void RegisterFinalTrItem() NOTHROW;

	virtual void AbortFinalTransaction() NOTHROW;
	virtual void CommitFinalTransaction();
	virtual void CommitFinalTransactionFinish(bool undo) NOTHROW;
#ifdef _ATL_DEBUG_INTERFACES
	std::vector<CComPtr<IUnknown> > cleanup;
#endif
};

#ifndef _ATL_DEBUG_INTERFACES
inline IUnknown *CastToUnknown(CCoreObject *p) { return (IUnknown*)(ICoreObject*)p; }
inline CCoreObject *CastToObject(IUnknown *p) { return (CCoreObject*)(ICoreObject*)p; }
inline IUnknown *CastToUnknown_Object(CCoreObject *p) { return CastToUnknown(p); }
#else
inline CComPtr<IUnknown> CastToUnknown(CCoreObject *p)
{
	CComPtr<IUnknown> pUnk;
	p->QueryInterface(IID_IUnknown, (void**)&pUnk.p);
	p->cleanup.push_back(pUnk);
	return pUnk;
}

inline CComPtr<IUnknown> CastToUnknown_Object(CCoreObject *p)
{
	CComPtr<IUnknown> pUnk;
	p->QueryInterface(IID_IUnknown, (void**)&pUnk.p);
	p->cleanup.push_back(pUnk);

	pUnk = 0;
	p->QueryInterface(__uuidof(ICoreObject), (void**)&pUnk.p);
	p->cleanup.push_back(pUnk);
	return pUnk;
}

bool IsQIThunk(IUnknown *p);

inline CCoreObject *CastToObject(IUnknown *p) {
	if (IsQIThunk(p))
		return (CCoreObject*)(ICoreObject*)((ATL::_QIThunk *)(p))->m_pUnk;
	else
		return (CCoreObject*)(ICoreObject*)p;
}
#endif

#endif//MGA_COREOBJECT_H
