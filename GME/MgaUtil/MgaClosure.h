#ifndef MGACLOSURE_H
#define MGACLOSURE_H

// --------------------------- CMgaClosure
//

class ATL_NO_VTABLE CMgaClosure 
	: public CComObjectRootEx<CComSingleThreadModel>
	, public CComCoClass<CMgaClosure, &__uuidof(MgaClosure)>
	, public IDispatchImpl<IMgaClosure, &__uuidof(IMgaClosure), &__uuidof(__MGAUtilLib)>  
{
public:
	CMgaClosure() { }
	~CMgaClosure() { }

DECLARE_REGISTRY_RESOURCEID(IDR_MGACLOSURE)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaClosure)
 	COM_INTERFACE_ENTRY(IMgaClosure)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ------- Properties

	STDMETHOD(GetLastClosureId)( IMgaProject* p_project, long* p_pMarkWith);
	STDMETHOD(PutLastClosureId)( IMgaProject* p_project, long  p_iMarkWith);

	STDMETHOD(SelectiveClosureDlg)
		( VARIANT_BOOL  p_folderContainmentNeeded
		, VARIANT_BOOL* p_refersTo           , VARIANT_BOOL* p_referredBy
		, VARIANT_BOOL* p_connections        , VARIANT_BOOL* p_connsThroughRefport
		, VARIANT_BOOL* p_setMembers         , VARIANT_BOOL* p_memberOfSets
		, VARIANT_BOOL* p_modelContainment   , VARIANT_BOOL* p_partOfModels
		, VARIANT_BOOL* p_folderContainment  , VARIANT_BOOL* p_partOfFolders
		, VARIANT_BOOL* p_baseTypeOf         , VARIANT_BOOL* p_derivedTypesOf
		, long* p_libraryElementHandling          // 0: stubs, 1: stop, 2: continue
		, long* p_wrappingMode                    // 0: miniproject, 1: automatic, 2: as is
		, long* p_kindFilter                      // -1 = all, otherwise calculate the 0b000rsamf mask, where f = Folders, m = Models, a = Atoms, s = Sets, r = References
		, long* p_markWith                        // mark all objects in closure
		, long* p_options
		, VARIANT_BOOL* p_directionDown
		, VARIANT_BOOL* p_copyToFile              // file or clipboard
		, VARIANT_BOOL* p_okPressed               // how ended the dialog
		, BSTR *p_userSelFileName
		);

	STDMETHOD(SelectiveClosureCalc)
		( IMgaFCOs *p_initSelFcos          , IMgaFolders *p_initSelFolders
		, VARIANT_BOOL p_refersTo          , VARIANT_BOOL p_referredBy
		, VARIANT_BOOL p_connections       , VARIANT_BOOL p_connsThroughRefport
		, VARIANT_BOOL p_setMembers        , VARIANT_BOOL p_memberOfSets
		, VARIANT_BOOL p_modelContainment  , VARIANT_BOOL p_partOfModels
		, VARIANT_BOOL p_folderContainment , VARIANT_BOOL p_partOfFolders
		, VARIANT_BOOL p_baseTypeOf        , VARIANT_BOOL p_derivedTypesOf
		, long p_libraryElementHandling          // 0: stubs, 1: stop, 2: continue
		, long p_wrapping                        // 0: miniproject, 1: automatic, 2: as is
		, long p_kindFilter                      // -1 = all, otherwise calculate the 0b000rsamf mask, where f = Folders, m = Models, a = Atoms, s = Sets, r = References
		, long p_markWith                        // mark all objects in closure
		, VARIANT_BOOL p_directionDown
		, IMgaFCOs **p_selFcos, IMgaFolders **p_selFolders
		, IMgaFCOs **p_topFcos, IMgaFolders **p_topFolders
		, VARIANT_BOOL* p_topCollectionsFilled
		, BSTR *p_acckinds
		);

	STDMETHOD(SmartCopy)
		( IMgaFCOs *in_fcos
		, IMgaFolders *in_folders
		, IMgaFCOs **sel_fcos
		, IMgaFolders **sel_folders
		, IMgaFCOs **top_fcos
		, IMgaFolders **top_folders
		, int options_in
		, long * options
		, BSTR *acckinds
		, BSTR *path
		);


// ------- Attributes and Methods
public:
	static const wchar_t* m_projLastClosureNode;
};

#endif // MGACLOSURE_H
