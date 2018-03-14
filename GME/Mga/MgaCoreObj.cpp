// 
// CoreObj-based operations
// 
#include "stdafx.h"
#include "MgaFCO.h"


// Throws !!!!!
metaid_type GetMetaID(const CoreObj &ob) {
				metaid_type t;
				CComPtr<ICoreMetaObject> mmo;
				COMTHROW(ob->get_MetaObject(&mmo));
				COMTHROW(mmo->get_MetaID(&t));
				return t;
}



CoreObj::CoreObj( IMgaObject *ptr) : CComPtr<ICoreObject>(ptr? ObjFor(ptr)->self:NULLCOREOBJ) {}

void CoreObj::Create(ICoreObject *c, metaid_type mm) {
	CComPtr<ICoreProject> p;
	COMTHROW(c->get_Project(&p));
	Create(p,mm);
}


void CoreObj::Create(ICoreProject *p, metaid_type mm) {
	Release();
	COMTHROW(p->CreateObject(mm, &ComPtr()));
}





void GetRootFCO(CoreObj &fco,  /* out */ CoreObj &rootFCO, int *level) {
	CoreObj f = fco;
	int l = 0;
	ASSERT(f.IsFCO());
	while(f[ATTRID_ROLEMETA] != METAREF_NULL) {
			f = f[ATTRID_FCOPARENT];
			l++;
			ASSERT(f.IsFCO());
	}
	rootFCO <<= f;
	if(level) *level = l;
}

void GetRootOfDeriv(CoreObj &fco,  /* out */ CoreObj &rootFCO, int *level) {
	CoreObj f = fco;
	CoreObj fret;
	int l = -1;
	ASSERT(f.IsFCO());
	while(CoreObjs(f[ATTRID_DERIVED+ATTRID_COLLECTION]).Count()) {
			l++;
			fret = f;
			if(f.IsRootFCO()) break;
			f = f[ATTRID_FCOPARENT];
			ASSERT(f.IsFCO());
	}
	rootFCO <<= fret;
	if(level) *level = l;
}


bool IsContained(CoreObj &fco, CoreObj &parentFCO, int *level) {
	CoreObj f = fco;
	int l = 0;
	if(!f) return false;
	ASSERT(f.IsFCO());
	while(!COM_EQUAL(f, parentFCO)) {
			if(f.IsRootFCO()) return false;
			f = f[ATTRID_FCOPARENT];
			ASSERT(f.IsFCO());
			l++;
	}
	if(level) *level = l;
	return true;
}


bool IsFolderContained(CoreObj &fold, CoreObj &parentFold, int *level) {
	CoreObj f = fold;
	int l = 0;
	if( !f) return false;
	ASSERT( !f.IsFCO());
	while( !COM_EQUAL( f, parentFold)) {
			if( f.IsRootFolder()) return false;
			f = f[ ATTRID_FPARENT];
			ASSERT( !f.IsFCO());
			l++;
	}
	if(level) *level = l;
	return true;
}


CoreObj CoreObj::GetMaster(int offset) {
	CoreObj ret;
	metaid_type t = GetMetaID();
	if(t == DTID_CONNROLESEG) {	// the masters of connrolesegments is determined by ATTRID_SEGORDNUM matching:
		CoreObj p = (*this)[ATTRID_CONNSEG];
		CoreObj pm = p.GetMaster(offset);
		if(pm) {
			long ord = (*this)[ATTRID_SEGORDNUM];
			CoreObjs ms = pm[ATTRID_CONNSEG + ATTRID_COLLECTION];
			ITERATE_THROUGH(ms) {
				if(ord == ITER[ATTRID_SEGORDNUM]) { ret = ITER; break; }
			}
			ASSERT(("connrolesegment number mismatch in master role",ret));
		}
	}
	else {
		ret = (*this)[ATTRID_MASTEROBJ];
		if(ret && offset > 1) ret = ret.GetMaster(offset - 1);
	}
	return ret;
}


CoreObj CoreObj::GetMgaObj() {
	CoreObj r = *this;
	while(true) {
		metaid_type t = r.GetMetaID();
		if(t >= DTID_MODEL && t <= DTID_FOLDER) break; 
		attrid_type ai;
		switch(t) {
		case DTID_CONNROLE:		ai = ATTRID_CONNROLE;  break;
		case DTID_CONNROLESEG:  ai = ATTRID_CONNSEG; break;
		case DTID_SETNODE:		ai = ATTRID_SETMEMBER; break;
		case DTID_CONSTRAINT:   ai = ATTRID_CONSTROWNER; break;
		default:				ai = ATTRID_ATTRPARENT;
		}
		r = r[ai];
		if (!r) {
			break;
		}
	}
	return r;
}


CoreObj CoreObj::FollowChain(attrid_type id, int offset) {  // FOLLOWS a chain (like ATTRID_FCOPARENT) 
	CoreObj h = *this; 									    // returns NULL if list is shorter
	while(h && offset--) { h = h[id]; }
	return h;
}

void CoreObjs::Sort() { 
	CREATECOLLECTION_FOR(ICoreObject, q);
	long l = Count();
//	for(long i = l; i > 0; i--) {
	for(long i = 1; i <= l; i++) {     // depends on what collection is used
									  // (where data is plugged in on Add())
		ITERATE_THROUGH((*this)) {
			if(ITER[ATTRID_SEGORDNUM] == i) {
				q->Add(ITER);
				break;
			}
		}
		if(!ITER_BROKEN) COMTHROW(E_MGA_DATA_INCONSISTENCY);
	}
	Attach(q.Detach());
}

bool CoreObj::IsRootOfDeriv() const {
	CoreObj parent = (*this)[ATTRID_FCOPARENT];
	return		(IsRootFCO() ||
				!CoreObjs(parent[ATTRID_DERIVED + ATTRID_COLLECTION]).Count()) && 
						CoreObjs((*this)[ATTRID_DERIVED + ATTRID_COLLECTION]).Count();
}


