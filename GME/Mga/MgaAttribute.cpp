// MgaAttribute.cpp : Implementation of CMgaAttribute
#include "stdafx.h"
#include "MgaAttribute.h"
#include "MgaFCO.h"

#include "atlsafe.h"

#include <unordered_map>

#include "Core.h"

/////////////////////////////////////////////////////////////////////////////
// CMgaAttribute


// Throws!!!
void CMgaAttribute::Initialize(metaref_type mr, FCO *o, CMgaProject *p) { 
	mref = mr;		
	fco = o;
	mgaproject = p;
	CComQIPtr<IMgaMetaAttribute> meta = mgaproject->FindMetaRef(mr);
	COMTHROW(meta->get_ValueType(&attrtyp));
	inputtyp = attrtyp;
	if(attrtyp == ATTVAL_ENUM) attrtyp = ATTVAL_STRING;
}

STDMETHODIMP CMgaAttribute::get_Owner( IMgaFCO **pVal) {  
	COMTRY { 
		CHECK_OUTPTRPAR(pVal);
		fco->getinterface(pVal); 
	} COMCATCH(;) 
};


STDMETHODIMP CMgaAttribute::get_Status( long *status) {
		COMTRY	{
			CHECK_OUTPAR(status);
			fco->CheckRead();
			if(load_status == ATTSTATUS_INVALID) {
				CComVariant v;
				get_Value(&v);
			}
			*status = load_status;
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_Value(VARIANT *pVal) {
		COMTRY {
			CHECK_OUTVARIANTPAR(pVal);
			fco->CheckRead();
			CComVariant var;
			if(load_status == ATTSTATUS_INVALID) {
				long ls = ATTSTATUS_HERE;
				CoreObj cur = fco->self;
				valueobj = NULL;
				while(cur) {
					CoreObjs attrs = cur[ATTRID_ATTRPARENT+ATTRID_COLLECTION];
					ITERATE_THROUGH(attrs) {
						if(mref == ITER[ATTRID_META]) {
							valueobj = ITER;
							break;
						}
					}
					if(valueobj != NULL) {
						load_status = ls;
						break;				// breaks here with >= HERE 
					}
					cur = cur[ATTRID_DERIVED];
					ls++;
				}
			}
			if(load_status >= ATTSTATUS_HERE) {
				COMTHROW(valueobj->get_AttributeValue((short)(ATTRID_ATTRTYPESBASE + attrtyp), &var));
			}
			else {
				CComQIPtr<IMgaMetaAttribute> m(mgaproject->FindMetaRef(mref));
				COMTHROW(m->get_DefaultValue(&var));
/* METADIFF !!				
				if(pVal->vt == VT_EMPTY) {
					COMTHROW(m->get_Status(&load_status));
				}
				else */ 
					load_status = ATTSTATUS_METADEFAULT;
			}
			{
				static const VARTYPE vartypes[] = { VT_NULL, VT_BSTR, VT_I4, VT_R8, VT_BOOL, VT_DISPATCH, VT_BSTR };
				if(vartypes[attrtyp] != var.vt) {
					COMTHROW(var.ChangeType(vartypes[attrtyp]));
				}
			}
			var.Detach(pVal);
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_OrigValue(VARIANT *pVal) {
		COMTRY {
			CHECK_OUTVARIANTPAR(pVal);
			fco->CheckRead();
			CComVariant var;
			if(load_status == ATTSTATUS_INVALID) {
				long ls = ATTSTATUS_HERE;
				CoreObj cur = fco->self;
				valueobj = NULL;
				while(cur) {
					CoreObjs attrs = cur[ATTRID_ATTRPARENT+ATTRID_COLLECTION];
					ITERATE_THROUGH(attrs) {
						if(mref == ITER[ATTRID_META]) {
							valueobj = ITER;
							break;
						}
					}
					if(valueobj != NULL) {
						load_status = ls;
						break;				// breaks here with >= HERE 
					}
					cur = cur[ATTRID_DERIVED];
					ls++;
				}
			}
			if(load_status >= ATTSTATUS_HERE) {
				COMTHROW(valueobj->get_AttributeValue((short)(ATTRID_ATTRTYPESBASE + attrtyp), &var));
			}
			else {
				CComQIPtr<IMgaMetaAttribute> m(mgaproject->FindMetaRef(mref));
				COMTHROW(m->get_DefaultValue(&var));
/* METADIFF !!				
				if(pVal->vt == VT_EMPTY) {
					COMTHROW(m->get_Status(&load_status));
				}
				else */ 
					load_status = ATTSTATUS_METADEFAULT;
			}
			{
				static const VARTYPE vartypes[] = { VT_NULL, VT_BSTR, VT_I4, VT_R8, VT_BOOL, VT_DISPATCH, VT_BSTR };
				if(vartypes[attrtyp] != var.vt) {
					COMTHROW(var.ChangeType(vartypes[attrtyp]));
				}
			}
			var.Detach(pVal);
		} COMCATCH(;)
}


STDMETHODIMP CMgaAttribute::get_HasChanged(VARIANT_BOOL *pVal) {
		COMTRY {
			CHECK_OUTPAR(pVal);
			*pVal = VARIANT_TRUE;
		} COMCATCH(;)
}


class attrnotifytask : public DeriveTreeTask {
	bool Do(CoreObj self, std::vector<CoreObj> *peers = NULL) {
		ObjForCore(self)->SelfMark(OBJEVENT_ATTR);
		return true;
	}
};


STDMETHODIMP CMgaAttribute::Clear() {
	COMTRY_IN_TRANSACTION {
		fco->CheckWrite();
		CoreObjs attrs = fco->self[ATTRID_ATTRPARENT+ATTRID_COLLECTION];
		ITERATE_THROUGH(attrs) {
			if(mref == ITER[ATTRID_META]) {
				// lph: Pre-Notification PRE_STATUS (the attribute is being changed to its default value)
				CComQIPtr<IMgaMetaAttribute> ma(mgaproject->FindMetaRef(mref));
				CComBSTR name;
				COMTHROW(ma->get_Name(&name));
				CComBSTR desc = L"ATTR,";
				COMTHROW(desc.Append(name));
				COMTHROW(desc.Append(L",Cleared"));
				// FIXME: COMTHROW?
				fco->PreNotify(OBJEVENT_PRE_STATUS, CComVariant(desc));
				//---------------------------------------------------------------------------------------
				COMTHROW(ITER->Delete());
				load_status = ATTSTATUS_INVALID;
				// lph: added notification of attribute mod
				attrnotifytask().DoWithDeriveds(fco->self);
				//-----------------------------------------
				break;
			}
		}
	} COMCATCH_IN_TRANSACTION(;);
}


STDMETHODIMP CMgaAttribute::put_Value(VARIANT newVal) {
		COMTRY_IN_TRANSACTION_MAYBE {
			CHECK_INPAR(newVal);
			fco->CheckWrite();
			if(load_status == ATTSTATUS_INVALID) {
				valueobj = NULL;
				CoreObjs attrs = fco->self[ATTRID_ATTRPARENT+ATTRID_COLLECTION];
				ITERATE_THROUGH(attrs) {
					if(mref == ITER[ATTRID_META]) {
						valueobj = ITER;
						load_status = ATTSTATUS_HERE;
						break;
					}
				}
			}
			if(load_status != ATTSTATUS_HERE) {
					load_status = ATTSTATUS_INVALID;
					valueobj = NULL;
					COMTHROW(mgaproject->dataproject->CreateObject(DTID_ATTRTYPESBASE+attrtyp, &valueobj.ComPtr()));
					// lph: Pre-Notification PRE_STATUS (the attribute is being changed from its default value)
					CComQIPtr<IMgaMetaAttribute> ma(mgaproject->FindMetaRef(mref));
					CComBSTR name;
					COMTHROW(ma->get_Name(&name));
					CComBSTR desc = L"ATTR,";
					COMTHROW(desc.Append(name));
					COMTHROW(desc.Append(L",Defined"));
					fco->PreNotify(OBJEVENT_PRE_STATUS, CComVariant(desc));
					//-----------------------------------------------------------------------------------------
					valueobj[ATTRID_META]=mref;
					valueobj[ATTRID_ATTRPARENT]=fco->self;
					load_status = ATTSTATUS_HERE;
			}

			if(inputtyp == ATTVAL_ENUM) {
				CComVariant v = newVal;
				if(v.vt != VT_BSTR) v.ChangeType(VT_BSTR);
				if(v.vt != VT_BSTR) COMTHROW(E_MGA_BAD_ENUMVALUE);
				CComQIPtr<IMgaMetaAttribute> m(mgaproject->FindMetaRef(mref));
				CComPtr<IMgaMetaEnumItems> enms;
				COMTHROW(m->get_EnumItems(&enms));
				MGACOLL_ITERATE(IMgaMetaEnumItem, enms) {
					CComBSTR b;
					COMTHROW(MGACOLL_ITER->get_Value(&b));
					if(b == v.bstrVal) break;
				} 
				if(MGACOLL_AT_END) COMTHROW(E_MGA_BAD_ENUMVALUE);
				MGACOLL_ITERATE_END;

				valueobj[ATTRID_ATTRTYPESBASE + ATTVAL_STRING] = v;
			}
			else valueobj[ATTRID_ATTRTYPESBASE + attrtyp] = newVal;
			{
				CComPtr<IMgaFCO> p;
				COMTHROW(get_Owner(&p));
				attrnotifytask().DoWithDeriveds(fco->self);
			}
		} COMCATCH_IN_TRANSACTION_MAYBE(;)
}


STDMETHODIMP CMgaAttribute::get_StringValue(BSTR *pVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_RDATTRTYPES) &&
			attrtyp != ATTVAL_STRING && inputtyp != ATTVAL_ENUM) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_OUTSTRPAR(pVal);
			CComVariant v;
			COMTHROW( get_Value(&v));
			CopyTo(v, pVal);
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::put_StringValue(BSTR newVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_WRATTRTYPES) &&
			attrtyp != ATTVAL_STRING) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_INSTRPAR(newVal);
			COMTHROW( put_Value(CComVariant(newVal)));
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_IntValue(long *pVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_RDATTRTYPES) &&
			attrtyp != ATTVAL_INTEGER && inputtyp != ATTVAL_ENUM) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_OUTPAR(pVal);
			CComVariant v;
			COMTHROW( get_Value(&v));
			if(inputtyp == ATTVAL_ENUM) {
				long ord = 0;
				if(v.vt != VT_BSTR) v.ChangeType(VT_BSTR);
				if(v.vt != VT_BSTR) COMTHROW(E_MGA_BAD_ENUMVALUE);
				CComQIPtr<IMgaMetaAttribute> m(mgaproject->FindMetaRef(mref));
				CComPtr<IMgaMetaEnumItems> enms;
				COMTHROW(m->get_EnumItems(&enms));
				MGACOLL_ITERATE(IMgaMetaEnumItem, enms) {
					CComBSTR b;
					COMTHROW(MGACOLL_ITER->get_Value(&b));
					if(b == v.bstrVal) break;
					ord++;
				} 
				if(MGACOLL_AT_END) COMTHROW(E_MGA_BAD_ENUMVALUE);
				MGACOLL_ITERATE_END;
				*pVal = ord;
			}
			else CopyTo(v, *pVal);  // throws if type mismatch
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::put_IntValue(long newVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_WRATTRTYPES) &&
		   attrtyp != ATTVAL_INTEGER && inputtyp != ATTVAL_ENUM) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_INPAR(newVal);
			if(inputtyp == ATTVAL_ENUM) {
				CComQIPtr<IMgaMetaAttribute> m(mgaproject->FindMetaRef(mref));
				CComPtr<IMgaMetaEnumItems> enms;
				CComPtr<IMgaMetaEnumItem> enm;
				COMTHROW(m->get_EnumItems(&enms));
				long count;
				COMTHROW(enms->get_Count(&count));
				if(newVal < 0 || newVal > count) COMTHROW(E_MGA_BAD_ENUMVALUE);
				COMTHROW(enms->get_Item(1+newVal, &enm));
				CComBSTR estr;
				COMTHROW(enm->get_Value(&estr));
				COMTHROW( put_Value(CComVariant(estr)));
			}
			COMTHROW( put_Value(CComVariant(newVal)));
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_BoolValue(VARIANT_BOOL *pVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_RDATTRTYPES) &&
			attrtyp != ATTVAL_BOOLEAN) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_OUTPAR(pVal);
			CComVariant v;
			COMTHROW( get_Value(&v));
			CopyTo(v, *pVal);
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::put_BoolValue(VARIANT_BOOL newVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_WRATTRTYPES) &&
		   attrtyp != ATTVAL_BOOLEAN) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_INPAR(newVal);
			COMTHROW( put_Value(CComVariant(newVal)));
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_FloatValue(double *pVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_RDATTRTYPES) &&
			attrtyp != ATTVAL_DOUBLE) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_OUTPAR(pVal);
			CComVariant v;
			COMTHROW( get_Value(&v));
			CopyTo(v, *pVal);
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::put_FloatValue(double newVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_WRATTRTYPES) &&
		   attrtyp != ATTVAL_DOUBLE) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_INPAR(newVal);
			COMTHROW( put_Value(CComVariant(newVal)));
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_FCOValue(IMgaFCO **pVal)
{
	if(attrtyp != ATTVAL_REFERENCE) return E_MGA_ACCESS_TYPE;
	COMTRY {
		CHECK_OUTPTRPAR(pVal);
		CComVariant v;
		COMTHROW( get_Value(&v));
		CoreObj h;
		if(v.vt != VT_DISPATCH) COMTHROW(E_MGA_MODULE_INCOMPATIBILITY);
		if(v.pdispVal) COMTHROW(v.pdispVal->QueryInterface(_uuidof(ICoreObject), (void **)&h));
		if(!h) COMTHROW(E_MGA_MODULE_INCOMPATIBILITY);
		ObjForCore(h)->getinterface(pVal);
	} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::put_FCOValue(IMgaFCO *newVal)
{
	if(attrtyp != ATTVAL_REFERENCE) return E_MGA_ACCESS_TYPE;
	COMTRY {
		CHECK_MYINPTRPAR(newVal);
		COMTHROW( put_Value(CComVariant(CoreObj(newVal))));
	} COMCATCH(;)

}


STDMETHODIMP CMgaAttribute::get_RegistryNode( BSTR path,  IMgaRegNode **pVal) {  
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CHECK_INSTRPAR(path);
      
			CComBSTR xpath(regprefix);
			COMTHROW(xpath.Append(L"/"));
			COMTHROW(xpath.AppendBSTR(path));
			COMTHROW(fco->get_RegistryNode(xpath, pVal));
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_Registry( VARIANT_BOOL virtuals, IMgaRegNodes **pVal) {  
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CComPtr<IMgaRegNode> p;
			COMTHROW(fco->get_RegistryNode(regprefix, &p));
			COMTHROW(p->get_SubNodes(virtuals, pVal));
		} COMCATCH(;);
}

STDMETHODIMP CMgaAttribute::get_RegistryValue( BSTR path,  BSTR *pVal) {  
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPAR(pVal);
			CHECK_INSTRPAR(path);
			CComPtr<IMgaRegNode> p;
			COMTHROW(get_RegistryNode(path, &p));
			COMTHROW(p->get_Value(pVal));
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::put_RegistryValue( BSTR path,  BSTR newval) {  
		COMTRY_IN_TRANSACTION {
			fco->CheckWrite();
			CHECK_INSTRPAR(path);
			CHECK_INSTRPAR(newval);
			CComPtr<IMgaRegNode> p;
			COMTHROW(get_RegistryNode(path, &p));
			COMTHROW(p->put_Value(newval));
		} COMCATCH_IN_TRANSACTION(;)
}



void MergeAttrs(const CoreObj &src, CoreObj &dst) {
	CoreObjs dstcoll = dst[ATTRID_ATTRPARENT+ATTRID_COLLECTION];
	ITERATE_THROUGH(src[ATTRID_ATTRPARENT+ATTRID_COLLECTION]) {
		CoreObj &srcn = ITER;
		CoreObj dstn;
		metaref_type srcmeta = srcn[ATTRID_META];
		ITERATE_THROUGH(dstcoll) {
			if(srcmeta == ITER[ATTRID_META]) {
				break;
			}
		}
		if(!ITER_BROKEN) {
			CComPtr<ICoreProject> p;
			COMTHROW(srcn->get_Project(&p));
			metaid_type mi = srcn.GetMetaID();
			COMTHROW(p->CreateObject(mi, &dstn.ComPtr()));
			dstn[ATTRID_META] = srcmeta;
			dstn[ATTRID_ATTRPARENT] = dst;
// MAGIC with attrtype numbers   to set the value attribute
			dstn[ATTRID_ATTRTYPESBASE + (mi - DTID_ATTRTYPESBASE) ] = srcn[ATTRID_ATTRTYPESBASE + (mi - DTID_ATTRTYPESBASE)];
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Registry   
/////////////////////////////////////////////////////////////////////////////////////////////////////



// THROWS!!!
// RECURSIVE!!!
// Used when an FCO is detached from its basetype. 
// All nodes present in src node will be added to dst node, unless dst already has 
// a node with the same name and a value.
// If dst node is opaque, src is not copied and recursion also stops.

void MergeRegs(const CoreObj &src, CoreObj &dst) {
	{
		// KMS: need to set ATTRID_REGNODE for write ops
		CComVariant attr = dst[ATTRID_REGNODE];
		dst[ATTRID_REGNODE] = attr;
	}

	CComVariant attr = dst[ATTRID_REGNODE];
	CComPtr<ICoreDictionaryAttributeValue> dictval;
	COMTHROW(attr.pdispVal->QueryInterface(&dictval));
	VARIANT vmap;
	COMTHROW(dictval->get_Map(&vmap));
	CMgaRegNode::map_type* map = (CMgaRegNode::map_type*)(void*)vmap.llVal;

	CoreObj s = src;
	do {
		CComVariant sattr = s[ATTRID_REGNODE];
		CComPtr<ICoreDictionaryAttributeValue> sdictval;
		COMTHROW(sattr.pdispVal->QueryInterface(&sdictval));
		VARIANT svmap;
		COMTHROW(sdictval->get_Map(&svmap));
		CMgaRegNode::map_type* smap = (CMgaRegNode::map_type*)(void*)svmap.llVal;

		for (auto it = smap->begin(); it != smap->end(); it++)
		{
			if (map->find(it->first) == map->end())
			{
				map->insert(CMgaRegNode::map_type::value_type(it->first, it->second));
			}
		}

		if (!s.IsFCO())
			break;
	} while (s = s[ATTRID_DERIVED]);
}

class regnotifytask : public DeriveTreeTask {
	bool Do(CoreObj self, std::vector<CoreObj> *peers = NULL) {
		ObjForCore(self)->SelfMark(OBJEVENT_REGISTRY);
		return true;
	}
};


template<class F>
void CMgaRegNode::WalkKeyValues(CoreObj& obj, F& f, long status, bool& continue_)
{
	CComVariant attr = obj[ATTRID_REGNODE];
	CComPtr<ICoreDictionaryAttributeValue> oldval;
	COMTHROW(attr.pdispVal->QueryInterface(&oldval));
	VARIANT vmap;
	COMTHROW(oldval->get_Map(&vmap));
	map_type* map = (map_type*)(void*)vmap.llVal;

	for (auto it = map->begin(); it != map->end(); it++)
	{
		f(*map, it, status, continue_);
	}
}

template<class F>
void CMgaRegNode::WalkKeyValuesInher(F& f)
{
	bool continue_ = true;
	CoreObj s = fco->self;
	long status = ATTSTATUS_HERE;
	do {
		WalkKeyValues(s, f, status, continue_);
		status = min(ATTSTATUS_IN_ARCHETYPE4, status + 1);
		if (!continue_)
			break;
		if (!s.IsFCO())
			break;
	} while (s = s[ATTRID_DERIVED]);
}

void CMgaRegNode::SetValue(const wchar_t* path, const wchar_t* value)
{
	{
		// KMS: need to set ATTRID_REGNODE for write ops
		CComVariant attr = fco->self[ATTRID_REGNODE];
		fco->self[ATTRID_REGNODE] = attr;
	}

	CComVariant attr = fco->self[ATTRID_REGNODE];
	CComPtr<ICoreDictionaryAttributeValue> newval;
	COMTHROW(attr.pdispVal->QueryInterface(&newval));

	VARIANT vmap;
	COMTHROW(newval->get_Map(&vmap));
	map_type* map = (map_type*)(void*)vmap.llVal;

	if (value == NULL_SENTINEL) {
		auto ent = map->find(CComBSTR(path));
		if (ent != map->end())
			map->erase(ent);
	} else
		(*map)[CComBSTR(path)] = CComBSTR(value);
}

const wchar_t* CMgaRegNode::NULL_SENTINEL = (const wchar_t*) "\0xFF\0xFE\0";


void CMgaRegNode::markchg() {
	regnotifytask().DoWithDeriveds(fco->self);
}





STDMETHODIMP CMgaRegNode::get_Object( IMgaObject **pVal) {  
	COMTRY {
		CHECK_OUTPTRPAR(pVal);
		IMgaFCO *p;
		fco->getinterface(&p); 
		*pVal = p;
	} COMCATCH(;) 
};

STDMETHODIMP CMgaRegNode::get_Status( long *status) {
		COMTRY	{
			fco->CheckRead();
			CHECK_OUTPAR(status);

			*status = ATTSTATUS_UNDEFINED;
			CoreObj s = fco->self;
			long _status = ATTSTATUS_HERE;
			do {
				CComVariant attr = s[ATTRID_REGNODE];
				CComPtr<ICoreDictionaryAttributeValue> oldval;
				COMTHROW(attr.pdispVal->QueryInterface(&oldval));

				CComPtr<ICoreDictionaryAttributeValue> newval = oldval;
				VARIANT vmap;
				COMTHROW(newval->get_Map(&vmap));
				map_type* map = (map_type*)(void*)vmap.llVal;
				if (map->find(mypath) != map->end())
				{
					*status = _status;
					return S_OK;
				}

				_status = min(ATTSTATUS_IN_ARCHETYPE4, _status + 1);
				if (!s.IsFCO())
					break;
			} while (s = s[ATTRID_DERIVED]);

			metaref_type mref = fco->self[ATTRID_META];
			if(mref) {
				CComQIPtr<IMgaMetaBase> m(mgaproject->FindMetaRef(mref));
				CComPtr<IMgaMetaRegNode> rn;
				HRESULT hr = m->get_RegistryNode(mypath, &rn);
				if (hr == E_NOTFOUND) {
					*status = ATTSTATUS_UNDEFINED;
				}
				else if (SUCCEEDED(hr)) {
					*status = ATTSTATUS_METADEFAULT;
				}
				else if(hr != E_NOTFOUND)
					COMTHROW(hr);
			}

			return S_OK;
		} COMCATCH(;)
}


void MgaRegNode_get_Value(CMgaProject* mgaproject, FCO * fco, CoreObj s, BSTR mypath, BSTR *pVal)
{
	do {
		CComVariant attr = s[ATTRID_REGNODE];
		CComPtr<ICoreDictionaryAttributeValue> dict;
		COMTHROW(attr.pdispVal->QueryInterface(&dict));
		VARIANT vmap;
		COMTHROW(dict->get_Map(&vmap));
		CMgaRegNode::map_type* map = (CMgaRegNode::map_type*)(void*)vmap.llVal;

		CMgaRegNode::map_type::iterator it = map->find(mypath);
		if (it != map->end()) {
			*pVal = CComBSTR(it->second).Detach();
			return;
		}
		if (!s.IsFCO())
			break;
	} while (s = s[ATTRID_DERIVED]);

	metaref_type mref = fco->self[ATTRID_META];
	if(mref) {
		CComQIPtr<IMgaMetaBase> m(mgaproject->FindMetaRef(mref));
		CComPtr<IMgaMetaRegNode> rn;
		HRESULT hr = m->get_RegistryNode(mypath, &rn);
		if(hr == S_OK) {
			COMTHROW(rn->get_Value(pVal));
			return;
		}
		else if(hr != E_NOTFOUND)
			COMTHROW(hr);
	}
	*pVal = NULL;
}

STDMETHODIMP CMgaRegNode::get_Value(BSTR *pVal) {
	COMTRY {
		CHECK_OUTVARIANTPAR(pVal);
		CoreObj s = fco->self;
		MgaRegNode_get_Value(mgaproject, fco, s, mypath, pVal);
	} COMCATCH(;)
}

STDMETHODIMP CMgaRegNode::get_FCOValue(IMgaFCO **pVal) {
		COMTRY	{
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);

			fco->getinterface(pVal);
		} COMCATCH(;)
}

STDMETHODIMP CMgaRegNode::get_Opacity( VARIANT_BOOL *pVal) {
	COMTRY {
		long status;
		COMTHROW(get_Status(&status));
		*pVal = status == ATTSTATUS_HERE ? VARIANT_TRUE : VARIANT_FALSE;
	} COMCATCH(;);
}

STDMETHODIMP CMgaRegNode::put_Opacity( VARIANT_BOOL newVal) {
	return S_OK;
}


STDMETHODIMP CMgaRegNode::put_Value(BSTR newVal) {
		COMTRY {
			CHECK_INSTRPAR(newVal);
			fco->CheckWrite();
			SetValue(mypath, newVal);
			markchg();
		} COMCATCH(;)
}

STDMETHODIMP CMgaRegNode::put_FCOValue(IMgaFCO *newVal) {
	return E_NOTIMPL;
}

STDMETHODIMP CMgaRegNode::get_SubNodes( VARIANT_BOOL virtuals, IMgaRegNodes **pVal) {
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);

			CoreObj s = fco->self;
			CREATEEXCOLLECTION_FOR(MgaRegNode,q);
			std::set<std::wstring> paths;
			WalkKeyValuesInher([&](map_type& map, map_type::iterator& it, int inher, bool& continue_) {
				if (virtuals == VARIANT_FALSE && inher != ATTSTATUS_HERE)
				{
					continue_ = false;
					return;
				}
				if (wcsncmp(it->first ? it->first : L"", mypath, mypath.Length()) == 0)
				{
					std::wstring path = it->first ? it->first : L"";
					if (path.length() > mypath.Length() && (mypath.Length() == 0 || path[mypath.Length()] == L'/'))
					{
						size_t end = path.find(L'/', mypath.Length() + 1);
						if (end != std::wstring::npos)
							path = path.substr(0, end);
						paths.insert(std::move(path));
					}
				}
			});
			if(virtuals) {
				metaref_type mref = fco->self[ATTRID_META];
				if(mref) {
					CComQIPtr<IMgaMetaBase> m(mgaproject->FindMetaRef(mref));
					CComPtr<IMgaMetaRegNode> rn;
					HRESULT hr;
					if (mypath == L"")
						hr = S_OK;
					else
						hr = m->get_RegistryNode(mypath, &rn);
					CComPtr<IMgaMetaRegNodes> rns;
					if(hr == S_OK) {
						if (mypath == L"")
							COMTHROW(m->get_RegistryNodes(&rns));
						else
							COMTHROW(rn->get_RegistryNodes(&rns));
						MGACOLL_ITERATE(IMgaMetaRegNode, rns) {
							CComBSTR path;
							COMTHROW(MGACOLL_ITER->get_Name(&path));
							CComBSTR subpath(mypath);
							if (mypath != L"")
								COMTHROW(subpath.Append("/"));
							COMTHROW(subpath.Append(path));
							paths.insert(std::wstring(subpath));
						} MGACOLL_ITERATE_END;
					}
				}
			}
			for (std::set<std::wstring>::iterator pathsIt = paths.begin(); pathsIt != paths.end(); pathsIt++)
			{
				CComPtr<CMgaRegNode> regnode;
				CreateComObject(regnode);
				regnode->Initialize(CComBSTR(pathsIt->c_str()), fco, mgaproject);
				q->Append(regnode);
			}
			*pVal = q.Detach();
		} COMCATCH(;)
}

STDMETHODIMP CMgaRegNode::get_SubNodeByName(BSTR name, IMgaRegNode **pVal) {
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CHECK_INSTRPAR(name);
      
			CComBSTR xpath(mypath);
			COMTHROW(xpath.Append(L"/"));
			COMTHROW(xpath.AppendBSTR(name));
			COMTHROW(fco->get_RegistryNode(xpath, pVal));
		} COMCATCH(;)
}

STDMETHODIMP CMgaRegNode::get_ParentNode(IMgaRegNode **pVal) {
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
      
			wchar_t* p = (mypath == NULL ? NULL : wcsrchr(mypath, L'/'));
			if(p) {
				CComBSTR xpath;
				xpath.Attach(::SysAllocStringLen(mypath,p-mypath));
				COMTHROW(fco->get_RegistryNode(xpath, pVal));
			}
		} COMCATCH(;);
}


