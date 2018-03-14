#include "stdafx.h"

#include "MgaClosure.h"
#include "ClosureDlg.h"
#include "ClosureRes.h"
#include "ClosureSmart.h"
#include "MakeClosure.h"
#include "ClosureSimple.h"
#include "MakeSimpleClosure.h"

const wchar_t* CMgaClosure::m_projLastClosureNode = L"lastClosureId";

STDMETHODIMP CMgaClosure::GetLastClosureId( IMgaProject* p_project, long* p_pMarkWith)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());

	COMTRY {
		if( !p_pMarkWith) return E_INVALID_USAGE;
		if( !p_project)   return E_INVALID_USAGE;

		CComObjPtr<IMgaFolder> rf;
		COMTHROW( p_project->get_RootFolder( PutOut( rf)));

		CString cst0;
		COMTHROW( rf->get_RegistryValue( CComBSTR( m_projLastClosureNode), PutOut( cst0)));
		
		int t_val = -1;
		if( 1 == _stscanf( (LPCTSTR) cst0, _T("%i"), &t_val))
		{
			*p_pMarkWith = t_val;
		}
		else
		{
			*p_pMarkWith = -1;
		}
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaClosure::PutLastClosureId( IMgaProject* p_project, long p_iMarkWith)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());
	COMTRY {
		if( !p_project)  return E_INVALID_USAGE;
		if( p_iMarkWith == -1) return E_INVALID_USAGE;

		CComObjPtr<IMgaFolder> rf;
		COMTHROW( p_project->get_RootFolder( PutOut( rf)));

		CString cst;
		cst.Format( _T("%i"), p_iMarkWith);

		COMTHROW( rf->put_RegistryValue( CComBSTR( m_projLastClosureNode), CComBSTR( cst)));
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaClosure::SelectiveClosureDlg
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
	, BSTR* p_userSelFileName
	)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());

	if 
	 (  !p_refersTo           || !p_referredBy
	 || !p_connections        || !p_connsThroughRefport
	 || !p_setMembers         || !p_memberOfSets
	 || !p_modelContainment   || !p_partOfModels
	 || !p_folderContainment  || !p_partOfFolders
	 || !p_baseTypeOf         || !p_derivedTypesOf
	 || !p_libraryElementHandling 
	 || !p_wrappingMode
	 || !p_kindFilter
	 || !p_markWith
	 || !p_options
	 || !p_directionDown
	 || !p_copyToFile
	 || !p_okPressed
	 || !p_userSelFileName)
	  return E_INVALID_USAGE;

	CWaitCursor w;
	CClosureDlg dlg;
	dlg.m_activeFolderContainment = p_folderContainmentNeeded == VARIANT_TRUE;
	if( *p_markWith != -1) dlg.m_iMarkVal = *p_markWith;
	*p_okPressed = VARIANT_FALSE;

	if ( dlg.DoModal() == IDOK)
	{
		*p_refersTo               = dlg.m_bRefersTo           ? VARIANT_TRUE: VARIANT_FALSE;
		*p_referredBy             = dlg.m_bReferredBy         ? VARIANT_TRUE: VARIANT_FALSE;
		*p_connections            = dlg.m_bConnection         ? VARIANT_TRUE: VARIANT_FALSE;
		*p_connsThroughRefport    = dlg.m_bRefPortConnection  ? VARIANT_TRUE: VARIANT_FALSE;
		*p_setMembers             = dlg.m_bSetMember          ? VARIANT_TRUE: VARIANT_FALSE;
		*p_memberOfSets           = dlg.m_bMemberOfSets       ? VARIANT_TRUE: VARIANT_FALSE;
		*p_modelContainment       = dlg.m_bContainment        ? VARIANT_TRUE: VARIANT_FALSE;
		*p_partOfModels           = dlg.m_bPartOfModels       ? VARIANT_TRUE: VARIANT_FALSE;
		*p_folderContainment      = dlg.m_bFolderContainment  ? VARIANT_TRUE: VARIANT_FALSE;
		*p_partOfFolders          = dlg.m_bPartOfFolders      ? VARIANT_TRUE: VARIANT_FALSE;
		*p_baseTypeOf             = dlg.m_bBaseTypes          ? VARIANT_TRUE: VARIANT_FALSE;
		*p_derivedTypesOf         = dlg.m_bDerivedTypes       ? VARIANT_TRUE: VARIANT_FALSE;

		*p_libraryElementHandling = dlg.m_libraryHandling;
		*p_wrappingMode           = dlg.m_wrappingOption;
		*p_kindFilter             = dlg.calcKindMask();
		*p_markWith               = dlg.m_bMarkObjs ? dlg.m_iMarkVal : -1;
		*p_options                = (dlg.m_libraryHandling == 0)?0x1:0; // in case of "stubs" set the options to 1
		*p_directionDown          = dlg.m_direction    == 1 ? VARIANT_TRUE : VARIANT_FALSE;
		*p_copyToFile             = dlg.m_outputOption == 1 ? VARIANT_TRUE : VARIANT_FALSE;
		*p_okPressed              = VARIANT_TRUE;

		CComBSTR filename( dlg.m_strOutputFile);
		*p_userSelFileName = filename.Detach();
	}
	return S_OK;
}

