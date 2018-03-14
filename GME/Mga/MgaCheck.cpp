// MgaCheck.cpp : Implementation of FCO checking operations
#include "stdafx.h"
#include "MgaFCO.h"


void setcheck(CMgaProject *mgaproject, CoreObj &ob, int code) {
	coreobjhash &list = mgaproject->objstocheck;
	coreobjhash::iterator ii;
	if((ii = list.find(ob)) != list.end()) (*ii).second |= code;
	else list.insert(coreobjhash::value_type(ob, code));
}


void docheck(CMgaProject *mgaproject) {
	coreobjhash &list = mgaproject->objstocheck;
	coreobjhash::iterator i;
	
	for(i = list.begin(); i != list.end(); ++i) {
		int &mode = (*i).second;
		if(mode & CHK_REFEREES) {
			mode = CHK_REFCONNS;

//#pragma bookmark todo: put back stuff that needs testing
		}
		if(mode & CHK_REFCONNS) {
			mode = CHK_SELF;
		}
//#pragma bookmark todo: put back stuff that needs testing
	}
	for(i = list.begin(); i != list.end(); ++i) {
		if (((*i).first).IsDeleted())
			continue;
		if ((*i).second & CHK_ILLEGAL)
		{
			COMTHROW(E_MGA_OP_REFUSED);
		}
		else if((*i).second & CHK_SELF)
		{
			HRESULT hr = ObjForCore((*i).first)->Check();
			if (FAILED(hr))
			{
				_bstr_t err;
				if (GetErrorInfo(err.GetAddress()))
				{
					throw_com_error(hr, err);
				}
				else
				{
					COMTHROW(hr);
				}
			}
		}
	}
	list.clear();
}






void get_absmetapath(CoreObj &t, CComBSTR &abspath) {
	metaref_type mr = t[ATTRID_ROLEMETA];
	CoreObj parent = t[ATTRID_FCOPARENT];
	if (!parent)
		COMTHROW(E_MGA_OBJECT_DELETED);
	if(mr) {
		get_absmetapath(parent, abspath);
	}
	else mr = t[ATTRID_META];
	OLECHAR buf[20];
	swprintf(buf, 20, OLESTR(" %ld"), mr);
	COMTHROW(abspath.Append(buf));
}

void get_relmetapath(CoreObj &t, CComBSTR &relpath, CoreObj &pathbeg) {
#ifdef DEBUG
	CComBSTR tname = t[ATTRID_NAME], bname = pathbeg[ATTRID_NAME];
#endif
	if (COM_EQUAL(t, pathbeg))
		return;
	metaref_type mr = t[ATTRID_ROLEMETA];
	CoreObj parent = t[ATTRID_FCOPARENT];
	if(!mr) {  // == t.IsRootFCO()
		COMTHROW(E_MGA_NOT_CONNECTIBLE);
	}
	get_relmetapath(parent, relpath, pathbeg);
	OLECHAR buf[20];
	swprintf(buf, 20, OLESTR(" %ld"), mr);
	COMTHROW(relpath.Append(buf));
}



HRESULT FCO::GetAbsMetaPath(BSTR *abspath) {
	COMTRY {
		CHECK_OUTSTRPAR(abspath);
		CComBSTR ap;
		ap.Attach(*abspath);
		get_absmetapath(self, ap);		
		*abspath = ap.Detach();
	} COMCATCH(;);
}

HRESULT FCO::GetRelMetaPath(IMgaFCO *begfco, BSTR *rp, IMgaFCOs *refs) {
	COMTRY {
		CHECK_OUTSTRPAR(rp);
		CHECK_MYINPTRPAR(begfco);
		CComBSTR relpath;
		relpath.Attach(*rp);
		CoreObj curp(begfco);
		if(refs) {
			MGACOLL_ITERATE(IMgaFCO, refs) {
				CoreObj r(MGACOLL_ITER);
				ASSERT(GetMetaID(r) == DTID_REFERENCE);
				get_relmetapath(r, relpath, curp);
				curp = r[ATTRID_REFERENCE];
				ASSERT(curp);
				{
					metaref_type mr = curp[ATTRID_META];
					OLECHAR buf[20];
					swprintf(buf, 20, OLESTR(" %ld"), mr);
					COMTHROW(relpath.Append(buf));
				}
			}
			MGACOLL_ITERATE_END;
		}
		get_relmetapath(self, relpath, curp);		
		*rp = relpath.Detach();
	} COMCATCH(;);
}


