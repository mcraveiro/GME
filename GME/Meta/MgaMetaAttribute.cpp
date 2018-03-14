
#include "stdafx.h"
#include "MgaMetaAttribute.h"

// --------------------------- CMgaMetaAttribute

STDMETHODIMP CMgaMetaAttribute::get_UsedIn(IMgaMetaFCOs **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CCoreObjectPtr me(GetUnknown());
		ASSERT( me != NULL );

		coreobjects_type coreobjects;
		me.GetCollectionValue(ATTRID_ATTRLINK_ATTR_PTR, coreobjects);

		typedef CCoreCollection<IMgaMetaFCOs, std::vector<IMgaMetaFCO*>, IMgaMetaFCO, IMgaMetaFCO> COMTYPE;

		CComObjPtr<COMTYPE> coll;
		CreateComObject(coll);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CCoreObjectPtr obj;
			(*i).GetPointerValue(ATTRID_ATTRLINK_USEDIN_PTR, obj);

			CComObjPtr<IMgaMetaFCO> q;
			if( SUCCEEDED( ::QueryInterface(obj, q)) )
				coll->Add(q);

			++i;
		}

		MoveTo(coll, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaAttribute::get_DefaultValue(VARIANT *p) 
{
	CHECK_OUT(p);

	COMTRY 
	{
		CCoreObjectPtr self(GetUnknown());

		CComVariant v;
		self.GetVariantValue(ATTRID_VALUE, PutOut(v));
		ASSERT( v.vt == VT_BSTR );

		attval_enum attval = static_cast<attval_enum>
			(self.GetLongValue(ATTRID_ATTVALTYPE));

		ChangeAttrValueType(v, attval);
		MoveTo(&v, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaAttribute::get_ValueType(attval_enum *p) 
{
	CHECK_OUT(p);

	COMTRY 
	{
		CCoreObjectPtr self(GetUnknown());
		*p = static_cast<attval_enum>(self.GetLongValue(ATTRID_ATTVALTYPE));
	} COMCATCH(;);
}

STDMETHODIMP CMgaMetaAttribute::get_Viewable(VARIANT_BOOL *p)
{
	CHECK_OUT(p);

	COMTRY 
	{
		CCoreObjectPtr self(GetUnknown());
		*p = self.GetLongValue(ATTRID_VIEWABLE)? VARIANT_TRUE : VARIANT_FALSE;
	} COMCATCH(;);
}

// ------- Traverse

void CMgaMetaAttribute::Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me)
{
	ASSERT( metaproject != NULL );
	ASSERT( me != NULL );

	CMgaMetaBase::Traverse(metaproject, me);

	TraverseCollection(metaproject, me, ATTRID_ATTRLINK_ATTR_PTR);
	TraverseCollection(metaproject, me, ATTRID_ENUMITEMS_COLL);
}
