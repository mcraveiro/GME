// MgaFilter.cpp : Implementation of CMgaFilter
#include "stdafx.h"
#include "Mga.h"
#include "MgaFilter.h"

/////////////////////////////////////////////////////////////////////////////
// CMgaFilter

#define MAXLEV 30

void CMgaFilter::init() {
	if(newlevel) {
		levels = 0;
		for(OLECHAR *p = wcstok(level, L" "); p; p = wcstok(NULL,L" ")) {
			int low, high, highincr = 0;
			OLECHAR c;
			switch(swscanf(p,L"%d%c%d", &low, &c, &high)) {
			case 0: if(swscanf(p,L"-%d", &high) != 1) COMTHROW(E_MGA_FILTERFORMAT);
					low = 0;
					break;
			case 1: high=low;
					break;
			case 2: 
					if(c != L'-') COMTHROW(E_MGA_FILTERFORMAT);
					high= MAXLEV; 
					highincr =1;
			case 3:
					if(low > high) COMTHROW(E_MGA_FILTERFORMAT);
			}
			if(high > MAXLEV) COMTHROW(E_MGA_FILTERFORMAT);
			high += highincr;
			unsigned long mask = 1 << low;
			for(int i = low; i <= high; i++) {
				levels |= mask;
				mask <<= 1;
			}
		}
		newlevel = false;
	}
	if(newobjtype) {
		objtypes = 0;
		for(OLECHAR *p = wcstok(objtype, L" "); p; p = wcstok(NULL,L" ")) {
			unsigned int code;
			if(swscanf(p,L"%u", &code) != 1) {
				const OLECHAR *c[] = {L"OBJTYPE_MODEL", L"OBJTYPE_ATOM", L"OBJTYPE_REFERENCE", L"OBJTYPE_CONNECTION", L"OBJTYPE_SET"};
				for(code = 0; code < DIM(c); code++) {
					if(!wcscmp(p,c[code])) break;
				}
				code += OBJTYPE_MODEL;  // offset of objtype
			}
			if(code < OBJTYPE_MODEL || code > OBJTYPE_SET) COMTHROW(E_MGA_FILTERFORMAT);
			objtypes |= 1 << code;
		}	
		newobjtype = false;
	}
	if(newkind) {
		kindnums.clear();
		kindnames.clear();
		wchar_t* kind = this->kind;
		if (kind == NULL)
			kind = L"";
		for(OLECHAR *p = wcstok(kind, L" "); p; p = wcstok(NULL,L" ")) {
			unsigned int code;
			if(swscanf(p,L"#%u", &code) == 1) kindnums.insert(code);
			//else kindnames.insert(CComBSTR(p));// NAMESPACE logic inserted here:
			else {
				kindnames.insert( CComBSTR(p));
				kindnames.insert( mgaproject->prefixWNmspc( CComBSTR(p)));
			}
		}	
		newkind = false;
	}
	if(newrole) {
		rolenums.clear();
		rolenames.clear();
		for(OLECHAR *p = wcstok(role, L" "); p; p = wcstok(NULL,L" ")) {
			unsigned int code;
			if(swscanf(p,L"#%u", &code) == 1) rolenums.insert(code);
			else rolenames.insert(CComBSTR(p));
		}	
		newrole = false;
	}
	if(newname) {
		names.clear();
		MGA_TRACE("Names at init1");
		for(BSTRhash::iterator i = names.begin(); i!=names.end(); ++i) {
			MGA_TRACE("Names: %ls\n",*i);
		}

		wchar_t *finish = name + wcslen(name);
		bool inquotes = false;
		wchar_t *begin = name;
		wchar_t *end;
		while (begin < finish) {
			end = wcschr(begin, L'\"');
			if (!end) {
				end = finish;
			}
			*end = L'\0';
			if (inquotes) {
				names.insert(CComBSTR(begin));
				inquotes = false;
			}
			else {
				for(OLECHAR *p = wcstok(begin, L" "); p; p = wcstok(NULL,L" ")) {
					names.insert(CComBSTR(p));
				}	
				inquotes = true;
			}
			begin = end + 1;
		}

		MGA_TRACE("Names at init");
		for(BSTRhash::iterator i = names.begin(); i!=names.end(); ++i) {
			MGA_TRACE("Names: %ls\n",*i);
		}
		newname = false;
	}
}