HRESULT FCO::CheckTree() {
	COMTRY {
		HRESULT hr = Check();
		if (FAILED(hr))
			return hr;
		objtype_enum typ;
		COMTHROW(get_ObjType(&typ));
		if(typ == OBJTYPE_FOLDER) {
				ITERATE_THROUGH(self[ATTRID_FCOPARENT+ATTRID_COLLECTION]) {
					if (FAILED(hr = ObjForCore(ITER)->CheckTree()))
						return hr;
				}
		}
		else {
			CComPtr<IMgaMetaFCO> mfco;
			COMTHROW(get_Meta(&mfco));
			CComPtr<IMgaMetaAttributes> mas;
			COMTHROW(mfco->get_Attributes(&mas));
			ITERATE_THROUGH(self[ATTRID_ATTRPARENT+ATTRID_COLLECTION]) {
					CComPtr<IMgaMetaAttribute> ma;
					COMTHROW(mfco->get_AttributeByRef(ITER[ATTRID_META], &ma));
					if(!ma) COMTHROW(E_MGA_META_VIOLATION);
					attval_enum vt;
					COMTHROW(ma->get_ValueType(&vt));
					int exp = -1;
					if(vt >= ATTVAL_STRING && vt <= ATTVAL_BOOLEAN) exp = DTID_STRATTR + vt - ATTVAL_STRING;
					else if(vt == ATTVAL_ENUM || vt == ATTVAL_DYNAMIC) exp = DTID_STRATTR;

					if(exp < 0 || exp != ITER.GetMetaID()) COMTHROW(E_MGA_META_VIOLATION);

					if(vt == ATTVAL_ENUM) {
						CComBSTR val = ITER[ATTRID_STRATTR];
						CComPtr<IMgaMetaEnumItems> enumitems;
						COMTHROW(ma->get_EnumItems(&enumitems));
						MGACOLL_ITERATE(IMgaMetaEnumItem, enumitems) {
							CComBSTR eval;
							COMTHROW(MGACOLL_ITER->get_Value(&eval));
							if(val == eval) break;
							
						} 
						if(MGACOLL_AT_END) COMTHROW(E_META_INVALIDATTR);
						MGACOLL_ITERATE_END;
					}
			}
			if(typ == OBJTYPE_MODEL) {
				ITERATE_THROUGH(self[ATTRID_FCOPARENT+ATTRID_COLLECTION]) {
					COMTHROW(ObjForCore(ITER)->CheckTree());
				}
			}
		}
	}
	COMCATCH(;);
}


