// MgaFilter.h : Declaration of the CMgaFilter

#ifndef __MGAFILTER_H_
#define __MGAFILTER_H_

#include "resource.h"       // main symbols
#include "MgaFCO.h"



/////////////////////////////////////////////////////////////////////////////
// CMgaFilter
class ATL_NO_VTABLE CMgaFilter : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaFilter, &__uuidof(MgaFilter)>,
	public IDispatchImpl<IMgaFilter, &__uuidof(IMgaFilter), &__uuidof(__MGALib)>,
	public ISupportErrorInfoImpl<&__uuidof(IMgaFilter)>
{
public:
	CMgaFilter()
	{
		levels = objtypes = 0;
		newlevel = newobjtype = newrole = newkind = newname = false;
	}


DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaFilter)
	COM_INTERFACE_ENTRY(IMgaFilter)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IID(IID_ISupportErrorInfo, IMyErrorInfoBase)
END_COM_MAP()

private:
	CComBSTR level, objtype, role, kind, name;
	bool newlevel, newobjtype, newrole, newkind, newname;
	unsigned long levels;
	unsigned long objtypes;
	typedef stdext::hash_set<CComBSTRNoAt, CComBSTR_hashfunc> BSTRhash;
	typedef stdext::hash_set<metaref_type, metaref_hashfunc> mrefhash;
	mrefhash rolenums, kindnums;
	BSTRhash rolenames, kindnames, names;
	CMgaProject *mgaproject;
// IMgaFilter
public:
	STDMETHOD(get_Level)(BSTR *pVal);
	STDMETHOD(put_Level)(BSTR newVal);
	STDMETHOD(get_ObjType)(BSTR *pVal);
	STDMETHOD(put_ObjType)(BSTR newVal);
	STDMETHOD(get_Role)(BSTR *pVal);
	STDMETHOD(put_Role)(BSTR newVal);
	STDMETHOD(get_Kind)(BSTR *pVal);
	STDMETHOD(put_Kind)(BSTR newVal);
	STDMETHOD(get_Name)(BSTR *pVal);
	STDMETHOD(put_Name)(BSTR newVal);
	STDMETHOD(get_Project)(IMgaProject **pVal);
	

	void searchFCOs(CoreObj &self, EXCOLLECTIONTYPE_FOR(MgaFCO) *retcoll, int level = 0);
	void init();
	void setproject(CMgaProject *p ) { mgaproject = p; }
};

#endif //__MGAFILTER_H_
