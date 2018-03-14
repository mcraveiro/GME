
#include "stdafx.h"

#include "CoreMetaProject.h"
#include "CoreMetaObject.h"
#include "CommonCollection.h"

// --------------------------- CCoreMetaProject

CCoreMetaProject::CCoreMetaProject()
{
}

CCoreMetaProject::~CCoreMetaProject()
{
	while( !objects.empty() )
	{
		delete objects.front();
		objects.pop_front();
	}
}

// ------- COM methods

STDMETHODIMP CCoreMetaProject::get_GUID(guid_type *p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CopyTo(guid, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreMetaProject::put_GUID(guid_type p)
{
	COMTRY
	{
		CopyTo(p, guid);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreMetaProject::get_Token(BSTR *p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CopyTo(token, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreMetaProject::put_Token(BSTR p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CopyTo(p, token);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreMetaProject::get_Name(BSTR *p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CopyTo(name, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreMetaProject::put_Name(BSTR p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CopyTo(p, name);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreMetaProject::get_Object(metaid_type metaid, ICoreMetaObject **p)
{
	CHECK_OUT(p);

	CCoreMetaObject *o = GetObject(metaid);
	if( o != NULL )
	{
		CopyTo(o, p);
		return S_OK;
	}

	return E_NOTFOUND;
}

CCoreMetaObject *CCoreMetaProject::GetObject(metaid_type metaid)
{
	objects_iterator i = objects.begin();
	objects_iterator e = objects.end();

	while( i != e )
	{
		ASSERT( *i != NULL );
		if( (*i)->metaid == metaid )
			return *i;

		++i;
	}

	return NULL;
}

STDMETHODIMP CCoreMetaProject::get_Objects(ICoreMetaObjects **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		typedef CCoreCollection<ICoreMetaObjects, std::vector<ICoreMetaObject*>, 
			ICoreMetaObject, CCoreMetaObject> COMTYPE;

		CComObjPtr<COMTYPE> q;
		CreateComObject(q);

		q->Fill(objects.begin(), objects.end());
		MoveTo(q, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreMetaProject::AddObject(metaid_type metaid, BSTR token, BSTR name, ICoreMetaObject **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CComObjPtr<CCoreMetaObject> q;
		CreateComPartObject(CastToUnknown(this), q);

		q->project = this;
		q->metaid = metaid;
		CopyTo(token, q->token);
		CopyTo(name, q->name);

		objects.push_front(q);
		MoveTo(q, p);
	}
	COMCATCH(;)
}
