
#include "stdafx.h"
#include "MgaMetaPointerSpec.h"
#include "MgaMetaPointerItem.h"

// --------------------------- CMgaMetaPointerSpec

STDMETHODIMP CMgaMetaPointerSpec::get_Parent(IDispatch **p)
{
	CHECK_OUT(p);

	{
		CComObjPtr<IMgaMetaReference> self;
		if( SUCCEEDED(::QueryInterface(GetUnknown(), self)) )
			return ::QueryInterface(GetUnknown(), p);
	}

	{
		CComObjPtr<IMgaMetaSet> self;
		if( SUCCEEDED(::QueryInterface(GetUnknown(), self)) )
			return ::QueryInterface(GetUnknown(), p);
	}

	return ComGetPointerValue(GetUnknown(), ATTRID_PTRSPECS_COLL,p);
}

// ------- Methods

void CMgaMetaPointerSpec::Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me)
{
	ASSERT( metaproject != NULL );
	ASSERT( me != NULL );

	TraverseCollection(metaproject, me, ATTRID_PTRITEMS_COLL);
}

bool CMgaMetaPointerSpec::CheckPath(CCoreObjectPtr &self, pathitems_type &pathitems, bool global)
{
	ASSERT( self != NULL );

	coreobjects_type coreobjects;
	self.GetCollectionValue(ATTRID_PTRITEMS_COLL, coreobjects);

	coreobjects_iterator i = coreobjects.begin();
	coreobjects_iterator e = coreobjects.end();
	while( i != e )
	{
		if( CMgaMetaPointerItem::CheckPath(*i, pathitems, global) )
			return true;

		++i;
	}

	return false;
}

