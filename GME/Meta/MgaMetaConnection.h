
#ifndef MGA_MGAMETACONNECTION_H
#define MGA_MGAMETACONNECTION_H

#ifndef MGA_MGAMETAFCO_H
#include "MgaMetaFCO.h"
#endif

// --------------------------- CMgaMetaConnection

class ATL_NO_VTABLE CMgaMetaConnection : 
	public CComCoClass<CMgaMetaConnection, &__uuidof(MgaMetaConnection)>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IMgaMetaConnection, &__uuidof(IMgaMetaConnection), &__uuidof(__MGAMetaLib)>,
	public CMgaMetaFCO
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaConnection*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETACONNECTION)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaConnection)

BEGIN_COM_MAP(CMgaMetaConnection)
	COM_INTERFACE_ENTRY(IMgaMetaConnection)
	COM_INTERFACE_ENTRY(IMgaMetaFCO)
	COM_INTERFACE_ENTRY2(IMgaMetaBase, CMgaMetaBase)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaConnection)
END_COM_MAP()

DECLARE_MGAMETAFCO()

public:
	STDMETHOD(get_Joints)(IMgaMetaConnJoints **p)
	{ return ComGetCollectionValue<IMgaMetaConnJoint>(GetUnknown(), ATTRID_CONNJOINTS_COLL, p); }

	STDMETHOD(CheckPaths)(BSTR paths, VARIANT_BOOL *p);

	STDMETHOD(get_IsSimple)(VARIANT_BOOL *p)
	{
		CHECK_OUT(p);

		*p = VARIANT_TRUE;
		return S_OK;
	}

// ------- Edit

public:
	STDMETHOD(CreateJoint)(IMgaMetaConnJoint **p);

// ------- Methods

public:
	static void Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me);
	static bool CheckPaths(CCoreObjectPtr &self, jointpaths_type jointpaths);

};

#endif//MGA_MGAMETACONNECTION_H
