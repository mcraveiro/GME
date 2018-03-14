
#ifndef MGA_CORETERRITORY_H
#define MGA_CORETERRITORY_H

#include "CoreTransactionItem.h"

#include <list>
#include <hash_map>

class CCoreProject;
class CCoreLockAttribute;

// --------------------------- CCoreTerritory

class ATL_NO_VTABLE CCoreTerritory : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICoreTerritory, &__uuidof(ICoreTerritory), &__uuidof(__MGACoreLib)>,
	public CCoreFinalTrItemImpl<&__uuidof(ICoreTerritory)>,
	public CCoreTransactionItem
{
public:
	CCoreTerritory();
	virtual ~CCoreTerritory();

	static CCoreTerritory *Create(CCoreProject *project);

BEGIN_COM_MAP(CCoreTerritory)
	COM_INTERFACE_ENTRY(ICoreTerritory)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ------- Attributes

protected:
	CComObjPtr<CCoreProject> project;

	typedef unsigned char status_type;
	status_type status;

// ------- COM methods

public:
	STDMETHODIMP get_Project(ICoreProject **p);
	STDMETHODIMP get_Attributes(ICoreAttributes **p);
	STDMETHODIMP Clear();

// ------- Methods

public:
	CCoreProject *GetProject() const { return project; }

// ------- Status

#define CORETERRITORY_DIRTY				0x0001
#define CORETERRITORY_FINAL				0x0002

private:
	void SetStatusFlag(status_type flags) { status |= flags; }
	void ResetStatusFlag(status_type flags) { status &= ~flags; }

	void ChangeStatusFlag(status_type flag, bool set) { if(set) status |= flag; else status &= ~flag; }
	bool GetStatusFlag(status_type flag) const { return (status & flag) != 0; }

	bool InTransaction() const;

// ------- NestedTrItem

public:
	virtual bool IsDirty() const NOTHROW { return GetStatusFlag(CORETERRITORY_DIRTY); }
	virtual void ChangeDirty(bool dirty) NOTHROW { ChangeStatusFlag(CORETERRITORY_DIRTY, dirty); }

	virtual void AbortNestedTransaction() NOTHROW { ResetStatusFlag(CORETERRITORY_DIRTY); }
	virtual void DiscardPreviousValue() NOTHROW { }

// ------- FinalTrItem

public:
	virtual void AbortFinalTransaction() NOTHROW { ResetStatusFlag(CORETERRITORY_DIRTY); }
	virtual void CommitFinalTransaction() {}
	virtual void CommitFinalTransactionFinish(bool undo) NOTHROW { ResetStatusFlag(CORETERRITORY_DIRTY); }
};

#ifndef _ATL_DEBUG_INTERFACES
inline IUnknown *CastToUnknown(CCoreTerritory *p) { return (IUnknown*)(ICoreTerritory*)p; }
inline CCoreTerritory *CastToTerritory(IUnknown *p) { return (CCoreTerritory*)(ICoreTerritory*)p; }
#else
inline IUnknown *CastToUnknown(CCoreTerritory *p) {
	DebugBreak();
	return (IUnknown*)(ICoreTerritory*)p;
}
inline CCoreTerritory *CastToTerritory(IUnknown *p)
{
	return (CCoreTerritory*)(ICoreTerritory*)((ATL::_QIThunk *)(p))->m_pUnk;
}
#endif

#endif//MGA_CORETERRITORY_H