HRESULT FCO::Check() {
	if(mgaproject->checkofftemporary) return S_OK;
	COMTRY {
		objtype_enum typ, parenttyp;
		COMTHROW(get_ObjType(&typ));
		CComPtr<IMgaContainer> parent;
		COMTHROW(GetParent(&parent, &parenttyp));

		if(typ == OBJTYPE_FOLDER) {
			CComPtr<IMgaMetaFolder> parentmf, metaf;
			COMTHROW(get_MetaFolder(&metaf));
			if (!metaf) {
				_bstr_t name;
				this->get_Name(name.GetAddress());
				metaref_type metaRef = 0;
				this->get_MetaRef(&metaRef);
				int iMetaRef = metaRef;
				wchar_t buf[512];
				swprintf_s(buf, L"'%s' contains child '%s' with unknown metaref %d", static_cast<const wchar_t*>(parent->Name), static_cast<const wchar_t*>(name), iMetaRef);
				throw_com_error(E_MGA_META_VIOLATION, buf);
			} else if(!parent) {												// root folder
				CComPtr<IMgaMetaProject> mp;
				COMTHROW(mgaproject->get_RootMeta(&mp));
				CComPtr<IMgaMetaFolder> mf;
				COMTHROW(mp->get_RootFolder(&mf));
				if(!COM_EQUAL(metaf, mf)) COMTHROW(E_MGA_META_VIOLATION);
			}
			else {														// non-root folder
				COMTHROW(CComQIPtr<IMgaFolder>(parent)->get_MetaFolder(&parentmf));
			    CComPtr<IMgaMetaFolders> fkinds;
			    COMTHROW(parentmf->get_LegalChildFolders(&fkinds));
				MGACOLL_ITERATE(IMgaMetaFolder, fkinds) {
						if(COM_EQUAL(metaf, MGACOLL_ITER)) break;
				}
				if(MGACOLL_AT_END) {
					//special case: libroot in rootfolder
					// (assume this possibility when checkoff is true)
					CComBSTR libname;
					bool stillok = false;
					COMTHROW(get_LibraryName(&libname));
					if(libname  || 
						mgaproject->checkoff || 
						self[ATTRID_PERMISSIONS] & EXEMPT_FLAG ) {
							objtype_enum t2;
							CComPtr<IMgaContainer> p2;
							COMTHROW(parent->GetParent( &p2, &t2));
							CComBSTR libnm2;
							COMTHROW( CComQIPtr<IMgaFolder>(parent)->get_LibraryName( &libnm2));

							// stillok will be turned true if
							// parent of library is either another library or the rootfolder
							if( t2 == OBJTYPE_NULL) stillok = true;
							else if( libnm2)        stillok = true;

							if( !stillok)
							{
								// inner libraries need some special care
								// because upon Import XML get_LibraryName gives back 0x0000
								// since the LIBROOT_FLAG, LIBRARY_FLAG flags are not set yet
								CComPtr<IMgaMetaProject> mp;
								COMTHROW(mgaproject->get_RootMeta(&mp));
								CComPtr<IMgaMetaFolder> mf;
								COMTHROW(mp->get_RootFolder(&mf));
								// ok, if current element and its parent are both rootfolders
								stillok = COM_EQUAL( metaf, parentmf) && COM_EQUAL( metaf, mf);
							}
							if (!stillok)
								COMTHROW(E_MGA_META_VIOLATION);
					}
					else
					{
						_bstr_t err = L"'";
						err += parentmf->Name + "' cannot contain '" + metaf->Name + L"'";
						throw_com_error(E_MGA_META_VIOLATION, err);
					}
				}
				MGACOLL_ITERATE_END;
			}
		}
		else {
#ifdef DEBUG
			CComBSTR bb = self[ATTRID_NAME];
#endif
			CComPtr<IMgaMetaFCO> meta;
			COMTHROW(get_Meta(&meta));
	
			CComPtr<IMgaMetaRole> metarole;
			COMTHROW(get_MetaRole( &metarole));
		
	
			if (meta == NULL) {
				_bstr_t name;
				this->get_Name(name.GetAddress());
				metaref_type metaRef = 0;
				this->get_MetaRef(&metaRef);
				int iMetaRef = metaRef;
				wchar_t buf[512];
				swprintf_s(buf, L"'%s' contains child '%s' with unknown metaref %d", static_cast<const wchar_t*>(parent->Name), static_cast<const wchar_t*>(name), iMetaRef);
				throw_com_error(E_MGA_META_VIOLATION, buf);
			} else if(parenttyp == OBJTYPE_MODEL) {
				if(!metarole)  COMTHROW(E_MGA_NO_ROLE);
				CComPtr<IMgaMetaModel> parentmeta1;
				CComPtr<IMgaMetaFCO> parentmeta2;
				COMTHROW(metarole->get_ParentModel(&parentmeta1));
				COMTHROW(CComQIPtr<IMgaModel>(parent)->get_Meta(&parentmeta2));
				if(!COM_EQUAL(parentmeta1, parentmeta2)) COMTHROW(E_MGA_INVALID_ROLE);
	
				CComPtr<IMgaMetaFCO> kindfromrole;
				COMTHROW(metarole->get_Kind(&kindfromrole));
				if(!COM_EQUAL(meta, kindfromrole)) COMTHROW(E_MGA_INVALID_ROLE);
			}
			else if(parenttyp == OBJTYPE_FOLDER) {
				if(metarole)  COMTHROW(E_MGA_INVALID_ROLE);
				CComPtr<IMgaMetaFolder> parentmf;
				COMTHROW(CComQIPtr<IMgaFolder>(parent)->get_MetaFolder(&parentmf));
				CComPtr<IMgaMetaFCOs> kinds;
				COMTHROW(parentmf->get_LegalRootObjects(&kinds));
				MGACOLL_ITERATE(IMgaMetaFCO, kinds) {
				  if(COM_EQUAL(meta, MGACOLL_ITER)) break;
				}
				if (MGACOLL_AT_END)
				{
					IMgaMetaFCOPtr r = meta.p;
					_bstr_t err = L"'";
					err += r->Name + "' cannot be contained in the root folder";
					throw_com_error(E_MGA_META_VIOLATION, err);
				}
				MGACOLL_ITERATE_END;
			}
			else if(parenttyp == OBJTYPE_NULL) {
				CComPtr<IMgaMetaFolder> mf;
				CComPtr<IMgaMetaProject> mp;
				COMTHROW(mgaproject->get_RootMeta(&mp));
				COMTHROW(mp->get_RootFolder(&mf));
				if(!COM_EQUAL(mf, meta)) COMTHROW(E_MGA_META_VIOLATION);
			}
			if(typ >= OBJTYPE_REFERENCE && typ <= OBJTYPE_SET) {
				if(mgaproject->checkoff) {
					if(mgaproject->deferredchecks.find(this) == mgaproject->deferredchecks.end()) {
						mgaproject->deferredchecks.insert(this);
					}
				}
				else
					CheckRCS();
			}
		}
	} COMCATCH(;);
}


