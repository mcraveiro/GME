
#include "stdafx.h"
#include "MgaMetaProject.h"
#include "MgaMetaFolder.h"
#include <time.h>
#include <functional>

// ------- CurrentTime

CComVariant CurrentTime(void) 
{
	time_t t;
	time(&t);
	char *p = ctime(&t);
	ASSERT( p != NULL );

	p[strlen(p)-1] = 0;
	return p;
}

// --------------------------- CMgaMetaProject

CMgaMetaProject::CMgaMetaProject()
{
	max_metaref = 1000;
}

CMgaMetaProject::~CMgaMetaProject()
{
	if( coreproject != NULL )
	{
		ASSERT( coreterritory != NULL );

		// If this assertion fails then we have killed the CMgaMetaProject
		// while working in the Close method. Probably the reference
		// count on this object is wrong.

		ASSERT( rootobject != NULL );
		ASSERT( !metaobj_lookup.empty() );

		Close();
	}

	ASSERT( coreproject == NULL );
	ASSERT( coreterritory == NULL );
	ASSERT( rootobject == NULL );
	ASSERT( metaobj_lookup.empty() );
}
	
// ------- Methods

STDMETHODIMP CMgaMetaProject::Open(BSTR connection)
{
	COMTRY
	{
		if( coreproject != NULL )
			__Close();

		CComPtr<ICoreMetaProject> coremetaproject;
		CreateCoreMetaProject(coremetaproject);

		COMTHROW( coreproject.CoCreateInstance(OLESTR("MGA.CoreProject")) );
		ASSERT( coreproject != NULL );

		coreproject->__OpenProject(connection, coremetaproject, NULL);

		coreproject->__CreateTerritory(PutOut(coreterritory));
		ASSERT( coreterritory != NULL );

		coreproject->__BeginTransaction(TRANSTYPE_READFIRST);
		coreproject->__PushTerritory(coreterritory);

		rootobject.p = coreproject->RootObject;

		CMgaMetaFolder::Traverse(this, rootobject);

		coreproject->__CommitTransaction(TRANSTYPE_READFIRST);
	}
	COMCATCH(
		metaobj_lookup.clear();
		rootobject = NULL;
		coreterritory = NULL;
		coreproject = NULL;
	)
}

STDMETHODIMP CMgaMetaProject::Create(BSTR connection)
{
	COMTRY
	{
		if( coreproject != NULL )
			COMTHROW( Close() );

		CComPtr<ICoreMetaProject> coremetaproject;
		CreateCoreMetaProject(coremetaproject);

		COMTHROW( coreproject.CoCreateInstance(OLESTR("MGA.CoreProject")) );
		ASSERT( coreproject != NULL );

		coreproject->__CreateProject(connection, coremetaproject);

		coreproject->__CreateTerritory(PutOut(coreterritory));
		ASSERT( coreterritory != NULL );

		COMTHROW( coreproject->BeginTransaction(TRANSTYPE_FIRST) );
		COMTHROW( coreproject->PushTerritory(coreterritory) );

		COMTHROW( coreproject->get_RootObject(PutOut(rootobject)) );
		COMTHROW( rootobject->put_AttributeValue(ATTRID_CDATE, CurrentTime()) );
		COMTHROW( rootobject->put_AttributeValue(ATTRID_MDATE, CurrentTime()) );
		COMTHROW( rootobject->put_AttributeValue(ATTRID_NAME, CComVariant()) );
		COMTHROW( rootobject->put_AttributeValue(ATTRID_METAREF, PutInVariant(max_metaref)) );

		COMTHROW( coreterritory->Clear() );

		CMgaMetaFolder::Traverse(this, rootobject);

		COMTHROW( coreproject->CommitTransaction(TRANSTYPE_FIRST) );
	}
	COMCATCH(
		metaobj_lookup.clear();
		rootobject = NULL;
		coreterritory = NULL;
		coreproject = NULL;
	)
}