STDMETHODIMP CMgaRegNode::Clear() {
		COMTRY {
			fco->CheckWrite();
			SetValue(mypath, NULL_SENTINEL);

			markchg();
		} COMCATCH(;);
}

STDMETHODIMP CMgaRegNode::RemoveTree() {
	COMTRY {
		fco->CheckWrite();
		// lph: Pre-Notification PRE_STATUS (the registry node is being destroyed)
		CComBSTR desc = L"REGISTRY,";
		COMTHROW(desc.Append(mypath));
		COMTHROW(desc.Append(L",Removed"));
		fco->PreNotify(OBJEVENT_PRE_STATUS, CComVariant(desc));

		{
		// KMS: need to set ATTRID_REGNODE for write ops
		CComVariant attr = fco->self[ATTRID_REGNODE];
		fco->self[ATTRID_REGNODE] = attr;
		}

		CComVariant attr = fco->self[ATTRID_REGNODE];
		CComPtr<ICoreDictionaryAttributeValue> oldval;
		COMTHROW(attr.pdispVal->QueryInterface(&oldval));
		VARIANT vmap;
		COMTHROW(oldval->get_Map(&vmap));
		map_type* map = (map_type*)(void*)vmap.llVal;

		for (auto it = map->begin(); it != map->end();)
		{
			std::wstring path = it->first ? it->first : L"";
			if (wcsncmp(path.c_str(), mypath, mypath.Length()) == 0)
			{
				// PV: Remove only true subtree nodes (not just prefix testing)
				if (mypath.Length() == 0 || mypath.Length() == path.length() || (path.length() > mypath.Length() && path[mypath.Length()] == L'/')) {
					map->erase(it++);
					continue;	// modify collection while iterating
				}
			}
			it++;
		}
		// TODO
		markchg();
	} COMCATCH(;);
}