void FCO::CheckRCS() {
	objtype_enum typ;
	CComPtr<IMgaMetaFCO> meta;
	COMTHROW(get_Meta(&meta));

	COMTHROW(get_ObjType(&typ));
	if(typ == OBJTYPE_REFERENCE) {
		CComQIPtr<IMgaMetaReference> refmeta = meta;
		CoreObj target = self[ATTRID_REFERENCE];
		if(target) {
			CComBSTR abspath = "";
			get_absmetapath(target, abspath);
			VARIANT_BOOL good;
			COMTHROW(refmeta->CheckPath(abspath, &good));
			if(!good)
				COMTHROW(E_MGA_META_VIOLATION);
		}
		// inherited ref can only refer to a derived instance of the target
		// of its base. (Derived null ref is also rejected, but null ref in archetype is accepted)
		CoreObj basetype = self[ATTRID_DERIVED];
		if (basetype && !(mgaproject->preferences & MGAPREF_FREEINSTANCEREFS))
		{
			CoreObj base_target = basetype[ATTRID_REFERENCE];
			if (base_target)
			{
				for ( ; target; target = target[ATTRID_DERIVED]) {
					if (COM_EQUAL(base_target, target))
						break;
				}
				if (!target)
					COMTHROW(E_MGA_INVALID_TARGET);
			}
		}
	}
	else if(typ == OBJTYPE_SET) {
		CComQIPtr<IMgaMetaSet> setmeta = meta;
		CComPtr<IMgaFCOs> members;
		COMTHROW(get_Members(&members));
		MGACOLL_ITERATE(IMgaFCO, members) {
			CComBSTR relpath = "";
			get_relmetapath(CoreObj(MGACOLL_ITER), relpath, CoreObj(self[ATTRID_FCOPARENT]));
			VARIANT_BOOL good;
			COMTHROW(setmeta->CheckPath(relpath, &good));
			if(!good)
				COMTHROW(E_MGA_META_VIOLATION);
		} MGACOLL_ITERATE_END;
	}
	else if(typ == OBJTYPE_CONNECTION) {
		auto error_gen = [this]() -> _bstr_t {
			IMgaSimpleConnectionPtr simpleConn;
			getinterface(&simpleConn, 0);
			if (!simpleConn)
			{
				throw;
			}
			_bstr_t err = L"Illegal connection ";
			err += simpleConn->Name;
			err += " (";
			err += simpleConn->Meta->Name;
			err += ")";
			if (simpleConn->Src)
			{
				err += L" from ";
				err += simpleConn->Src->Name;
				err += L" (";
				err += simpleConn->Src->Meta->Name;
				err += L") ";
				if (simpleConn->SrcReferences && simpleConn->SrcReferences->Count > 0)
				{
					err += L" through refport ";
					err += simpleConn->SrcReferences->GetItem(1)->Name;
					err += "->";
					IMgaReferencePtr ref = simpleConn->SrcReferences->GetItem(1);
					err += ref->Referred->Name;
					err += L" (";
					err += simpleConn->SrcReferences->GetItem(1)->Meta->Name;
					err += L")";
				}
			}
			if (simpleConn->Dst)
			{
				err += L" to ";
				err += simpleConn->Dst->Name;
				err += L" (";
				err += simpleConn->Dst->Meta->Name;
				err += L")";
				if (simpleConn->DstReferences && simpleConn->DstReferences->Count > 0)
				{
					err += L" through refport ";
					err += simpleConn->DstReferences->GetItem(1)->Name;
					err += "->";
					IMgaReferencePtr ref = simpleConn->DstReferences->GetItem(1);
					err += ref->Referred->Name;
					err += L" (";
					err += simpleConn->DstReferences->GetItem(1)->Meta->Name;
					err += L")";
				}
			}
			return err;
		};
		CComQIPtr<IMgaMetaConnection> connmeta = meta;
		CComBSTR relpath;
		CComBSTR separator = "";
		CoreObjs roles = self[ATTRID_CONNROLE+ATTRID_COLLECTION];
		long c;
		COMTHROW(roles->get_Count(&c));
		if( simpleconn() && 
			!(mgaproject->preferences & MGAPREF_IGNORECONNCHECKS) &&
			c != 2)
		{
			CComBSTR msg;
			msg += "The connection named \"";
			CComBSTR name;
			get_Name(&name);
			msg += name;
			if (c == 0)
				msg += "\" doesn't have endpoints!";
			else if (c == 1)
				msg += "\" has only one endpoint!";
			else
				msg += "\" has more than two endpoints!";
			msg += "\nProperties:\n";
			CComBSTR relpath2;
			CComBSTR separator2 = "";
			COMTRY {
				ITERATE_THROUGH(roles) {
					CComBSTR n = ITER[ATTRID_NAME];
					COMTHROW(relpath2.Append(separator2));
					COMTHROW(relpath2.Append("Connection role and id: "));
					COMTHROW(relpath2.Append(n));
					CoreObj curp(self[ATTRID_FCOPARENT]);
					CoreObjs segs = ITER[ATTRID_CONNSEG+ATTRID_COLLECTION];
					segs.Sort();
					{
						ITERATE_THROUGH(segs) {
							CoreObj seg = ITER[ATTRID_SEGREF];
							ASSERT(seg && GetMetaID(seg) == DTID_REFERENCE);
							get_relmetapath(seg, relpath2, curp);
							curp = seg[ATTRID_REFERENCE];
							if (!curp)
								COMTHROW(E_MGA_META_VIOLATION);;
						}
					}
					CoreObj end = ITER[ATTRID_XREF];
					CComPtr<IMgaFCO> endfco;
					ObjForCore(end)->getinterface(&endfco);
					get_relmetapath(end, relpath2, curp);
					COMTHROW(relpath2.Append(", FCO name: "));
					CComBSTR fcoName;
					COMTHROW(endfco->get_Name(&fcoName));
					COMTHROW(relpath2.Append(fcoName));
					COMTHROW(relpath2.Append(", FCO ID: "));
					CComBSTR fcoID;
					COMTHROW(endfco->get_ID(&fcoID));
					COMTHROW(relpath2.Append(fcoID));
					separator2 = "\n";
				}
			}
			catch(hresult_exception &e)
			{
				throw_com_error(e.hr, error_gen());
			}
			msg += relpath2;
			msg += "\nMissing endpoints sometimes can be generated by UDM based interpreters.";
			throw_com_error(E_MGA_META_VIOLATION, msg);
		}
		try
		{
			ITERATE_THROUGH(roles) {
				CComBSTR n = ITER[ATTRID_NAME];
				COMTHROW(relpath.Append(separator));
				COMTHROW(relpath.Append(n));
				CoreObj curp(self[ATTRID_FCOPARENT]);
				CoreObjs segs = ITER[ATTRID_CONNSEG+ATTRID_COLLECTION];
				segs.Sort();
				{
					ITERATE_THROUGH(segs) {
						CoreObj seg = ITER[ATTRID_SEGREF];
 						ASSERT(seg && GetMetaID(seg) == DTID_REFERENCE);
						get_relmetapath(seg, relpath, curp);
						curp = seg[ATTRID_REFERENCE];
						if (!curp)
							COMTHROW(E_MGA_META_VIOLATION);;
					}
				}
				CoreObj end = ITER[ATTRID_XREF];
				CComPtr<IMgaFCO> endfco;
				ObjForCore(end)->getinterface(&endfco);
				get_relmetapath(end, relpath, curp);
				separator = ", ";
			}
		}
		catch (const hresult_exception& e)
		{
			_bstr_t err = error_gen();
			throw_com_error(E_MGA_NOT_CONNECTIBLE, err);
		}
		VARIANT_BOOL good;
		COMTHROW(connmeta->CheckPaths(relpath, &good));
		if (!good)
			throw_com_error(E_MGA_META_VIOLATION, error_gen() + L": meta violation");

	}
}


