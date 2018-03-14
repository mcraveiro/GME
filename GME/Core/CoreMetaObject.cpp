
#include "stdafx.h"

#include "CoreMetaProject.h"
#include "CoreMetaObject.h"
#include "CoreMetaAttribute.h"
#include "CommonCollection.h"

// --------------------------- CCoreMetaObject

CCoreMetaObject::CCoreMetaObject()
{

}

CCoreMetaObject::~CCoreMetaObject()
{

	while( !attributes.empty() )
	{
		delete attributes.front();
		attributes.pop_front();
	}
}

// ------- COM methods

STDMETHODIMP CCoreMetaObject::get_Project(ICoreMetaProject **p)
{
	CHECK_OUT(p);

	ASSERT( project != NULL );
	CopyTo(project, p);
	return S_OK;
}

STDMETHODIMP CCoreMetaObject::get_MetaID(metaid_type *p)
{
	CHECK_OUT(p);

	ASSERT( metaid > 0 );
	*p = metaid;
	return S_OK;
}

STDMETHODIMP CCoreMetaObject::get_Token(BSTR *p)
{
	CHECK_OUT(p);

	CopyTo(token, p);
	return S_OK;
}

STDMETHODIMP CCoreMetaObject::get_Name(BSTR *p)
{
	CHECK_OUT(p);

	CopyTo(name, p);
	return S_OK;
}

STDMETHODIMP CCoreMetaObject::get_Attribute(attrid_type attrid, ICoreMetaAttribute **p)
{
	CHECK_OUT(p);

	attributes_type::iterator i = attributes.begin();
	attributes_type::iterator e = attributes.end();

	while( i != e )
	{
		CCoreMetaAttribute *o = *i;
		++i;

		ASSERT( o != NULL );
		if( o->attrid == attrid )
		{
			CopyTo(o, p);
			return S_OK;
		}
	}
	*p = NULL;
	return S_OK;
}

STDMETHODIMP CCoreMetaObject::get_Attributes(ICoreMetaAttributes **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		typedef CCoreCollection<ICoreMetaAttributes, std::vector<ICoreMetaAttribute*>,
			ICoreMetaAttribute, CCoreMetaAttribute> COMTYPE;

		CComObjPtr<COMTYPE> q;
		CreateComObject(q);

		q->Fill(attributes.begin(), attributes.end());
		MoveTo(q, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreMetaObject::get_ClassIDs(SAFEARRAY **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CopyTo(classids, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreMetaObject::AddAttribute(attrid_type attrid, BSTR token, BSTR name, 
	valtype_type valtype, ICoreMetaAttribute **p)
{
	COMTRY
	{
		CComObjPtr<CCoreMetaAttribute> q;
		CreateComPartObject(CastToUnknown(project), q);

		q->object = this;
		q->attrid = attrid;
		q->token = token;
		q->name = name;
		q->valtype = valtype;
	
		attributes.push_front(q);

		if( p != NULL )
			MoveTo(q, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreMetaObject::AddClassID(guid_type classid)
{
	COMTRY
	{
		GUID null_classid; memset( &null_classid, 0, sizeof( null_classid));
		classids.push_back(null_classid);
		try
		{
			CopyTo(classid, classids.back());
		}
		catch(...)
		{
			classids.pop_back();
			throw;
		}
	}
	COMCATCH(;)
}
