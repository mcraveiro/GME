
#include "stdafx.h"
#include "MgaMetaFolder.h"
#include "MgaMetaProject.h"

// --------------------------- CMgaMetaFolder

// ------- Traverse

STDMETHODIMP CMgaMetaFolder::get_DefinedFolderByName(BSTR name, VARIANT_BOOL inscope, IMgaMetaFolder **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());

		CComBstrObj_lightequal equal( GetUnknown());
		coreobjects_type coreobjects;
		self.GetCollectionValue(ATTRID_DEFFOLDER_PTR, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComBstrObj n;
			(*i).GetStringValue(ATTRID_NAME, PutOut(n));
			
			if( equal( n, name))//if( n == name )
				COMRETURN( ::QueryInterface(*i, p) );

			++i;
		}

		if( inscope != VARIANT_FALSE )
		{
			CComObjPtr<IMgaMetaFolder> parent;
			COMTHROW( get_DefinedIn(PutOut(parent)) );
			
			if( parent != NULL )
				COMRETURN( parent->get_DefinedFolderByName(name, inscope, p) );
		}

		COMRETURN(E_NOTFOUND);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaFolder::get_DefinedFCOByName(BSTR name, VARIANT_BOOL inscope, IMgaMetaFCO **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());

		CComBstrObj_lightequal equal( GetUnknown());
		coreobjects_type coreobjects;
		self.GetCollectionValue(ATTRID_DEFFCO_PTR, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComBstrObj n;
			(*i).GetStringValue(ATTRID_NAME, PutOut(n));
			
			if( equal( n, name))//if( n == name )
				COMRETURN( ::QueryInterface(*i, p) );

			++i;
		}

		if( inscope != VARIANT_FALSE )
		{
			CComObjPtr<IMgaMetaFolder> parent;
			COMTHROW( get_DefinedIn(PutOut(parent)) );
			
			if( parent != NULL )
				COMRETURN( parent->get_DefinedFCOByName(name, inscope, p) );
		}

		COMRETURN(E_NOTFOUND);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaFolder::get_DefinedAttributeByName(BSTR name,
	VARIANT_BOOL inscope, IMgaMetaAttribute **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());

		coreobjects_type coreobjects;
		self.GetCollectionValue(ATTRID_DEFATTR_PTR, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComBstrObj n;
			(*i).GetStringValue(ATTRID_NAME, PutOut(n));
			
			if( n == name )
				COMRETURN( ::QueryInterface(*i, p) );

			++i;
		}

		if( inscope != VARIANT_FALSE )
		{
			CComObjPtr<IMgaMetaFolder> parent;
			COMTHROW( get_DefinedIn(PutOut(parent)) );
			
			if( parent != NULL )
				COMRETURN( parent->get_DefinedAttributeByName(name, inscope, p) );
		}

		COMRETURN(E_NOTFOUND);
	}
	COMCATCH(;)
}

void CMgaMetaFolder::Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me)
{
	ASSERT( metaproject != NULL );
	ASSERT( me != NULL );

	CMgaMetaBase::Traverse(metaproject, me);

	TraverseCollection(metaproject, me, ATTRID_DEFFOLDER_PTR);
	TraverseCollection(metaproject, me, ATTRID_DEFFCO_PTR);
	TraverseCollection(metaproject, me, ATTRID_DEFATTR_PTR);
	TraverseCollection(metaproject, me, ATTRID_FOLDERLINK_CHILD_PTR);
	TraverseCollection(metaproject, me, ATTRID_ROOTOBJLINK_FOLDER_PTR);
}