STDMETHODIMP CMgaMetaProject::BeginTransaction()
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		coreproject->__BeginTransaction(TRANSTYPE_FIRST);

		ASSERT( coreterritory != NULL );
		coreproject->__PushTerritory(coreterritory);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::CommitTransaction()
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		rootobject->AttributeValue[ATTRID_MDATE] = CurrentTime();
		coreproject->__CommitTransaction(TRANSTYPE_FIRST);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::AbortTransaction()
{

	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		coreproject->__AbortTransaction(TRANSTYPE_FIRST);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::Close()
{
	COMTRY
	{
		if( coreproject != NULL )
		{
			short count = 0;
			COMTHROW( coreproject->get_NestedTransactionCount(&count) );

			if( count == 1 )
				COMTHROW( coreproject->AbortTransaction(TRANSTYPE_ANY) );

			short undos;
			COMTHROW(coreproject->get_UndoQueueSize(&undos));
			if (undos)
			{
				HRESULT hr = coreproject->SaveProject(NULL, VARIANT_TRUE);
				if (FAILED(hr))
					return hr;
			}
			COMTHROW(coreproject->FlushRedoQueue());
			COMTHROW(coreproject->FlushUndoQueue());

			COMTHROW( coreproject->BeginTransaction(TRANSTYPE_FIRST) );
			COMTHROW( coreproject->PushTerritory(coreterritory) );


			for (auto it = metaobj_lookup.begin(); it != metaobj_lookup.end(); it++)
			{
				CCoreObjectPtr self(it->second);
				COMTHROW(self->put_AttributeValue(ATTRID_LOCK, CComVariant((locking_type)-1)));
			}
			for (auto it = core_object_cleanup.begin(); it != core_object_cleanup.end(); it++)
			{
				COMTHROW((*it)->put_AttributeValue(ATTRID_LOCK, CComVariant((locking_type)-1)));
			}
			core_object_cleanup.clear();

			COMTHROW( coreproject->CommitTransaction(TRANSTYPE_FIRST) );

			metaobj_lookup.clear();
			max_metaref = 1000;

			rootobject = NULL;

			if( coreterritory != NULL )
			{
				COMTHROW( coreterritory->Clear() );
				coreterritory = NULL;
			}

			coreproject->__CloseProject(VARIANT_TRUE);
			coreproject = NULL;
		}

		ASSERT( coreproject == NULL );
		ASSERT( coreterritory == NULL );
		ASSERT( rootobject == NULL );
		ASSERT( metaobj_lookup.empty() );
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::get_GUID(VARIANT *p)
{

	CHECK_OUT(p);

	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		CCoreObjectPtr me(rootobject);
		me.GetVariantValue(ATTRID_GUID, p);

		if( p->vt != (VT_UI1 | VT_ARRAY) || GetArrayLength(*p) != sizeof(::GUID) )
		{
			::GUID guid;
			memset(&guid, 0, sizeof(::GUID));

			CopyTo(guid, p);
		}
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::put_GUID(VARIANT p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		if( p.vt != (VT_UI1 | VT_ARRAY) || GetArrayLength(p) != sizeof(::GUID) )
			COMRETURN(E_INVALIDARG);

		return ComPutAttrValue(rootobject, ATTRID_GUID, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::get_Name(BSTR *p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComGetAttrValue(rootobject, ATTRID_PARNAME, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::put_Name(BSTR p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComPutAttrValue(rootobject, ATTRID_PARNAME, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::get_DisplayedName(BSTR *p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComGetDisplayedName(rootobject, ATTRID_PARDISPNAME, ATTRID_PARNAME, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::put_DisplayedName(BSTR p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComPutAttrValue(rootobject, ATTRID_PARDISPNAME, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::get_Version(BSTR *p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComGetAttrValue(rootobject, ATTRID_VERSION, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::put_Version(BSTR p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComPutAttrValue(rootobject, ATTRID_VERSION, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::get_Author(BSTR *p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComGetAttrValue(rootobject, ATTRID_AUTHOR, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::put_Author(BSTR p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComPutAttrValue(rootobject, ATTRID_AUTHOR, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::get_Comment(BSTR *p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComGetAttrValue(rootobject, ATTRID_COMMENT, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::put_Comment(BSTR p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComPutAttrValue(rootobject, ATTRID_COMMENT, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::get_CreatedAt(BSTR *p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComGetAttrValue(rootobject, ATTRID_CDATE, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::put_CreatedAt(BSTR p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComPutAttrValue(rootobject, ATTRID_CDATE, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::get_ModifiedAt(BSTR *p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComGetAttrValue(rootobject, ATTRID_MDATE, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::put_ModifiedAt(BSTR p)
{
	COMTRY
	{
		_ThrowExceptionIfNotOpen();

		return ComPutAttrValue(rootobject, ATTRID_MDATE, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaProject::get_FindObject(metaref_type metaref, IMgaMetaBase **p)
{
	metaobj_lookup_iterator i = metaobj_lookup.find(metaref);
	if( i == metaobj_lookup.end() )
		COMRETURN(E_NOTFOUND);

	CopyTo((*i).second.p, p);
	return S_OK;
}

STDMETHODIMP CMgaMetaProject::SetNmspc(BSTR p)
{
	m_namespace = p;
	return S_OK;
}

STDMETHODIMP CMgaMetaProject::GetNmspc(BSTR *p)
{
	CComBSTR nms = m_namespace;
	*p = nms.Detach();

	return S_OK;
}

// ------- metaobj_lookup

void CMgaMetaProject::RegisterMetaBase(metaref_type metaref, IMgaMetaBase *obj)
{
	ASSERT( obj != NULL );

	if (metaobj_lookup.find(metaref) != metaobj_lookup.end())
	{
		wchar_t buf[256];
		swprintf_s(buf, L"Invalid duplicate metaref '%ld'", metaref);
		throw_com_error(E_METAREF, buf);
	}

	if( metaref > max_metaref )
		max_metaref = metaref;

	metaobj_lookup[metaref] = obj;
}

void CMgaMetaProject::UnregisterMetaBase(metaref_type metaref, IMgaMetaBase *obj)
{
	ASSERT( obj != NULL );
	// Under GC, we may be here via ~CMgaMetaBase after Close clear()s the map
	if (metaobj_lookup.size() == 0)
		return;
	ASSERT( metaobj_lookup.find(metaref) != metaobj_lookup.end() );
	ASSERT( metaobj_lookup[metaref] == obj );

	metaobj_lookup.erase(metaref);
}

//  ------- Edit

void CMgaMetaProject::CreateMetaBase(metaid_type metaid, CCoreObjectPtr &obj)
{
	ASSERT( obj == NULL );
	ASSERT( coreproject != NULL );

	COMTHROW( coreproject->CreateObject(metaid, PutOut(obj)) );
	COMTHROW( obj->put_AttributeValue(ATTRID_METAREF, PutInVariant(max_metaref + 1)) );
	CMgaMetaBase::Traverse(this, obj);
	
	++max_metaref;
}

void CMgaMetaProject::CreateMetaObj(metaid_type metaid, CCoreObjectPtr &obj)
{
	ASSERT( obj == NULL );
	ASSERT( coreproject != NULL );

	COMTHROW( coreproject->CreateObject(metaid, PutOut(obj)) );
}

// ------- CheckPath

void CMgaMetaProject::CreatePathItems(bstr_const_iterator i, bstr_const_iterator e, pathitems_type &pathitems)
{
	ASSERT( pathitems.empty() );

	while( i != e && *(e-1) == L' ' )
		--e;

	while( i < e )
	{
		wchar_t *j = 0;
		long metaref = wcstol(i, &j, 10);

		if( i == j || j == 0 )
			COMTHROW(E_INVALID_PATH);

		metaobj_lookup_iterator k = metaobj_lookup.find(metaref);
		if( k == metaobj_lookup.end() )
			COMTHROW(E_INVALID_PATH);

		pathitems.push_back(pathitem_type());
		
		COMTHROW( k->second->get_Name(PutOut(pathitems.back().continual_name)) );

		CComObjPtr<IMgaMetaRole> role;
		if( SUCCEEDED( ::QueryInterface(k->second, role)) )
		{
			CComObjPtr<IMgaMetaFCO> fco;
			COMTHROW( role->get_Kind(PutOut(fco)) );
			COMTHROW( fco->get_Name(PutOut(pathitems.back().terminal_name)) );
		}
		else
		{
			pathitems.back().terminal_name = pathitems.back().continual_name;
		}

		i = j;
	}
}

void CMgaMetaProject::CreateJointPaths(BSTR paths, jointpaths_type &jointpaths)
{
	ASSERT( jointpaths.empty() );

	bstr_const_iterator i = begin(paths);
	bstr_const_iterator e = end(paths);
	while( i != e )
	{
		i = std::find_if(i, e, std::bind2nd( std::not_equal_to<wchar_t>(), L' '));
		if( i == e )
			break;
		
		bstr_const_iterator j = std::find(i, e, L' ');
		bstr_const_iterator k = std::find(j, e, L',');

		jointpaths.push_back(jointpath_type());
		CopyTo(i, j, jointpaths.back().ptrspec_name);

		CreatePathItems(j, k, jointpaths.back().pathitems);

		i = k;
		if( i != e )
		{
			ASSERT( *i == L',' );
			++i;
		}
	}
}

void CMgaMetaProject::_ThrowExceptionIfNotOpen()
{
	if (rootobject == NULL)
		throw_com_error(E_META_NOTOPEN, L"MgaMetaProject is not open");
}
