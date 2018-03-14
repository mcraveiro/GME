
#include "stdafx.h"

#include "CoreTerritory.h"
#include "CoreProject.h"
#include "CoreAttribute.h"
#include "CommonCollection.h"

#include <hash_set>

// --------------------------- CCoreTerritory

CCoreTerritory::CCoreTerritory()
{
	status = 0;
}

CCoreTerritory *CCoreTerritory::Create(CCoreProject *project)
{
	CCoreTerritory *territory = NULL;

	typedef CComObject< CCoreTerritory > COMTYPE;
	COMTHROW( COMTYPE::CreateInstance((COMTYPE **)&territory) );
	ASSERT( territory != NULL );

	territory->project = project;
	project->RegisterTerritory(territory);

	return territory;
}

CCoreTerritory::~CCoreTerritory()
{
	ASSERT( project != NULL );

	project->UnregisterTerritory(this);

#ifdef _DEBUG
	project = NULL;
#endif
}

// ------- COM methods

STDMETHODIMP CCoreTerritory::get_Project(ICoreProject **p)
{
	CHECK_OUT(p);

	ASSERT( project != NULL );
	CopyTo(project, p);
	return S_OK;
}

STDMETHODIMP CCoreTerritory::get_Attributes(ICoreAttributes **p)
{
	ASSERT(0);
	return E_NOTIMPL;
}

STDMETHODIMP CCoreTerritory::Clear()
{
	return S_OK;
}

