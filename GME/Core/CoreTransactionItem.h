
#ifndef MGA_CORETRANSACTIONITEM_H
#define MGA_CORETRANSACTIONITEM_H

// --------------------------- CCoreTransactionItem

// IMPORTANT: Each transaction item cares only about itself and those
// of its parts which are not transaction items themselves.
// Never make any modification to other transaction items during
// abort, discard, commit or commit finish.

// call CCoreProject::RegisterTransactionItem to get into the
// latest nested transaction, be dirty when registering

class ATL_NO_VTABLE CCoreTransactionItem
{
public:
	// dirty if modified in the latest transaction
	virtual bool IsDirty() const NOTHROW = 0;

	// we clear the flag at the beggining of a nested transaction
	// we set it at abort, or committing a nested tr and gaining focus
	virtual void ChangeDirty(bool dirty) NOTHROW = 0;

	// must be dirty, discard the latest transaction value, clear dirty
	virtual void AbortNestedTransaction() NOTHROW = 0;

	// must be dirty, discard the previous transaction value, keep dirty
	// called only for nested transactions when we merge the latest two values
	virtual void DiscardPreviousValue() NOTHROW = 0;
};

// --------------------------- CCoreFinalTrItem

// call CCoreProject::RegisterFinalTrItem to register, call it only once
// CoreObjects register to group database access of their CoreAttributes

class ATL_NO_VTABLE CCoreFinalTrItem : public ISupportErrorInfo // all subclasses will inherit ISupportErrorInfoImpl, so inherit here to save a vtable entry
{
public:
	// called for final abort, clear dirty
	virtual void AbortFinalTransaction() NOTHROW = 0;

	// should do the work, may be called several times,
	virtual void CommitFinalTransaction() = 0;

	// clean up, clear dirty
	virtual void CommitFinalTransactionFinish(bool undo) NOTHROW = 0;
};

template<const IID* piid>
class ATL_NO_VTABLE CCoreFinalTrItemImpl : public CCoreFinalTrItem // all subclasses will inherit ISupportErrorInfoImpl, so inherit here to save a vtable entry
{
	// ISupportErrorInfoImpl
public:
	STDMETHOD(InterfaceSupportsErrorInfo)(_In_ REFIID riid)
	{
		return (InlineIsEqualGUID(riid,*piid)) ? S_OK : S_FALSE;
	}

};

// --------------------------- CCoreUndoItem

// call CCoreProject::RegisterUndoItem to register,
// call it only from CommitFinalTransactionFinish

class ATL_NO_VTABLE CCoreUndoItem : public CCoreTransactionItem
	// KMS: inherit from CCoreTransactionItem so one less vtable pointer is required in subclasses
	// (the only subclass is CCoreDataAttribute which also implements CCoreTransactionItem)
{
public:
	// do the work here, called only once for each level
	// this is not a regular transaction, do not register new
	// transaction items and final transaction items
	virtual void UndoTransaction() = 0;
	virtual void UndoTransactionFinish() NOTHROW = 0;

	virtual void RedoTransaction() = 0;
	virtual void RedoTransactionFinish() NOTHROW = 0;

	// not a regular transaction, only locking attributes can register
	virtual void DiscardLastItem() = 0;
	virtual void DiscardLastItemFinish() NOTHROW = 0;
	virtual void DiscardLastItemCancel() NOTHROW = 0;
};

#endif//MGA_CORETRANSACTIONITEM_H