STDMETHODIMP CMgaPart::get_Model(struct IMgaModel **pVal ) {
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CComPtr<IMgaContainer> parent;
			COMTHROW(fco->GetParent(&parent));
			COMTHROW(parent.QueryInterface(pVal));
		} COMCATCH(;);
};

STDMETHODIMP CMgaPart::get_MetaAspect(struct IMgaMetaAspect **pVal ) { 
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CComPtr<IMgaMetaPart> part;
			part = CComQIPtr<IMgaMetaPart>(mgaproject->FindMetaRef(mref));
			COMTHROW(part->get_ParentAspect(pVal));
		} COMCATCH(;);
};

STDMETHODIMP CMgaPart::get_Meta(struct IMgaMetaPart **pVal ) { 
		COMTRY {
			fco->CheckRead();
		CHECK_OUTPTRPAR(pVal);
		*pVal = CComQIPtr<IMgaMetaPart>(mgaproject->FindMetaRef(mref)).Detach();
		} COMCATCH(;);
};

STDMETHODIMP CMgaPart::get_MetaRole(struct IMgaMetaRole **pVal ) { 
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			COMTHROW(fco->get_MetaRole(pVal));
		} COMCATCH(;);
}

 
STDMETHODIMP CMgaPart::get_FCO(struct IMgaFCO **pVal) { 
		COMTRY {
			CHECK_OUTPTRPAR(pVal);
			fco->getinterface(pVal); 
		} COMCATCH(;);
};

