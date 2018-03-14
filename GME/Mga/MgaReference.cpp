// MgaReference.cpp : Implementation of CMgaReference
#include "stdafx.h"
#include "Mga.h"
#include "MgaReference.h"


/////////////////////////////////////////////////////////////////////////////
// CMgaReference


HRESULT FCO::CreateReference( IMgaMetaRole *meta,  IMgaFCO *target,  IMgaFCO **newobj) {
	COMTRY_IN_TRANSACTION {
		CheckWrite();
		CHECK_INPTRPAR(meta);
		CHECK_MYINPTRPARVALIDNULL(target);
		CHECK_OUTPTRPAR(newobj);
// parameters will be checked by functions called 
		CComPtr<IMgaFCO> nobj;
		{
			booltempflag fl(mgaproject->checkofftemporary);
			COMTHROW(CreateChildObject(meta,&nobj));
			objtype_enum ot;
			COMTHROW(nobj->get_ObjType(&ot));
			if(ot != OBJTYPE_REFERENCE) COMTHROW(E_MGA_INVALID_ROLE);
		}
		COMTHROW(ObjFor(nobj)->put_Referred(target));
		*newobj = nobj.Detach();
	} COMCATCH_IN_TRANSACTION(mgaproject->checkofftemporary = false;);
}


HRESULT FCO::get_Referred(IMgaFCO **pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CoreObj retval;
			retval = self[ATTRID_REFERENCE];
			if(retval) ObjForCore(retval)->getinterface(pVal);
		} COMCATCH(;)
}

bool putreftask::Do(CoreObj self, std::vector<CoreObj> *peers) {
	CoreObj t = self[ATTRID_REFERENCE];
	if(!COM_EQUAL(t, (*peers)[0])) {
		if(t) {
			CoreObjs segs = self[ATTRID_SEGREF + ATTRID_COLLECTION];
			if(segs.Count()) COMTHROW(E_MGA_REFPORTS_USED);
			ObjForCore(t)->SelfMark(OBJEVENT_REFRELEASED);
		}
		t = (*peers)[0];
		self[ATTRID_REFERENCE] = t;
		if(t) ObjForCore(t)->SelfMark(OBJEVENT_REFERENCED);
		ObjForCore(self)->SelfMark(OBJEVENT_RELATION);
		COMTHROW(ObjForCore(self)->Check());

		ITERATE_THROUGH(self[ATTRID_DERIVED + ATTRID_COLLECTION])
		{
			if (!static_cast<CoreObj>(ITER[ATTRID_MASTEROBJ]))
			{
				COMTHROW(ObjForCore(ITER)->Check());
			}
		}
	}
	return true;
}


HRESULT FCO::put_Referred(IMgaFCO *newVal)	{
		// Inherited ref can only refer to a derived instance of the target
		// of its base. This is checked in FCO::CheckRCS
		COMTRY_IN_TRANSACTION_MAYBE {
			CheckWrite();
			CHECK_MYINPTRPARVALIDNULL(newVal);
			std::vector<CoreObj> peer(1);
			peer[0] = CoreObj(newVal);
			CoreObj t = self[ATTRID_REFERENCE];

			// FIXME: this is suspect; it makes ref.Referred = ref.Referred a mutating operation (i.e. => CompareToBase() == true)
			// fix: if(!COM_EQUAL(t, peer[0])) {
			self[ATTRID_MASTEROBJ] = NULLCOREOBJ;
			putreftask(false).DoWithDeriveds(self, &peer);
		} COMCATCH_IN_TRANSACTION_MAYBE(;);
}

HRESULT FCO::get_RefAspect(IMgaMetaAspect **pVal)
{
	COMTRY {
		CHECK_OUTPTRPAR(pVal);
		metaref_type t = self[ATTRID_REFASPECT];
		if(t) {
			CComQIPtr<IMgaMetaAspect> metaa = mgaproject->FindMetaRef(t);
			*pVal = metaa.Detach();
		}
	} COMCATCH(;);
}

HRESULT FCO::put_RefAspect(IMgaMetaAspect *newVal)
{
	COMTRY_IN_TRANSACTION {
		CHECK_INPTRPAR(newVal);
		metaref_type t = METAREF_NULL;
		if(newVal) COMTHROW(newVal->get_MetaRef(&t));
		self[ATTRID_REFASPECT] = t;
	} COMCATCH_IN_TRANSACTION(;);

}


// get references pointing to this object
HRESULT FCO::get_ReferencedBy(IMgaFCOs **pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CoreObjs segs = self[ATTRID_REFERENCE+ATTRID_COLLECTION];
			CREATEEXCOLLECTION_FOR(MgaFCO, q);
			ITERATE_THROUGH(segs) {
				CComPtr<IMgaFCO> ffco;
				ObjForCore(ITER)->getinterface(&ffco);
				q->Add(ffco); 
			}
			*pVal = q.Detach();
		} COMCATCH(;)
}	


HRESULT FCO::RefCompareToBase(short *status) { 
	COMTRY {
		CHECK_INPAR(status);
		CoreObj base = self[ATTRID_DERIVED];
		if(!base) COMTHROW(E_MGA_NOT_DERIVED);
		if(CoreObj(self[ATTRID_MASTEROBJ])) *status = 0;
		else *status = 1;
	} COMCATCH(;);
}


HRESULT FCO::RefRevertToBase()  { 
	COMTRY_IN_TRANSACTION { 
		CheckWrite();
		CoreObj base = self[ATTRID_DERIVED];
		if (!base)
			COMTHROW(E_MGA_NOT_DERIVED);
		std::vector<CoreObj> peer(1);
		CoreObj rb;
		int d;
		GetRootOfDeriv(base, rb, &d);
		CoreObj subt = self.FollowChain(ATTRID_FCOPARENT,d);
//  if it is external in the most original base, the target must be kept the same
		bool isinternal = IsInternalRelation(base);
		if (isinternal) {
			GetDerivedEquivalent(base[ATTRID_REFERENCE], subt, peer[0]);
		}
		else {
			peer[0] = base[ATTRID_REFERENCE];
		}
		self[ATTRID_MASTEROBJ] = base;
		putreftask(!isinternal).DoWithDeriveds(self, &peer);
	} COMCATCH_IN_TRANSACTION(;);
}
	
