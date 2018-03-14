
#include "stdafx.h"
#include "MgaMetaBase.h"
#include "MgaMetaProject.h"
#include "MgaMetaFolder.h"

// --------------------------- CMgaMetaBase

CMgaMetaBase::~CMgaMetaBase()
{
	if (metaprojectref != NULL)
		metaproject->UnregisterMetaBase(metaref, this);

#ifdef _DEBUG
	metaref = 0;
#endif
}

HRESULT CMgaMetaBase::PutMetaRef_(metaref_type p)
{
	ASSERT( metaprojectref != NULL );

	if( p == metaref )
		return S_OK;

    COMTRY {

        metaproject->RegisterMetaBase(p, this);
        metaproject->UnregisterMetaBase(metaref, this);

        metaref = p;
    } COMCATCH(;)
}

void CMgaMetaBase::Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me)
{
	ASSERT( metaproject != NULL );

	me.Load();
	metaref_type metaref = me.GetLongValue(ATTRID_METAREF);

	CComObjPtr<IMgaMetaBase> ibase;
	COMTHROW( ::QueryInterface(me, ibase) );
	ASSERT( ibase != NULL );

#ifdef _ATL_DEBUG_INTERFACES
	IUnknown* pUnk = ((ATL::_QIThunk *)(ibase.p))->m_pUnk;
	CMgaMetaBase *base = (CMgaMetaBase *)(IMgaMetaBase*)(pUnk);
#else
	CMgaMetaBase *base = static_cast<CMgaMetaBase*>((IMgaMetaBase*)ibase);
#endif
	ASSERT( base != NULL );

	ASSERT( base->metaproject == NULL );

#ifdef _ATL_DEBUG_INTERFACES
	CComQIPtr<IMgaMetaBase> base2;
	base->QueryInterface(&base2.p);
	metaproject->RegisterMetaBase(metaref, base2);
#else
	metaproject->RegisterMetaBase(metaref, base);
#endif

	base->metaprojectref = metaproject;
	base->metaref = metaref;

	TraverseCollection(metaproject, me, ATTRID_REGNODES_COLL);
	TraverseCollection(metaproject, me, ATTRID_CONSTRAINT_PTR);
}