STDMETHODIMP CMgaPart::get_AccessMask(long *) { return E_MGA_NOT_IMPLEMENTED; }

STDMETHODIMP CMgaPart::GetGmeAttrs(BSTR *icon,long *x,long *y) { 
		COMTRY {
			fco->CheckRead();
			CComBSTR vval;
			_bstr_t regValueIcon;
			if (icon)
				regValueIcon = RegistryValue[L"Icon"];
			if(x || y) {
				CComBSTR bb;
				COMTHROW(get_RegistryValue(CComBSTR(L"Position"), &bb));
				long ld, *lx = x?x:&ld, *ly = y?y:&ld;
				if(!bb) { *lx = -1; *ly = -1;  }
				else if(swscanf(bb,OLESTR("%ld,%ld"), lx, ly) != 2) COMTHROW(E_MGA_BAD_POSITIONVALUE);
			}
			if (icon)
				*icon = regValueIcon.Detach();
		} COMCATCH(;)
}

STDMETHODIMP CMgaPart::SetGmeAttrs(BSTR icon,long x,long y) { 
	COMTRY_IN_TRANSACTION_MAYBE {
			fco->CheckWrite();
			CComBSTR vval;
			if(icon != NULL) COMTHROW(put_RegistryValue(CComBSTR(L"Icon"), icon));
			if(x >= 0 && y >= 0) {
				OLECHAR bbc[40];
				swprintf(bbc, 40, OLESTR("%ld,%ld"), x, y);
				CComBSTR bb(bbc);
				COMTHROW(put_RegistryValue(CComBSTR(L"Position"), bb));
			}
	} COMCATCH_IN_TRANSACTION_MAYBE(;)
};


