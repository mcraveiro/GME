#include "stdAfx.h"
#include "MgaTerritory.h"
// ----------------------------------------
// Implementation for the MgaTerritory and MgaAddOn  classes
// ----------------------------------------



STDMETHODIMP CMgaTerritory::Associate( IMgaObject *obj,  VARIANT userdata) { 
	COMTRY {
			CHECK_MYINPTRPAR(obj);
			if(!coreterr) COMTHROW( E_MGA_TARGET_DESTROYED);
			ObjFor(obj)->objsetuserdata(this, userdata);
	} COMCATCH(;);
			
};

STDMETHODIMP CMgaTerritory::OpenObj(IMgaObject *oldptr, IMgaObject **newptr) { 
	COMTRY {
		CHECK_MYINPTRPAR(oldptr); 
		CHECK_OUTPAR(newptr); 
		CComPtr<IMgaObject> np;
		if(!coreterr) COMTHROW(E_MGA_TARGET_DESTROYED);
		ObjFor(oldptr)->getinterface(&np,this);
		*newptr = np.Detach();
	} COMCATCH(;) 
}

STDMETHODIMP CMgaTerritory::OpenFCO(IMgaFCO *oldptr, IMgaFCO **newptr) { 
	COMTRY {
		CHECK_MYINPTRPAR(oldptr); 
		CHECK_OUTPAR(newptr); 
		CComPtr<IMgaFCO> np;
		if(!coreterr) COMTHROW(E_MGA_TARGET_DESTROYED);
		ObjFor(oldptr)->getinterface(&np,this);
		*newptr = np.Detach();
	} COMCATCH(;) 
}

STDMETHODIMP CMgaTerritory::OpenFCOs(IMgaFCOs *oldptr, IMgaFCOs **newptr) { 
	COMTRY {
		CHECK_MYINPTRSPAR(oldptr); 
		CHECK_OUTPAR(newptr); 
		CREATEEXCOLLECTION_FOR(MgaFCO, q);
		if(!coreterr) COMTHROW(E_MGA_TARGET_DESTROYED);
		MGACOLL_ITERATE(IMgaFCO, oldptr) {
			CComPtr<IMgaFCO> bb;
		    ObjFor(MGACOLL_ITER)->getinterface(&bb);
			q->Add(bb);
		}
		MGACOLL_ITERATE_END;
		*newptr = q.Detach();
	} COMCATCH(;) 
}


STDMETHODIMP CMgaTerritory::CloseObj(IMgaObject *obj) {
	COMTRY {
		CHECK_MYINPTRPAR(obj); 
		if(!coreterr) COMTHROW(E_MGA_TARGET_DESTROYED);
		COMTHROW(obj->Close());
	} COMCATCH(;) 
}

STDMETHODIMP CMgaTerritory::Flush() {
	COMTRY {
		if(coreterr) {
			HRESULT hr = coreterr->Clear();
			if (FAILED(hr))
				COMRETURN(hr);
		}
	} COMCATCH(;);
	
};


STDMETHODIMP CMgaTerritory::CheckProject(IMgaProject *project) {
	return(project == mgaproject? S_OK : E_MGA_FOREIGN_PROJECT);
}


STDMETHODIMP CMgaTerritory::Destroy() {
	COMTRY {
		if (!coreterr) 
			return E_MGA_TARGET_DESTROYED;
		// n.b. Flush fails if the project has been closed, but if we're being destructed, we must still remove
		// this from allterrs, or MgaProject::~MgaProject will crash (as allterrs does not count references)
		HRESULT hr = Flush();
		CMgaProject::tercoll::iterator i = mgaproject->allterrs.begin(), end = mgaproject->allterrs.end();
		while (i != end) {
			if (*i == this) {
				i = mgaproject->allterrs.erase(i);
				coreterr = NULL;   // release CoreTerritory object
				handler = NULL;	// release sink
				rwhandler = NULL;	// release sink
				COMRETURN(hr);
			}
			else {
				i++;
			}
			
		}
		ASSERT(false);	// not found among project territories
		COMRETURN(hr);
	} COMCATCH(;)
}


STDMETHODIMP CMgaTerritory::SetNamespace( BSTR pNmsp)
{
	COMTRY {
		m_bstrCurrNamespace = pNmsp;
		mgaproject->SetNmspaceInMeta(); // set into the metaproject if open
	} COMCATCH(;)
}

STDMETHODIMP CMgaTerritory::GetNamespace( BSTR * pNmsp)
{
	COMTRY {
		if( m_bstrCurrNamespace.Length() > 0) // namespace set
		{
			CComBSTR res;
			COMTHROW(res.AppendBSTR( m_bstrCurrNamespace));
			*pNmsp = res.Detach();
		}
		else
		{
			*pNmsp = nullptr;
		}
	} COMCATCH(;)
}

STDMETHODIMP CMgaAddOn::CheckProject(IMgaProject *project) {
	COMTRY {
		if (!IsEqualObject(project, (IMgaProject*)mgaproject)) {
			COMTHROW(E_MGA_FOREIGN_PROJECT);
		}
	} COMCATCH(;)
}


STDMETHODIMP CMgaAddOn::Destroy() {
	COMTRY {
		MARKSIG('8'); 
		if (!handler)
			COMTHROW(E_MGA_TARGET_DESTROYED);
		CMgaProject::addoncoll::iterator i = mgaproject->alladdons.begin(), end = mgaproject->alladdons.end();
		while (i != end) {
			if (*i == this) {
				i = mgaproject->alladdons.erase(i);
				if (mgaproject->alladdons.empty())
					mgaproject->reserveterr = NULL;   // release 
				handler = NULL;	// release sink
				return S_OK;
			}
			else {
				i++;
			}
			
		}
		ASSERT(("addon not found among project addons",false));	
	} COMCATCH(;)
}


STDMETHODIMP CMgaAddOn::get_Priority( long *pVal) { return E_MGA_NOT_IMPLEMENTED; }
STDMETHODIMP CMgaAddOn::put_Priority( long newVal) { return E_MGA_NOT_IMPLEMENTED; }


