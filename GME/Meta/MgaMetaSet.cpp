
#include "stdafx.h"
#include "MgaMetaSet.h"
#include "MgaMetaPointerSpec.h"

// --------------------------- CMgaMetaSet

STDMETHODIMP CMgaMetaSet::CheckPath(BSTR path, VARIANT_BOOL *p)
{
	CHECK_OUT(p);

	COMTRY
	{
		ASSERT( metaprojectref != NULL );

		pathitems_type pathitems;
		metaproject->CreatePathItems(begin(path), end(path), pathitems);

		CCoreObjectPtr self(GetUnknown());

		bool valid = CMgaMetaPointerSpec::CheckPath(self, pathitems, false);

		*p = valid ? VARIANT_TRUE : VARIANT_FALSE;
	}
	COMCATCH(;)
}

// ------- Traverse

void CMgaMetaSet::Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me)
{
	ASSERT( metaproject != NULL );
	ASSERT( me != NULL );

	CMgaMetaFCO::Traverse(metaproject, me);
	CMgaMetaPointerSpec::Traverse(metaproject, me);
}
