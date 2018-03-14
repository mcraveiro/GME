
#ifndef MGA_MGAMETAREGNODE_H
#define MGA_MGAMETAREGNODE_H

// --------------------------- CMgaMetaRegNodes

class ATL_NO_VTABLE CMgaMetaRegNodes
{
public:
	static HRESULT get_RegistryNode(IUnknown *me, BSTR path, IMgaMetaRegNode **p);
	static HRESULT get_RegistryValue(IUnknown *me, BSTR path, BSTR *p);
	static HRESULT put_RegistryValue(IUnknown *me, BSTR path, BSTR p);
};

#define DECLARE_MGAMETAREGNODES() \
\
public: \
	STDMETHOD(get_RegistryNodes)(IMgaMetaRegNodes **p) \
	{ return ComGetCollectionValue<IMgaMetaRegNode>(GetUnknown(), ATTRID_REGNODES_COLL, p); } \
\
	STDMETHOD(get_RegistryNode)(BSTR path, IMgaMetaRegNode **p) \
	{ return CMgaMetaRegNodes::get_RegistryNode(GetUnknown(), path, p); } \
\
	STDMETHOD(GetRegistryNodeDisp)(BSTR path, IMgaMetaRegNode **p) \
	{ return get_RegistryNode( path, p); } \
\
	STDMETHOD(get_RegistryValue)(BSTR path, BSTR *p) \
	{ return CMgaMetaRegNodes::get_RegistryValue(GetUnknown(), path, p); } \
\
	STDMETHOD(GetRegistryValueDisp)(BSTR path, BSTR *p) \
	{ return get_RegistryValue( path, p); } \
\
	STDMETHOD(put_RegistryValue)(BSTR path, BSTR p) \
	{ return CMgaMetaRegNodes::put_RegistryValue(GetUnknown(), path, p); } \
\
	STDMETHOD(SetRegistryValueDisp)(BSTR path, BSTR p) \
	{ return put_RegistryValue( path, p); }


// --------------------------- CMgaMetaRegNode

class ATL_NO_VTABLE CMgaMetaRegNode : 
	public CComCoClass<CMgaMetaRegNode, &__uuidof(MgaMetaRegNode)>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IMgaMetaRegNode, &__uuidof(IMgaMetaRegNode), &__uuidof(__MGAMetaLib)>,
	public CMgaMetaRegNodes
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaRegNode*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETAREGNODE)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaRegNode)

BEGIN_COM_MAP(CMgaMetaRegNode)
	COM_INTERFACE_ENTRY(IMgaMetaRegNode)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

DECLARE_MGAMETAREGNODES()

public:
	STDMETHOD(get_Name)(BSTR *p)
	{ return ComGetAttrValue(GetUnknown(), ATTRID_NAME, p); }

	STDMETHOD(get_Value)(BSTR *p)
	{ return ComGetAttrValue(GetUnknown(), ATTRID_VALUE, p); }

	STDMETHOD(put_Value)(BSTR p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_VALUE, p); }

	STDMETHOD(Delete)();

// ------- Traverse

public:
	static void Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me);
};

#endif//MGA_MGAMETAREGNODE_H