HRESULT FCO::put_Exempt(VARIANT_BOOL newVal) {
	COMTRY_IN_TRANSACTION {
		CHECK_INBOOLPAR(newVal);
		if(newVal) {
			self[ATTRID_PERMISSIONS] = self[ATTRID_PERMISSIONS] | EXEMPT_FLAG;
		}
		else {
			self[ATTRID_PERMISSIONS] = self[ATTRID_PERMISSIONS] & ~EXEMPT_FLAG;
			COMTHROW(Check());
		}
		SelfMark(OBJEVENT_PROPERTIES);
	} COMCATCH_IN_TRANSACTION(;);
}

HRESULT FCO::get_Exempt(VARIANT_BOOL *pVal) {
	COMTRY {
		CHECK_OUTPAR(pVal);
		*pVal = self[ATTRID_PERMISSIONS] & EXEMPT_FLAG ? VARIANT_TRUE : VARIANT_FALSE; 
	} COMCATCH(;);
}

void change_mode( CoreObj& obj, bool ro)
{
	if( ro)
	{
		obj[ATTRID_PERMISSIONS] = obj[ATTRID_PERMISSIONS] | READONLY_FLAG;
		CoreObjMark( obj, OBJEVENT_MARKEDRO|OBJEVENT_PROPERTIES);
	}
	else
	{
		obj[ATTRID_PERMISSIONS] = obj[ATTRID_PERMISSIONS] & ~READONLY_FLAG;
		CoreObjMark( obj, OBJEVENT_MARKEDRW|OBJEVENT_PROPERTIES);
	}
}