STDMETHODIMP CMgaClosure::SelectiveClosureCalc
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
	, IMgaFCOs** p_selFcos, IMgaFolders** p_selFolders
	, IMgaFCOs** p_topFcos, IMgaFolders** p_topFolders
	, VARIANT_BOOL* p_topCollectionsFilled
	, BSTR* p_accKinds
	)
{
	if 
	 (  ! p_selFcos
	 || ! p_selFolders
	 || ! p_topFcos
	 || ! p_topFolders
	 || ! p_topCollectionsFilled
	 || ! p_accKinds)
	  return E_INVALID_USAGE;

	int modcont = p_modelContainment  == VARIANT_TRUE? 1: 0;
	int folcont = p_folderContainment == VARIANT_TRUE? 1: 0;
	int partmod = p_partOfModels      == VARIANT_TRUE? 1: 0;
	int partfol = p_partOfFolders     == VARIANT_TRUE? 1: 0;

	CMakeClosure mc
		( p_initSelFcos
		, p_initSelFolders
		, p_refersTo            == VARIANT_TRUE? 1 : 0
		, p_referredBy          == VARIANT_TRUE? 1 : 0
		, p_connections         == VARIANT_TRUE? 1 : 0
		, p_connsThroughRefport == VARIANT_TRUE? 1 : 0
		, p_setMembers          == VARIANT_TRUE? 1 : 0
		, p_memberOfSets        == VARIANT_TRUE? 1 : 0 
		// compare the containment options with the direction option
		, p_directionDown  == VARIANT_TRUE  ? modcont:0
		, p_directionDown  == VARIANT_FALSE ? partmod:0
		, p_directionDown  == VARIANT_TRUE  ? folcont:0
		, p_directionDown  == VARIANT_FALSE ? partfol:0
		, p_baseTypeOf     == VARIANT_TRUE  ? 1: 0
		, p_derivedTypesOf == VARIANT_TRUE  ? 1: 0
		, (int) p_libraryElementHandling // == 2 // in case of "continue" ("go on") only
		, (int) p_wrapping
		, p_directionDown  == VARIANT_TRUE  ? 1: 0
		, (int) p_kindFilter
		, (int) p_markWith
		);

	mc.process(); // do the hard job

	// load the results
	mc.getFCOs( p_selFcos);
	mc.getFolders( p_selFolders);

	bool normal_tops = !mc.noTopObjs();
	if ( normal_tops) // the top fcos/folds is calculated and The RootFolder is not among them
	{
		mc.getTopFCOs( p_topFcos);
		mc.getTopFolders( p_topFolders);
		
		*p_topCollectionsFilled = VARIANT_TRUE;
	}
	else
		*p_topCollectionsFilled = VARIANT_FALSE;

	
	if( normal_tops &&               // avoid in case of non-normal tops
		p_wrapping > 0 &&            // avoid in case of miniproject
		mc.isAnyAccepting())         // avoid if is no accepting kind/folder
	{
		std::string res_str;

		mc.getAccKindsInString( res_str);

		CComBSTR acc_kind = res_str.c_str();
		*p_accKinds = acc_kind.Detach();
	}

	return S_OK;
}


STDMETHODIMP CMgaClosure::SmartCopy(
	IMgaFCOs *p_fcosIn, IMgaFolders *p_foldersIn,
	IMgaFCOs **p_fcosSelOut, IMgaFolders **p_foldersSelOut,
	IMgaFCOs **p_topFcosOut, IMgaFolders **p_topFoldersOut,
	int   p_optionsIn, 
	long *p_optionsOut,
	BSTR *p_accept,
	BSTR *p_path
	)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());
	ASSERT( p_optionsOut);

	bool res = true;
	CWaitCursor w;
	CClosureSimple dlg;

	bool any_cont = (p_optionsIn & 1024) == 1024; // is any container
	bool any_fold = (p_optionsIn & 16384) == 16384; // is any folder
	dlg.m_enableContainChkBox = any_cont;
	dlg.m_enableFolderChkBox = any_fold;

	res = dlg.DoModal() == IDOK;

	if( res)
	{
		CSimpleSelectClosure mc
			( p_fcosIn
			, p_foldersIn
			, dlg.m_bCont     // need containment?
			, dlg.m_bFoldCont // need folder containment?
			, dlg.m_bAtom == TRUE // exclude atoms?
			, dlg.m_bConn == TRUE //  -//-   conns?
			, dlg.m_bRef == TRUE  //  -//-   refs?
			, dlg.m_bSet == TRUE  //  -//-   sets?
			);

		mc.process(); // do the hard job

		// load the results
		mc.getFCOs( p_fcosSelOut);
		mc.getFolders( p_foldersSelOut);

		bool normal_tops = !mc.noTopObjs();
		
		if( normal_tops) // the top fcos/folds is calculated and The RootFolder is not among them
		{
			mc.getTopFCOs( p_topFcosOut);
			mc.getTopFolders( p_topFoldersOut);
		}

		*p_optionsOut = 0;
		//*pOptions = (dlg.m_libraryHandling == 0)?0x1:0; // in case of "stubs" set the options to 1
		*p_optionsOut = (LIB_STUBS == 0)?0x1:0; // in case of "stubs" set the options to 1
		*p_optionsOut = *p_optionsOut | 0x2; // dump the closure2 like attributes
		*p_optionsOut = *p_optionsOut | 0x4; // smart copy signal

		std::string res_str;
		mc.getAccKindsInString( res_str);

		CComBSTR acckind = res_str.c_str();
		*p_accept = acckind.Detach();

		CComBSTR name;
		*p_path = name.Detach();
	}

	return S_OK;
}
