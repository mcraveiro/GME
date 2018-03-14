// MgaFolder.h : Declaration of the CMgaFolder

#ifndef __MGAFOLDER_H_
#define __MGAFOLDER_H_

#include "resource.h"       // main symbols
#include "MgaFCO.h"
/////////////////////////////////////////////////////////////////////////////

// CMgaContainer


template <class T> 
class ATL_NO_VTABLE IMgaContainerImpl : 	public T {
public:

	STDMETHOD(GetChildrenOfKind)(BSTR kn, IMgaFCOs **pVal) { return inFCO->GetChildrenOfKind(kn, pVal); }
	STDMETHOD(GetDescendantFCOs)(IMgaFilter* filt, IMgaFCOs **pVal) { 
													return inFCO->GetDescendantFCOs(filt, pVal); };
	STDMETHOD(get_ChildFCOs)(IMgaFCOs ** pVal)    { return inFCO->get_ChildFCOs(pVal); };
	STDMETHOD(get_ChildFCO)(BSTR n,IMgaFCO**pVal) { return inFCO->get_ChildFCO(n, pVal); };
	STDMETHOD(GetChildFCODisp)(BSTR n,IMgaFCO**pVal) { return inFCO->get_ChildFCO(n, pVal); };


};


// CMgaFolder
class ATL_NO_VTABLE CMgaFolder : 
	public CComCoClass<CMgaFolder, &__uuidof(MgaFolder)>,
	public IMgaContainerImpl < IMgaFCOImpl <CComObjectRootEx<CComSingleThreadModel> ,
											 IDispatchImpl<IMgaFolder, &__uuidof(IMgaFolder), &__uuidof(__MGALib)> > >,
	public ISupportErrorInfoImpl<&__uuidof(IMgaFolder)>
{
public:

DECLARE_ONLY_AGGREGATABLE(_CoClass)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaFolder)
	COM_INTERFACE_ENTRY(IMgaFolder)
	COM_INTERFACE_ENTRY_IID(__uuidof(IMgaObject),IMgaFolder)
//	COM_INTERFACE_ENTRY_IID(IID_IMgaContainer,IMgaFolder)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IID(IID_ISupportErrorInfo, IMyErrorInfoBase)
END_COM_MAP()

