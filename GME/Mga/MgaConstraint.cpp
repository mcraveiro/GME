// MgaConstraint.cpp : Implementation of CMgaConstraint

#include "StdAfx.h"
#include "MgaFCO.h"
#include "MgaConstraint.h"

/////////////////////////////////////////////////////////////////////////////
// CMgaConstraint


HRESULT FCO::DefineConstraint(BSTR name, long eventmask, BSTR expression, IMgaConstraint **pp) { 
		COMTRY {
			CheckWrite();
//			CoreObj cp(p);
//			cp[ATTRID_CONSTROWNER] = self;
		} COMCATCH(;);
}


HRESULT FCO::AddConstraint(struct IMgaConstraint *p) { 
		COMTRY {
			CheckWrite();
#pragma message ( "TODO: Object-based constraints are not implemented!" )
//			CoreObj cp(p);
//			cp[ATTRID_CONSTROWNER] = self;
		} COMCATCH(;);
	}


HRESULT FCO::get_Constraints(VARIANT_BOOL includemeta, IMgaConstraints **pVal) { 
	COMTRY {
			CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CoreObjs children = self[ATTRID_CONSTROWNER + ATTRID_COLLECTION];
			CREATECOLLECTION_FOR(IMgaConstraint,q);

			ITERATE_THROUGH(children) {
				CComPtr<IMgaConstraint> ff;
#pragma message ( "TODO: Object-based constraints are not implemented!" )
//			    ITER->QueryInterface(&ff);
				q->Add(ff);
			}
			*pVal = q.Detach();
		} COMCATCH(;);
	};


HRESULT FCO::CheckConstraints(long,int *,unsigned short ** ) { return E_MGA_NOT_IMPLEMENTED; };
