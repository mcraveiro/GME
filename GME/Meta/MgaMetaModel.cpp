
#include "stdafx.h"
#include "MgaMetaModel.h"
#include "MgaMetaConnection.h"
#include "MgaMetaPointerSpec.h"

// --------------------------- CMgaMetaModel

STDMETHODIMP CMgaMetaModel::get_DefinedFCOByName(BSTR name, VARIANT_BOOL inscope, IMgaMetaFCO **p)
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
			
			if (equal(n, name)) {
				HRESULT hr = ::QueryInterface(*i, p);
				if (FAILED(hr))
					COMTHROW(hr);
				return hr;
			}

			++i;
		}

		if( inscope == VARIANT_FALSE )
			COMRETURN(E_NOTFOUND);

		CComObjPtr<IMgaMetaBase> parent;
		COMTHROW( get_DefinedIn(PutOut(parent)) );
		ASSERT( parent != NULL );

		CComObjPtr<IMgaMetaFolder> folder;
		if( SUCCEEDED(parent.QueryInterface(folder)) ) {
			*p = folder->DefinedFCOByName[name, inscope].Detach();
			return S_OK;
		}

		CComObjPtr<IMgaMetaModel> model;
		COMTHROW( parent.QueryInterface(model) );
		*p = model->DefinedFCOByName[name, inscope].Detach();
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaModel::get_RoleByName(BSTR name, IMgaMetaRole **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());

		CComBstrObj_lightequal equal( GetUnknown());
		coreobjects_type coreobjects;
		self.GetCollectionValue(ATTRID_ROLES_COLL, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComBstrObj n;
			(*i).GetStringValue(ATTRID_NAME, PutOut(n));
			
			if (equal(n, name)) {
				HRESULT hr = ::QueryInterface(*i, p);
				if (FAILED(hr))
					COMTHROW(hr);
				return hr;
			}

			++i;
		}

		throw_com_error(E_NOTFOUND, _bstr_t(L"Role name '") + name + L"' not found in model " + static_cast<CMgaMetaFCO *const>(this)->GetName());
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaModel::get_AspectByName(BSTR name, IMgaMetaAspect **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());

		coreobjects_type coreobjects;
		self.GetCollectionValue(ATTRID_ASPECTS_COLL, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComBstrObj n;
			(*i).GetStringValue(ATTRID_NAME, PutOut(n));
			
			if( n == name ) {
				HRESULT hr =::QueryInterface(*i, p);
				if (FAILED(hr))
					COMTHROW(hr);
				return hr;
			}
			++i;
		}

		COMTHROW(E_NOTFOUND);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaModel::LegalConnectionRoles(BSTR paths, IMgaMetaRoles **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		// create the collection
		CComObjPtr<RolesExCOMType> coll;
		CreateComObject(coll);

		// prepare the jointpaths
		jointpaths_type jointpaths;

		ASSERT( metaprojectref != NULL );
		metaproject->CreateJointPaths(paths, jointpaths);

		// traverse the roles
		CCoreObjectPtr self(GetUnknown());

		coreobjects_type roles;
		self.GetCollectionValue(ATTRID_ROLES_COLL, roles);

		coreobjects_iterator i = roles.begin();
		coreobjects_iterator e = roles.end();
		while( i != e )
		{
			CCoreObjectPtr kind;
			i->GetPointerValue(ATTRID_KIND_PTR, kind);

			if( kind.GetMetaID() == METAID_METACONNECTION &&
				CMgaMetaConnection::CheckPaths(kind, jointpaths) )
			{
				CComObjPtr<IMgaMetaRole> role;
				COMTHROW( ::QueryInterface(*i, role) );

				coll->Add(role);
			}

			++i;
		}

		// return
		MoveTo(coll, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaModel::LegalReferenceRoles(BSTR path, IMgaMetaRoles **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		// create the collection
		CComObjPtr<RolesExCOMType> coll;
		CreateComObject(coll);

		// prepare the pathitems
		pathitems_type pathitems;

		ASSERT( metaprojectref != NULL );
		metaproject->CreatePathItems(begin(path), end(path), pathitems);

		// traverse the roles
		CCoreObjectPtr self(GetUnknown());

		coreobjects_type roles;
		self.GetCollectionValue(ATTRID_ROLES_COLL, roles);

		coreobjects_iterator i = roles.begin();
		coreobjects_iterator e = roles.end();
		while( i != e )
		{
			CCoreObjectPtr kind;
			i->GetPointerValue(ATTRID_KIND_PTR, kind);

			if( kind.GetMetaID() == METAID_METAREFERENCE &&
				CMgaMetaPointerSpec::CheckPath(kind, pathitems, true) )
			{
				CComObjPtr<IMgaMetaRole> role;
				COMTHROW( ::QueryInterface(*i, role) );

				coll->Add(role);
			}

			++i;
		}

		// return
		MoveTo(coll, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaModel::LegalSetRoles(BSTR path, IMgaMetaRoles **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		// create the collection
		CComObjPtr<RolesExCOMType> coll;
		CreateComObject(coll);

		// prepare the pathitems
		pathitems_type pathitems;

		ASSERT( metaprojectref != NULL );
		metaproject->CreatePathItems(begin(path), end(path), pathitems);

		// traverse the roles
		CCoreObjectPtr self(GetUnknown());

		coreobjects_type roles;
		self.GetCollectionValue(ATTRID_ROLES_COLL, roles);

		coreobjects_iterator i = roles.begin();
		coreobjects_iterator e = roles.end();
		while( i != e )
		{
			CCoreObjectPtr kind;
			i->GetPointerValue(ATTRID_KIND_PTR, kind);

			if( kind.GetMetaID() == METAID_METASET &&
				CMgaMetaPointerSpec::CheckPath(kind, pathitems, false) )
			{
				CComObjPtr<IMgaMetaRole> role;
				COMTHROW( ::QueryInterface(*i, role) );

				coll->Add(role);
			}

			++i;
		}

		// return
		MoveTo(coll, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaModel::LegalRoles(IMgaMetaFCO *kind, IMgaMetaRoles **p)
{
	CHECK_OUT(p);

	if( kind == NULL )
		return E_INVALIDARG;

	COMTRY
	{
		// create the collection

		CComObjPtr<RolesExCOMType> coll;
		CreateComObject(coll);

		// traverse the roles
		CCoreObjectPtr self(GetUnknown());

		coreobjects_type roles;
		self.GetCollectionValue(ATTRID_ROLES_COLL, roles);

		coreobjects_iterator i = roles.begin();
		coreobjects_iterator e = roles.end();
		while( i != e )
		{
			CCoreObjectPtr rolekind;
			i->GetPointerValue(ATTRID_KIND_PTR, rolekind);

			if( IsEqualObject(rolekind,kind) )
			{
				CComObjPtr<IMgaMetaRole> role;
				COMTHROW( ::QueryInterface(*i, role) );

				coll->Add(role);
			}

			++i;
		}

		// return
		MoveTo(coll, p);
	}
	COMCATCH(;)
}

// ------- Edit

STDMETHODIMP CMgaMetaModel::CreateRole(IMgaMetaFCO *kind, IMgaMetaRole **p)
{
	CHECK_OUT(p);

	ASSERT( metaprojectref != NULL );

	COMTRY
	{
		if( kind == NULL )
			COMTHROW(E_POINTER);

		CCoreObjectPtr self(GetUnknown());
		ASSERT( self != NULL );

		CCoreObjectPtr other(kind);
		ASSERT( other != NULL );

		CCoreObjectPtr role;
		metaproject->CreateMetaBase(METAID_METAROLE, role);

		role.PutPointerValue(ATTRID_ROLES_COLL, self);
		role.PutPointerValue(ATTRID_KIND_PTR, other);

		COMTHROW( ::QueryInterface(role, p) );
	}
	COMCATCH(;)
}

// ------- Traverse

void CMgaMetaModel::Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me)
{
	ASSERT( metaproject != NULL );
	ASSERT( me != NULL );

	CMgaMetaFCO::Traverse(metaproject, me);

	TraverseCollection(metaproject, me, ATTRID_DEFFCO_PTR);
	TraverseCollection(metaproject, me, ATTRID_ROLES_COLL);
	TraverseCollection(metaproject, me, ATTRID_ASPECTS_COLL);
}
