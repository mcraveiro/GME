#pragma once

#include "Comdef.h"

#define COM_EQUAL(a, b) (static_cast<IUnknown *>(a) == b ? true : a.IsEqualObject(b))

// from CommonMgaTrukk.h

#define MGACOLL_ITERATE(iftype, collifptr) \
{ \
	ASSERT( collifptr != NULL ); \
	long iter_count = 0; \
	COMTHROW( collifptr->get_Count(&iter_count) ); \
	ASSERT( iter_count >= 0 ); \
	CComPtr<iftype> *arrptr, *arrend, *array = new CComPtr<iftype>[iter_count]; \
	if(iter_count > 0) \
		COMTHROW( collifptr->GetAll(iter_count, &(*array)) ); \
	arrend = array+iter_count; \
	for(arrptr = array; arrptr != arrend; arrptr++)

#define MGACOLL_ITER (*arrptr)

#define MGACOLL_AT_END (arrptr == arrend)

#define MGACOLL_ITERATE_END \
	delete[] array; \
}

// from CommonError.h

#define COMTHROW(FUNC) \
do { \
	HRESULT _hr = (FUNC); \
	if( FAILED(_hr) ) { \
		ASSERT(("COMTHROW: Throwing HRESULT exception. Press IGNORE", false)); \
		throw _hr; \
	} \
} while(false)

#define COMTRY try

#define COMCATCH(CLEANUP) \
	catch(HRESULT hr) \
	{ \
		ASSERT( FAILED(hr) ); \
		{ CLEANUP; } \
		return hr; \
	} \
	catch(_com_error &e) \
	{ \
		{ CLEANUP; } \
		if (e.Description() != _bstr_t()) \
			SetErrorInfo(e.Error(), e.Description()); \
		return e.Error(); \
	} \
	return S_OK;

	// Defined in ComHelp.cpp
	void SetErrorInfo(HRESULT hr, const wchar_t* err);

