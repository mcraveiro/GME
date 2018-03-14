// MgaResolver.cpp : Implementation of CMgaResolver
#include "stdafx.h"

#include "CommonCollection.h"
#include "MgaResolver.h"
#include "DialogList.h"
#include "AlterNmspDlg.h"
#include "CommonError.h"

#include <algorithm>

/////////////////////////////////////////////////////////////////////////////

CString helper_ObjTypeStr(objtype_enum objtype) {

	switch(objtype) {

		case OBJTYPE_MODEL : {
			return CString("model");
		}
		case OBJTYPE_ATOM : {
			return CString("atom");
		}
		case OBJTYPE_REFERENCE : {
			return CString("reference");
		}
		case OBJTYPE_CONNECTION : {
			return CString("connection");
		}
		case OBJTYPE_SET : {
			return CString("set");
		}
		case OBJTYPE_FOLDER : {
			return CString("folder");
		}
		case OBJTYPE_ASPECT : {
			return CString("aspect");
		}
		case OBJTYPE_ROLE : {
			return CString("role");
		}
		case OBJTYPE_ATTRIBUTE : {
			return CString("attribute");
		}
		default : {
			return CString("unknown object type");
		}
	}
} // helper_ObjTypeStr()

/////////////////////////////////////////////////////////////////////////////
// CMgaResolver
CComBSTR CMgaResolver::prefixIt(CComBSTR pIn)
{
	CComBSTR rval;
	COMTHROW(rval.AppendBSTR( m_prefix));
	COMTHROW(rval.AppendBSTR( pIn));

	return rval;
}

CComBSTR CMgaResolver::truncIt(CComBSTR pIn)
{
	CComBSTR rval;

	CString crole( pIn);
	CString trunc( m_trunc);

	if( crole.Left( trunc.GetLength()) == trunc)
	{
		COMTHROW(rval.Append( crole.Right( crole.GetLength() - trunc.GetLength())));
	}

	return rval;
}

bool CMgaResolver::isAlterationSet() const
{
	return m_alter_option == CH_PREFIX 
		|| m_alter_option == CH_TRUNCATE
		|| m_alter_option == CH_MIGRATE;
}

void CMgaResolver::setAlteration( alter_mode_enum v)
{
	if( v >= CH_NONE && v <= CH_MIGRATE)
		m_alter_option = v;
	else
		m_alter_option = CH_NONE;
}

STDMETHODIMP CMgaResolver::Clear() {

	this->clear_mappings();

	return S_OK;
}

void CMgaResolver::clear_mappings() {

	while (! map_FolderByStr.IsEmpty() )
		delete map_FolderByStr.RemoveHead();

	while (! map_KindByStr.IsEmpty() )
		delete map_KindByStr.RemoveHead();

	while (! map_RoleByStr.IsEmpty() )
		delete map_RoleByStr.RemoveHead();

	while (! map_AttrByStr.IsEmpty() )
		delete map_AttrByStr.RemoveHead();

	while (! map_RoleByPathStr.IsEmpty() )
		delete map_RoleByPathStr.RemoveHead();

	mb_use_sticky = false;

	// namespace related
	m_alter_option = CH_NONE;
	m_prefix = "";
	m_trunc  = "";
	mb_name_alteration_sticky = false;
}

