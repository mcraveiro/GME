// MgaSet.cpp : Implementation of CMgaSet
#include "stdafx.h"
#include "Mga.h"
#include "MgaSet.h"

/////////////////////////////////////////////////////////////////////////////
// CMgaSet


HRESULT FCO::get_Members(IMgaFCOs **pVal)
{
	COMTRY {
		CheckRead();
		CHECK_OUTPTRPAR(pVal);
		CREATEEXCOLLECTION_FOR(MgaFCO, q);
		CoreObjs members = self[ATTRID_SETMEMBER + ATTRID_COLLECTION];
		ITERATE_THROUGH(members) {
			CComPtr<IMgaFCO> s;
			ObjForCore(CoreObj(ITER[ATTRID_XREF]))->getinterface(&s);
			q->Add(s);
		}
		*pVal = q.Detach();
	} COMCATCH(;);
}

HRESULT FCO::get_IsMember(IMgaFCO *obj, VARIANT_BOOL *pVal) {
	COMTRY {
		CheckRead();
		CHECK_INPTRPAR(obj);
		CHECK_OUTPAR(pVal);
		CoreObj search(obj);
		CoreObjs members = self[ATTRID_SETMEMBER + ATTRID_COLLECTION];
		*pVal = VARIANT_FALSE;
		ITERATE_THROUGH(members) {
			if(COM_EQUAL(search, ITER)) {
				*pVal = VARIANT_TRUE;
				break;
			}	
		}
	} COMCATCH(;);
}

bool addmember::Do(CoreObj self, std::vector<CoreObj> *peers) {
	CoreObjs members = self[ATTRID_SETMEMBER + ATTRID_COLLECTION];
	auto fco = ObjForCore(self);
	ITERATE_THROUGH(members) {
		if(COM_EQUAL((*peers)[0], CoreObj(ITER[ATTRID_XREF]))) { // Already there !!!
			ASSERT(!CoreObj(ITER[ATTRID_MASTEROBJ]));
			ITER[ATTRID_MASTEROBJ] = (*peers)[1];
			(*peers)[1] = ITER;
			return false;
		}   
	}
	CoreObj nnode;
	COMTHROW(fco->mgaproject->dataproject->CreateObject(DTID_SETNODE,&nnode.ComPtr()));
	nnode[ATTRID_MASTEROBJ] = (*peers)[1];
	nnode[ATTRID_SETMEMBER] = self;
	nnode[ATTRID_XREF] = (*peers)[0];
	(*peers)[1] = nnode;
	ObjForCore((*peers)[0])->SelfMark(OBJEVENT_SETINCLUDED);
	fco->SelfMark(OBJEVENT_RELATION);
	COMTHROW(fco->Check());
	return true;
};


// TODO: define semantics for assigning members that already exist for a set.
HRESULT FCO::AddMember(IMgaFCO *member)
{
	COMTRY_IN_TRANSACTION { 
		CheckWrite();
		CHECK_MYINPTRPAR(member);
		std::vector<CoreObj> peers(2);
		peers[0] = CoreObj(member);
		peers[1] = NULLCOREOBJ;
		addmember(false).DoWithDeriveds(self, &peers);
		CoreObj d = self[ATTRID_DERIVED];
		CoreObj memd = peers[0][ATTRID_DERIVED];
		if(d && memd) {
			CoreObjs members = d[ATTRID_SETMEMBER + ATTRID_COLLECTION];
			ITERATE_THROUGH(members) {
				if(COM_EQUAL(memd, CoreObj(ITER[ATTRID_XREF]))) { 
					peers[1][ATTRID_MASTEROBJ] = ITER;
					break;
				}   
			}
		}
	} COMCATCH_IN_TRANSACTION(;);
}


bool removemember::Do(CoreObj self, std::vector<CoreObj> *peers) {
	ObjForCore(CoreObj(self[ATTRID_XREF]))->SelfMark(OBJEVENT_SETEXCLUDED);
	ObjForCore(CoreObj(self[ATTRID_SETMEMBER]))->SelfMark(OBJEVENT_RELATION);
	ITERATE_THROUGH(self[ATTRID_MASTEROBJ+ATTRID_COLLECTION]) {
		ITER[ATTRID_MASTEROBJ] = NULLCOREOBJ;
	}
	COMTHROW(self->Delete());
	return true;
}