// IMgaFolder
	STDMETHOD(DeriveRootObject)(IMgaFCO* base, VARIANT_BOOL instance, IMgaFCO **nobj) {
											return inFCO->DeriveFCO(base, NULL, instance, nobj); 	}
	STDMETHOD(CreateRootObject)(IMgaMetaFCO* meta, IMgaFCO **nobj) {
											return inFCO->CreateRootObject(meta,nobj); }
	STDMETHOD(CreateFolder)(IMgaMetaFolder *meta, IMgaFolder **nf) {
											return inFCO->CreateFolder(meta, nf); }
	STDMETHOD(get_ChildFolders)(IMgaFolders **pVal) {
											return inFCO->get_ChildFolders(pVal); }
	STDMETHOD(get_ParentFolder)(IMgaFolder **pVal) { return 
		inFCO->GetParent(reinterpret_cast<IMgaContainer **>(pVal),NULL); }

	STDMETHOD(get_MetaFolder)(IMgaMetaFolder **pVal) { return inFCO->get_MetaFolder(pVal); } 
	STDMETHOD(CopyFCOs)(IMgaFCOs *list, IMgaFCOs **objs) { return inFCO->CopyFCOs(list, NULL, objs); } 
	STDMETHOD(MoveFCOs)(IMgaFCOs *list, IMgaFCOs **objs) { return inFCO->MoveFCOs(list, NULL, objs); } 
	STDMETHOD(CopyFolders)(IMgaFolders *list, IMgaFolders **objs) { return inFCO->CopyFolders(list, /*NULL,*/ objs); } 
	STDMETHOD(MoveFolders)(IMgaFolders *list, IMgaFolders **objs) { return inFCO->MoveFolders(list, /*NULL,*/ objs); } 
	STDMETHOD(CopyFCODisp)(IMgaFCO *obj, IMgaFCO **nobj) { return inFCO->CopyFCODisp( obj, NULL, nobj); } 
	STDMETHOD(MoveFCODisp)(IMgaFCO *obj, IMgaFCO **nobj) { return inFCO->MoveFCODisp( obj, NULL, nobj); } 
	STDMETHOD(CopyFolderDisp)(IMgaFolder *fol, IMgaFolder **nfol) { return inFCO->CopyFolderDisp( fol, nfol); } 
	STDMETHOD(MoveFolderDisp)(IMgaFolder *fol, IMgaFolder **nfol) { return inFCO->MoveFolderDisp( fol, nfol); } 
	//STDMETHOD(MoveFCOsAndFolders)(IMgaFolders *list, IMgaFCOs *list2, IMgaFolders **objs, IMgaFCOs **objs2) { return inFCO->MoveFCOsAndFolders(list, list2, objs, objs2); } 
	STDMETHOD(RefreshParent)(IMgaFolder *parent) { return inFCO->RefreshParent( parent); }

	STDMETHOD(AttachLibraryV3)(BSTR libname, VARIANT_BOOL ungroup, IMgaFolder **nf) 
												{ return inFCO->AttachLibraryV3(libname, ungroup, nf); }
	STDMETHOD(RefreshLibraryV3)(BSTR libname, VARIANT_BOOL ungroup, long *numOfErrors) 
												{ return inFCO->RefreshLibraryV3(libname, ungroup, numOfErrors); }

	STDMETHOD(AttachLibrary)(BSTR libname, IMgaFolder **nf) 
												{ return inFCO->AttachLibrary(libname, nf); }
	STDMETHOD(RefreshLibrary)(BSTR libname) 
												{ return inFCO->RefreshLibrary(libname); }
	STDMETHOD(get_LibraryName)(BSTR *libname) 
												{ return inFCO->get_LibraryName(libname); }
	STDMETHOD(put_LibraryName)(BSTR libname) 
												{ return inFCO->put_LibraryName(libname); }

	STDMETHOD(GetVirtuallyIncludedBy)( IMgaFolders** pDep) { return inFCO->GetVirtuallyIncludedBy( pDep); }
	STDMETHOD(SetVirtuallyIncludedBy)( IMgaFolders*  pDep) { return inFCO->SetVirtuallyIncludedBy( pDep); }
	STDMETHOD(GetVirtuallyIncludes)  ( IMgaFolders** pDep) { return inFCO->GetVirtuallyIncludes  ( pDep); }
	STDMETHOD(SetVirtuallyIncludes)  ( IMgaFolders*  pDep) { return inFCO->SetVirtuallyIncludes  ( pDep); }

	STDMETHOD(GetSourceControlInfo)( long* p_scInfo)    { return inFCO->GetSourceControlInfo( p_scInfo); }

	STDMETHOD(get_ChildRelIDCounter)(long *pVal) { 
		return inFCO->get_ChildRelIDCounter(pVal); }
	STDMETHOD(put_ChildRelIDCounter)(long pVal) { 
		return inFCO->put_ChildRelIDCounter(pVal); }
	
	STDMETHOD(get_Attributes)(/*[out, retval]*/ IMgaAttributes * *pVal)							{ return E_MGA_NOT_SUPPORTED; };
	STDMETHOD(get_Attribute)(/*[in]*/ IMgaMetaAttribute *meta, /*[out, retval]*/ VARIANT *pVal) { return E_MGA_NOT_SUPPORTED; };
	STDMETHOD(get_AttributeByName)(/*[in]*/ BSTR name, /*[out, retval]*/ VARIANT *pVal)			{ return E_MGA_NOT_SUPPORTED; };
	STDMETHOD(put_AttributeByName)(/*[in]*/ BSTR name, /*[in]*/ VARIANT newVal)					{ return E_MGA_NOT_SUPPORTED; };

	STDMETHOD(get_StrAttrByName)( BSTR name,  BSTR *pVal) {  return E_MGA_NOT_SUPPORTED; }
	STDMETHOD(put_StrAttrByName)( BSTR name,  BSTR newval) {  return E_MGA_NOT_SUPPORTED; }
	STDMETHOD(get_IntAttrByName)( BSTR name,  long *pVal) {  return E_MGA_NOT_SUPPORTED; }
	STDMETHOD(put_IntAttrByName)( BSTR name,  long newval) {  return E_MGA_NOT_SUPPORTED; }
	STDMETHOD(get_FloatAttrByName)( BSTR name,  double *pVal) {  return E_MGA_NOT_SUPPORTED; }
	STDMETHOD(put_FloatAttrByName)( BSTR name,  double newval) {  return E_MGA_NOT_SUPPORTED; }
	STDMETHOD(get_BoolAttrByName)( BSTR name,  VARIANT_BOOL *pVal) {  return E_MGA_NOT_SUPPORTED; }
	STDMETHOD(put_BoolAttrByName)( BSTR name,  VARIANT_BOOL newval) {  return E_MGA_NOT_SUPPORTED; }
	STDMETHOD(get_RefAttrByName)( BSTR name,  IMgaFCO **pVal) {  return E_MGA_NOT_SUPPORTED; }
	STDMETHOD(put_RefAttrByName)( BSTR name,  IMgaFCO * newval) {  return E_MGA_NOT_SUPPORTED; }
	STDMETHOD(RemoveAttrByName)( BSTR name) {  return E_MGA_NOT_SUPPORTED; }
	STDMETHOD(get_Meta)(IMgaMetaFCO * *pVal) { return E_MGA_NOT_SUPPORTED; }
	// by ZolMol
	//STDMETHOD(get_FolderMeta)(IMgaMetaFolder * *pVal) { return inFCO->get_MetaFolder( pVal); }

};

#endif //__MGAFOLDER_H_
