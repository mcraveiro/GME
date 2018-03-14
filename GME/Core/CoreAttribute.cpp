
#include "stdafx.h"

#include "CoreAttribute.h"
#include "CommonCollection.h"
#include "CoreProject.h"
#include "CoreTerritory.h"
#include "..\Mga\MgaGeneric.h"

// --------------------------- Defines

#define LOCKING_GETVALUE LOCKING_READ
#define LOCKING_PUTVALUE LOCKING_EXCLUSIVE
#define LOCKING_GETCOLLECTION LOCKING_READ

#define CHECK_ZOMBIE() \
	if( IsZombie() ) \
		COMRETURN(E_ZOMBIE);

// --------------------------- LOCKVAL Constants

#define LOCKVAL_NONE		0
#define LOCKVAL_EXCLUSIVE	-1
#define LOCKVAL_READ_START	1
#define LOCKVAL_READ_END	32767
#define LOCKVAL_WRITE_START	-2
#define LOCKVAL_WRITE_END	-32768

// --------------------------- CCoreAttribute

CCoreAttribute::CCoreAttribute()
{
	status = 0;

#ifdef DEBUG
	footprint[0] = '(';
	footprint[1] = 'C';
	footprint[2] = 'A';
	footprint[3] = ')';
#endif
}

typedef CCoreDataAttribute<CCorePointerAttrBase, VALTYPE_POINTER> CCorePointerAttribute;
typedef CCoreDataAttribute<CCoreDataAttrBase<long>, VALTYPE_LONG> CCoreLongAttribute;
typedef CCoreDataAttribute<CCoreDataAttrBase<CComBstrObj>, VALTYPE_STRING> CCoreStringAttribute;
typedef CCoreDataAttribute<CCoreDataAttrBase<bindata>, VALTYPE_BINARY> CCoreBinaryAttribute;
typedef CCoreDataAttribute<CCoreDataAttrBase<double>, VALTYPE_REAL> CCoreRealAttribute;
typedef CCoreDataAttribute<CCoreDataAttrBase<CComPtr<CCoreDictionaryAttributeValue>>, VALTYPE_DICT> CCoreDictAttribute;

void CCoreAttribute::Create(CCoreObject *object, CCoreMetaAttribute *metaattribute)
{
	ASSERT( object != NULL );
	ASSERT( metaattribute != NULL );

	CCoreAttribute* attribute = NULL;

	switch( metaattribute->GetValType() )
	{
	case VALTYPE_POINTER:
		{
			typedef CComPartObject< CCorePointerAttribute > COMTYPE;

			COMTYPE *p = NULL;
			COMTHROW( COMTYPE::CreateInstance(CastToUnknown_Object(object), &p) );
			attribute = p;

			break;
		}

	case VALTYPE_COLLECTION:
		{
			typedef CComPartObject< CCoreCollectionAttribute > COMTYPE;

			COMTYPE *p = NULL;
			COMTHROW( COMTYPE::CreateInstance(CastToUnknown_Object(object), &p) );
			attribute = p;

			break;
		}

	case VALTYPE_LONG:
		{
			typedef CComPartObject< CCoreLongAttribute > COMTYPE;

			COMTYPE *p = NULL;
			COMTHROW( COMTYPE::CreateInstance(CastToUnknown_Object(object), &p) );
			attribute = p;

			break;
		}

		case VALTYPE_DICT:
		{
			typedef CComPartObject< CCoreDictAttribute > COMTYPE;

			COMTYPE *p = NULL;
			COMTHROW( COMTYPE::CreateInstance(CastToUnknown_Object(object), &p) );
			attribute = p;

			break;
		}

	case VALTYPE_STRING:
		{
			typedef CComPartObject< CCoreStringAttribute > COMTYPE;

			COMTYPE *p = NULL;
			COMTHROW( COMTYPE::CreateInstance(CastToUnknown_Object(object), &p) );
			attribute = p;

			break;
		}

	case VALTYPE_BINARY:
		{
			typedef CComPartObject< CCoreBinaryAttribute > COMTYPE;

			COMTYPE *p = NULL;
			COMTHROW( COMTYPE::CreateInstance(CastToUnknown_Object(object), &p) );
			attribute = p;

			break;
		}

	case VALTYPE_REAL:
		{
			typedef CComPartObject< CCoreRealAttribute > COMTYPE;

			COMTYPE *p = NULL;
			COMTHROW( COMTYPE::CreateInstance(CastToUnknown_Object(object), &p) );
			attribute = p;

			break;
		}

	case VALTYPE_LOCK:
		{
			typedef CComPartObject< CCoreLockAttribute > COMTYPE;

			COMTYPE *p = NULL;
			COMTHROW( COMTYPE::CreateInstance(CastToUnknown_Object(object), &p) );
			attribute = p;

			break;
		}

	default:
		HR_THROW(E_VALTYPE);
	}

	ASSERT( attribute != NULL );

	// nothing throws here
	attribute->metaattribute = metaattribute;

	object->RegisterAttribute(attribute);
	// object is ready for deletion
}

CCoreAttribute::~CCoreAttribute()
{
	ASSERT( metaattribute != NULL );
	ASSERT( !IsDirty() );

#ifndef _ATL_DEBUG_INTERFACES
	GetObject()->UnregisterAttribute(this);
#endif
}

#ifdef DEBUG


void CCoreAttribute::Dump() {
	CComBstrObj attrbstr;
	COMTHROW( metaattribute->get_Name(PutOut(attrbstr)) );

	std::string attrname;
	CopyTo(attrbstr, attrname);

	AtlTrace("attribute_dump attrid=%d name=\"%s\" \n",
		(int)GetAttrID(), attrname.c_str());

}

#endif


// ------- Inline

inline CCoreProject *CCoreAttribute::GetProject() const
{
	return GetObject()->GetProject();
}

inline CCoreTerritory *CCoreAttribute::GetTerritory() const
{
	return GetProject()->GetTerritory();
}

// ------- COM methods

STDMETHODIMP CCoreAttribute::get_Object(ICoreObject **p)
{
	CHECK_OUT(p);

	CopyTo(GetObject(), p);

	return S_OK;
}

STDMETHODIMP CCoreAttribute::get_MetaAttribute(ICoreMetaAttribute **p)
{
	CHECK_OUT(p);

	ASSERT( metaattribute != NULL );
	CopyTo(metaattribute, p);

	return S_OK;
}

// ------- Methods

CCoreLockAttribute *CCoreAttribute::GetLockAttr() const
{
	ASSERT( metaattribute != NULL );

	attrid_type attrid = 0;
	COMTHROW( metaattribute->get_LockAttrID(&attrid) );
	ASSERT( attrid != 0 );

	CCoreAttribute *attribute = GetObject()->FindAttribute(attrid);
	ASSERT( attribute != NULL );

	ASSERT( attribute->GetValType() == VALTYPE_LOCK );
	return (CCoreLockAttribute*)attribute;
}

