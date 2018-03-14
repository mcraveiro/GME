
#include "stdafx.h"
#include "MgaMetaFCO.h"

// --------------------------- CMgaMetaFCO

HRESULT CMgaMetaFCO::get_AttributeByName(IMgaMetaFCO *me, BSTR name, IMgaMetaAttribute **p)
{
	ASSERT( me != NULL );
	CHECK_OUT(p);

	COMTRY
	{
		CCoreObjectPtr self(me);

		coreobjects_type coreobjects;
		self.GetCollectionValue(ATTRID_ATTRLINK_USEDIN_PTR, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CCoreObjectPtr obj;
			(*i).GetPointerValue(ATTRID_ATTRLINK_ATTR_PTR, obj);

			CComBstrObj n;
			obj.GetStringValue(ATTRID_NAME, PutOut(n));

			if( n == name )
				COMRETURN( ::QueryInterface(obj, p) );

			++i;
		}

		HR_THROW(E_NOTFOUND);
	} 
	COMCATCH(;)
}

HRESULT CMgaMetaFCO::get_AttributeByRef(IMgaMetaFCO *me, metaref_type mref, IMgaMetaAttribute **p)
{
	ASSERT( me != NULL );
	CHECK_OUT(p);

	COMTRY 
	{
		CCoreObjectPtr self(me);

		coreobjects_type coreobjects;
		self.GetCollectionValue(ATTRID_ATTRLINK_USEDIN_PTR, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CCoreObjectPtr obj;
			(*i).GetPointerValue(ATTRID_ATTRLINK_ATTR_PTR, obj);

			if( obj.GetLongValue(ATTRID_METAREF) == mref )
			{
				COMTHROW( ::QueryInterface(obj, p) );
				return S_OK;
			}

			++i;
		}

		HR_THROW(E_NOTFOUND);
	}
	COMCATCH(;)
}

HRESULT CMgaMetaFCO::get_DefinedAttributeByName(IMgaMetaFCO *me, BSTR name,
	VARIANT_BOOL inscope, IMgaMetaAttribute **p)
{
	ASSERT( me != NULL );
	CHECK_OUT(p);

	COMTRY
	{
		CCoreObjectPtr self(me);

		coreobjects_type coreobjects;
		self.GetCollectionValue(ATTRID_DEFATTR_PTR, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComBstrObj n;
			(*i).GetStringValue(ATTRID_NAME, PutOut(n));
			
			if( n == name )
			{
				COMTHROW( ::QueryInterface(*i, p) );
				return S_OK;
			}

			++i;
		}

		if( inscope == VARIANT_FALSE )
			COMRETURN(E_NOTFOUND);

		CComObjPtr<IMgaMetaBase> parent;
		COMTHROW( me->get_DefinedIn(PutOut(parent)) );
		ASSERT( parent != NULL );

		CComObjPtr<IMgaMetaFolder> folder;
		if( SUCCEEDED(parent.QueryInterface(folder)) )
			COMRETURN( folder->get_DefinedAttributeByName(name, inscope, p) );

		CComObjPtr<IMgaMetaFCO> fco;
		COMTHROW( parent.QueryInterface(fco) );
		COMRETURN( fco->get_DefinedAttributeByName(name, inscope, p) );
	}
	COMCATCH(;)
}

void CMgaMetaFCO::Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me)
{
	ASSERT( metaproject != NULL );
	ASSERT( me != NULL );

	CMgaMetaBase::Traverse(metaproject, me);

	TraverseCollection(metaproject, me, ATTRID_DEFATTR_PTR);
	TraverseCollection(metaproject, me, ATTRID_ATTRLINK_USEDIN_PTR);
}

HRESULT CMgaMetaFCO::get_AliasingEnabled(IMgaMetaFCO *me, VARIANT_BOOL *p)
{
	ASSERT( me != NULL );
	if( p == NULL ) COMRETURN(E_POINTER);
	COMTRY
	{
		CCoreObjectPtr self(me);
		*p = 0; //self.GetBoolValue(ATTRID_ALIASENABLED);

	}
	COMCATCH(;)
}

HRESULT CMgaMetaFCO::put_AliasingEnabled(IMgaMetaFCO *me, VARIANT_BOOL p)
{
	ASSERT( me != NULL );
	COMTRY
	{
		CCoreObjectPtr self(me);
//		self.PutBoolValue(ATTRID_ALIASENABLED,p);

	}
	COMCATCH(;)
}