STDMETHODIMP CMgaResolver::put_IsInteractive(VARIANT_BOOL p) {
	mb_is_interactive = (p != VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP CMgaResolver::get_IsInteractive(VARIANT_BOOL *p) {
	*p = (this->mb_is_interactive ? VARIANT_TRUE : VARIANT_FALSE );
	return S_OK;
}

STDMETHODIMP CMgaResolver::put_IsStickyEnabled(VARIANT_BOOL p) {
	mb_use_sticky = (p != VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP CMgaResolver::get_IsStickyEnabled(VARIANT_BOOL *p) {
	*p = (this->mb_use_sticky ? VARIANT_TRUE : VARIANT_FALSE );
	return S_OK;
}

STDMETHODIMP CMgaResolver::getUserOptions()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !this->mb_is_interactive) return S_OK;

	COMTRY {
		AlterNmspDlg cdl;
		if( cdl.DoModal() == IDOK)
		{
			this->m_prefix = cdl.m_strPrefix;
			this->m_trunc  = cdl.m_strTrunc;
			this->setAlteration( (alter_mode_enum) cdl.m_vRadAlter);
			this->mb_name_alteration_sticky = cdl.m_bAlterSticky == TRUE;
		}
	} COMCATCH(;)
}


STDMETHODIMP CMgaResolver::get_FolderByStr(IMgaFolder *parent, 
										   BSTR kind, 
										   IMgaMetaFolder **p) {

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_OUT(p);

	ASSERT(parent!=NULL);

	HRESULT hr;

	COMTRY {

		CComPtr<IMgaMetaFolder> parent_mfolder;
		CComPtr<IMgaMetaFolder> mfolder;
		CComPtr<IMgaMetaFolder> mfolder_ret;
		CComPtr<IMgaMetaFolders> mfolders;
		long mfs_count = -1;

		CString sz_debug(kind);

		COMTHROW( parent->get_MetaFolder(&parent_mfolder) );
		ASSERT(parent_mfolder != NULL);

		COMTHROW( 
			(hr = parent_mfolder->get_LegalChildFolderByName(kind, &mfolder)) == E_NOTFOUND ?
			S_OK : hr
		);

		// if not found and namespace settings are available
		if (mfolder.p == NULL && isAlterationSet())
		{
			// (exclusive or: either truncate or prefix or migrate)
			if( m_alter_option == CH_PREFIX)
			{
				CComBSTR kind2 = prefixIt( kind);
				// try with prefixed name
				COMTHROW( 
					(hr = parent_mfolder->get_LegalChildFolderByName(kind2, &mfolder)) == E_NOTFOUND ?
					S_OK : hr
				);
			}
			else if( m_alter_option == CH_TRUNCATE)
			{
				CComBSTR kind2 = truncIt( kind);
				// try with truncated name
				COMTHROW( 
					(hr = parent_mfolder->get_LegalChildFolderByName(kind2, &mfolder)) == E_NOTFOUND ?
					S_OK : hr
				);
			}
			else if( m_alter_option == CH_MIGRATE)
			{
				CComBSTR kind1 = truncIt( kind);
				CComBSTR kind2 = prefixIt( kind1);
				// try with migrated name from one namespace to another
				COMTHROW( 
					(hr = parent_mfolder->get_LegalChildFolderByName(kind2, &mfolder)) == E_NOTFOUND ?
					S_OK : hr
				);
			}
		}

		if (mfolder.p != NULL) {
			COMTHROW( mfolder.CopyTo(p) );
			return S_OK;
		}

		// no explicit match: now check mapping

		if (this->map_get_FolderByStr(parent_mfolder.p, kind, p)) {
			return S_OK;
		}

		// no match in mapping: allow for selection of possible folders

		COMTHROW( parent_mfolder->get_LegalChildFolders(&mfolders) );

		if (mfolders.p != NULL) {

			COMTHROW( mfolders->get_Count(&mfs_count) );

			if (mfs_count > 0) {

				// Get the first defined folder, but do not automatically resolve 
				// root folder (otherwise, library import does not work)
				if (mfs_count == 1 && CString(kind) != "RootFolder") {

					COMTHROW( mfolders->get_Item(1, p) );

					//    ' update the mapping: (parent.kind, kindname)->metafolder

					this->map_put_FolderByStr(parent_mfolder.p, kind, *p);

					return S_OK;
				}

				// no unique match in mapping and no obvious pick: 
				//		since not interactive, exit

				if (! this->mb_is_interactive) {
					SetErrorInfo(CString(L"Could not resolve folder for kind '") + kind + "'");
					return RESOLV_ERR_get_FolderByStr;
				}

				// else create a dialog to pick from

				CComPtr<IMgaMetaFolder> immf_ix;
				CDialogList cdl("Resolve Folder Kind", CDialogList::CHKTEXT_ONETIMER);

				MGACOLL_ITERATE(IMgaMetaFolder, mfolders) {

					immf_ix = MGACOLL_ITER;
					ASSERT(immf_ix != NULL);

					CComBSTR name;

					COMTHROW( immf_ix->get_Name(&name) );

					cdl.m_sz_prelist.AddTail(CString(name)); 

				} // eo for()
				MGACOLL_ITERATE_END;

				if ((cdl.DoModal() != IDOK) || (cdl.mn_selection_index == LB_ERR)) {
					SetErrorInfo(L"No item chosen from dialog");
					return RESOLV_ERR_get_FolderByStr;
				}

				// else, find item from dialog, copy pointer, and update map

				// copy pointer
				COMTHROW( mfolders->get_Item(cdl.mn_selection_index+1,p) );
				ASSERT(*p != NULL);

				// update map

				if (cdl.mb_check_once == TRUE)
					this->map_put_FolderByStr(parent_mfolder.p, kind, *p);

				return S_OK;

			}
		}

//		#pragma bookmark (note: insert helpful message box here get_FolderByStr[] )
		CComBSTR parfolder_name;
		COMTHROW( parent->get_Name(&parfolder_name) );

		CComBSTR parmfolder_name;
		COMTHROW( parent_mfolder->get_Name(&parmfolder_name) );

		CString sz_format;
		sz_format.Format(	_T("Paradigm violation: Cannot create folder.\n")
							_T("Parent name: %s\n")
							_T("Parent type: %s\n")
							_T("Child type: %s"),
							(BSTR) parfolder_name, (BSTR) parmfolder_name,
							kind
		);

		CComPtr<IMgaProject> mgaProject;
		COMTHROW(parent->get_Project(&mgaProject));
		SetErrorInfo(sz_format);

		return RESOLV_ERR_get_FolderByStr;
	
	}

	COMCATCH(;)
	

}


STDMETHODIMP CMgaResolver::get_KindByStr(IMgaFolder *parent, BSTR kind, 
										 objtype_enum objtype, 
										 IMgaMetaFCO **p) {

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

#ifdef DEBUG
	CComBSTR sz_debug_kind(kind);
#endif

	CHECK_OUT(p);
	ASSERT(parent != NULL);

	HRESULT hr;

	COMTRY {

		CComPtr<IMgaMetaFCO> mfco;
		objtype_enum mfco_objtype;

		CComPtr<IMgaMetaFolder> parent_mf;

		COMTHROW( parent->get_MetaFolder(&parent_mf) );
		ASSERT(parent_mf!=NULL);

		CString sentence;
		if(CComBSTR(kind).Length()) {
			sentence = _T("Specified metaobject '") + CString(kind) + _T("'");
			COMTHROW( 
				((hr = parent_mf->get_LegalRootObjectByName(kind, &mfco)) == E_NOTFOUND) ?
				S_OK : hr
			);

			// if not found and namespace settings are available
			if( hr == E_NOTFOUND && isAlterationSet())
			{
				// exclusive or
				if( m_alter_option == CH_PREFIX)
				{
					CComBSTR kind2 = prefixIt( kind);
					// try with prefixed name
					COMTHROW( 
						((hr = parent_mf->get_LegalRootObjectByName(kind2, &mfco)) == E_NOTFOUND) ?
						S_OK : hr
					);
				}
				else if( m_alter_option == CH_TRUNCATE)
				{
					CComBSTR kind2 = truncIt( kind);
					// try with truncated name
					if( kind2.Length() > 0)
						COMTHROW( 
							((hr = parent_mf->get_LegalRootObjectByName(kind2, &mfco)) == E_NOTFOUND) ?
							S_OK : hr
						);
				}
				else if( m_alter_option == CH_MIGRATE)
				{
					CComBSTR kind1 = truncIt( kind);
					CComBSTR kind2 = prefixIt( kind1);
					// try with migrated name from one namespace to another
					if( kind2.Length() > 0) 
						COMTHROW( 
							((hr = parent_mf->get_LegalRootObjectByName(kind2, &mfco)) == E_NOTFOUND) ?
							S_OK : hr
						);
				}
			}

			if (mfco != NULL) {

				COMTHROW( mfco->get_ObjType(&mfco_objtype) );

				if ((objtype == OBJTYPE_NULL) || (mfco_objtype == objtype)) {
					COMTHROW( mfco.CopyTo(p) );
					// insert mapping

					this->map_put_KindByStr(parent_mf.p, kind, objtype, *p);

					return S_OK;
				}

				// match on name object occurs, but incompatible user spec'd objtype


				sentence += _T(" is of invalid object type. ");
			}

			// either a fail on name match && proper objtype,
			//  or no name match (so look up on objtype)

			// check mapping for non-match

			else {

				sentence += _T(" is not found. ");

			}
		}
		// check mapping
		if (this->map_get_KindByStr(parent_mf.p, kind, objtype, p)) {
			return S_OK;
		}
		//  enumerate list of children with proper objtypes

		// perhaps a call to: this->get_KindByObjtype()

		CComPtr<IMgaMetaFCOs> _immfcos;

		COMTHROW( parent_mf->get_LegalRootObjects(&_immfcos) );
		ASSERT(_immfcos!=NULL);

		long _fs_ct = 0;

		COMTHROW( _immfcos->get_Count(&_fs_ct) );

		if (_fs_ct == 0) {
//			#pragma bookmark (note: insert helpful message box here get_KindByStr[1] )
			CComBSTR parent_name;
			COMTHROW( parent->get_Name(&parent_name) );

			CComBSTR mparent_name;
			COMTHROW( parent_mf->get_Name(&mparent_name) );

			CString sz_format;

			sz_format.Format(	_T("%sParent folder %s\n")
								_T("of type %s\n")
								_T("cannot have any children."),
								sentence,
								(BSTR) parent_name,
								(BSTR) mparent_name
			);
			SetErrorInfo(sz_format);

			return RESOLV_ERR_get_KindByStr;
		}

		
		CDialogList cdl(sentence + _T("Resolve Object Kind of ObjType ")+helper_ObjTypeStr(objtype),
			CDialogList::CHKTEXT_ONETIMER);
		CComPtr<IMgaMetaFCO> fco_ix;
		CList<int,int> valid_obj_map;
		objtype_enum objt_ix;

		for (long z = 1; z <= _fs_ct; z++) {

			fco_ix = NULL;
			COMTHROW( _immfcos->get_Item(z, &fco_ix) );

			ASSERT(fco_ix != NULL);

			COMTHROW( fco_ix->get_ObjType(&objt_ix) );

			if ((objt_ix == objtype) || (objtype == OBJTYPE_NULL)) {

				CComBSTR name;

				COMTHROW( fco_ix->get_Name(&name) );

				cdl.m_sz_prelist.AddTail(CString(name)); 
				valid_obj_map.AddTail(z);
			}
		}

		if (valid_obj_map.GetCount() == 0) {
//			#pragma bookmark (note: insert helpful message box here get_KindByStr[1] )
			CComBSTR parent_name;
			COMTHROW( parent->get_Name(&parent_name) );

			CComBSTR mparent_name;
			COMTHROW( parent_mf->get_Name(&mparent_name) );

			CString sz_format;
			CString sz_format_tmp;

			sz_format_tmp.Format(	_T("Paradigm Violation: parent folder %s ")
									_T("of type %s ")
									_T("cannot have any children"),
									(BSTR) parent_name,
									(BSTR) mparent_name
			);
			sz_format += sz_format_tmp;

			if (objtype != OBJTYPE_NULL) {
				sz_format_tmp.Format(	_T(" of object type %s\n"),
										helper_ObjTypeStr(objtype)
										);
				sz_format += sz_format_tmp;
			}

			sz_format += sentence;

			SetErrorInfo(sz_format);

			return RESOLV_ERR_get_KindByStr;
		}

		if (valid_obj_map.GetCount() == 1) {
			// only one object of that type can exist-- grab it and update map
			int index = valid_obj_map.GetHead();

			fco_ix = NULL;
			COMTHROW( _immfcos->get_Item(index, &fco_ix) );
			ASSERT(fco_ix != NULL);

			COMTHROW( fco_ix.CopyTo(p) );

			if(this->mb_is_interactive && sentence.GetLength()) {
				CComBSTR newname;
				COMTHROW(fco_ix->get_Name(&newname));
				sentence += _T("\nUsing the only valid choice '") + CString(newname) + _T("' instead.");

				CComPtr<IMgaProject> mgaProject;
				COMTHROW(parent->get_Project(&mgaProject));
				CComPtr<IGMEOLEApp> pGME = get_GME(mgaProject);
				if (pGME) {
					COMTHROW(pGME->ConsoleMessage(CComBSTR(sentence), MSG_ERROR));
				}
				else {
					AfxMessageBox(sentence, MB_ICONSTOP | MB_OK);
				}

			}
			if (!this->mb_is_interactive)
			{
				SetErrorInfo(CString(L"Could not resolve kind '") + kind + L"'");
				return RESOLV_ERR_get_KindByStr;
			}
			// update mapping
			this->map_put_KindByStr(parent_mf.p, kind, objtype, *p);

			return S_OK;
		}

		// > 1 valid objects: show enumeration if possible

		// if not interactive, we have to quit-- no more choices!

		if (!this->mb_is_interactive) {
			SetErrorInfo(CString(L"Could not resolve kind '") + kind + L"'");
			return RESOLV_ERR_get_KindByStr;
		}

		if ((cdl.DoModal() != IDOK) || (cdl.mn_selection_index == LB_ERR)) {
//			#pragma bookmark (note: insert helpful message box here get_KindByStr[1] )
			SetErrorInfo(_T("No item chosen from dialog"));
			return RESOLV_ERR_get_KindByStr;
		}

		int coll_ix = valid_obj_map.GetAt(valid_obj_map.FindIndex(
			cdl.mn_selection_index));


		fco_ix = NULL;
		COMTHROW( _immfcos->get_Item(coll_ix, &fco_ix) );
		ASSERT(fco_ix != NULL);

		COMTHROW( fco_ix.CopyTo(p) );

		// update mapping
		if (cdl.mb_check_once == TRUE)
			this->map_put_KindByStr(parent_mf.p, kind, objtype, *p);

		return S_OK;

	}


	COMCATCH(;)

}


STDMETHODIMP CMgaResolver::get_RoleByStr(IMgaModel *parent, BSTR kind, 
										 objtype_enum objtype,
										 BSTR role, BSTR aspect, 
										 IMgaMetaRole **p) {

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr;

	CHECK_OUT(p);

	CComPtr<IMgaMetaFCO> parent_meta_fco;
	CComPtr<IMgaMetaModel> parent_meta;
	CComPtr<IMgaMetaRole> targ_role;
	CComPtr<IMgaMetaRoles> targ_roles;
	CComPtr<IMgaMetaFCO> targ_fco;
	CComPtr<IMgaMetaAspect> targ_aspect;
	CComPtr<IMgaMetaParts> targ_parts;
	CComPtr<IMgaMetaPart> targ_part_ix;
	objtype_enum targ_objtype;

	ASSERT(parent != NULL);

	COMTRY 
	{

		COMTHROW( parent->get_Meta(&parent_meta_fco) );
		ASSERT(parent_meta_fco != NULL);
		COMTHROW( parent_meta_fco.QueryInterface(&parent_meta) );
		ASSERT(parent_meta!=NULL);

		CComBSTR meta_parent_model_debug;
		COMTHROW( parent_meta->get_Name(&meta_parent_model_debug) );
		CComBSTR kind_debug(kind);

		// let's try mapping first since resolution is expensive

		if (this->map_get_RoleByStr(parent_meta.p,kind, objtype, role, aspect, p)) {
			return S_OK;
		}

		// now resort to starting with the role

		COMTHROW( 
			((hr = parent_meta->get_RoleByName(role, &targ_role)) == E_NOTFOUND) ? 
			S_OK : hr
		);

		// if not found and namespace settings are available
		if( hr == E_NOTFOUND && isAlterationSet())
		{
			// (exclusive or: either truncate or prefix)
			if( m_alter_option == CH_PREFIX)
			{
				CComBSTR role2 = prefixIt( role);
				// try with prefixed name
				COMTHROW( 
					((hr = parent_meta->get_RoleByName(role2, &targ_role)) == E_NOTFOUND) ? 
					S_OK : hr
				);
			}
			else if( m_alter_option == CH_TRUNCATE)
			{
				CComBSTR role2 = truncIt( role);
				// try with truncated name
				if( role2.Length() > 0) 
					COMTHROW( 
						((hr = parent_meta->get_RoleByName(role2, &targ_role)) == E_NOTFOUND) ? 
						S_OK : hr
					);
			}
			else if( m_alter_option == CH_MIGRATE)
			{
				CComBSTR role1 = truncIt( role);
				CComBSTR role2 = prefixIt( role1);
				// try with migrated name from one namespace to another
				if( role2.Length() > 0)
					COMTHROW( 
						((hr = parent_meta->get_RoleByName(role2, &targ_role)) == E_NOTFOUND) ? 
						S_OK : hr
					);
			}

		}

		

		if ((hr == S_OK) && (targ_role != NULL ) ) {

			CComBSTR targ_kind_name;

			COMTHROW( targ_role->get_Kind(&targ_fco) );
			ASSERT(targ_fco != NULL);

			COMTHROW( targ_fco->get_Name(&targ_kind_name) );

			CString c1((BSTR) targ_kind_name);
			CString c2(kind);
			CString c3;

			if (isAlterationSet() && (m_alter_option == CH_PREFIX || m_alter_option == CH_MIGRATE))
			{
				CComBSTR role2 = prefixIt(role);
				CopyTo(role2, c3);
			}
			else if (isAlterationSet() && m_alter_option == CH_TRUNCATE)
			{
				CComBSTR role2 = truncIt(role);
				CopyTo(role2, c3);
			}

			if ((c2 == _T("")) || (c2 == c1) || (c3 != _T("") && c1 == c3)) {

				COMTHROW(targ_fco->get_ObjType(&targ_objtype));

				if (
					(objtype == OBJTYPE_NULL) || 
					(targ_objtype == objtype)
					) {

					COMTHROW(targ_role->get_Parts(& targ_parts));
					ASSERT(targ_parts!=NULL);

					bool found = false;
					MGACOLL_ITERATE(IMgaMetaPart, targ_parts) {

						if( !found) {
							targ_part_ix = MGACOLL_ITER;
							ASSERT(targ_part_ix != NULL);

							CComBSTR name;

							VARIANT_BOOL vb_primary;

							COMTHROW(targ_part_ix->get_IsPrimary(&vb_primary) );

							if (vb_primary != VARIANT_FALSE) {

								targ_aspect = NULL;
								COMTHROW( targ_part_ix->get_ParentAspect(&targ_aspect) );
								ASSERT(targ_aspect != NULL);
								COMTHROW( targ_aspect->get_Name(&name) );
								
								CString cb1((BSTR) name);
								CString cb2(aspect);

								if ((cb2 == _T("")) || (cb2 == cb1 )) {

									// finally-- success!

									// ??? PETER: Why do we increase the refcount here ?
									COMTHROW( targ_role.CopyTo(p) );
									
									

									// map entry

									this->map_put_RoleByStr(
										parent_meta.p,
										kind,
										objtype,
										role,
										aspect,
										*p);

									found = true;  // do not return from the middle of an iteration
									//return S_OK; // hopefully p will not change
								}
							}

						}


//							cdl.m_sz_prelist.AddTail(CString(name)); 

					} // eo for()
					MGACOLL_ITERATE_END;
					if( found) return S_OK;
				}
			}
		} // eo if(exact match from MetaModel->get_RoleByName[] )

		// now resort to starting with the kind

		targ_fco = NULL;
		COMTHROW( 
			((hr = parent_meta->get_DefinedFCOByName(kind, VARIANT_TRUE, &targ_fco)) == E_NOTFOUND) ? 
			S_OK : hr
		);

		// if not found and namespace settings are available
		if( hr == E_NOTFOUND && isAlterationSet())
		{
			// (exclusive or: either truncate or prefix)
			if( m_alter_option == CH_PREFIX)
			{
				CComBSTR kind2 = prefixIt( kind);
				// try with prefixed name
				COMTHROW( 
					((hr = parent_meta->get_DefinedFCOByName(kind2, VARIANT_TRUE, &targ_fco)) == E_NOTFOUND) ? 
					S_OK : hr
				);
			}
			else if( m_alter_option == CH_TRUNCATE)
			{
				CComBSTR kind2 = truncIt( kind);
				// try with truncated name
				if( kind2.Length() > 0)
					COMTHROW( 
						((hr = parent_meta->get_DefinedFCOByName(kind2, VARIANT_TRUE, &targ_fco)) == E_NOTFOUND) ? 
						S_OK : hr
					);
			}
			else if( m_alter_option == CH_MIGRATE)
			{
				CComBSTR kind1 = truncIt( kind);
				CComBSTR kind2 = prefixIt( kind1);
				// try with migrated name from one namespace to another
				if( kind2.Length() > 0) 
					COMTHROW( 
						((hr = parent_meta->get_DefinedFCOByName(kind2, VARIANT_TRUE, &targ_fco)) == E_NOTFOUND) ? 
						S_OK : hr
					);
			}

		}

		if ((hr == S_OK) && (targ_fco != NULL)) {

			COMTHROW( targ_fco->get_ObjType(&targ_objtype) );

			if ((objtype == OBJTYPE_NULL) || (objtype == targ_objtype)) {

				targ_roles = NULL;
				COMTHROW( parent_meta->LegalRoles(targ_fco, &targ_roles) );
				ASSERT(targ_roles != NULL);

				long role_count = 0;

				COMTHROW( targ_roles->get_Count(&role_count));

				if (role_count == 1) {

					// ensure aspect ok

					targ_role = NULL;
					COMTHROW( targ_roles->get_Item(1, &targ_role) );
					ASSERT(targ_role != NULL);

					if (CString(aspect) == _T("")) { // done

						COMTHROW( targ_role.CopyTo(p) );

						// update map

						this->map_put_RoleByStr(parent_meta.p, 
							kind, objtype, role, aspect, *p);

						return S_OK;
					}
					if (!this->mb_is_interactive) {
						SetErrorInfo(CString(L"Could not resolve kind '") + kind + L"'");
						return RESOLV_ERR_get_RoleByStr;
					}

					// else, check each possible aspect

					targ_parts = NULL;
					COMTHROW( targ_role->get_Parts(&targ_parts) );
					ASSERT(targ_parts != NULL);

					bool found = false;
					MGACOLL_ITERATE(IMgaMetaPart, targ_parts) {

						if( !found) {
							targ_part_ix = MGACOLL_ITER;
							ASSERT( targ_part_ix != NULL );

							VARIANT_BOOL vb_primary;

							COMTHROW(targ_part_ix->get_IsPrimary(&vb_primary) );

							if (vb_primary != VARIANT_FALSE) {

								targ_aspect = NULL;
								COMTHROW( targ_part_ix->get_ParentAspect(&targ_aspect) );
								ASSERT( targ_aspect != NULL);

								CComBSTR asp_name;

								COMTHROW( targ_aspect->get_Name(&asp_name) );

								CString cz1(aspect);
								CString cz2(asp_name);

								if (cz1 == cz2) {

									COMTHROW( targ_role.CopyTo(p) );

									// update map

									this->map_put_RoleByStr(parent_meta.p, 
										kind, objtype, role, aspect, *p);

									found = true;  // do not return from the middle of an iteration
									//return S_OK; // hopefully p will not change after this

								}
							}

						}

					}
					MGACOLL_ITERATE_END;
					if( found) return S_OK;

				} // eo if (count == 1)

				CList<int, int> valid_role_map;
				CDialogList cdl(_T("Resolve Role Type"), CDialogList::CHKTEXT_ONETIMER); // TODO support ignore button

				for (int z = 1; z <= role_count; z++) {

					targ_role = NULL;
					COMTHROW( targ_roles->get_Item(z, &targ_role) );
					ASSERT(targ_role != NULL);

					if (CString(aspect) == _T("")) { // done

						CComBSTR role_name;

						COMTHROW( targ_role->get_Name(&role_name) );

						cdl.m_sz_prelist.AddTail(CString((BSTR) role_name));
						valid_role_map.AddTail(z);

					} else { // else, check each possible aspect

						targ_parts = NULL;
						COMTHROW( targ_role->get_Parts(&targ_parts) );

						MGACOLL_ITERATE(IMgaMetaPart, targ_parts) {

							targ_part_ix = MGACOLL_ITER;
							ASSERT( targ_part_ix != NULL );


							VARIANT_BOOL vb_primary;

							COMTHROW(targ_part_ix->get_IsPrimary(&vb_primary) );

							if (vb_primary != VARIANT_FALSE) {

								targ_aspect = NULL;	// Akos
								COMTHROW( targ_part_ix->get_ParentAspect(&targ_aspect) );
								ASSERT( targ_aspect != NULL);

								CComBSTR asp_name;

								COMTHROW( targ_aspect->get_Name(&asp_name) );

								CString cz1(aspect);
								CString cz2(asp_name);

								if (cz1 == cz2) {

									CComBSTR role_name;

									COMTHROW( targ_role->get_Name(&role_name) );

									cdl.m_sz_prelist.AddTail(CString((BSTR) role_name) );
									valid_role_map.AddTail(z);

									break; // back to for(z) loop

								}
							}

						} // iterate over aspects
					MGACOLL_ITERATE_END;

					} // check if aspect names important
				} // eo for(all roles of this kind name)


				// we have a fork in the road-- if no user input, we can't resolve

				if (!this->mb_is_interactive) {
					SetErrorInfo(CString(L"Could not resolve kind '") + kind + L"'");
					return RESOLV_ERR_get_RoleByStr;
				}

				INT_PTR dlgResult = IDOK;
				if ((cdl.m_sz_prelist.GetCount() != 0) 
					&& 
					((dlgResult = cdl.DoModal()) == IDOK) 
					&& 
					(cdl.mn_selection_index != LB_ERR)) {

					int index_into_roles_coll = 
						valid_role_map.GetAt(valid_role_map.FindIndex(
							cdl.mn_selection_index));

					targ_role = NULL;
					COMTHROW( targ_roles->get_Item(index_into_roles_coll, 
						&targ_role) );
					ASSERT(targ_role != NULL);

					// we now have our damn role

					COMTHROW( targ_role.CopyTo(p) );

					// update map

					if (cdl.mb_check_once == TRUE)
						this->map_put_RoleByStr(parent_meta.p, 
							kind, objtype, role, aspect, *p);

					return S_OK;
										
				}
				if (dlgResult == IDCANCEL)
				{
					SetErrorInfo(CString(L"Parsing cancelled"));
					return RESOLV_ERR_get_RoleByStr;
				}


			} // if objtype match

		} // eo if(match from FCO kind as source)


//		#pragma bookmark (note: insert helpful message box here get_AttrByStr[] )

		CComBSTR parent_name;
		COMTHROW( parent->get_Name(&parent_name) );

		CComBSTR mparent_name;
		COMTHROW( parent_meta_fco->get_Name(&mparent_name) );

		CString sz_format;
		CString sz_format_tmp;

		sz_format_tmp.Format(	_T("Paradigm Violation: Cannot create child.\n")
								_T("Parent name: %s\n")
								_T("Parent type: %s\n"),
								(BSTR) parent_name,
								(BSTR) mparent_name);
		sz_format+=sz_format_tmp;

		if (CString(kind) != _T("")) {
			sz_format_tmp.Format(_T("Child kind %s\n"), kind);
			sz_format+=sz_format_tmp;
		}

		if (objtype != OBJTYPE_NULL) {
			sz_format_tmp.Format(_T("Child type %s\n"), helper_ObjTypeStr(objtype));
			sz_format+=sz_format_tmp;
		}

		if (CString(aspect) != _T("")) {
			sz_format_tmp.Format(_T("Aspect: %s"), aspect);
			sz_format+=sz_format_tmp;
		}
		SetErrorInfo(sz_format);
		return RESOLV_ERR_get_RoleByStr;
	
	}
	
	COMCATCH(;)


} // eo CMgaResolver::get_RoleByStr()


STDMETHODIMP CMgaResolver::get_AttrByStr(IMgaFCO *parent, 
										 BSTR kind, 
										 IMgaMetaAttribute **p) {

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_OUT(p);

	HRESULT hr;

	COMTRY
	{

		CComPtr<IMgaMetaFCO> parent_metafco;
		CComPtr<IMgaMetaAttribute> imma;
		CComPtr<IMgaMetaAttributes> mattrs;
	//	objtype_enum objtype;
		long ma_count = 0;


		CComBSTR sz_debug(kind);

		COMTHROW( parent->get_Meta(&parent_metafco) );
		ASSERT( parent_metafco != NULL );
		_bstr_t kindName;
		COMTHROW(parent_metafco->get_Name(kindName.GetAddress()));

		COMTHROW( 
			((hr = parent_metafco->get_AttributeByName(kind, &imma)) == E_NOTFOUND) ?
			S_OK : hr
		);

		if (imma.p != NULL) {
			imma.CopyTo(p);
			return S_OK;
		}

		// no explicit match: now check mapping

		if (this->map_get_AttrByStr(parent_metafco.p, kind, p)) {
			return S_OK;
		}

		// no match in mapping: allow for selection of possible attrs

		COMTHROW( parent_metafco->get_Attributes(&mattrs) );

		if (mattrs.p != NULL) {

			COMTHROW( mattrs->get_Count(&ma_count) );

			if (ma_count > 0) {

				if (ma_count == 1) {
					// GME-414: Can't do anything here for xme importing:
					//  Consider the case where the new meta has 1 attribute, and the xme has 2 (and maybe we've only parsed 1 so far): we may want to ignore
				}

				// no unique match in mapping and no obvious pick: 
				//		since not interactive, exit

				if (! this->mb_is_interactive) {
					SetErrorInfo(CString(L"Could not resolve attribute '") + kind + L"'");
					return RESOLV_ERR_get_AttrByStr;
				}

				// else create a dialog to pick from

				CComPtr<IMgaMetaAttribute> imma_ix;
				CString sz_dialog_name;
				sz_dialog_name.Format(_T("Resolve Attribute '%s' for Kind '%s':"), kind, static_cast<BSTR>(kindName));
				CDialogList cdl( sz_dialog_name, CDialogList::CHKTEXT_ONETIMER, true );

				MGACOLL_ITERATE(IMgaMetaAttribute, mattrs) {

					imma_ix = MGACOLL_ITER;

					CComBSTR name;

					COMTHROW( imma_ix->get_Name(&name) );

					cdl.m_sz_prelist.AddTail(CString(name)); 

				} // eo for()
				MGACOLL_ITERATE_END;

                INT_PTR dlgres = cdl.DoModal();
                if( dlgres == IDIGNORE )
                {
                    *p = NULL;
					if (cdl.mb_check_once == TRUE)
						map_put_AttrByStr(parent_metafco.p, kind, NULL);
                    return S_OK;
                }
                else if( dlgres == IDCANCEL )
                {
					SetErrorInfo(CString(L"Could not resolve attribute '") + kind + L"'");
                    return RESOLV_ERR_get_AttrByStr;
                }				

				// else, find item from dialog, copy pointer, and update map

				if (cdl.mn_selection_index != LB_ERR) { // LB_ERR == init_val

					// copy pointer
					COMTHROW( mattrs->get_Item(cdl.mn_selection_index+1, p) );

					// update map

					if (cdl.mb_check_once == TRUE)
						this->map_put_AttrByStr(parent_metafco.p, kind, *p);

					return S_OK;
				}

			}
		}

//		#pragma bookmark (note: insert helpful message box here get_AttrByStr[] )
		CString sz_format;
		CComBSTR name;
		CComBSTR meta_name;
		objtype_enum objtype;

		COMTHROW( parent->get_ObjType(&objtype) );
		COMTHROW( parent->get_Name(&name) );
		COMTHROW( parent_metafco->get_Name(&meta_name) );
  
			
		sz_format.Format(	_T("No such attribute %s\n")
							_T("can be found in %s %s\n")
							_T("of kind %s"),
				kind,
				helper_ObjTypeStr(objtype),
				name.m_str,
				meta_name.m_str
		);
		SetErrorInfo(sz_format);

		return RESOLV_ERR_get_AttrByStr;

	}


	COMCATCH(;)

}

STDMETHODIMP CMgaResolver::get_RoleByMeta(IMgaModel *parent, 
										  IMgaMetaFCO *kind, 
										  objtype_enum objtype,
										  IMgaMetaRole *role, 
										  IMgaMetaAspect *aspect, 
										  IMgaMetaRole **p) {

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_OUT(p);

	CComBSTR sz_kind;
	CComBSTR sz_role;
	CComBSTR sz_aspect;

	COMTRY {


		if (kind != NULL) {
			COMTHROW( kind->get_Name(&sz_kind) );
		}

		if (role != NULL) {
			COMTHROW( role->get_Name(&sz_role) );
		}

		if (aspect != NULL) {
			COMTHROW( aspect->get_Name(&sz_aspect) );
		}


	HRESULT hr;

		hr = this->get_RoleByStr(	parent, 
									sz_kind, 
									objtype, 
									sz_role, 
									sz_aspect, 
									p
								);

		if (SUCCEEDED(hr)) {
			return hr;
		}
		else {
			SetErrorInfo(CString(L"Could not resolve role for kind '") + sz_kind + L"'");
			return ((hr == RESOLV_ERR_get_RoleByStr) ? RESOLV_ERR_get_RoleByMeta : hr);
		}

	}

	COMCATCH(;)

}

STDMETHODIMP CMgaResolver::get_ConnRoleByMeta(IMgaModel *parent, 
										  IMgaMetaAspect *aspect, 
										  IMgaFCO *src, IMgaFCO *src_port,
										  IMgaFCO *dst, IMgaFCO *dst_port,
										  IMgaMetaRole **p) {

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_OUT(p);

	ASSERT(src!=NULL);
	ASSERT(dst!=NULL);
	ASSERT(parent!=NULL);

	CComPtr<IMgaMetaRole> meta_src_role;
	CComPtr<IMgaMetaRole> meta_dst_role;

	CComPtr<IMgaMetaRole> meta_src_port_role;
	CComPtr<IMgaMetaRole> meta_dst_port_role;

	COMTRY {

		COMTHROW( src->get_MetaRole(& meta_src_role) );
		COMTHROW( dst->get_MetaRole(& meta_dst_role) );

		CString path = _T("src ");
		CString tmp;
		metaref_type mr;
		COMTHROW( meta_src_role->get_MetaRef(&mr) );
		tmp.Format(_T("%d "),mr);
		path += tmp;
		if(src_port!=NULL) {
			COMTHROW( src_port->get_MetaRole( & meta_src_port_role ) );
			ASSERT(meta_src_port_role!=NULL);

			COMTHROW( meta_src_port_role->get_MetaRef(&mr) );
			tmp.Format(_T("%d "),mr);
			path += tmp;
		}
		path += _T(", dst ");

		COMTHROW( meta_dst_role->get_MetaRef(&mr) );
		tmp.Format(_T("%d "),mr);
		path += tmp;

		if(dst_port != NULL) {
			COMTHROW( dst_port->get_MetaRole( & meta_dst_port_role ) );
			ASSERT(meta_dst_port_role!=NULL);

			COMTHROW( meta_dst_port_role->get_MetaRef(&mr) );
			tmp.Format(_T("%d "),mr);
			path += tmp;
		}

		CComBSTR bstr(path);


		CComPtr<IMgaMetaRoles> roles;
		CComPtr<IMgaMetaFCO> metaFco;

		COMTHROW(parent->get_Meta(&metaFco));

		CComPtr<IMgaMetaModel> metaModel;

		COMTHROW(metaFco.QueryInterface(&metaModel));

		COMTHROW(metaModel->LegalConnectionRoles(bstr, &roles));

		// now get valid roles for input aspect (if any)

		CDialogList cdl(_T("Select Connection Role Type"), CDialogList::CHKTEXT_STICKY);

		struct role_info {
			IMgaMetaRolePtr role;
			CString roleName;
		};
		std::vector<struct role_info> valid_roles;

		long role_count = roles->Count;

		for (int i = 1; i <= role_count; i++) {

			auto metaRole = roles->Item[i];
			ASSERT(metaRole != NULL);
			CString role_name = CString(static_cast<const TCHAR*>(metaRole->Name));

			if (aspect == NULL) {

				role_info info = { metaRole, role_name };

				valid_roles.push_back(info);
			} else {

				CComPtr<IMgaMetaParts> parts;
				COMTHROW( metaRole->get_Parts(&parts) );
				ASSERT(parts!=NULL);

				MGACOLL_ITERATE(IMgaMetaPart,parts) {

					CComPtr<IMgaMetaPart>& part_ix = MGACOLL_ITER;
					ASSERT(part_ix != NULL);

					if (part_ix->IsPrimary != VARIANT_FALSE && part_ix->ParentAspect == aspect) {

						role_info info = { metaRole, role_name };

						valid_roles.push_back(info);

						break;
					}

				}
				MGACOLL_ITERATE_END;

			} // else aspect not null

		} // iterate over roles


		std::sort(valid_roles.begin(), valid_roles.end(), [](const struct role_info& info1, const struct role_info& info2) {
			return wcscmp(info1.roleName, info2.roleName);
		});
		std::for_each(valid_roles.begin(), valid_roles.end(), [&cdl](const struct role_info& info) {
			cdl.m_sz_prelist.AddTail(info.roleName);
		});


		int valid_role_count = valid_roles.size();

		if (valid_role_count == 1) {
			*p = valid_roles[0].role.Detach();
			return S_OK;
		}

		// before popping up the dialog, see if it is in the knowledge map
		if(map_get_RoleByPathStr(bstr, metaModel,OBJTYPE_CONNECTION,p)) {
			if(mb_use_sticky) { // if sticky is enabled, return happily
				return S_OK;
			}
			else {		// if sticky is disabled, remove it from the map
				(*p)->Release();
				*p = NULL;
				map_put_RoleByPathStr(bstr, metaModel,OBJTYPE_CONNECTION,*p);
			}
		}

		if (!this->mb_is_interactive) {

//			if (valid_role_count == 0)
//				return RESOLV_ERR_get_ConnRoleByMeta;

			SetErrorInfo(CString(L"Could not resolve connection role"));
			return RESOLV_ERR_get_ConnRoleByMeta;
		}

		if (valid_role_count > 0) {

			if ((cdl.DoModal() != IDOK) || (cdl.mn_selection_index == LB_ERR)) {
				SetErrorInfo(CString(L"Cancelled by user"));
				return E_ABORT;

			} else {

				// get the entry in the list
				*p = valid_roles[cdl.mn_selection_index].role.Detach();

				if(cdl.mb_check_once == TRUE) {
					map_put_RoleByPathStr(bstr, metaModel, OBJTYPE_CONNECTION, *p);
				}
				return S_OK;

			}
		
		} else {

			CString sz_aspect_name;

			CComBSTR name;

			CComPtr<IMgaMetaRole> parent_role;
			COMTHROW( parent->get_MetaRole(&parent_role) );

			CString sz_parent_role(name);

			if (parent_role != NULL) {
				name.Empty();
				COMTHROW( parent_role->get_Name(&name) );
				sz_parent_role = CString(name);
			} else {

				CComPtr<IMgaMetaFCO> meta_parent;
				COMTHROW( parent->get_Meta(&meta_parent) );

				name.Empty();
				COMTHROW( meta_parent->get_Name(&name) );

				sz_parent_role = CString(name);
			}

			if (aspect != NULL) {
				name.Empty();
				COMTHROW( aspect->get_Name(&name) );
				sz_aspect_name = CString(name);
			}

			name.Empty();
			COMTHROW( parent->get_Name(&name) );
			CString sz_parent_name(name);

			name.Empty();
			COMTHROW( src->get_Name(&name) );
			CString sz_src_name(name);

			name.Empty();
			COMTHROW( meta_src_role->get_Name(&name) );
			CString sz_src_role_name(name);

			name.Empty();
			COMTHROW( dst->get_Name(&name) );
			CString sz_dst_name(name);

			name.Empty();
			COMTHROW( meta_dst_role->get_Name(&name) );
			CString sz_dst_role_name(name);

			CString sz_src_port(_T("<no port>"));
			CString sz_src_port_role(_T("<no role>"));
			if (src_port != NULL) {
				name.Empty();
				COMTHROW( src_port->get_Name(&name) );
				sz_src_port = CString(name);

				name.Empty();
				COMTHROW( meta_src_port_role->get_Name(&name) );
				sz_src_port_role = CString(name);
			}

			CString sz_dst_port(_T("<no port>"));
			CString sz_dst_port_role(_T("<no role>"));
			if (dst_port != NULL) {
				name.Empty();
				COMTHROW( dst_port->get_Name(&name) );
				sz_dst_port = CString(name);

				name.Empty();
				COMTHROW( meta_dst_port_role->get_Name(&name) );
				sz_dst_port_role = CString(name);
			}

			CString sz_format;
			CString sz_format_tmp;

			sz_format_tmp.Format(
				_T("Paradigm violation: cannot make a connection.\n")
				_T("Source: %s (%s)\n"),
				sz_src_name, sz_src_role_name
			);

			sz_format += sz_format_tmp;
				

			if (src_port != NULL) {

				sz_format_tmp.Format(
					_T("Source port: %s (%s)\n"),
					sz_src_port, sz_src_port_role				
				);

				sz_format += sz_format_tmp;
			}
				
			sz_format_tmp.Format(
				_T("Destination: %s (%s)\n"),
				sz_dst_name, sz_dst_role_name
			);
			sz_format += sz_format_tmp;

			if (dst_port != NULL) {
				sz_format_tmp.Format(
					_T("Destination port: %s (%s)\n"),
					sz_dst_port, sz_dst_port_role				
				);

				sz_format += sz_format_tmp;
			}

			if (sz_aspect_name != _T("")) {
				sz_format_tmp.Format(_T("Aspect: %s\n"),
					sz_aspect_name);
				sz_format += sz_format_tmp;
			}

			sz_format_tmp.Format(_T("Parent: %s (%s)"),
				sz_parent_name, sz_parent_role
			);
			sz_format += sz_format_tmp;

			SetErrorInfo(sz_format);
			return RESOLV_ERR_get_ConnRoleByMeta;

		}
	}

	COMCATCH(;)

}


STDMETHODIMP CMgaResolver::get_RefRoleByMeta(IMgaModel *parent, 
											 IMgaMetaAspect *aspect, 
											 IMgaFCO *src, IMgaMetaRole **p) {

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_OUT(p);

	ASSERT(parent != NULL);
	ASSERT(src!=NULL);

	COMTRY {

		CComBSTR path;

		CComPtr<IMgaMetaFCO> metafco;
		CComPtr<IMgaMetaModel> metamodel;

		COMTHROW( parent->get_Meta(&metafco) );
		ASSERT(metafco != NULL);

		COMTHROW( metafco.QueryInterface(&metamodel) );

		COMTHROW(src->GetAbsMetaPath(&path));
		CComPtr<IMgaMetaRoles> roles;
		COMTHROW(metamodel->LegalReferenceRoles(path,&roles));

		// copy roles into the dialog

		CDialogList cdl(_T("Select Reference Role Type"), CDialogList::CHKTEXT_ONETIMER);
		CList<int, int> valid_role_map;
		CComPtr<IMgaMetaRole> immr_ix;

		long role_count = 0;

		COMTHROW( roles->get_Count(&role_count) );

		for (int z = 1; z <= role_count; z++) {

			immr_ix = NULL;
			COMTHROW( roles->get_Item(z, &immr_ix) );
			ASSERT(immr_ix != NULL);

			if (aspect == NULL) {

				CComBSTR role_name;

				COMTHROW( immr_ix->get_Name(&role_name ) );

				cdl.m_sz_prelist.AddTail(CString(role_name)); 

				valid_role_map.AddTail(z);
				
			} else {

				CComPtr<IMgaMetaParts> parts;
				COMTHROW( immr_ix->get_Parts(&parts) );
				ASSERT(parts!=NULL);

				CComPtr<IMgaMetaPart> part_ix;

				MGACOLL_ITERATE(IMgaMetaPart,parts) {

					part_ix = MGACOLL_ITER;
					ASSERT(part_ix != NULL);

					VARIANT_BOOL vb_primary;

					COMTHROW(part_ix->get_IsPrimary(&vb_primary) );

					if (vb_primary != VARIANT_FALSE) {

						CComPtr<IMgaMetaAspect> part_aspect;

						COMTHROW( part_ix->get_ParentAspect(&part_aspect) );
						ASSERT(part_aspect != NULL);

						if (part_aspect == aspect) {

							CComBSTR role_name;

							COMTHROW( immr_ix->get_Name(&role_name ) );

							cdl.m_sz_prelist.AddTail(CString(role_name)); 

							valid_role_map.AddTail(z);

							break;
						}

					}
				}
				MGACOLL_ITERATE_END;

			} // else aspect not null

		} // iterate over roles

		int valid_role_count = valid_role_map.GetCount();

		if (valid_role_count == 1) {

			COMTHROW( roles->get_Item(valid_role_map.GetHead(), p) );
			return S_OK;

		}

		// before popping up the dialog, see if it is in the knowledge map
		if(map_get_RoleByPathStr(path, metamodel,OBJTYPE_REFERENCE,p)) {
			if(mb_use_sticky) { // if sticky is enabled, return happily
				return S_OK;
			}
			else {		// if sticky is disabled, remove it from the map
				(*p)->Release(); *p = NULL;
				map_put_RoleByPathStr(path, metamodel,OBJTYPE_REFERENCE,*p);
			}
		}

		if (!this->mb_is_interactive) {

			//if (valid_role_count == 0) {
			//	return RESOLV_ERR_get_ConnRoleByMeta;
			//}

			SetErrorInfo(CString(L"Could not resolve reference role'"));
			return RESOLV_ERR_get_ConnRoleByMeta;

		}

		if (valid_role_count > 0) {

			if (cdl.DoModal() != IDOK || (cdl.mn_selection_index == LB_ERR)) {
				SetErrorInfo(L"No item chosen from dialog");
				return E_ABORT;
			} else {

				// get the entry in the list

				immr_ix = NULL;
				COMTHROW( roles->get_Item(
					valid_role_map.GetAt(valid_role_map.FindIndex(cdl.mn_selection_index)), p) );

				if(cdl.mb_check_once == TRUE) {
					map_put_RoleByPathStr(path, metamodel,OBJTYPE_REFERENCE,*p);
				}
				return S_OK;

			}
		
		} else {

			CString sz_aspect_name;

			CComBSTR name;

			CComPtr<IMgaMetaRole> parent_role;
			COMTHROW( parent->get_MetaRole(&parent_role) );

			CComPtr<IMgaMetaRole> src_role;
			COMTHROW( src->get_MetaRole(&src_role) );
			// note: it is ok to have a null src_role

			CString sz_parent_role;

			if (parent_role != NULL) {
				name.Empty();
				COMTHROW( parent_role->get_Name(&name) );
				sz_parent_role = CString(name);
			} else {
				CComPtr<IMgaMetaFCO> meta_parent;
				COMTHROW( parent->get_Meta(&meta_parent) );

				name.Empty();
				COMTHROW( meta_parent->get_Name(&name) );

				sz_parent_role = CString(name);
			}

			if (aspect != NULL) {
				name.Empty();
				COMTHROW( aspect->get_Name(&name) );
				sz_aspect_name = CString(name);
			}

			name.Empty();
			COMTHROW( parent->get_Name(&name) );
			CString sz_parent_name(name);

			name.Empty();
			COMTHROW( src->get_Name(&name) );
			CString sz_src_name(name);

			CString sz_src_role_name;
			name.Empty();
			if (src_role != NULL) {
				COMTHROW( src_role->get_Name(&name) );
			} else {
				CComPtr<IMgaMetaFCO> meta_src;
				COMTHROW( src->get_Meta(&meta_src) );

				name.Empty();
				COMTHROW( meta_src->get_Name(&name) );
			}
			sz_src_role_name = CString(name);

			CString sz_format;
			CString sz_format_tmp;

			sz_format_tmp.Format(
				_T("Cannot insert reference.\n")
				_T("Source: %s (%s)\n"),
				sz_src_name, sz_src_role_name
				);
			sz_format += sz_format_tmp;

			if (sz_aspect_name != _T("")) {

				sz_format_tmp.Format(
					_T("Aspect: %s\n"),
					sz_aspect_name
					);
				sz_format += sz_format_tmp;


			}

			sz_format_tmp.Format(
				_T("Parent: %s (%s)"),
				sz_parent_name, sz_parent_role
			);
			sz_format += sz_format_tmp;

			SetErrorInfo(sz_format);

			return RESOLV_ERR_get_ConnRoleByMeta;

		}

	}

	COMCATCH(;)

}

STDMETHODIMP CMgaResolver::get_RolesByMetas(IMgaModel *dst_parent, 
											IMgaFCOs *src, 
											IMgaMetaAspect * aspect, 
											BSTR sz_aspect,
											IMgaMetaRoles ** p) {


	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr;

	CComPtr<IMgaFCO> fco_ix;
	CComPtr<IMgaMetaFCO> mfco_ix;
	CComPtr<IMgaMetaRole> role_ix;

	CHECK_OUT(p);

	ASSERT(dst_parent != NULL);
	ASSERT(src != NULL);

	COMTRY {

		long fco_count = 0;

		CComBSTR sz_aspect_found;

		if (aspect != NULL) {
			COMTHROW( aspect->get_Name(&sz_aspect_found) );
		}

		COMTHROW( src->get_Count(&fco_count) );

		CComPtr<IMgaMetaRoles> roles;

		COMTHROW(roles.CoCreateInstance(L"Mga.MgaMetaRoles"));

		for (int ctr_z = fco_count; ctr_z > 0; ctr_z--) {

			fco_ix = NULL;
			COMTHROW( src->get_Item(ctr_z, &fco_ix) );
			ASSERT(fco_ix != NULL);

			mfco_ix = NULL;
			COMTHROW( fco_ix->get_Meta(&mfco_ix) );
			ASSERT(mfco_ix != NULL);

			CComBSTR mfco_name_ix;

			COMTHROW( mfco_ix->get_Name(&mfco_name_ix) );

			CComPtr<IMgaMetaRole> mfco_role_ix;
			CComBSTR mfco_role_ix_sz;

			COMTHROW( fco_ix->get_MetaRole(&mfco_role_ix) );

			if (mfco_role_ix != NULL) {
				COMTHROW( mfco_role_ix->get_Name(&mfco_role_ix_sz) );
			}

			role_ix = NULL;
			COMTHROW( 
				((hr = this->get_RoleByStr(dst_parent, 
					mfco_name_ix, 
					OBJTYPE_NULL, 
					(BSTR) mfco_role_ix_sz, // role
					((aspect == NULL) ? sz_aspect : (BSTR) sz_aspect_found), // aspect
					&role_ix)) == RESOLV_ERR_get_RoleByStr) ?
					S_OK : hr
					);

			// add pointer to collection

			roles->Append(role_ix);
		}

		// copy collection to p

		*p = roles.Detach();

//		return RESOLV_ERR_get_RolesByMetas;

	}

	COMCATCH(;)

}


bool CMgaResolver::map_get_FolderByStr(IMgaMetaFolder *&parent, 
									   BSTR kind, 
									   IMgaMetaFolder **p) {

	Cmap_elmt_type_FolderByStr * map_find = NULL;

	// get a key instance

	Cmap_elmt_type_FolderByStr * map_key = 
		new Cmap_elmt_type_FolderByStr(parent, kind);

	// now do "lookup" function

	bool found = false;
	POSITION pos = this->map_FolderByStr.GetHeadPosition();

	while (pos != NULL) {

		map_find = this->map_FolderByStr.GetNext(pos);

		if (*map_find == *map_key) {
			found = true;
			break;
		}
	}

	if (!found) {
		delete map_key;
		return false;
	}

	map_find->p.CopyTo(p);

	delete map_key;
	return true;

} // eo CMgaResolver::map_get_FolderByStr()

bool CMgaResolver::map_put_FolderByStr(IMgaMetaFolder *&parent , 
									   BSTR kind, 
									   IMgaMetaFolder *p) {

	// ASSERT: this thing had better not be in here already!!

	// get a key instance

	Cmap_elmt_type_FolderByStr * map_key = 
		new Cmap_elmt_type_FolderByStr(parent, kind);

	// set map_to element

	map_key->p = p;

	this->map_FolderByStr.AddTail(map_key);

	return true;

} // eo CMgaResolver::map_put_FolderByStr()


bool CMgaResolver::map_get_AttrByStr(IMgaMetaFCO *&parent, 
									 BSTR kind, 
									 IMgaMetaAttribute **p) {

	Cmap_elmt_type_AttrByStr * map_find = NULL;

	// get a key instance

	Cmap_elmt_type_AttrByStr * map_key = 
		new Cmap_elmt_type_AttrByStr(parent, kind);

	// now do "lookup" function

	bool found = false;
	POSITION pos = this->map_AttrByStr.GetHeadPosition();

	while (pos != NULL) {

		map_find = this->map_AttrByStr.GetNext(pos);

		if (*map_find == *map_key) {
			found = true;
			break;
		}
	}

	if (!found) {
		delete map_key;
		return false;
	}

	map_find->p.CopyTo(p);

	delete map_key;
	return true;

}

bool CMgaResolver::map_put_AttrByStr(IMgaMetaFCO *&parent, 
									 BSTR kind, 
									 IMgaMetaAttribute *p) {

	// ASSERT: this thing had better not be in here already!!

	// get a key instance

	Cmap_elmt_type_AttrByStr * map_key = 
		new Cmap_elmt_type_AttrByStr(parent, kind);

	// set map_to element

	map_key->p = p;

	this->map_AttrByStr.AddTail(map_key);

	return true;

}

bool CMgaResolver::map_get_KindByStr(IMgaMetaFolder *&parent, 
									 BSTR kind, 
									 objtype_enum objtype, 
									 IMgaMetaFCO **p) {

	ASSERT(p != 0 && *p == 0);

	Cmap_elmt_type_KindByStr * map_find = NULL;

	// get a key instance

	Cmap_elmt_type_KindByStr * map_key = 
		new Cmap_elmt_type_KindByStr(parent, kind, objtype);

	// now do "lookup" function

	bool found = false;
	POSITION pos = this->map_KindByStr.GetHeadPosition();

	while (pos != NULL) {

		map_find = this->map_KindByStr.GetNext(pos);

		if (*map_find == *map_key) {
			found = true;
			break;
		}
	}

	if (!found) {
		delete map_key;
		return false;
	}

	map_find->p.CopyTo(p);

	delete map_key;
	return true;

}
 

bool CMgaResolver::map_put_KindByStr(IMgaMetaFolder *&parent, 
									 BSTR kind, 
									 objtype_enum objtype, 
									 IMgaMetaFCO *p) {

	// ASSERT: this thing had better not be in here already!!

	// get a key instance

	Cmap_elmt_type_KindByStr * map_key = 
		new Cmap_elmt_type_KindByStr(parent, kind, objtype);

	// set map_to element

	map_key->p = p;

	this->map_KindByStr.AddTail(map_key);

	return true;

}

bool CMgaResolver::map_get_RoleByStr(IMgaMetaModel *&parent, 
									 BSTR kind, 
									 objtype_enum objtype,
									 BSTR &role, 
									 BSTR &aspect, 
									 IMgaMetaRole **p) {

	Cmap_elmt_type_RoleByStr * map_find = NULL;

	// get a key instance

	Cmap_elmt_type_RoleByStr * map_key = 
		new Cmap_elmt_type_RoleByStr(parent, kind, objtype, role, aspect);

	// now do "lookup" function

	bool found = false;
	POSITION pos = this->map_RoleByStr.GetHeadPosition();

	while (pos != NULL) {

		map_find = this->map_RoleByStr.GetNext(pos);

		if (*map_find == *map_key) {
			found = true;
			break;
		}
	}

	if (!found) {
		delete map_key;
		return false;
	}

	map_find->p.CopyTo(p);

	delete map_key;
	return true;

}

bool CMgaResolver::map_put_RoleByStr(IMgaMetaModel *&parent, 
									 BSTR kind, 
									 objtype_enum objtype,
									 BSTR &role, 
									 BSTR &aspect, 
									 IMgaMetaRole *p) {


	// ASSERT: this thing had better not be in here already!!

	// get a key instance

	Cmap_elmt_type_RoleByStr * map_key = 
		new Cmap_elmt_type_RoleByStr(parent, kind, objtype, role, aspect);

	// set map_to element

	map_key->p = p;

	this->map_RoleByStr.AddTail(map_key);

	return true;

}


bool CMgaResolver::map_get_RoleByPathStr(BSTR path, IMgaMetaBase *parent, objtype_enum objtype, IMgaMetaRole **p) {

	Cmap_elmt_type_RoleByPathStr * map_find = NULL;

	// get a key instance

	Cmap_elmt_type_RoleByPathStr * map_key = 
		new Cmap_elmt_type_RoleByPathStr(path, parent, objtype);

	// now do "lookup" function

	bool found = false;
	POSITION pos = this->map_RoleByPathStr.GetHeadPosition();

	while (pos != NULL) {

		map_find = this->map_RoleByPathStr.GetNext(pos);

		if (*map_find == *map_key) {
			found = true;
			break;
		}
	}

	delete map_key;
	if (found) 	map_find->p.CopyTo(p);

	return found;

}

bool CMgaResolver::map_put_RoleByPathStr(BSTR path, IMgaMetaBase *parent, objtype_enum objtype, IMgaMetaRole *p) {

	// get a key instance

	Cmap_elmt_type_RoleByPathStr * map_key = 
		new Cmap_elmt_type_RoleByPathStr(path, parent, objtype);

	POSITION pos = this->map_RoleByPathStr.GetHeadPosition();
	while (pos != NULL) {

		Cmap_elmt_type_RoleByPathStr *map_find = this->map_RoleByPathStr.GetAt(pos);
		if (*map_find == *map_key) {
			if(p) {
				map_find->p = p;
			}
			else {
				delete map_find;
				map_RoleByPathStr.RemoveAt(pos);
			}
			delete map_key;
			return true;
		}
		map_RoleByPathStr.GetNext(pos);
	}

	// not found in list, set map_to element
	
	if(p) {
		map_key->p = p;
		this->map_RoleByPathStr.AddTail(map_key);
	}

	return true;

}

CComPtr<IGMEOLEApp>	CMgaResolver::get_GME(CComPtr<IMgaProject> project)
{
	CComPtr<IGMEOLEApp> gme;
	if (mb_is_interactive && (project != NULL)) {		
		CComBSTR bstrName(L"GME.Application");
		CComPtr<IMgaClient> pClient;
		HRESULT hr = project->GetClientByName(bstrName, &pClient);
		if (SUCCEEDED(hr) && pClient) {
			CComPtr<IDispatch> pDispatch;
			hr = pClient->get_OLEServer(&pDispatch);
			if (SUCCEEDED(hr) && pDispatch) {
				hr = pDispatch.QueryInterface(&gme);
				if (FAILED(hr)) {
					gme = NULL;
				}
			}
		}
	}
	return gme;
}


// eof MgaResolver.cpp;