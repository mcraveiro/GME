
#include "stdafx.h"
#include "MgaMetaRegNode.h"

// --------------------------- CMgaMetaRegNodes

HRESULT CMgaMetaRegNodes::get_RegistryNode(IUnknown *me, BSTR bpath, IMgaMetaRegNode **p)
{
	ASSERT( me != NULL );

	CHECK_OUT(p);

	COMTRY
	{
		CCoreObjectPtr self(me);

		std::wstring path = bpath ? bpath : L"";

		CComObjPtr<ICoreProject> coreproject;
		COMTHROW( self->get_Project(PutOut(coreproject)) );
		ASSERT( coreproject != NULL );

		for(;;)
		{
			size_t pos = path.find(L'/');//pos will be either in range 0..len-1 or will be equal with string::npos 
			std::wstring name(path, 0, pos);

			CComBstrObj bname(name.c_str());

			CCoreObjectPtr node;

			coreobjects_type coreobjects;
			self.GetCollectionValue(ATTRID_REGNODES_COLL, coreobjects);

			coreobjects_iterator i = coreobjects.begin();
			coreobjects_iterator e = coreobjects.end();
			while( i != e )
			{
				CComBstrObj bstr;
				i->GetStringValue(ATTRID_NAME, PutOut(bstr));

				if( bname == bstr )
				{
					node = *i;
					break;
				}

				++i;
			}

			if( node == NULL )
			{
				HRESULT hr = coreproject->CreateObject(METAID_METAREGNODE, PutOut(node));

				if( hr == E_TRANSACTION )
					COMRETURN(E_NOTFOUND);
				COMTHROW(hr);

				ASSERT( node != NULL );

				node.PutPointerValue(ATTRID_REGNODES_COLL, self);
				node.PutStringValue(ATTRID_NAME, PutInBstr(name));
			}

			ASSERT( node != NULL );

			if( pos == std::string::npos) //<-- correct test, instead of "if( (unsigned int) pos >= path.length() )"
			{
				COMTHROW( ::QueryInterface(node, p) );
				return S_OK;
			}

			self = node;
			path.erase(0, pos+1);
		}
	}
	COMCATCH(;)
}

HRESULT CMgaMetaRegNodes::get_RegistryValue(IUnknown *me, BSTR path, BSTR *p)
{
	CComObjPtr<IMgaMetaRegNode> node;

	HRESULT hr;
	if( FAILED(hr = get_RegistryNode(me, path, PutOut(node))) )
	{
		if( hr == E_NOTFOUND )
			hr = S_OK;

		return hr;
	}

	ASSERT( node != NULL );
	return node->get_Value(p);
}

HRESULT CMgaMetaRegNodes::put_RegistryValue(IUnknown *me, BSTR path, BSTR p)
{
	CComObjPtr<IMgaMetaRegNode> node;

	HRESULT hr;
	if( FAILED(hr = get_RegistryNode(me, path, PutOut(node))) )
	{
		if( hr == E_NOTFOUND )
			hr = S_OK;

		return hr;
	}

	ASSERT( node != NULL );
	return node->put_Value(p);
}

// --------------------------- CMgaMetaRegNode

STDMETHODIMP CMgaMetaRegNode::Delete()
{
	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());
	
		self.PutLockValue(ATTRID_LOCK, LOCKING_EXCLUSIVE);

		coreobjects_type coreobjects;
		self.LoadCollectionValue(ATTRID_REGNODES_COLL, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComObjPtr<IMgaMetaRegNode> node;
			COMTHROW( ::QueryInterface(*i, node) );

			COMTHROW( node->Delete() );

			++i;
		}

		COMTHROW( self->Delete() );
	}
	COMCATCH(;)
}

// ------- Traverse

void CMgaMetaRegNode::Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me)
{
	ASSERT( metaproject != NULL );
	ASSERT( me != NULL );

	TraverseCollection(metaproject, me, ATTRID_REGNODES_COLL);
}
