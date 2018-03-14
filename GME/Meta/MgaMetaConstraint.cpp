
#include "stdafx.h"
#include "MgaMetaConstraint.h"

// --------------------------- CMgaMetaConstraint

STDMETHODIMP CMgaMetaConstraint::get_Depth(constraint_depth_enum *p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CCoreObjectPtr me(GetUnknown());
		*p = static_cast<constraint_depth_enum>(me.GetLongValue(ATTRID_CONSDEPTH));

		if( *p != CONSTRAINT_DEPTH_ZERO &&
			*p != CONSTRAINT_DEPTH_ANY )
			*p = CONSTRAINT_DEPTH_ONE;
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaConstraint::get_Type(constraint_type_enum *p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CCoreObjectPtr me(GetUnknown());
		*p = static_cast<constraint_type_enum>(me.GetLongValue(ATTRID_CONSTYPE));

		if( *p != CONSTRAINT_TYPE_EVENTBASED &&
			*p != CONSTRAINT_TYPE_FUNCTION )
			*p = CONSTRAINT_TYPE_ONDEMAND;
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaConstraint::SetDefinedForNamespace( BSTR pStr)
{
	return ComPutAttrValue(GetUnknown(), ATTRID_CONSNAMESPC, pStr);
}

STDMETHODIMP CMgaMetaConstraint::GetDefinedForNamespace( BSTR * pStr)
{
	return ComGetAttrValue(GetUnknown(), ATTRID_CONSNAMESPC, pStr);
}
