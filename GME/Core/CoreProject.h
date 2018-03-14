
#ifndef MGA_COREPROJECT_H
#define MGA_COREPROJECT_H

#include <list>
//#include <slist>
#include <hash_map>
#include "CommonVersionInfo.h"

class CCoreProject;
class CCoreObject;
class CCoreTransactionItem;
class CCoreFinalTrItem;
class CCoreUndoItem;
class CCoreTerritory;
class CCoreMetaProject;


#define MAX_UNDO_STEP			10

// --------------------------- typedefs

typedef CCoreTransactionItem *transaction_item_type;
typedef core::forward_list<transaction_item_type> transaction_items_type;
typedef transaction_items_type::iterator transaction_items_iterator;

typedef struct transaction_type
{
	transaction_items_type items;
	bool readonly;
} transaction_type;

typedef core::list<transaction_type> transactions_type;
typedef transactions_type::iterator transactions_iterator;

typedef CCoreFinalTrItem *finaltr_item_type;
typedef core::forward_list<finaltr_item_type> finaltr_items_type;
typedef finaltr_items_type::iterator finaltr_items_iterator;

typedef CCoreUndoItem *undo_item_type;
typedef core::list<undo_item_type> undo_items_type;
typedef undo_items_type::iterator undo_items_iterator;

typedef undo_items_type undo_type;
typedef core::list<undo_type> undos_type;
typedef undos_type::iterator undos_iterator;

typedef core::hash_map< metaobjidpair_type
                        , CCoreObject*
                        , metaobjid2pair_hashfunc
                        > object_lookup_type;
typedef object_lookup_type::iterator object_lookup_iterator;

typedef core::list< CComObjPtr<CCoreTerritory> > pushed_territorys_type;
typedef core::list<CCoreTerritory*> created_territorys_type;

// --------------------------- CCoreProject

class ATL_NO_VTABLE CCoreProject : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCoreProject, &__uuidof(CoreProject)>,
	public IDispatchImpl<ICoreProject, &__uuidof(ICoreProject), &__uuidof(__MGACoreLib)>,
	public ISupportErrorInfoImpl<&__uuidof(ICoreProject)>,
	public IGMEVersionInfoImpl
{
public:
	CCoreProject();
	~CCoreProject();

DECLARE_REGISTRY_RESOURCEID(IDR_COREPROJECT)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCoreProject)
	COM_INTERFACE_ENTRY(ICoreProject)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_IID(__uuidof(IGMEVersionInfo), IGMEVersionInfoImpl)
END_COM_MAP()

// ------- COM methods

public:
	STDMETHOD(put_Storage)(ICoreStorage *p);
	STDMETHOD(get_Storage)(ICoreStorage **p);
	STDMETHOD(get_MetaProject)(ICoreMetaProject **p);

	STDMETHOD(OpenProject)(BSTR connection, ICoreMetaProject *p, VARIANT_BOOL *ro_mode = NULL);
	STDMETHOD(CloseProject)(VARIANT_BOOL abort = VARIANT_FALSE);
	STDMETHOD(SaveProject)(BSTR connection = NULL, VARIANT_BOOL keepoldname = VARIANT_FALSE);
	STDMETHOD(CreateProject)(BSTR connection, ICoreMetaProject *p);
	STDMETHOD(DeleteProject)(BSTR connection);
	// undosize
	STDMETHOD(OpenProject2)(BSTR connection, long pUndoSize, ICoreMetaProject *p, VARIANT_BOOL *ro_mode = NULL);
	STDMETHOD(CreateProject2)(BSTR connection, long pUndoSize, ICoreMetaProject *p);

	STDMETHOD(get_Object)(metaid_type metaid, objid_type objid, ICoreObject **p);
	STDMETHOD(GetObjectDisp)(metaid_type metaid, objid_type objid, ICoreObject **p) { return get_Object( metaid, objid, p); }
	STDMETHOD(CreateObject)(metaid_type metaid, ICoreObject **p);
	STDMETHOD(get_RootObject)(ICoreObject **p);

	STDMETHOD(BeginTransaction)(transtype_enum transtype);
	STDMETHOD(CommitTransaction)(transtype_enum transtype);
	STDMETHOD(AbortTransaction)(transtype_enum transtype);

	STDMETHOD(get_NestedTransactionCount)(short *p);

	STDMETHOD(UndoTransaction)();
	STDMETHOD(RedoTransaction)();
	STDMETHOD(FlushUndoQueue)();
	STDMETHOD(FlushRedoQueue)();
	STDMETHOD(get_UndoQueueSize)(short *p);
	STDMETHOD(get_RedoQueueSize)(short *p);

	STDMETHOD(CreateTerritory)(ICoreTerritory **p);
	STDMETHOD(PushTerritory)(ICoreTerritory *p);
	STDMETHOD(PopTerritory)();

	std::string GetFirstToken(BSTR connection);