STDMETHODIMP CMgaPart::get_RegistryMode( VARIANT_BOOL *own) {  return E_MGA_NOT_IMPLEMENTED; }
STDMETHODIMP CMgaPart::put_RegistryMode( VARIANT_BOOL own) {  return E_MGA_NOT_IMPLEMENTED; }

STDMETHODIMP CMgaPart::get_RegistryNode( BSTR path,  IMgaRegNode **pVal) {  
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CHECK_INSTRPAR(path);
      
			CComBSTR xpath(regprefix);
			COMTHROW(xpath.Append(L"/"));
			COMTHROW(xpath.AppendBSTR(path));
			COMTHROW(fco->get_RegistryNode(xpath, pVal));
		} COMCATCH(;)
}

STDMETHODIMP CMgaPart::get_Registry(VARIANT_BOOL virtuals, IMgaRegNodes **pVal) {  
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CComPtr<IMgaRegNode> p;
			COMTHROW(fco->get_RegistryNode(regprefix, &p));
			COMTHROW(p->get_SubNodes(virtuals, pVal));
		} COMCATCH(;);
}

STDMETHODIMP CMgaPart::get_RegistryValue( BSTR path,  BSTR *pVal) {  
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPAR(pVal);
			CHECK_INSTRPAR(path);
			CComPtr<IMgaRegNode> p;
			COMTHROW(get_RegistryNode(path, &p));
			COMTHROW(p->get_Value(pVal));
		} COMCATCH(;)
}

STDMETHODIMP CMgaPart::put_RegistryValue( BSTR path,  BSTR newval) {  
		COMTRY_IN_TRANSACTION_MAYBE {
			fco->CheckWrite();
			CHECK_INSTRPAR(path);
			CHECK_INSTRPAR(newval);
			CComPtr<IMgaRegNode> p;
			COMTHROW(get_RegistryNode(path, &p));
			COMTHROW(p->put_Value(newval));
		} COMCATCH_IN_TRANSACTION_MAYBE(;)
}
	

CMgaPart::CMgaPart()	: prevptr(NULL), next(NULL), load_status(ATTSTATUS_INVALID) {	}
CMgaPart::~CMgaPart() {						// remove object from hash
		if (next)
			next->prevptr = prevptr;
		if (prevptr)
			*prevptr = next;
}
void CMgaPart::Initialize(metaref_type mr, ::FCO *o, CMgaProject *p) {   // Throws!!!
		mref = mr;		
		fco = o;
		mgaproject = p;
}
