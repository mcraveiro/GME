
#include "stdafx.h"
#include "MgaMetaAspect.h"

// --------------------------- CMgaMetaAspect

// ------- Edit

STDMETHODIMP CMgaMetaAspect::CreatePart(IMgaMetaRole *role, IMgaMetaPart **p)
{
	CHECK_OUT(p);

	if( role == NULL )
		COMRETURN(E_POINTER);

	ASSERT( metaprojectref != NULL );

	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());
		ASSERT( self != NULL );

		CCoreObjectPtr other(role);
		ASSERT( other != NULL );

		CCoreObjectPtr part;
		metaproject->CreateMetaBase(METAID_METAPART, part);

		part.PutPointerValue(ATTRID_PARTASPECT_PTR, self);
		part.PutPointerValue(ATTRID_PARTROLE_PTR, other);

		COMTHROW( ::QueryInterface(part, p) );
	}
	COMCATCH(;)
}

// ------- Traverse

void CMgaMetaAspect::Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me)
{
	ASSERT( metaproject != NULL );
	ASSERT( me != NULL );

	CMgaMetaBase::Traverse(metaproject, me);

	TraverseCollection(metaproject, me, ATTRID_PARTASPECT_PTR);
	TraverseCollection(metaproject, me, ATTRID_ATTRLINK_USEDIN_PTR);
}