void CMgaFilter::searchFCOs(CoreObj &self, EXCOLLECTIONTYPE_FOR(MgaFCO) *retcoll, int level) {
	bool add = true;
	init();
	if(self.GetMetaID() == DTID_FOLDER) add = false;
	if(add && levels) {
		if((levels & 1 << (level > MAXLEV+1 ? MAXLEV+1: level)) == 0) add = false;
	}
	if(add && objtypes) {
		if((objtypes & 1 << (GetMetaID(self)-DTID_BASE)) == 0) add =false;
	}
	if(add && !names.empty()) {
		MGA_TRACE("Names at testbeg");
		for(BSTRhash::iterator i = names.begin(); i!=names.end(); ++i) {
			MGA_TRACE("Names: %ls\n",*i);
		}
		if(names.find(CComBSTR(self[ATTRID_NAME])) == names.end()) add =false;
		MGA_TRACE("Names at testend");
		for(BSTRhash::iterator i = names.begin(); i!=names.end(); ++i) {
			MGA_TRACE("Names: %ls\n",*i);
		}
	}
	if(add && !(rolenums.empty() && rolenames.empty())) {
		if(self.IsRootFCO()) add = false;
		else {
			long roleref = self[ATTRID_ROLEMETA];
			if(rolenums.find(roleref) == rolenums.end()) {
				CComBSTR(b);
				COMTHROW(mgaproject->FindMetaRef(roleref)->get_Name(&b));
				if(rolenames.find(b) == rolenames.end()) add = false;
			}
		}
	}
	if(add && !(kindnums.empty() && kindnames.empty())) {
		long kindref = self[ATTRID_META];
		if(kindnums.find(kindref) == kindnums.end()) {
			CComBSTR(b);
			COMTHROW(mgaproject->FindMetaRef(kindref)->get_Name(&b));
			if(kindnames.find(b) == kindnames.end()) add = false;
		}
	}
	if(add) {
		CComPtr<IMgaFCO> bb;
		ObjForCore(self)->getinterface(&bb);
		retcoll->Add(bb);
	}
	level += 1;
	if((self.GetMetaID() == DTID_MODEL || self.GetMetaID() == DTID_FOLDER) && 
		(!levels || levels & 0xFFFFFFFF << ((level > MAXLEV+1 ? MAXLEV+1: level)))) {
		ITERATE_THROUGH(self[ATTRID_FCOPARENT+ATTRID_COLLECTION]) {
			searchFCOs(ITER, retcoll, level);
		}
	}
}






STDMETHODIMP CMgaFilter::get_Name(BSTR *pVal)
{
  COMTRY {
	CHECK_OUTSTRPAR(pVal);
	*pVal = name.Copy();
  } COMCATCH(;);
}

STDMETHODIMP CMgaFilter::put_Name(BSTR newVal)
{
  COMTRY {
	CHECK_INSTRPAR(newVal);
	name = newVal;
	newname = true;
  } COMCATCH(;);
}

STDMETHODIMP CMgaFilter::get_Kind(BSTR *pVal)
{
  COMTRY {
	CHECK_OUTSTRPAR(pVal);
	*pVal = kind.Copy();
  } COMCATCH(;);
}

STDMETHODIMP CMgaFilter::put_Kind(BSTR newVal)
{
  COMTRY {
	CHECK_INSTRPAR(newVal);
	kind = newVal;
	newkind = true;
  } COMCATCH(;);
}

STDMETHODIMP CMgaFilter::get_Role(BSTR *pVal)
{
  COMTRY {
	CHECK_OUTSTRPAR(pVal);
	*pVal = role.Copy();
  } COMCATCH(;);
}

STDMETHODIMP CMgaFilter::put_Role(BSTR newVal)
{
  COMTRY {
	CHECK_INSTRPAR(newVal);
	role = newVal;
	newrole = true;
  } COMCATCH(;);
}

STDMETHODIMP CMgaFilter::get_ObjType(BSTR *pVal)
{
  COMTRY {
	CHECK_OUTSTRPAR(pVal);
	*pVal = objtype.Copy();
  } COMCATCH(;);
}

STDMETHODIMP CMgaFilter::put_ObjType(BSTR newVal)
{
  COMTRY {
	CHECK_INSTRPAR(newVal);
	objtype = newVal;
	newobjtype = true;
  } COMCATCH(;);
}

STDMETHODIMP CMgaFilter::get_Level(BSTR *pVal)
{
  COMTRY {
	CHECK_OUTSTRPAR(pVal);
	*pVal = level.Copy();
  } COMCATCH(;);
}

STDMETHODIMP CMgaFilter::put_Level(BSTR newVal)
{
  COMTRY {
	CHECK_INSTRPAR(newVal);
	level = newVal;
	newlevel = true;
  } COMCATCH(;);
}


STDMETHODIMP CMgaFilter::get_Project(IMgaProject **pVal) {
  COMTRY {
	CHECK_OUTPTRPAR(pVal);
	if (!mgaproject)
		throw_com_error(E_MGA_ZOMBIE_NOPROJECT, L"MgaFilter must be created by IMgaProject.CreateFilter, and not CoCreateInstance");
	(*pVal = mgaproject)->AddRef();
  } COMCATCH(;);
}