ICoreStorage *CCoreAttribute::SetStorageThisAttribute()
{
	ICoreStorage *storage = GetProject()->SetStorageObject(GetObject());
	ASSERT( storage != NULL );

	ASSERT( metaattribute != NULL );
	COMTHROW( storage->put_MetaAttribute(metaattribute) );

	return storage;
}

// ------- Status

inline bool CCoreAttribute::InTransaction() const
{
	return GetObject()->InTransaction();
}

inline bool CCoreAttribute::InWriteTransaction() const
{
	return GetObject()->InWriteTransaction();
}

inline bool CCoreAttribute::IsZombie() const
{
	return GetObject()->IsZombie();
}

// --------------------------- CCoreLockAttribute

CCoreLockAttribute::CCoreLockAttribute()
{
	original_locking = 0;
	others_lockval = 0;
	read_count = 0;
	write_count = 0;
}

#ifdef _DEBUG

CCoreLockAttribute::~CCoreLockAttribute()
{
	ASSERT( read_count == 0 );
	ASSERT( write_count == 0 );
	ASSERT( !GetStatusFlag(COREATTRIBUTE_LOCKGROUP_LOADED) );
}

#endif

STDMETHODIMP CCoreLockAttribute::get_Value(VARIANT *p)
{
	CHECK_OUT(p);
	CHECK_ZOMBIE();

	COMTRY
	{
		// this may not be right, but it shouldn't matter
		CopyTo(original_locking, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreLockAttribute::put_Value(VARIANT p)
{
	CHECK_ZOMBIE();

	if( !InTransaction() )
		COMRETURN(E_TRANSACTION);

	COMTRY
	{
		locking_type locking = LOCKING_NONE;
		CopyTo(p, locking);

		// this kludge value is used by meta to indicate that the project is about to be closed, so just call Release on our object and be done with it
		if (locking == (unsigned char)-1)
		{
			SetStatusFlag(COREATTRIBUTE_LOCK_CLOSED);
			read_count = write_count = 0;
			SetDirty();
			Unload();
			return S_OK;
		}
		locking_type oldVal = 0;
		if (locking == PUT_DELETE_DONE_LOCK) {
			locking = LOCKING_NONE;
			oldVal = LOCKING_EXCLUSIVE;
		}

		if( !(LOCKING_NONE <= locking && locking <= LOCKING_EXCLUSIVE) )
			HR_THROW(E_INVALIDARG);

		RegisterLockTry(oldVal, locking);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreLockAttribute::get_PeerLockValue(locking_type *p)
{
	CHECK_ZOMBIE();

	COMTRY
	{
		if ( read_count > 0 && write_count > 0 )
			*p = LOCKING_NONE;		
		else if( IsDirty() || !InTransaction() )
			*p = CalcLocking(others_lockval);
		else 
		{
			ICoreStorage *storage = SetStorageThisAttribute();
			ASSERT( storage != NULL );

			CComVariant v;
			COMTHROW( storage->get_AttributeValue(PutOut(v)) );

			lockval_type lockval;
			CopyTo(v, lockval);

			locking_type locking = CombineLock(read_count, write_count);

			// if this fails then something is wrong in the storage
			lockval = CalcUnlock(locking, lockval);
			*p = CalcLocking(lockval);
		}
	}
	COMCATCH(;)
}

void CCoreLockAttribute::RegisterLockTry(locking_type unreg, locking_type reg)
{
	if (GetStatusFlag(COREATTRIBUTE_LOCK_CLOSED))
		return;
	ASSERT( InTransaction() );

	locking_type old_locking = CombineLock(read_count, write_count);

	ASSERT( ((unreg & LOCKING_READ) ? 1 : 0) <= read_count );
	ASSERT( ((unreg & LOCKING_WRITE) ? 1 : 0) <= write_count );

	// temporary, possible exception
	lock_count_type new_read_count = read_count + ((reg & LOCKING_READ) != 0) - ((unreg & LOCKING_READ) != 0);
	lock_count_type new_write_count = write_count + ((reg & LOCKING_WRITE) != 0) - ((unreg & LOCKING_WRITE) != 0);

	ASSERT( ((reg & LOCKING_READ) ? 1 : 0) <= new_read_count );
	ASSERT( ((reg & LOCKING_WRITE) ? 1 : 0) <= new_write_count );

	locking_type new_locking = CombineLock(new_read_count, new_write_count);

	if( old_locking != new_locking )
	{
		if( !IsDirty() )
			Load();

		// no sideeffect, may throw, make sure that we can have new_locking
		CalcLock(new_locking, others_lockval);
	}

	read_count = new_read_count;
	write_count = new_write_count;

	// Load() should take care of ControlLockGroup
	ASSERT( IsLoaded() == GetStatusFlag(COREATTRIBUTE_LOCKGROUP_LOADED) );
}

void CCoreLockAttribute::RegisterLockDo(locking_type unreg, locking_type reg)
{
	ASSERT( InTransaction() );

	ASSERT( ((unreg & LOCKING_READ) ? 1 : 0) <= read_count );
	ASSERT( ((unreg & LOCKING_WRITE) ? 1 : 0) <= write_count );

	read_count += ((reg & LOCKING_READ) != 0) - ((unreg & LOCKING_READ) != 0);
	write_count += ((reg & LOCKING_WRITE) != 0) - ((unreg & LOCKING_WRITE) != 0);

	ASSERT( ((reg & LOCKING_READ) ? 1 : 0) <= read_count );
	ASSERT( ((reg & LOCKING_WRITE) ? 1 : 0) <= write_count );

	ControlLockGroupDo();
}

void CCoreLockAttribute::Load()
{
	ASSERT( InTransaction() );
	ASSERT( !IsDirty() );

	if( read_count > 0 && write_count > 0 )
	{
		original_locking = LOCKING_EXCLUSIVE;
		others_lockval = LOCKVAL_NONE;	// no other can have access
	}
	else
	{
		ICoreStorage *storage = SetStorageThisAttribute();
		ASSERT( storage != NULL );

		COMTHROW( storage->LockObject() );

		CComVariant v;
		COMTHROW( storage->get_AttributeValue(PutOut(v)) );

		lockval_type original_lockval;
		CopyTo(v, original_lockval);

		original_locking = CombineLock(read_count, write_count);

		// if this fails then something is wrong in the storage
		others_lockval = CalcUnlock(original_locking, original_lockval);
	}

	// we have to set to dirty because in LoadAttributes
	// we try to lock this object (while put_Value), and
	// the lock attribute will still not be loaded (infinite cycle)

	SetDirty();

	try
	{
		ControlLockGroup();
	}
	catch(hresult_exception &)
	{
		ResetDirty();
		throw;
	}

	GetObject()->RegisterFinalTrItem();
}

void CCoreLockAttribute::ControlLockGroup()
{
	if( GetStatusFlag(COREATTRIBUTE_LOCKGROUP_LOADED) && !IsLoaded() )
	{
		GetObject()->UnloadAttributes(this);
		ResetStatusFlag(COREATTRIBUTE_LOCKGROUP_LOADED);
		GetObject()->Release();
	}
	else if( !GetStatusFlag(COREATTRIBUTE_LOCKGROUP_LOADED) && IsLoaded() )
	{
		// this may fail
		GetObject()->LoadAttributes(this);

		GetObject()->AddRef();
		SetStatusFlag(COREATTRIBUTE_LOCKGROUP_LOADED);
	}
}

void CCoreLockAttribute::ControlLockGroupDo()
{
	if( GetStatusFlag(COREATTRIBUTE_LOCKGROUP_LOADED) && !IsLoaded() )
	{
		GetObject()->UnloadAttributes(this);
		ResetStatusFlag(COREATTRIBUTE_LOCKGROUP_LOADED);
		GetObject()->Release();
	}

	// no loading
	ASSERT( GetStatusFlag(COREATTRIBUTE_LOCKGROUP_LOADED) == IsLoaded() );
}

void CCoreLockAttribute::Unload()
{
	ASSERT( IsDirty() );

	ResetDirty();
	ControlLockGroupDo();

	// nothing can be here, the object might get destroyed
}

inline void CCoreLockAttribute::Save()
{
	ASSERT( InTransaction() );
	ASSERT( IsDirty() );

	locking_type local_locking = CombineLock(read_count, write_count);

	if( local_locking != original_locking )
	{
#ifdef DEBUG
		lockval_type total_lockval;
		try
		{
			total_lockval = CalcLock(local_locking, others_lockval);
		}
		catch(hresult_exception &)
		{
			// if this fails then something is wrong in RegisterLockTry
			ASSERT(false);
		}
#else
		lockval_type total_lockval = CalcLock(local_locking, others_lockval);
#endif

		ICoreStorage *storage = SetStorageThisAttribute();
		ASSERT( storage != NULL );

		CComVariant v;
		CopyTo(total_lockval, v);

		COMTHROW( storage->put_AttributeValue(v) );
	}
}

void CCoreLockAttribute::FillAfterCreateObject()
{
	ASSERT( InTransaction() );
	ASSERT( !IsDirty() );

	ASSERT( read_count == 0 );
	ASSERT( write_count == 0 );

	original_locking = LOCKING_NONE;
	others_lockval = LOCKVAL_NONE;

	SetDirty();

	// do not call ControlLockGroup, FillAfterCreateObject is called for each attribute
	GetObject()->AddRef();
	SetStatusFlag(COREATTRIBUTE_LOCKGROUP_LOADED);
}

void CCoreLockAttribute::CommitFinalTransaction()
{
	ASSERT( InTransaction() );
	ASSERT( IsDirty() );

	Save();
}

lockval_type CCoreLockAttribute::CalcLock(locking_type locking, lockval_type lockval)
{
	switch( locking )
	{
	case LOCKING_READ:
		if( lockval == LOCKVAL_NONE )
			lockval = LOCKVAL_READ_START;
#if LOCKVAL_READ_START < LOCKVAL_READ_END
		else if( LOCKVAL_READ_START <= lockval && lockval < LOCKVAL_READ_END )
			++lockval;
#else
		else if( LOCKVAL_READ_END < lockval && lockval <= LOCKVAL_READ_START )
			--lockval;
#endif
		else
		{
#ifdef _DEBUG
			AtlTrace("Lock violation while locking with LOCKING_READ\n");
			GetObject()->Dump();
#endif
			HR_THROW(E_LOCK_VIOLATION);
		}
		break;

	case LOCKING_WRITE:
		if( lockval == LOCKVAL_NONE )
			lockval = LOCKVAL_WRITE_START;
#if LOCKVAL_WRITE_START < LOCKVAL_WRITE_END
		else if( LOCKVAL_WRITE_START <= lockval && lockval < LOCKVAL_WRITE_END )
			++lockval;
#else
		else if( LOCKVAL_WRITE_END < lockval && lockval <= LOCKVAL_WRITE_START )
			--lockval;
#endif
		else
		{
#ifdef _DEBUG
			AtlTrace("Lock violation while locking with LOCKING_WRITE\n");
			GetObject()->Dump();
#endif
			HR_THROW(E_LOCK_VIOLATION);
		}
		break;

	case LOCKING_EXCLUSIVE:
		if( lockval == LOCKVAL_NONE )
			lockval = LOCKVAL_EXCLUSIVE;
		else
		{
#ifdef _DEBUG
			AtlTrace("Lock violation while locking with LOCKING_EXCLUSIVE\n");
			GetObject()->Dump();
#endif
			HR_THROW(E_LOCK_VIOLATION);
		}
		break;

	default:
		ASSERT( locking == LOCKING_NONE );
	}

	return lockval;
}

lockval_type CCoreLockAttribute::CalcUnlock(locking_type locking, lockval_type lockval)
{
	switch( locking )
	{
	case LOCKING_READ:
		if( lockval == LOCKVAL_READ_START )
			lockval = LOCKVAL_NONE;
#if LOCKVAL_READ_START < LOCKVAL_READ_END
		else if( LOCKVAL_READ_START < lockval && lockval <= LOCKVAL_READ_END )
			--lockval;
#else
		else if( LOCKVAL_READ_END <= lockval && lockval < LOCKVAL_READ_START )
			++lockval;
#endif
		else
			HR_THROW(E_INVALID_LOCK_VALUE);
		break;

	case LOCKING_WRITE:
		if( lockval == LOCKVAL_WRITE_START )
			lockval = LOCKVAL_NONE;
#if LOCKVAL_WRITE_START < LOCKVAL_WRITE_END
		else if( LOCKVAL_WRITE_START < lockval && lockval <= LOCKVAL_WRITE_END )
			--lockval;
#else
		else if( LOCKVAL_WRITE_END <= lockval && lockval < LOCKVAL_WRITE_START )
			++lockval;
#endif
		else
			HR_THROW(E_INVALID_LOCK_VALUE);
		break;

	case LOCKING_EXCLUSIVE:
		if( lockval == LOCKVAL_EXCLUSIVE )
			lockval = LOCKVAL_NONE;
		else
			HR_THROW(E_INVALID_LOCK_VALUE);
		break;

	default:
		ASSERT( locking == LOCKING_NONE );
	}

	return lockval;
}

locking_type CCoreLockAttribute::CalcLocking(lockval_type lockval)
{
	if( lockval == LOCKVAL_NONE )
		return LOCKING_NONE;
	else if( lockval == LOCKVAL_EXCLUSIVE )
		return LOCKING_EXCLUSIVE;
#if LOCKVAL_READ_START < LOCKVAL_READ_END
	else if( LOCKVAL_READ_START <= lockval && lockval <= LOCKVAL_READ_END )
#else
	else if( LOCKVAL_READ_END <= lockval && lockval <= LOCKVAL_READ_START )
#endif
		return LOCKING_READ;
#if LOCKVAL_WRITE_START < LOCKVAL_WRITE_END
	else if( LOCKVAL_WRITE_START <= lockval && lockval <= LOCKVAL_WRITE_END )
#else
	else if( LOCKVAL_WRITE_END <= lockval && lockval <= LOCKVAL_WRITE_START )
#endif
		return LOCKING_WRITE;

	HR_THROW(E_INVALID_LOCK_VALUE);
}

locking_type CCoreLockAttribute::CombineLock(lock_count_type read_count,
	lock_count_type write_count)
{
	ASSERT( 0 <= read_count );
	ASSERT( 0 <= write_count );
	ASSERT( LOCKING_READ + LOCKING_WRITE == LOCKING_EXCLUSIVE );

	return (read_count ? LOCKING_READ : LOCKING_NONE) +
		(write_count ? LOCKING_WRITE : LOCKING_NONE) - LOCKING_NONE;
};

// ------- Debug

#ifdef _DEBUG

void CCoreLockAttribute::Dump()
{
	ASSERT( metaattribute != NULL );

	CComBstrObj attrbstr;
	COMTHROW( metaattribute->get_Name(PutOut(attrbstr)) );

	std::string attrname;
	CopyTo(attrbstr, attrname);

	AtlTrace("attribute_dump attrid=%d name=\"%s\" read_count=%d write_count=%d"
		" original_locking=%d others_lockval=%d ",
		(int)GetAttrID(), attrname.c_str(), (int)read_count, (int)write_count, 
		(int)original_locking, (int)others_lockval );

	if( InTransaction() )
	{
		ICoreStorage *storage = SetStorageThisAttribute();
		ASSERT( storage != NULL );

		CComVariant v;
		COMTHROW( storage->get_AttributeValue(PutOut(v)) );

		lockval_type val;
		CopyTo(v, val);

		AtlTrace("storage=%d\n", (int)val);
	}
	else
		AtlTrace("storage=<unknown>\n");
}

#endif

// --------------------------- CCoreDataAttrBase

// ------- Low level

template<class DATA>
inline void CCoreDataAttrBase<DATA>::LockSelfTry()
{
	GetLockAttr()->RegisterLockTry(LOCKING_NONE, LOCKING_EXCLUSIVE);
}

template<class DATA>
inline void CCoreDataAttrBase<DATA>::LockSelfCancel()
{
	GetLockAttr()->RegisterLockDo(LOCKING_EXCLUSIVE, LOCKING_NONE);
}

template<class DATA>
inline void CCoreDataAttrBase<DATA>::UnlockSelfTry()
{
	GetLockAttr()->RegisterLockTry(LOCKING_EXCLUSIVE, LOCKING_NONE);
}

template<class DATA>
inline void CCoreDataAttrBase<DATA>::UnlockSelfCancel()
{
	GetLockAttr()->RegisterLockDo(LOCKING_NONE, LOCKING_EXCLUSIVE);
}

template<class DATA>
inline void CCoreDataAttrBase<DATA>::UnlockSelfDo()
{
	GetLockAttr()->RegisterLockDo(LOCKING_EXCLUSIVE, LOCKING_NONE);
}

// ------- Medium level

template<class DATA>
inline void CCoreDataAttrBase<DATA>::RemoveValueTry(values_iterator pos)
{
	ASSERT( !values.empty() && pos != values.end() );

	if( limited_size(values, 3) == 2 )
		UnlockSelfTry();
}

template<class DATA>
inline void CCoreDataAttrBase<DATA>::RemoveValueCancel(values_iterator pos)
{
	ASSERT( !values.empty() && pos != values.end() );

	if( limited_size(values, 3) == 2 )
		UnlockSelfCancel();
}

template<class DATA>
inline void CCoreDataAttrBase<DATA>::RemoveValueFinish(values_iterator pos)
{
	ASSERT( !values.empty() && pos != values.end() );

	values.erase(pos);
}

template<class DATA>
inline void CCoreDataAttrBase<DATA>::RemoveValue(values_iterator pos)
{
	RemoveValueTry(pos);

	// we do not clean up, since try cleans up if falis
	RemoveValueFinish(pos);
}

template<class DATA>
inline void CCoreDataAttrBase<DATA>::RemoveValueDo(values_iterator pos)
{
	ASSERT( !values.empty() && pos != values.end() );

	// UnlockSelfDo may call Unload back
	// so we must remove the value first

	values.erase(pos);

	if( limited_size(values, 2) == 1 )
		UnlockSelfDo();
}

template<class DATA>
inline void CCoreDataAttrBase<DATA>::SpliceValue(values_iterator before, values_iterator pos)
{
	ASSERT( !values.empty() );

	// nothing throw here
	values.splice(before, values, pos);
}

template<class DATA>
inline void CCoreDataAttrBase<DATA>::ChangeFrontValue(VARIANT &v)
{
	ASSERT( values.size() >= 2 );

	// only users overwrite
	CopyTo(v, values.front());
}

template<>
inline void CCoreDataAttrBase<CComPtr<CCoreDictionaryAttributeValue>>::ChangeFrontValue(VARIANT &v)
{
	ASSERT( values.size() >= 2 );
}


template<class DATA>
DATA CCoreDataAttrBase<DATA>::CreateValue()
{
	return value_type();
}

template<>
CComPtr<CCoreDictionaryAttributeValue> CCoreDataAttrBase<CComPtr<CCoreDictionaryAttributeValue>>::CreateValue()
{
	CCoreDictionaryAttributeValue *val = NULL;
	typedef CComObject< CCoreDictionaryAttributeValue > COMTYPE;
	// FIXME: is this necessary?
	COMTHROW( COMTYPE::CreateInstance((COMTYPE **)&val) );
	return CComPtr<CCoreDictionaryAttributeValue>(val);
}

template<class DATA>
void CCoreDataAttrBase<DATA>::InsertFrontValue(VARIANT &v)
{
	if( limited_size(values, 2) == 1 )
	{
		LockSelfTry();
		values.push_front(CreateValue());
		try
		{
			CopyTo(v, values.front());
		}
		catch(hresult_exception &)
		{
			values.pop_front();
			LockSelfCancel();

			throw;
		}
	}
	else
	{
		values.push_front(CreateValue());
		try
		{
			CopyTo(v, values.front());
		}
		catch(hresult_exception &)
		{
			values.pop_front();
			throw;
		}
	}
}

// ------- Methods

template<class DATA>
inline bool CCoreDataAttrBase<DATA>::DoesMatchBase(const value_type &a, const VARIANT &v)
{
	return false;
}

template<>
inline bool CCoreDataAttrBase<long>::DoesMatchBase(const long &a, const VARIANT &v)
{
	return v.vt == VT_I4 && a == v.lVal;
}

template<>
inline bool CCoreDataAttrBase<CComBstrObj>::DoesMatchBase(const CComBstrObj &a, const VARIANT &v)
{
	return v.vt == VT_BSTR && a == v.bstrVal;
}

// ------- Debug

#ifdef _DEBUG

template<>
void CCoreDataAttrBase<CComBstrObj>::Dump()
{
	ASSERT( metaattribute != NULL );

	CComBstrObj bstr;
	COMTHROW( metaattribute->get_Name(PutOut(bstr)) );

	std::string name;
	CopyTo(bstr, name);

	attrid_type attrid = GetAttrID();

	AtlTrace("attribute_dump attrid=%d name=\"%s\" ", 
		(int)attrid, name.begin());

	if( values.empty() )
		AtlTrace("value=<unloaded> ");
	else
	{
		std::string val;
		CopyTo(values.front(), val);

		AtlTrace("value=\"%s\" ", val.begin());
	}

	if( InTransaction() )
	{
		ICoreStorage *storage = SetStorageThisAttribute();
		ASSERT( storage != NULL );

		CComVariant v;
		COMTHROW( storage->get_AttributeValue(PutOut(v)) );

		std::string val;
		CopyTo(v, val);

		AtlTrace("storage=\"%s\"\n", val.begin());
	}
	else
		AtlTrace("storage=<unknown>\n");
}

template<>
void CCoreDataAttrBase<long>::Dump()
{
	ASSERT( metaattribute != NULL );

	CComBstrObj bstr;
	COMTHROW( metaattribute->get_Name(PutOut(bstr)) );

	std::string name;
	CopyTo(bstr, name);

	attrid_type attrid = GetAttrID();

	AtlTrace("attribute_dump attrid=%d name=\"%s\" ", 
		(int)attrid, name.begin());

	if(values.empty())
		AtlTrace("value=<unloaded> ");
	else
		AtlTrace("value=%ld ", (long)values.front());

	if( InTransaction() )
	{
		ICoreStorage *storage = SetStorageThisAttribute();
		ASSERT( storage != NULL );

		CComVariant v;
		COMTHROW( storage->get_AttributeValue(PutOut(v)) );

		long val;
		CopyTo(v, val);

		AtlTrace("storage=%ld\n", (long)val);
	}
	else
		AtlTrace("storage=<unknown>\n");
}

#endif

// --------------------------- CCorePointerAttrBase

CCorePointerAttrBase::CCorePointerAttrBase() : isEmpty(true)
{
}

#ifdef _DEBUG

CCorePointerAttrBase::~CCorePointerAttrBase()
{
	ASSERT( values.empty() );
	ASSERT( isEmpty );
}


#endif

// ------- CopyTo

void CCorePointerAttrBase::UserCopyTo(CCoreCollectionAttribute* const p, VARIANT *v)
{
	ASSERT( v != NULL && v->vt == VT_EMPTY );

	v->pdispVal = NULL;
	v->vt = VT_DISPATCH;

	if( p != NULL )
	{
		CCoreObject * o = p->GetObject();
		ASSERT( o != NULL );

		CopyTo(o, &v->pdispVal);
	}
}

void CCorePointerAttrBase::UserCopyTo(const VARIANT &v, CComObjPtr<CCoreCollectionAttribute> &p)
{
	if( v.vt == VT_DISPATCH )
	{
		if( v.pdispVal == NULL )
			p = NULL;
		else
		{
			CComObjPtr<ICoreObject> iobject;
			COMTHROW( ::QueryInterface(v.pdispVal, iobject) );
			ASSERT( iobject != NULL );

			CCoreObject *object = GetProject()->CastObject(iobject);
			ASSERT( object != NULL );

			CComObjPtr<CCoreAttribute> attribute = object->FindAttribute(GetAttrID() + ATTRID_COLLECTION);
			ASSERT( attribute != NULL && attribute->GetValType() == VALTYPE_COLLECTION );
			ASSERT( attribute->GetObject() == object );

			p.Attach((CCoreCollectionAttribute*)attribute.Detach());
		}
	}
	else if( v.vt == VT_EMPTY )
		p = NULL;
	else
		HR_THROW(E_INVALIDARG);
}

void CCorePointerAttrBase::StorageCopyTo(CCoreCollectionAttribute* const p, VARIANT *v)
{
	ASSERT( v != NULL && v->vt == VT_EMPTY );

	metaobjidpair_type idpair;

	if( p != NULL )
	{
		CCoreObject *object = p->GetObject();
		ASSERT( object != NULL );

		idpair.objid = object->GetObjID();
		idpair.metaid = object->GetMetaID();
	}
	else
	{
		idpair.objid = OBJID_NONE;
		idpair.metaid = METAID_NONE;
	}

	CopyTo(idpair, v);
}

void CCorePointerAttrBase::StorageCopyTo(const VARIANT &v, CComObjPtr<CCoreCollectionAttribute> &p)
{
	metaobjidpair_type idpair;
	CopyTo(v, idpair);

	if( idpair.metaid != METAID_NONE )
	{
		ASSERT( idpair.objid != OBJID_NONE );

#pragma warning( disable: 4244) // conversion from 'long' to 'short', possible loss of data
		CComObjPtr<CCoreObject> object = GetProject()->GetObject(idpair.metaid, idpair.objid);
#pragma warning( default: 4244) // conversion from 'long' to 'short', possible loss of data
		ASSERT( object != NULL );

		CComObjPtr<CCoreAttribute> attribute = object->FindAttribute(GetAttrID() + ATTRID_COLLECTION);
		ASSERT( attribute != NULL && attribute->GetValType() == VALTYPE_COLLECTION );
		ASSERT( attribute->GetObject() == object );

		p.Attach((CCoreCollectionAttribute*)attribute.Detach());
	}
	else
	{
		ASSERT( idpair.objid == OBJID_NONE );

		p = NULL;
	}
}

// ------- Low level

void CCorePointerAttrBase::InsertIntoCollection()
{
	ASSERT( isEmpty );
	ASSERT( !values.empty() );

	if( values.front() != NULL )
	{
		CCoreCollectionAttribute::objects_type &collection = values.front()->GetCollection();

		std::pair<objects_iterator, bool> t = collection.insert(GetObject());
		ASSERT( t.second );
		backref = t.first;
		isEmpty = false;
	}
}

void CCorePointerAttrBase::RemoveFromCollection()
{
	ASSERT( !values.empty() );

	if( !isEmpty )
	{
		ASSERT( values.front() != NULL );

		objects_type &collection = values.front()->GetCollection();
		ASSERT( IsValidIterator(collection, backref) );

		collection.erase( backref );
		// KMS: assigning a singular iterator is undefined. I saw a crash when registering MetaGME
		// backref = objects_iterator(); // was: backref = NULL;
		isEmpty = true;
	}
	else
		ASSERT( values.front() == NULL );
}

void CCorePointerAttrBase::CollectionNotUpToDate()
{
	ASSERT( values.size() == 1 );

	if( values.front() != NULL )
		values.front()->NotUpToDate();
}

inline void CCorePointerAttrBase::LockCollectionTry(CCoreCollectionAttribute *p)
{
	if( p != NULL )
		p->GetLockAttr()->RegisterLockTry(LOCKING_NONE, LOCKING_WRITE);
}

inline void CCorePointerAttrBase::LockCollectionCancel(CCoreCollectionAttribute *p)
{
	if( p != NULL )
		p->GetLockAttr()->RegisterLockDo(LOCKING_WRITE, LOCKING_NONE);
}

inline void CCorePointerAttrBase::UnlockCollectionTry(CCoreCollectionAttribute *p)
{
	if( p != NULL )
		p->GetLockAttr()->RegisterLockTry(LOCKING_WRITE, LOCKING_NONE);
}

inline void CCorePointerAttrBase::UnlockCollectionCancel(CCoreCollectionAttribute *p)
{
	if( p != NULL )
		p->GetLockAttr()->RegisterLockDo(LOCKING_NONE, LOCKING_WRITE);
}

inline void CCorePointerAttrBase::UnlockCollectionDo(CCoreCollectionAttribute *p)
{
	if( p != NULL )
		p->GetLockAttr()->RegisterLockDo(LOCKING_WRITE, LOCKING_NONE);
}

inline void CCorePointerAttrBase::LockSelfTry()
{
	GetLockAttr()->RegisterLockTry(LOCKING_NONE, LOCKING_EXCLUSIVE);
}

inline void CCorePointerAttrBase::LockSelfCancel()
{
	GetLockAttr()->RegisterLockDo(LOCKING_EXCLUSIVE, LOCKING_NONE);
}

inline void CCorePointerAttrBase::UnlockSelfTry()
{
	GetLockAttr()->RegisterLockTry(LOCKING_EXCLUSIVE, LOCKING_NONE);
}

inline void CCorePointerAttrBase::UnlockSelfCancel()
{
	GetLockAttr()->RegisterLockDo(LOCKING_NONE, LOCKING_EXCLUSIVE);
}

inline void CCorePointerAttrBase::UnlockSelfDo()
{
	GetLockAttr()->RegisterLockDo(LOCKING_EXCLUSIVE, LOCKING_NONE);
}

// ------- Medium level

void CCorePointerAttrBase::RemoveValueTry(values_iterator pos)
{
	ASSERT( !values.empty() && pos != values.end() );

	switch( limited_size(values, 3) )
	{
	case 3:
		ASSERT( values.size() >= 3 );

		UnlockCollectionTry(*pos);

		break;

	case 2:
		ASSERT( values.size() == 2 );

		UnlockSelfTry();
		try
		{
			UnlockCollectionTry(values.front());
			try
			{
				UnlockCollectionTry(values.back());
			}
			catch(hresult_exception &)
			{
				UnlockCollectionCancel(values.front());
				throw;
			}
		}
		catch(hresult_exception &)
		{
			UnlockSelfCancel();
			throw;
		}

		break;
	}
}

void CCorePointerAttrBase::RemoveValueCancel(values_iterator pos)
{
	ASSERT( !values.empty() && pos != values.end() );

	switch( limited_size(values, 3) )
	{
	case 3:
		ASSERT( values.size() >= 3 );

		UnlockCollectionCancel(*pos);

		break;

	case 2:
		ASSERT( values.size() == 2 );

		// in reverse order
		UnlockCollectionCancel(values.back());
		UnlockCollectionCancel(values.front());
		UnlockSelfCancel();

		break;
	}
}

void CCorePointerAttrBase::RemoveValueFinish(values_iterator pos)
{
	ASSERT( !values.empty() && pos != values.end() );

	if( pos == values.begin() )
	{
		RemoveFromCollection();

		if( limited_size(values, 2) == 1 )
			CollectionNotUpToDate();

		values.erase(pos);

		if( !values.empty() )
			InsertIntoCollection();
	}
	else
		values.erase(pos);
}

void CCorePointerAttrBase::RemoveValue(values_iterator pos)
{
	RemoveValueTry(pos);

	// we do not clean up, since try cleans up if falis
	RemoveValueFinish(pos);
}

void CCorePointerAttrBase::RemoveValueDo(values_iterator pos)
{
	ASSERT( !values.empty() && pos != values.end() );

	// RemoveValueDo may call Unload back
	// so we have to remove the value first

	switch( limited_size(values, 3) )
	{
	case 3:
		ASSERT( values.size() >= 3 );

		UnlockCollectionDo(*pos);

		break;

	case 2:
		ASSERT( values.size() == 2 );

		UnlockCollectionDo(values.front());
		UnlockCollectionDo(values.back());

		break;
	}

	if( pos == values.begin() )
	{
		RemoveFromCollection();

		if( limited_size(values, 2) == 1 )
			CollectionNotUpToDate();

		values.erase(pos);

		if( !values.empty() )
			InsertIntoCollection();
	}
	else
		values.erase(pos);

	if( limited_size(values, 2) == 1 )
		UnlockSelfDo();
}

void CCorePointerAttrBase::SpliceValue(values_iterator before, values_iterator pos)
{
	ASSERT( !values.empty() );

	// nothing throw here
	RemoveFromCollection();
	values.splice(before, values, pos);
	InsertIntoCollection();
}

void CCorePointerAttrBase::ChangeFrontValue(VARIANT &v)
{
	ASSERT( values.size() >= 2 );

	// templorary, possible exception
	CComObjPtr<CCoreCollectionAttribute> collection;

	// only users overwrite
	UserCopyTo(v, collection);

	LockCollectionTry(collection);
	try
	{
		UnlockCollectionTry(values.front());
	}
	catch(hresult_exception &)
	{
		LockCollectionCancel(collection);
		throw;
	}

	// nothing throw here
	RemoveFromCollection();
	values.front() = collection;
	InsertIntoCollection();
}

void CCorePointerAttrBase::InsertFrontValue(VARIANT &v)
{
	// templorary, possible exception
	CComObjPtr<CCoreCollectionAttribute> collection;

	if( values.empty() )
	{
		StorageCopyTo(v, collection);

		// nothing throw here
		values.push_front(collection);
		InsertIntoCollection();
	}
	else if( limited_size(values, 2) == 1 )
	{
		ASSERT( values.size() == 1 );

		UserCopyTo(v, collection);

		LockSelfTry();
		try
		{
			LockCollectionTry(collection);
			try
			{
				LockCollectionTry(values.front());
			}
			catch(hresult_exception &)
			{
				LockCollectionCancel(collection);
				throw;
			}
		}
		catch(hresult_exception &)
		{
			LockSelfCancel();
			throw;
		}

		RemoveFromCollection();
		values.push_front(collection);
		InsertIntoCollection();
	}
	else
	{
		ASSERT( values.size() >= 2 );
		
		UserCopyTo(v, collection);
		LockCollectionTry(collection);

		RemoveFromCollection();
		values.push_front(collection);
		InsertIntoCollection();
	}
}

// ------- Methods

inline bool CCorePointerAttrBase::DoesMatchBase(const value_type &a, const VARIANT &v)
{
	if( v.vt != VT_DISPATCH && v.vt != VT_UNKNOWN )
		return false;

	if( a == NULL )
		return v.punkVal == NULL;

	return IsEqualObject(CastToUnknown(a->GetObject()), v.punkVal);
}

// --------------------------- CCoreDataAttribute

// ------- Methods

template<class BASE, const int VALTYPE>
STDMETHODIMP CCoreDataAttribute<BASE, VALTYPE>::get_Value(VARIANT *p)
{
	CHECK_OUT(p);
	CHECK_ZOMBIE();

	COMTRY
	{
		if( InTransaction() )
		{
			if( !IsLoaded() )
				GetLockAttr()->Load();

            // BGY: this hack makes sure that the sourcecontrol status is always
            // read from the storage.
			if( GetValType() == VALTYPE_LONG)
			{
				ICoreStorage *storage = SetStorageThisAttribute();
				ASSERT( storage != NULL );
				COMTHROW( storage->LockObject() );

				attrid_type a_id;
				COMTHROW( storage->get_AttrID( &a_id));
				if( a_id == ATTRID_FILESTATUS)
				{
					COMTHROW( storage->get_AttributeValue( p));
					return S_OK;
				}
			}
        }
		else
		{
//			if( GetTerritory()->GetLocking(GetLockAttr()) == LOCKING_NONE )
//				HR_THROW(E_NOTLOCKED);

			if( !IsLoaded() )
				HR_THROW(E_NOTLOCKED);
		}

		ASSERT( !values.empty() );
		UserCopyTo(values.front(), p);
	}
	COMCATCH(;)
}

template<class BASE, const int VALTYPE>
STDMETHODIMP CCoreDataAttribute<BASE, VALTYPE>::put_Value(VARIANT p)
{
	CHECK_ZOMBIE();

	if( !InWriteTransaction() )
		COMRETURN(E_TRANSACTION);

	COMTRY
	{
		if( !IsLoaded() )
			GetLockAttr()->Load();

		ASSERT( !values.empty() );

		if( !IsDirty() )
		{
			InsertFrontValue(p);

			// nothing will throw here

			GetObject()->RegisterFinalTrItem();
			GetProject()->RegisterTransactionItem(this);
			SetDirty();
		}
		else
			ChangeFrontValue(p);
	}
	COMCATCH(;)
}

template<class BASE, const int VALTYPE>
STDMETHODIMP CCoreDataAttribute<BASE, VALTYPE>::get_LoadedValue(VARIANT *p)
{
	CHECK_OUT(p);
	CHECK_ZOMBIE();

	COMTRY
	{
		if( values.empty() )
			HR_THROW(E_NOTLOCKED);

		UserCopyTo(values.front(), p);
	}
	COMCATCH(;)
}

template<class BASE, const int VALTYPE>
STDMETHODIMP CCoreDataAttribute<BASE, VALTYPE>::get_PreviousValue(VARIANT *p)
{
	CHECK_OUT(p);
	CHECK_ZOMBIE();

	COMTRY
	{
		if (GetLockAttr()->IsLoaded() == false)
			HR_THROW(E_NOTLOCKED);

		if( IsDirty() )
		{
			// lph: Return last committed value.  Previously returned last set value (commented below).
			ICoreStorage *storage = SetStorageThisAttribute();
			ASSERT( storage != NULL );
			COMTHROW(storage->get_AttributeValue(p));

			//ASSERT( values.size() >= 2 );
			//UserCopyTo(*(++values.begin()), p);
		}
		else
		{
			ASSERT( !values.empty() );
			UserCopyTo(values.front(), p);
		}
	}
	COMCATCH(;)
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::Load()
{
	ASSERT( !IsZombie() );
	ASSERT( values.empty() );

	ICoreStorage *storage = SetStorageThisAttribute();
	ASSERT( storage != NULL );

	CComVariant v;
	COMTHROW( storage->get_AttributeValue(PutOut(v)) );

	InsertFrontValue(v);
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::Unload()
{
	ASSERT( values.size() == 1 );
	ASSERT( !IsDirty() );

	RemoveValue(values.begin());

	ASSERT( values.empty() );
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::Save(value_type &value)
{
	ASSERT( !IsZombie() );

	ICoreStorage *storage = SetStorageThisAttribute();
	ASSERT( storage != NULL );

	CComVariant v;
	StorageCopyTo(value, &v);

	COMTHROW( storage->put_AttributeValue(v) );
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::FillAfterCreateObject()
{
	ASSERT( values.empty() );

	CComVariant v;
	InsertFrontValue(v);

	ASSERT( values.size() == 1 );
}

// ------- NestedTrItem

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::AbortNestedTransaction()
{
	ASSERT( IsDirty() );
	ASSERT( values.size() >= 2 );

	RemoveValueDo(values.begin());

	ResetDirty();
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::DiscardPreviousValue()
{
	ASSERT( IsDirty() );
	ASSERT( values.size() >= 3 );

	RemoveValueDo(++values.begin());
}

// ------- FinalTrItem

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::AbortFinalTransaction()
{
	ASSERT( IsDirty() );
	ASSERT( values.size() >= 2 );

	// RemoveValueDo may call Unload back
	// so we have to reset dirty first

	ResetDirty();

	RemoveValueDo(values.begin());
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::CommitFinalTransaction()
{
	ASSERT( IsDirty() );
	ASSERT( values.size() >= 2 );

	Save( values.front() );
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::CommitFinalTransactionFinish(bool undo)
{
	ASSERT( IsDirty() );
	ASSERT( values.size() >= 2 );

	GetProject()->RegisterUndoItem(this);

	ResetDirty();
}

// ------- UndoItem

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::UndoTransaction()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	Save( *(++values.begin()) );
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::UndoTransactionFinish()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	SpliceValue(values.end(), values.begin());
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::RedoTransaction()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	Save( values.back() );
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::RedoTransactionFinish()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	SpliceValue(values.begin(), --values.end());
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::DiscardLastItem()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	RemoveValueTry(--values.end());
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::DiscardLastItemFinish()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	RemoveValueFinish(--values.end());
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::DiscardLastItemCancel()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	RemoveValueCancel(--values.end());
}

// ------- DoesMatch

template<class BASE, const int VALTYPE>
bool CCoreDataAttribute<BASE, VALTYPE>::DoesMatch(bool do_load, const VARIANT &v)
{
	if( IsLoaded() )
		return DoesMatchBase(values.front(), v);
	else if( do_load )
	{
		GetLockAttr()->Load();
		ASSERT( IsLoaded() );

		return DoesMatchBase(values.front(), v);
	}

	return false;
}

// --------------------------- CCoreCollectionAttribute

#ifdef _DEBUG

CCoreCollectionAttribute::~CCoreCollectionAttribute()
{
	ASSERT( collection.empty() );
}

#endif

STDMETHODIMP CCoreCollectionAttribute::get_Value(VARIANT *p)
{
	CHECK_OUT(p);
	CHECK_ZOMBIE();

	COMTRY
	{
		if( InTransaction() )
		{
			if (GetLockAttr()->IsLoaded() &&
				GetStatusFlag(COREATTRIBUTE_COLL_UPTODATE) )
			{
				CopyCollectionFromMemory(*p);
			}
			else
			{
				if (GetLockAttr()->IsLoaded() == false)
					GetLockAttr()->Load();

				CopyCollectionFromStorage(*p);
			}
		}
		else
			CopyCollectionFromMemory(*p);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreCollectionAttribute::put_Value(VARIANT p)
{
	COMRETURN(E_INVALID_USAGE);
}

STDMETHODIMP CCoreCollectionAttribute::get_LoadedValue(VARIANT *p)
{
	CHECK_OUT(p);
	CHECK_ZOMBIE();

	COMTRY
	{
		CopyCollectionFromMemory(*p);
	}
	COMCATCH(;)
}

void CCoreCollectionAttribute::CopyCollectionFromMemory(VARIANT &v)
{
	ASSERT( v.vt == VT_EMPTY );

	typedef CCoreCollection<TYPENAME_ELEM2COLL(ICoreObject), std::vector<ICoreObject*>, ICoreObject, CCoreObject> COMTYPE;

	CComObjPtr<COMTYPE> p;
	CreateComObject(p);

	p->FillAll(collection);

	v.pdispVal = p.Detach();
	v.vt = VT_DISPATCH;
}

void CCoreCollectionAttribute::CopyCollectionFromStorage(VARIANT &v)
{
	ASSERT( v.vt == VT_EMPTY );

	ICoreStorage *storage = SetStorageThisAttribute();
	ASSERT( storage != NULL );

	CComVariant a;
	COMTHROW( storage->get_AttributeValue(PutOut(a)) );

	CCoreProject *project = GetProject();
	ASSERT( project != NULL );

	attrid_type pointer_attrid = GetAttrID() - ATTRID_COLLECTION;
	ASSERT( pointer_attrid != ATTRID_NONE && pointer_attrid != - ATTRID_COLLECTION );

	metaobjidpair_type *i;
	metaobjidpair_type *e;
	GetArrayBounds(a, i, e);

	typedef CCoreCollection<ICoreObjects, std::vector<ICoreObject*>, ICoreObject, CCoreObject> COMTYPE;

	CComObjPtr<COMTYPE> p;
	CreateComObject(p);

	SetStatusFlag(COREATTRIBUTE_COLL_UPTODATE);

	while( i != e )
	{
		// *** later every loaded object will be locked

#pragma warning( disable: 4244) // conversion from 'long' to 'short', possible loss of data
		CComObjPtr<CCoreObject> object = project->FindObject((*i).metaid, (*i).objid);
#pragma warning( default: 4244) // conversion from 'long' to 'short', possible loss of data
		if( object == NULL )
		{
#pragma warning( disable: 4244) // conversion from 'long' to 'short', possible loss of data
			object = project->CreateObject((*i).metaid, (*i).objid);
#pragma warning( default: 4244) // conversion from 'long' to 'short', possible loss of data
			ASSERT( object != NULL );
		}

		// By creating an object the value of the collection could be modified.
		// Aggregated objects can make the object loaded, and thus the loaded
		// pointers can point to this object.

		CCoreAttribute *attribute = object->FindAttribute(pointer_attrid);
		ASSERT( attribute != NULL );

		if( !((CCorePointerAttrBase*)attribute)->IsLoaded() )
		{
			ASSERT( collection.find(object) == collection.end() );
			p->Add(object);

			ResetStatusFlag(COREATTRIBUTE_COLL_UPTODATE);
		}

		// If the attribute is loaded then the object is taken into account by the collection. 
		// Actually, the pointer can point to somewhere else, or can be NULL.

		++i;
	}

	p->FillAll(collection);

	v.pdispVal = p.Detach();
	v.vt = VT_DISPATCH;
}

bool CCoreCollectionAttribute::IsEmptyFromStorage()
{
	if( !collection.empty() )
		return false;

	if( GetStatusFlag(COREATTRIBUTE_COLL_UPTODATE) )
		return true;

	ICoreStorage *storage = SetStorageThisAttribute();
	ASSERT( storage != NULL );

	CComVariant a;
	COMTHROW( storage->get_AttributeValue(PutOut(a)) );

	CCoreProject *project = GetProject();
	ASSERT( project != NULL );

	metaobjidpair_type *i;
	metaobjidpair_type *e;
	GetArrayBounds(a, i, e);

	while( i != e )
	{
#pragma warning( disable: 4244) // conversion from 'long' to 'short', possible loss of data
		CComObjPtr<CCoreObject> object = project->FindObject((*i).metaid, (*i).objid);
#pragma warning( default: 4244) // conversion from 'long' to 'short', possible loss of data
		if( object == NULL )
			return false;

		++i;
	}

	return true;
}