HRESULT FCO::RemoveMember(IMgaFCO *member) {
	COMTRY_IN_TRANSACTION { 
		CheckWrite();
		CHECK_MYINPTRPAR(member);
		bool f = false;
		CoreObj mem(member);
		CoreObjs members = self[ATTRID_SETMEMBER + ATTRID_COLLECTION];
		ITERATE_THROUGH(members) {
			if(COM_EQUAL(mem, CoreObj(ITER[ATTRID_XREF]))) {
				removemember().DoWithDeriveds(ITER);
				f = true;
			}
		}
		if(!f) COMTHROW(E_MGA_OBJECT_NOT_MEMBER);
	} COMCATCH_IN_TRANSACTION(;);
}

HRESULT FCO::SetRemoveAll()
{
	COMTRY_IN_TRANSACTION { 
		CheckWrite();
		CoreObjs members = self[ATTRID_SETMEMBER + ATTRID_COLLECTION];
		ITERATE_THROUGH(members) {
			removemember().DoWithDeriveds(ITER);
		}
	} COMCATCH_IN_TRANSACTION(;);
}


HRESULT FCO::SetCompareToBase(IMgaFCO *a, short *status) { 
	COMTRY {
		CheckRead();
		CHECK_MYINPTRPARVALIDNULL(a);
		CHECK_OUTPAR(status);
		CoreObj base = self[ATTRID_DERIVED];
		if(!base) COMTHROW(E_MGA_NOT_DERIVED);
		CoreObjs bmembers = base[ATTRID_SETMEMBER + ATTRID_COLLECTION];
		CoreObjs members = self[ATTRID_SETMEMBER + ATTRID_COLLECTION];
		*status = 1;
		//CoreObj ca(a);
		ITERATE_THROUGH(members) {
			if(!CoreObj(ITER[ATTRID_MASTEROBJ])) {
				/*if(!ca || COM_EQUAL(ca, CoreObj(ITER[ATTRID_XREF])))*/ break; //commented by ZolMol since always true
			}
		}
		if( !ITER_BROKEN && members.Count() == bmembers.Count()) *status = 0; // by ZolMol: equal if & only if the self has only master objects and the # of the members is equal in the derived and the base set
	} COMCATCH(;);
}

HRESULT FCO::SetRevertToBase(IMgaFCO* a)  { 
	COMTRY_IN_TRANSACTION { 
		CheckWrite();
//		CHECK_MYINPTRPAR(a);
		CoreObj base = self[ATTRID_DERIVED];
		if(!base) COMTHROW(E_MGA_NOT_DERIVED);
		COMTHROW(SetRemoveAll());	// simple solution: first delete everything, then re-create all.
		CoreObjs bmembers = base[ATTRID_SETMEMBER + ATTRID_COLLECTION];
		CoreObj rb;
		int d;
		GetRootOfDeriv(base, rb, &d);
		CoreObj subt = self.FollowChain(ATTRID_FCOPARENT,d);
		ITERATE_THROUGH(bmembers) {
			CoreObj newmember;
			std::vector<CoreObj> peers(2);
//  if it is external in the most original base, the target must be kept the same
			bool isinternal = IsInternalRelation(ITER);
			if(isinternal)	GetDerivedEquivalent(ITER[ATTRID_XREF],subt,peers[0]);
			else			peers[0] = ITER[ATTRID_XREF];
			peers[1] = ITER;
			addmember(!isinternal).DoWithDeriveds(self, &peers);
		}
		
	} COMCATCH_IN_TRANSACTION(;);
}

// ----------------------------------------
// get sets this object is member of (available for all FCO-s)
// ----------------------------------------
HRESULT FCO::get_MemberOfSets(IMgaFCOs **pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CoreObjs segs = self[ATTRID_XREF+ATTRID_COLLECTION];
			CREATEEXCOLLECTION_FOR(MgaFCO, q);
			ITERATE_THROUGH(segs) {
				if(GetMetaID(ITER) != DTID_SETNODE) continue;
				CoreObj fco = ITER[ATTRID_SETMEMBER];
				ASSERT(fco);
				CComPtr<IMgaFCO> ffco;
				ObjForCore(fco)->getinterface(&ffco);
				q->Add(ffco); 
			}
			*pVal = q.Detach();
		} COMCATCH(;)
}


