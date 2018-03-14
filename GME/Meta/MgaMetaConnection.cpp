
#include "stdafx.h"
#include "MgaMetaConnection.h"
#include "MgaMetaConnJoint.h"
#include <functional>

// --------------------------- CMgaMetaConnection

STDMETHODIMP CMgaMetaConnection::CheckPaths(BSTR paths, VARIANT_BOOL *p)
{
	CHECK_OUT(p);

	COMTRY
	{
		ASSERT( metaprojectref != NULL );

		jointpaths_type jointpaths;
		metaproject->CreateJointPaths(paths, jointpaths);

		CCoreObjectPtr self(GetUnknown());

		*p = CheckPaths(self, jointpaths) ? VARIANT_TRUE : VARIANT_FALSE;
	}
	COMCATCH(;)
}

bool CMgaMetaConnection::CheckPaths(CCoreObjectPtr &self, jointpaths_type jointpaths)
{
	coreobjects_type objects;
	self.GetCollectionValue(ATTRID_CONNJOINTS_COLL, objects);

	coreobjects_iterator i = objects.begin();
	coreobjects_iterator e = objects.end();
	while( i != e )
	{
		if( CMgaMetaConnJoint::CheckPaths(*i, jointpaths) )
			return true;

		++i;
	}

	return false;
}

// ------- Edit

STDMETHODIMP CMgaMetaConnection::CreateJoint(IMgaMetaConnJoint **p)
{
	CHECK_OUT(p);

	ASSERT( metaprojectref != NULL );

	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());
		ASSERT( self != NULL );

		CCoreObjectPtr joint;
		metaproject->CreateMetaObj(METAID_METACONNJOINT, joint);

		joint.PutPointerValue(ATTRID_CONNJOINTS_COLL, self);

		COMTHROW( ::QueryInterface(joint, p) );
	}
	COMCATCH(;)
}

// ------- Traverse

void CMgaMetaConnection::Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me)
{
	ASSERT( metaproject != NULL );
	ASSERT( me != NULL );

	CMgaMetaFCO::Traverse(metaproject, me);

	TraverseCollection(metaproject, me, ATTRID_CONNJOINTS_COLL);
}