// ------- Attributes

protected:
	CComObjPtr<ICoreStorage> storage;
	CComObjPtr<CCoreMetaProject> metaproject;

	typedef unsigned int status_type;
	status_type status;

	transactions_type transactions;
	finaltr_items_type finaltr_items;
	undos_type undos;
	short redo_count;

	object_lookup_type object_lookup;
	pushed_territorys_type pushed_territorys;
	created_territorys_type created_territorys;

	unsigned long    m_maxUndoStep;
	inline void      setMaxUndoStep( long p_undoSize);
// ------- Status

public:
	void SetStatusFlag(status_type flags) { status |= flags; }
	void ResetStatusFlag(status_type flags) { status &= ~flags; }
	bool GetStatusFlag(status_type flag) const { return (status & flag) != 0; }

public:
	bool InTransaction() const NOTHROW { return !transactions.empty(); }
	bool InReadTransaction() const NOTHROW
	{ return !transactions.empty() && transactions.front().readonly; }
	bool InWriteTransaction() const NOTHROW
	{ return !transactions.empty() && !transactions.front().readonly; }

// ------- Transaction

public:
	int GetNestedTrCount() const;

	void RegisterTransactionItem(CCoreTransactionItem *modified) NOTHROW;
	void RegisterFinalTrItem(CCoreFinalTrItem *modified) NOTHROW;
	void RegisterUndoItem(CCoreUndoItem *modified) NOTHROW;
	
	void BeginFinalTr();
	void CommitFinalTr(bool undo);
	void AbortFinalTr();

	void BeginNestedTr() NOTHROW;
	void CommitNestedTr() NOTHROW;
	void AbortNestedTr() NOTHROW;

	void TryUndoTransaction();
	void TryRedoTransaction();
	void TryDiscardLastItem();

// ------- Methods

public:
	ICoreStorage *GetStorage() const NOTHROW { return storage; }
	CCoreMetaProject *GetMetaProject() const NOTHROW { return metaproject; }

	ICoreStorage *SetStorageObject(CCoreObject *object);

	CCoreObject *FindObject(metaid_type metaid, objid_type objid);
	CComObjPtr<CCoreObject> CreateObject(metaid_type metaid, objid_type objid);
	CComObjPtr<CCoreObject> GetObject(metaid_type metaid, objid_type objid);

	CComObjPtr<CCoreObject> CreateObject(metaid_type metaid);

	void RegisterObject(metaid_type metaid, objid_type objid, CCoreObject *object);
	void UnregisterObject(metaid_type metaid, objid_type objid) NOTHROW;

	void CastProject(ICoreProject *project) const;		// throws E_SAMEPROJECT
	CCoreObject *CastObject(ICoreObject *object) const;
	CCoreTerritory *CastTerritory(ICoreTerritory *territory) const;

	CCoreTerritory *GetTerritory() const;
	void RegisterTerritory(CCoreTerritory *territory) NOTHROW;
	void UnregisterTerritory(CCoreTerritory *territory) NOTHROW;
};

#endif//MGA_COREPROJECT_H
