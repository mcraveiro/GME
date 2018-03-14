
#include "stdafx.h"

#include "CoreMetaObject.h"
#include "CoreMetaAttribute.h"

// --------------------------- CCoreMetaAttribute

CCoreMetaAttribute::CCoreMetaAttribute()
{
}

// ------- COM methods

STDMETHODIMP CCoreMetaAttribute::get_Object(ICoreMetaObject **p)
{
	CHECK_OUT(p);

	ASSERT( object != NULL );
	CopyTo(object, p);
	return S_OK;
}

STDMETHODIMP CCoreMetaAttribute::get_AttrID(attrid_type *p)
{
	CHECK_OUT(p);

	ASSERT( attrid > 0 );
	*p = attrid;
	return S_OK;
}

STDMETHODIMP CCoreMetaAttribute::get_Token(BSTR *p)
{
	CHECK_OUT(p);

	*p = SysAllocString(token.c_str());
	if (*p == NULL)
		return E_OUTOFMEMORY;
	return S_OK;
}

STDMETHODIMP CCoreMetaAttribute::get_Name(BSTR *p)
{
	CHECK_OUT(p);

	*p = SysAllocString(name.c_str());
	if (*p == NULL)
		return E_OUTOFMEMORY;
	return S_OK;
}

STDMETHODIMP CCoreMetaAttribute::get_ValueType(valtype_type *p)
{
	CHECK_OUT(p);

	ASSERT( valtype > 0 );
	*p = valtype;
	return S_OK;
}

STDMETHODIMP CCoreMetaAttribute::get_LockAttrID(attrid_type *p)
{
	CHECK_OUT(p);

	if( valtype == VALTYPE_LOCK )
		*p = ATTRID_NONE;
	else
		*p = ATTRID_LOCK;

	return S_OK;
}

STDMETHODIMP CCoreMetaAttribute::get_MaxSize(long *p)
{
	CHECK_OUT(p);

	if( valtype == VALTYPE_STRING || valtype ==VALTYPE_BINARY )
		*p = 254;
	else
		*p = 0;

	return S_OK;
}
