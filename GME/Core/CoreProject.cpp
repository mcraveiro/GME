
#include "stdafx.h"

#include "CoreProject.h"
#include "CoreObject.h"
#include "CoreTransactionItem.h"
#include "CoreTerritory.h"
#include "CoreBinFile.h"
#include "CoreMetaProject.h"
//#include "XmlBackEnd.h"

// --------------------------- CCoreProject

CCoreProject::CCoreProject()
	: m_maxUndoStep( MAX_UNDO_STEP) // = 10, keep value in sync with CMgaProject::getMaxUndoSize()'s default
{
	status = 0;
	redo_count = 0;
}

CCoreProject::~CCoreProject()
{
	// try to clean up
	put_Storage(NULL);

	ASSERT( transactions.empty() );
	ASSERT( finaltr_items.empty() );
	ASSERT( object_lookup.empty() );
	ASSERT( undos.empty() );
	ASSERT( pushed_territorys.empty() );
	ASSERT( created_territorys.empty() );
}

// ------- COM methods

STDMETHODIMP CCoreProject::put_Storage(ICoreStorage *p)
{
	COMTRY
	{
		if( storage )
		{
			// to prevent being called twice
			CComObjPtr<CCoreProject> self(this);

			while( InTransaction() )
				AbortTransaction(TRANSTYPE_ANY);

			ASSERT( pushed_territorys.empty() );

			if( !created_territorys.empty() )
			{
				BeginFinalTr();
				try
				{
					created_territorys_type::iterator i = created_territorys.begin();
					created_territorys_type::iterator e = created_territorys.end();
					while( i != e )
					{
						CCoreTerritory *territory = *i;
						ASSERT( territory != NULL );

						++i;

						COMTHROW( territory->Clear() );
					}
					
					CommitFinalTr(false);
				}
				catch(hresult_exception &)
				{
					AbortFinalTr();

					throw;
				}
			}
			ASSERT( !InTransaction() );

			while( !undos.empty() )
				TryDiscardLastItem();

#ifdef DEBUG_OBJECTLOOKUP
			if( !object_lookup.empty() )
			{
				AtlTrace("object_lookup begin\n");
				object_lookup_iterator i = object_lookup.begin();
				object_lookup_iterator e = object_lookup.end();
				while( i != e )
				{
					(*i).second->Dump();

					++i;
				}
				AtlTrace("object_lookup end\n");
			}
#endif

			object_lookup_iterator i = object_lookup.begin();
			object_lookup_iterator e = object_lookup.end();
			while( i != e )
			{
#ifdef _ATL_DEBUG_INTERFACES
				CComPtr<IUnknown> obj1 = (IDispatchImpl<ICoreObject, &__uuidof(ICoreObject), &__uuidof(__MGACoreLib)>*)i->second;
				CComPtr<IUnknown> obj2 = (IDispatchImpl<ICoreObject, &__uuidof(ICoreObject), &__uuidof(__MGACoreLib)>*)i->second;
#endif
				(*i).second->SetZombie();

				++i;
			}
			object_lookup.clear();
			
			ASSERT( transactions.empty() );
			ASSERT( finaltr_items.empty() );
			ASSERT( object_lookup.empty() );
			ASSERT( undos.empty() );
			ASSERT( pushed_territorys.empty() );
			ASSERT( status == 0 );
			ASSERT( redo_count == 0 );

			metaproject = NULL;
		}

		storage = p;

		if( storage )
		{
			CComObjPtr<ICoreMetaProject> p;
			COMTHROW( storage->get_MetaProject(PutOut(p)) );
			ASSERT( p != NULL );

			// HACK: we should check that the apartments are the same
			metaproject = CastToMetaProject(p);
		}
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::get_Storage(ICoreStorage **p)
{
	CHECK_OUT(p);

	CopyTo(storage, p);
	return S_OK;
}

STDMETHODIMP CCoreProject::get_MetaProject(ICoreMetaProject **p)
{
	CHECK_OUT(p);

	if( storage == NULL )
		COMRETURN(E_INVALID_USAGE);

	CopyTo(metaproject, p);
	return S_OK;
}

std::string CCoreProject::GetFirstToken(BSTR connection)
{
	std::string con;
	CopyTo(connection, con);

	size_t pos = con.find('=');
	if( pos != std::string::npos )
		con.resize(pos);

	pos = con.find(';');
	if( pos != std::string::npos )
		con.resize(pos);

	return con;
}

STDMETHODIMP CCoreProject::OpenProject2(BSTR connection, long p_undoSize, ICoreMetaProject *p, VARIANT_BOOL *ro_mode)
{
	setMaxUndoStep( p_undoSize);
	return OpenProject( connection, p, ro_mode);
}

STDMETHODIMP CCoreProject::OpenProject(BSTR connection, ICoreMetaProject *p, VARIANT_BOOL *ro_mode)
{
	if( p == NULL || connection == NULL )
		return E_INVALIDARG;

	if( storage != NULL || metaproject != NULL )
		COMRETURN(E_INVALID_USAGE);

	COMTRY
	{
		std::string con = GetFirstToken(connection);

		CComObjPtr<ICoreStorage> corestorage;

		if( con == "MGA" )
		{
			CComObjPtr<CCoreBinFile> binfile;
			CreateComObject(binfile);

			corestorage = binfile;
		}
        else if( con == "MGX" )
		{
            CComObjPtr<ICoreStorage> xmlfile;
            xmlfile.CoCreateInstance( L"Mga.CoreXmlFile" );
			
			corestorage = xmlfile;
		}
		else
			COMTHROW(E_UNKNOWN_STORAGE);

		ASSERT( corestorage != NULL );

		COMTHROW( corestorage->put_MetaProject(p) );
		corestorage->__OpenProject(connection, ro_mode);

		COMTHROW( put_Storage(corestorage) );
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::CloseProject(VARIANT_BOOL abort)
{

	if( storage == NULL && metaproject == NULL ) return S_OK;
	if( storage == NULL || metaproject == NULL )
		COMRETURN(E_INVALID_USAGE);

	COMTRY
	{
		CComObjPtr<ICoreStorage> corestorage;
		corestorage = storage;

		COMTHROW( put_Storage(NULL) );
		COMTHROW( corestorage->CloseProject(abort) );
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::SaveProject(BSTR newname, VARIANT_BOOL keepoldname)
{
	if( storage == NULL || metaproject == NULL )
		COMRETURN(E_INVALID_USAGE);

	COMTRY
	{
		ICoreStoragePtr corestorage = storage.p;

		corestorage->__SaveProject(newname, keepoldname);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::CreateProject2(BSTR connection, long p_undoSize, ICoreMetaProject *p)
{
	setMaxUndoStep( p_undoSize);
	return CreateProject( connection, p);
}

STDMETHODIMP CCoreProject::CreateProject(BSTR connection, ICoreMetaProject *p)
{
	if( p == NULL || connection == NULL )
		return E_INVALIDARG;

	if( storage != NULL || metaproject != NULL )
		COMRETURN(E_INVALID_USAGE);

	COMTRY
	{
		std::string con = GetFirstToken(connection);

		CComObjPtr<ICoreStorage> corestorage;

		if( con == "MGA" )
		{
			CComObjPtr<CCoreBinFile> binfile;
			CreateComObject(binfile);

			corestorage = binfile;
		}
        else if( con == "MGX" )
		{
			CComObjPtr<ICoreStorage> xmlfile;
            COMTHROW(xmlfile.CoCreateInstance(L"Mga.CoreXmlFile"));
			
			corestorage = xmlfile;
		}
		else
			COMTHROW(E_UNKNOWN_STORAGE);

		ASSERT( corestorage != NULL );

		COMTHROW( corestorage->put_MetaProject(p) );
		COMTHROW( corestorage->CreateProject(connection) );

		COMTHROW( put_Storage(corestorage) );
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::DeleteProject(BSTR connection)
{
	return E_NOTIMPL;
}

STDMETHODIMP CCoreProject::get_RootObject(ICoreObject **p)
{
	return get_Object(METAID_ROOT, OBJID_ROOT, p);
}

STDMETHODIMP CCoreProject::get_Object(metaid_type metaid, objid_type objid, ICoreObject **p)
{
	CHECK_OUT(p);

	if( metaid == 0 || objid == 0 )
		return E_INVALIDARG;

	if( storage == NULL )
		COMRETURN(E_INVALID_USAGE);

	if( !InTransaction() )
		COMRETURN(E_TRANSACTION);

	COMTRY
	{
		CComObjPtr<CCoreObject> object = GetObject(metaid, objid);
		ASSERT( object != NULL );

		MoveTo(object, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::CreateObject(metaid_type metaid, ICoreObject **p)
{
	CHECK_OUT(p);

	if( metaid == METAID_NONE )
		return E_INVALIDARG;

	if( storage == NULL )
		COMRETURN(E_INVALID_USAGE);

	if( !InWriteTransaction() )
		COMRETURN(E_TRANSACTION);

	COMTRY
	{
		CComObjPtr<CCoreObject> object(CreateObject(metaid));
		ASSERT( object != NULL );

		CopyTo(object, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::BeginTransaction(transtype_enum transtype)
{
	if (InReadTransaction() && (transtype & TRANSTYPE_READ) == 0)
	{
		SetErrorInfo(L"Project is in read-only transaction and cannot be modified");
		return E_INVALID_USAGE;
	}
	if (storage == NULL || (transtype & TRANSTYPE_ANY) == 0 ||
		(InTransaction() && (transtype & TRANSTYPE_NESTED) == 0))
	{
		COMRETURN(E_INVALID_USAGE);
	}

	COMTRY
	{
		if( InTransaction() )
		{
			// note that in a read-transaction we might have redo_count > 0

			ASSERT( undos.size() <= m_maxUndoStep + 1);
			ASSERT( undos.size() <= m_maxUndoStep || (transtype & TRANSTYPE_READ) != 0 );

			BeginNestedTr();
		}
		else
		{
			// if this is a read/write transaction then flush the redo queue
			if( (transtype & TRANSTYPE_READ) == 0 )
			{
				while( redo_count > 0 )
					TryDiscardLastItem();

				ASSERT( m_maxUndoStep); // m_maxUndoStep better not be 0 (chance of infinite loop)
				while( undos.size() >= m_maxUndoStep )
					TryDiscardLastItem();

				ASSERT( redo_count == 0 );
			}

			BeginFinalTr();
		}

		// we set the transaction type
		transactions.front().readonly = ((transtype & TRANSTYPE_READ) != 0);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::CommitTransaction(transtype_enum transtype)
{
	if( storage == NULL || (transtype & TRANSTYPE_ANY) == 0 || !InTransaction() ||
		(limited_size(transactions, 2) == 1 && (transtype & TRANSTYPE_FIRST) == 0) ||
		(transactions.front().readonly != ((transtype & TRANSTYPE_READ) != 0)) )
	{
		COMRETURN(E_INVALID_USAGE);
	}

	COMTRY
	{
		if( limited_size(transactions, 2) >= 2 )
		{
			CommitNestedTr();
		}
		else
		{
			CommitFinalTr(true);
		}
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::AbortTransaction(transtype_enum transtype)
{
	if( storage == NULL || (transtype & TRANSTYPE_ANY) == 0 || !InTransaction() ||
		(limited_size(transactions, 2) == 1 && (transtype & TRANSTYPE_FIRST) == 0) )
	{
		COMRETURN(E_INVALID_USAGE);
	}

	COMTRY
	{
		if( limited_size(transactions, 2) >= 2 )
		{
			AbortNestedTr();
		}
		else
		{
			AbortFinalTr();
		}
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::get_NestedTransactionCount(short *p)
{
	CHECK_OUT(p);

	*p = GetNestedTrCount();

	return S_OK;
}

// ------- Undo

STDMETHODIMP CCoreProject::UndoTransaction()
{
	ASSERT( (int) undos.size() >= redo_count && redo_count >= 0 );

	if( storage == NULL || InTransaction() || (int) undos.size() <= redo_count )
		COMRETURN(E_INVALID_USAGE);

	COMTRY
	{
		TryUndoTransaction();
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::RedoTransaction()
{
	ASSERT( (int) undos.size() >= redo_count && redo_count >= 0 );

	if( storage == NULL || InTransaction() || redo_count <= 0 )
		COMRETURN(E_INVALID_USAGE);

	COMTRY
	{
		TryRedoTransaction();
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::FlushUndoQueue()
{
	ASSERT( (int) undos.size() >= redo_count && redo_count >= 0 );

	if( storage == NULL || InTransaction() )
		COMRETURN(E_INVALID_USAGE);

	COMTRY
	{
		while( !undos.empty() )
			TryDiscardLastItem();
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::FlushRedoQueue()
{
	ASSERT( (int) undos.size() >= redo_count && redo_count >= 0 );

	if( storage == NULL || InTransaction() )
		COMRETURN(E_INVALID_USAGE);

	COMTRY
	{
		while( redo_count > 0 )
			TryDiscardLastItem();
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::get_UndoQueueSize(short *p)
{
	CHECK_OUT(p);

	ASSERT( (int) undos.size() >= redo_count );
	*p = undos.size() - redo_count - (InTransaction() ? 1 : 0);
	ASSERT( *p >= 0 );

	return S_OK;
}

STDMETHODIMP CCoreProject::get_RedoQueueSize(short *p)
{
	CHECK_OUT(p);

	ASSERT( (int) undos.size() >= redo_count );
	*p = redo_count;

	return S_OK;
}

STDMETHODIMP CCoreProject::CreateTerritory(ICoreTerritory **p)
{
	CHECK_OUT(p);

	if( storage == NULL )
		COMRETURN(E_INVALID_USAGE);

	COMTRY
	{
		CComObjPtr<CCoreTerritory> territory;
		
		territory = CCoreTerritory::Create(this);
		ASSERT( territory != NULL );

		CopyTo(territory, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::PushTerritory(ICoreTerritory *p)
{
	if( p == NULL )
		return E_INVALIDARG;

	if( storage == NULL || !InTransaction() )
		COMRETURN(E_INVALID_USAGE);

	COMTRY
	{
		CCoreTerritory *territory = CastTerritory(p);
		pushed_territorys.push_front( territory );
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreProject::PopTerritory()
{
	if( storage == NULL || pushed_territorys.empty() || !InTransaction() )
		COMRETURN(E_INVALID_USAGE);

	ASSERT( pushed_territorys.front() != NULL );
	pushed_territorys.pop_front();

	return S_OK;
}

// ------- Methods

CCoreObject *CCoreProject::FindObject(metaid_type metaid, objid_type objid)
{
	ASSERT( storage != NULL );
	ASSERT( metaid != METAID_NONE && objid != OBJID_NONE );

	metaobjidpair_type idpair;
	idpair.metaid = metaid;
	idpair.objid = objid;

	object_lookup_iterator i = object_lookup.find(idpair);
	if( i != object_lookup.end() )
	{
		ASSERT( (*i).second != NULL );
		return (*i).second;
	}

	return NULL;
}

CComObjPtr<CCoreObject> CCoreProject::CreateObject(metaid_type metaid, objid_type objid)
{
	ASSERT( storage != NULL );
	ASSERT( metaid != METAID_NONE && objid != OBJID_NONE );
	ASSERT( InTransaction() );

#ifdef _DEBUG
	metaobjidpair_type idpair;
	idpair.metaid = metaid;
	idpair.objid = objid;

	ASSERT( object_lookup.find(idpair) == object_lookup.end() );
#endif

	return CCoreObject::Create(this, metaid, objid);
}

CComObjPtr<CCoreObject> CCoreProject::GetObject(metaid_type metaid, objid_type objid)
{
	ASSERT( storage != NULL );
	ASSERT( metaid != METAID_NONE && objid != OBJID_NONE );

	metaobjidpair_type idpair;
	idpair.metaid = metaid;
	idpair.objid = objid;

	object_lookup_iterator i = object_lookup.find(idpair);
	if( i != object_lookup.end() )
	{
		ASSERT( (*i).second != NULL );
		return (*i).second;
	}

	if( !InTransaction() )
		HR_THROW(E_TRANSACTION);

	return CCoreObject::Create(this, metaid, objid);
}

CComObjPtr<CCoreObject> CCoreProject::CreateObject(metaid_type metaid)
{
	ASSERT( storage != NULL );
	ASSERT( metaid != METAID_NONE );
	ASSERT( InWriteTransaction() );

	COMTHROW( storage->put_MetaID(metaid) );

	objid_type objid = OBJID_NONE;
	COMTHROW( storage->CreateObject(&objid) );
	ASSERT( objid != OBJID_NONE );

#ifdef _DEBUG
	metaobjidpair_type idpair;
	idpair.metaid = metaid;
	idpair.objid = objid;
	ASSERT( object_lookup.find(idpair) == object_lookup.end() );
#endif

	CComObjPtr<CCoreObject> object(CCoreObject::Create(this, metaid, objid));

	object->FillAfterCreateObject();

	return object;
}

void CCoreProject::RegisterObject(metaid_type metaid, objid_type objid, CCoreObject *object)
{
	ASSERT( storage != NULL );
	ASSERT( metaid != 0 && objid != 0 );
	ASSERT( object != NULL );

	metaobjidpair_type idpair;
	idpair.metaid = metaid;
	idpair.objid = objid;

	ASSERT( object_lookup.find(idpair) == object_lookup.end() );
	auto old = object_lookup.insert( object_lookup_type::value_type(idpair, object) );
	if (old.second != true) {
		// this can happen if the user calls MgaProject::GetObjectByID with and ID that doesn't exist, and then we create an object with that ID
		COMTHROW(E_INVALIDARG);
	}
}

void CCoreProject::UnregisterObject(metaid_type metaid, objid_type objid)
{
	ASSERT( storage != NULL );
	ASSERT( metaid != 0 && objid != 0 );

	metaobjidpair_type idpair;
	idpair.metaid = metaid;
	idpair.objid = objid;

	if( object_lookup.find(idpair) != object_lookup.end() ) // TL: This used to be an assert
	{
		object_lookup.erase( idpair );
	}
	else
	{
		ASSERT(0); // Leave it as an assert, too
	}
}

void CCoreProject::CastProject(ICoreProject *project) const
{
	ASSERT( project != NULL );

	if( !IsEqualObject(project, (ICoreProject*)this) )
		HR_THROW(E_SAMEPROJECT);
}

CCoreObject *CCoreProject::CastObject(ICoreObject *object) const
{
	ASSERT( object != NULL );

	CComObjPtr<ICoreProject> project;
	COMTHROW( object->get_Project(PutOut(project)) );

	CastProject(project);

	CComObjPtr<IUnknown> p;
	COMTHROW( ::QueryInterface(object, p) );
	ASSERT( p != NULL );

	CCoreObject *the_object = CastToObject(p);
	ASSERT( the_object != NULL );

	if( the_object->GetProject() != this )
		HR_THROW(E_SAMEPROJECT);

	return the_object;
}

CCoreTerritory *CCoreProject::CastTerritory(ICoreTerritory *territory) const
{
	ASSERT( territory != NULL );

	CComObjPtr<ICoreProject> project;
	COMTHROW( territory->get_Project(PutOut(project)) );

	CastProject(project);

	CComObjPtr<IUnknown> p;
	COMTHROW( ::QueryInterface(territory, p) );
	ASSERT( p != NULL );

	CCoreTerritory *the_territory = CastToTerritory(p);
	ASSERT( the_territory != NULL );

#ifndef _ATL_DEBUG_INTERFACES
	if( the_territory->GetProject() != this )
		HR_THROW(E_SAMEPROJECT);
#endif

	return the_territory;
}

CCoreTerritory *CCoreProject::GetTerritory() const
{
	if( pushed_territorys.empty() )
		HR_THROW(E_TERRITORY);

	ASSERT( pushed_territorys.front() != NULL );
	return pushed_territorys.front();
}

void CCoreProject::RegisterTerritory(CCoreTerritory *territory)
{
	ASSERT( territory != NULL );
	ASSERT( territory->GetProject() == this );

#ifdef DEBUG_CONTAINERS
	ASSERT( find(created_territorys.begin(), created_territorys.end(), territory) ==
		created_territorys.end() );
#endif

	created_territorys.push_front(territory);
}

void CCoreProject::UnregisterTerritory(CCoreTerritory *territory)
{
	ASSERT( territory != NULL );
	ASSERT( territory->GetProject() == this );

#ifdef DEBUG_CONTAINERS
	ASSERT( find(created_territorys.begin(), created_territorys.end(), territory) !=
		created_territorys.end() );
#endif

	created_territorys.remove(territory);
}

// ------- Transactions

int CCoreProject::GetNestedTrCount() const
{
	ASSERT( InTransaction() || transactions.empty() );

	return transactions.size();
}

void CCoreProject::RegisterTransactionItem(CCoreTransactionItem *modified)
{
	ASSERT( InTransaction() );
	ASSERT( !transactions.empty() );
	ASSERT( modified != NULL );

	transaction_items_type &transaction_items = transactions.front().items;

#ifdef DEBUG_CONTAINERS
	ASSERT( find(transaction_items.begin(), transaction_items.end(), modified) == 
		transaction_items.end() );
#endif

	transaction_items.push_front(modified); 
}

void CCoreProject::RegisterFinalTrItem(CCoreFinalTrItem *modified)
{
	ASSERT( InTransaction() );
	ASSERT( modified != NULL );

#ifdef DEBUG_CONTAINERS
	ASSERT( find(finaltr_items.begin(), finaltr_items.end(), modified) ==
		finaltr_items.end() );
#endif

	finaltr_items.push_front(modified);
}

void CCoreProject::RegisterUndoItem(CCoreUndoItem *modified)
{
	ASSERT( InWriteTransaction() );
	ASSERT( modified != NULL );
	ASSERT( !undos.empty() );

#ifdef DEBUG_CONTAINERS
	ASSERT( find(undos.front().begin(), undos.front().end(), modified) ==
		undos.front().end() );
#endif

	undos.front().push_front(modified);
}

void CCoreProject::BeginFinalTr()
{
	ASSERT( transactions.empty() );
	ASSERT( finaltr_items.empty() );

#ifdef TRACE_CORE
	AtlTrace("CCoreProject::BeginFinalTr\n");
#endif

	ASSERT( storage );
	COMTHROW( storage->BeginTransaction() );

	ASSERT( undos.size() <= m_maxUndoStep );
	undos.push_front(undo_type());

	transactions.push_front(transaction_type());
	transactions.front().readonly = false;
}

void CCoreProject::CommitFinalTr(bool undo)
{
	ASSERT( transactions.size() == 1 );
	ASSERT( !undos.empty() && undos.front().empty() );

#ifdef TRACE_CORE
	AtlTrace("CCoreProject::CommitFinalTr\n");
#endif

	finaltr_items_iterator i = finaltr_items.begin();
	finaltr_items_iterator e = finaltr_items.end();
	while( i != e )
	{
		(*i)->CommitFinalTransaction();
		++i;
	}

	ASSERT( storage );
	COMTHROW( storage->CommitTransaction() );

	// nothing will throw here

#ifdef _DEBUG
	transaction_items_iterator j = transactions.front().items.begin();
	transaction_items_iterator f = transactions.front().items.end();
	while( j != f )
	{
		ASSERT( (*j)->IsDirty() );
		++j;
	}
#endif

	ASSERT( !undos.empty() && undos.front().empty() );

	// some objects might get destroyed in CommitFinalTransactionFinish
	i = finaltr_items.begin();
	ASSERT( e == finaltr_items.end() );
#ifdef _DEBUG
	// int finaltr_count = finaltr_items.size();
	finaltr_items_type::size_type finaltr_count = 0;
	std::for_each(finaltr_items.begin(), finaltr_items.end(), 
		[&finaltr_count](const finaltr_items_type::value_type& e) { finaltr_count++; });
#endif
	while( i != e )
	{
		// they can register undo_items
		(*i)->CommitFinalTransactionFinish(undo);
		++i;

		ASSERT( --finaltr_count >= 0 );
	}
	ASSERT( finaltr_count == 0 );

	// if this was a read-only transaction, then no one could have registered
	ASSERT( !transactions.front().readonly || undos.front().empty() );

	finaltr_items.clear();
	transactions.pop_front();
		
	// if no undo info here then we don't record it
	if( undos.front().empty() )
		undos.pop_front();

	// clear territories
	pushed_territorys.clear();
}

void CCoreProject::AbortFinalTr()
{
	ASSERT( transactions.size() == 1 );
	ASSERT( !undos.empty() && undos.front().empty() );

#ifdef TRACE_CORE
	AtlTrace("CCoreProject::AbortFinalTr\n");
#endif

	// *** do we want to throw when the abort does not succeed?

	ASSERT( storage );
	COMTHROW( storage->AbortTransaction() );

	// nothing will throw here

	undos.pop_front();

#ifdef _DEBUG
	transaction_items_iterator j = transactions.front().items.begin();
	transaction_items_iterator f = transactions.front().items.end();
	while( j != f )
	{
		ASSERT( (*j)->IsDirty() );
		++j;
	}
#endif

	// some objects might get destroyed in AbortFinalTransaction
	finaltr_items_iterator i = finaltr_items.begin();
	finaltr_items_iterator e = finaltr_items.end();
	while( i != e )
	{
		(*i)->AbortFinalTransaction();
		++i;
	}

	finaltr_items.clear();
	transactions.pop_front();

	// clear territories
	pushed_territorys.clear();
}

void CCoreProject::BeginNestedTr()
{
	ASSERT( InTransaction() );
	ASSERT( !transactions.empty() );

#ifdef TRACE_CORE
	AtlTrace("CCoreProject::BeginNestedTr\n");
#endif

	transaction_items_iterator i = transactions.front().items.begin();
	transaction_items_iterator e = transactions.front().items.end();
	while( i != e )
	{
		ASSERT( (*i)->IsDirty() );
		(*i)->ChangeDirty(false);
		ASSERT( !(*i)->IsDirty() );

		++i;
	}

	transactions.push_front(transaction_type());
	transactions.front().readonly = false;
}

void CCoreProject::AbortNestedTr()
{
	ASSERT( transactions.size() >= 2 );

#ifdef TRACE_CORE
	AtlTrace("CCoreProject::AbortNestedTr\n");
#endif

	transaction_items_iterator i = transactions.front().items.begin();
	transaction_items_iterator e = transactions.front().items.end();
	while( i != e )
	{
		ASSERT( (*i)->IsDirty() );
		(*i)->AbortNestedTransaction();
		ASSERT( !(*i)->IsDirty() );

		++i;
	}

	transactions.pop_front();

	i = transactions.front().items.begin();
	e = transactions.front().items.end(); // bug correction [zolmol]
	while( i != e )
	{
		ASSERT( !(*i)->IsDirty() );
		(*i)->ChangeDirty(true);
		ASSERT( (*i)->IsDirty() );

		++i;
	}
}

void CCoreProject::CommitNestedTr()
{
	ASSERT( transactions.size() >= 2 );

#ifdef TRACE_CORE
	AtlTrace("CCoreProject::CommitNestedTr\n");
#endif

	transaction_type &current = transactions.front();
	transactions_iterator previous = ++transactions.begin();

	transaction_items_iterator i = (*previous).items.begin();
	transaction_items_iterator e = (*previous).items.end();
	while( i != e )
	{
		if( (*i)->IsDirty() )
		{
#ifdef DEBUG_CONTAINERS
			ASSERT( find(current.items.begin(), current.items.end(), *i) != current.items.end() );
#endif
			(*i)->DiscardPreviousValue();
		}
		else
		{
#ifdef DEBUG_CONTAINERS
			ASSERT( find(current.items.begin(), current.items.end(), *i) == current.items.end() );
#endif
			(*i)->ChangeDirty(true);
			current.items.push_front( (*i) );
		}

		ASSERT( (*i)->IsDirty() );

		++i;
	}

	// don't forget to set the read-only flag;
	current.readonly = previous->readonly;
	transactions.erase(previous);
}

void CCoreProject::TryUndoTransaction()
{
	ASSERT( !InTransaction() );
	ASSERT( transactions.empty() );
	ASSERT( finaltr_items.empty() );

	try
	{
		ASSERT( storage );
		COMTHROW( storage->BeginTransaction() );

		transactions.push_front(transaction_type());
		transactions.front().readonly = true;

		ASSERT( !undos.empty() );

		undo_items_iterator i = undos.front().begin();
		undo_items_iterator e = undos.front().end();
		while( i != e )
		{
			ASSERT( (*i) != NULL );
			(*i)->UndoTransaction();

			++i;
		}

		ASSERT( transactions.size() == 1 );
		ASSERT( transactions.front().items.empty() );
		ASSERT( finaltr_items.empty() );

		transactions.pop_front();

		ASSERT( storage );
		COMTHROW( storage->CommitTransaction() );

		// nothing will throw here

		i = undos.front().begin();
		ASSERT( e == undos.front().end() );
		while( i != e )
		{
			ASSERT( (*i) != NULL );
			(*i)->UndoTransactionFinish();

			++i;
		}

		++redo_count;
		undos.splice(undos.end(), undos, undos.begin());
	}
	catch(hresult_exception &)
	{
		ASSERT(false);

		if( !transactions.empty() )
		{
			ASSERT( transactions.front().items.empty() );
			transactions.pop_front();
		}
		ASSERT( transactions.empty() );

		ASSERT( storage );
		storage->AbortTransaction();
	}

	ASSERT( transactions.empty() );
	ASSERT( finaltr_items.empty() );
}

void CCoreProject::TryRedoTransaction()
{
	ASSERT( !InTransaction() );
	ASSERT( transactions.empty() );
	ASSERT( finaltr_items.empty() );
	ASSERT( !undos.empty() );

	try
	{
		ASSERT( storage );
		COMTHROW( storage->BeginTransaction() );

		transactions.push_front(transaction_type());
		transactions.front().readonly = true;

		undo_items_iterator i = undos.back().begin();
		undo_items_iterator e = undos.back().end();
		while( i != e )
		{
			ASSERT( (*i) != NULL );
			(*i)->RedoTransaction();

			++i;
		}

		ASSERT( transactions.size() == 1 );
		ASSERT( transactions.front().items.empty() );
		ASSERT( finaltr_items.empty() );

		transactions.pop_front();

		ASSERT( storage );
		COMTHROW( storage->CommitTransaction() );

		// nothing will throw here

		i = undos.back().begin();
		ASSERT( e == undos.back().end() );
		while( i != e )
		{
			ASSERT( (*i) != NULL );
			(*i)->RedoTransactionFinish();

			++i;
		}

		--redo_count;
		undos.splice(undos.begin(), undos, --undos.end());
	}
	catch(hresult_exception &)
	{
		if( !transactions.empty() )
		{
			ASSERT( transactions.front().items.empty() );
			transactions.pop_front();
		}
		ASSERT( transactions.empty() );

		ASSERT( storage );
		storage->AbortTransaction();
	}

	ASSERT( transactions.empty() );
	ASSERT( finaltr_items.empty() );
}

void CCoreProject::TryDiscardLastItem()
{
	ASSERT( !InTransaction() );
	ASSERT( transactions.empty() );
	ASSERT( finaltr_items.empty() );
	ASSERT( !undos.empty() );

	undo_items_type discarded;

	try
	{
		ASSERT( storage );
		COMTHROW( storage->BeginTransaction() );

		transactions.push_front(transaction_type());
		transactions.front().readonly = true;

		undo_items_iterator i = undos.back().begin();
		undo_items_iterator e = undos.back().end();
		while( i != e )
		{
			ASSERT( (*i) != NULL );
			(*i)->DiscardLastItem();

			discarded.push_front(*i);

			++i;
		}

		ASSERT( transactions.size() == 1 );
		ASSERT( transactions.front().items.empty() );

		// now the final transaction items contain the modified locks

		finaltr_items_iterator j = finaltr_items.begin();
		finaltr_items_iterator d = finaltr_items.end();
		while( j != d )
		{
			(*j)->CommitFinalTransaction();

			++j;
		}

		transactions.pop_front();

		ASSERT( storage );
		COMTHROW( storage->CommitTransaction() );

		// nothing will throw here

		// we clear undo info at attributes, before 
		i = discarded.begin();
		e = discarded.end();
		while( i != e )
		{
			ASSERT( (*i) != NULL );
			(*i)->DiscardLastItemFinish();

			++i;
		}

		discarded.clear();

		// unloading by locking attributes
		j = finaltr_items.begin();
		ASSERT( d == finaltr_items.end() );
		while( j != d )
		{
			(*j)->CommitFinalTransactionFinish(false);

			++j;
		}

		finaltr_items.clear();

		if( --redo_count < 0 )
			redo_count = 0;

		undos.pop_back();
	}
	catch(hresult_exception &)
	{
		undo_items_iterator i = discarded.begin();
		undo_items_iterator e = discarded.end();
		while( i != e )
		{
			ASSERT( (*i) != NULL );
			(*i)->DiscardLastItemCancel();

			++i;
		}

		discarded.clear();

		finaltr_items_iterator j = finaltr_items.begin();
		finaltr_items_iterator d = finaltr_items.end();
		while( j != d )
		{
			(*j)->AbortFinalTransaction();

			++j;
		}

		finaltr_items.clear();

		if( !transactions.empty() )
		{
			ASSERT( transactions.front().items.empty() );
			transactions.pop_front();
		}
		ASSERT( transactions.empty() );

		ASSERT( storage );
		storage->AbortTransaction();
	}

	ASSERT( transactions.empty() );
	ASSERT( finaltr_items.empty() );
	ASSERT( discarded.empty() );
}

ICoreStorage *CCoreProject::SetStorageObject(CCoreObject *object)
{
	ASSERT( object != NULL );

	COMTHROW( storage->put_MetaObject(object->GetMetaObject()) );

	objid_type objid = object->GetObjID();
	ASSERT( objid != OBJID_NONE );
	
	COMTHROW( storage->OpenObject(objid) );
	
	return storage;
}

//inline 
void CCoreProject::setMaxUndoStep( long p_undoSize)
{
	if( p_undoSize > 0 && p_undoSize < 100)
		m_maxUndoStep = (unsigned long) p_undoSize;
}