void change_mode_rec( CoreObj& obj, bool ro)
{
	change_mode( obj, ro);

	if( obj.IsContainer())
	{
		ITERATE_THROUGH(obj[ATTRID_FCOPARENT+ATTRID_COLLECTION]) 
		{
			change_mode_rec( ITER, ro);
		}
	}
}

HRESULT FCO::PutReadOnlyAccess( VARIANT_BOOL pReadOnly)
{
	COMTRY_IN_TRANSACTION {
		CHECK_INBOOLPAR(pReadOnly);

		change_mode( self, pReadOnly == VARIANT_TRUE);
		COMTHROW(Check());

	} COMCATCH_IN_TRANSACTION(;);
}

HRESULT FCO::PutReadOnlyAccessWithPropagate( VARIANT_BOOL pReadOnly)
{
	COMTRY_IN_TRANSACTION {
		CHECK_INBOOLPAR(pReadOnly);

		change_mode_rec( self, pReadOnly == VARIANT_TRUE);
		COMTHROW(Check());

	} COMCATCH_IN_TRANSACTION(;);
}

HRESULT FCO::HasReadOnlyAccess( VARIANT_BOOL *pPtrReadOnly)
{
	COMTRY {
		CHECK_OUTPAR(pPtrReadOnly);
		long perm = self[ATTRID_PERMISSIONS];
		*pPtrReadOnly = perm & READONLY_FLAG ? VARIANT_TRUE : VARIANT_FALSE; 
	} COMCATCH(;);
}
