
#include "stdafx.h"
#include "MgaMetaPart.h"


// --------------------------- CMgaMetaPart

#define MGAMETAPART_PRIMARY		0x0001
#define MGAMETAPART_LINKED		0x0002

STDMETHODIMP CMgaMetaPart::get_IsPrimary(VARIANT_BOOL *p)
{
	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());
		ASSERT( self != NULL );

		long a = self.GetLongValue(ATTRID_PARTDATA);

		*p = (a & MGAMETAPART_PRIMARY) ? VARIANT_TRUE : VARIANT_FALSE;
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaPart::get_IsLinked(VARIANT_BOOL *p)
{
	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());
		ASSERT( self != NULL );

		long a = self.GetLongValue(ATTRID_PARTDATA);

		*p = (a & MGAMETAPART_LINKED) ? VARIANT_TRUE : VARIANT_FALSE;
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaPart::put_IsPrimary(VARIANT_BOOL p)
{
	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());
		ASSERT( self != NULL );

		long a = self.GetLongValue(ATTRID_PARTDATA);

		if( p == VARIANT_FALSE )
			a &= (~MGAMETAPART_PRIMARY);
		else
			a |= MGAMETAPART_PRIMARY;

		self.PutLongValue(ATTRID_PARTDATA, a);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaPart::put_IsLinked(VARIANT_BOOL p)
{
	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());
		ASSERT( self != NULL );

		long a = self.GetLongValue(ATTRID_PARTDATA);

		if( p == VARIANT_FALSE )
			a &= (~MGAMETAPART_LINKED);
		else
			a |= MGAMETAPART_LINKED;

		self.PutLongValue(ATTRID_PARTDATA, a);
	}
	COMCATCH(;)
}

// ------- Traverse

void CMgaMetaPart::Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me)
{
	ASSERT( metaproject != NULL );
	ASSERT( me != NULL );

	CMgaMetaBase::Traverse(metaproject, me);

	TraverseCollection(metaproject, me, ATTRID_REGNODES_COLL);
}
