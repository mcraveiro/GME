
#ifndef MGA_MGAMETAFOLDER_H
#define MGA_MGAMETAFOLDER_H

#ifndef MGA_MGAMETABASE_H
#include "MgaMetaBase.h"
#endif

// --------------------------- CMgaMetaFolder

class ATL_NO_VTABLE CMgaMetaFolder : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaMetaFolder, &__uuidof(MgaMetaFolder)>,
	public IDispatchImpl<IMgaMetaFolder, &__uuidof(IMgaMetaFolder), &__uuidof(__MGAMetaLib)>,
	public CMgaMetaBase
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaFolder*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETAFOLDER)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaFolder)

BEGIN_COM_MAP(CMgaMetaFolder)
	COM_INTERFACE_ENTRY(IMgaMetaFolder)
	COM_INTERFACE_ENTRY2(IMgaMetaBase, CMgaMetaBase)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaFolder)
END_COM_MAP()

DECLARE_MGAMETABASE()

// ------- Methods

public:
	STDMETHOD(get_DefinedIn)(IMgaMetaFolder **p)
	{ return ComGetPointerValue(GetUnknown(), ATTRID_DEFFOLDER_PTR, p); }

	STDMETHOD(get_DefinedFolders)(IMgaMetaFolders **p)
	{ return ComGetCollectionValue<IMgaMetaFolder>(GetUnknown(), ATTRID_DEFFOLDER_PTR, p); }

	STDMETHOD(get_DefinedFolderByName)(BSTR name, VARIANT_BOOL inscope, IMgaMetaFolder **p);

	STDMETHOD(GetDefinedFolderByNameDisp)(BSTR name, VARIANT_BOOL inscope, IMgaMetaFolder **p)
	{ return get_DefinedFolderByName( name, inscope, p); }

	STDMETHOD(get_DefinedFCOs)(IMgaMetaFCOs **p)
	{ 	return ComGetSortedCollValue<IMgaMetaFCO>(GetUnknown(), ATTRID_DEFFCO_PTR, p); }

	STDMETHOD(get_DefinedFCOByName)(BSTR name, VARIANT_BOOL inscope, IMgaMetaFCO **p);

	STDMETHOD(GetDefinedFCOByNameDisp)(BSTR name, VARIANT_BOOL inscope, IMgaMetaFCO **p)
	{ return get_DefinedFCOByName( name, inscope, p); }

	STDMETHOD(get_DefinedAttributes)(IMgaMetaAttributes **p)
	{ return ComGetSortedCollValue<IMgaMetaAttribute>(GetUnknown(), ATTRID_DEFATTR_PTR, p); }

	STDMETHOD(get_DefinedAttributeByName)(BSTR name, VARIANT_BOOL inscope, IMgaMetaAttribute **p);

	STDMETHOD(GetDefinedAttributeByNameDisp)(BSTR name, VARIANT_BOOL inscope, IMgaMetaAttribute **p)
	{ return get_DefinedAttributeByName( name, inscope, p); }

	STDMETHOD(get_LegalChildFolders)(IMgaMetaFolders **p)
	{ return ComGetSortedLinkCollValue<IMgaMetaFolder>(GetUnknown(),
		ATTRID_FOLDERLINK_PARENT_PTR, ATTRID_FOLDERLINK_CHILD_PTR, p); }

	STDMETHOD(get_LegalChildFolderByName)(BSTR name, IMgaMetaFolder **p)
	{ return ComGetLinkCollValueByName(name, GetUnknown(), 
		ATTRID_FOLDERLINK_PARENT_PTR, ATTRID_FOLDERLINK_CHILD_PTR, p); }

	STDMETHOD(GetLegalChildFolderByNameDisp)(BSTR name, IMgaMetaFolder **p)
	{ return get_LegalChildFolderByName( name, p); }

	STDMETHOD(get_LegalRootObjects)(IMgaMetaFCOs **p)
	{   return ComGetSortedLinkCollValue<IMgaMetaFCO>(GetUnknown(),
		ATTRID_ROOTOBJLINK_FOLDER_PTR, ATTRID_ROOTOBJLINK_OBJECT_PTR, p); }

	STDMETHOD(get_LegalRootObjectByName)(BSTR name, IMgaMetaFCO **p)
	{ return ComGetLinkCollValueByName(name, GetUnknown(),
		ATTRID_ROOTOBJLINK_FOLDER_PTR, ATTRID_ROOTOBJLINK_OBJECT_PTR, p); }

	STDMETHOD(GetLegalRootObjectByNameDisp)(BSTR name, IMgaMetaFCO **p)
	{ return get_LegalRootObjectByName( name, p); }

	STDMETHOD(get_UsedInFolders)(IMgaMetaFolders **p)
	{ return ComGetLinkCollectionValue<IMgaMetaFolder>(GetUnknown(),
		ATTRID_FOLDERLINK_CHILD_PTR, ATTRID_FOLDERLINK_PARENT_PTR, p); }

// ------- Edit

public:
	STDMETHOD(DefineFolder)(IMgaMetaFolder **p)
	{ return ComDefineBase(this, METAID_METAFOLDER, ATTRID_DEFFOLDER_PTR, p); }

	STDMETHOD(DefineModel)(IMgaMetaModel **p)
	{ return ComDefineBase(this, METAID_METAMODEL, ATTRID_DEFFCO_PTR, p); }

	STDMETHOD(DefineAtom)(IMgaMetaAtom **p)
	{ return ComDefineBase(this, METAID_METAATOM, ATTRID_DEFFCO_PTR, p); }

	STDMETHOD(DefineReference)(IMgaMetaReference **p)
	{ return ComDefineBase(this, METAID_METAREFERENCE, ATTRID_DEFFCO_PTR, p); }

	STDMETHOD(DefineSet)(IMgaMetaSet **p)
	{ return ComDefineBase(this, METAID_METASET, ATTRID_DEFFCO_PTR, p); }

	STDMETHOD(DefineConnection)(IMgaMetaConnection **p)
	{ return ComDefineBase(this, METAID_METACONNECTION, ATTRID_DEFFCO_PTR, p); }

	STDMETHOD(DefineAttribute)(IMgaMetaAttribute **p)
	{ return ComDefineBase(this, METAID_METAATTRIBUTE, ATTRID_DEFATTR_PTR, p); }

	STDMETHOD(AddLegalChildFolder)(IMgaMetaFolder *p)
	{ return ComAddLink(this, METAID_METAFOLDERLINK, ATTRID_FOLDERLINK_PARENT_PTR,
		ATTRID_FOLDERLINK_CHILD_PTR, p); }

	STDMETHOD(RemoveLegalChildFolder)(IMgaMetaFolder *p)
	{ return ComRemoveLink(this, METAID_METAFOLDERLINK, ATTRID_FOLDERLINK_PARENT_PTR,
		ATTRID_FOLDERLINK_CHILD_PTR, p); }

	STDMETHOD(AddLegalRootObject)(IMgaMetaFCO *p)
	{ return ComAddLink(this, METAID_METAROOTOBJLINK, ATTRID_ROOTOBJLINK_FOLDER_PTR,
		ATTRID_ROOTOBJLINK_OBJECT_PTR, p); }

	STDMETHOD(RemoveLegalRootObject)(IMgaMetaFCO *p)
	{ return ComRemoveLink(this, METAID_METAROOTOBJLINK, ATTRID_ROOTOBJLINK_FOLDER_PTR,
		ATTRID_ROOTOBJLINK_OBJECT_PTR, p); }

// ------- Traverse

public:
	static void Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me);
};

#endif//MGA_MGAMETAFOLDER_H
