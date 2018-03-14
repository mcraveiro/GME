//
// MgaDeriveInfoOps.cpp : Implementation of FCO Derive dependency operations
//
#include "stdafx.h"
#include "MgaFCO.h"

// ----------------------------------------
// Derive information
// ----------------------------------------
HRESULT FCO::GetBaseType(IMgaFCO ** basetype, IMgaFCO ** immbase, VARIANT_BOOL *isinst) {
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPARVALIDNULL(basetype)
			CHECK_OUTPTRPARVALIDNULL(immbase)
			if(isinst) {
				get_IsInstance(isinst);
			}
			if(basetype || immbase) {
				CoreObj base = self[ATTRID_DERIVED];
				if(base) {
					if(immbase) { ObjForCore(base)->getinterface(immbase); }
					if(basetype) {
						while(self[ATTRID_PERMISSIONS] & INSTANCE_FLAG) {
							base = base[ATTRID_DERIVED];
							ASSERT(base);
						}
						ObjForCore(base)->getinterface(basetype); 
					}
				}
				else {
					if(immbase) *immbase = NULL;
					if(basetype) *basetype = NULL;
				}
			}
		} COMCATCH( if(immbase) *immbase = NULL; )
}


HRESULT FCO::get_DerivedFrom( IMgaFCO **pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CoreObj d = self[ATTRID_DERIVED];
			if(d) ObjForCore(d)->getinterface(pVal);
		} COMCATCH(; )
}

HRESULT FCO::get_DerivedObjects( IMgaFCOs **pVal) {
		COMTRY {
			CheckRead();
			CREATEEXCOLLECTION_FOR(MgaFCO, q);
			CoreObjs d = self[ATTRID_DERIVED+ATTRID_COLLECTION];
			ITERATE_THROUGH(d) {
				CComPtr<IMgaFCO> p;
				ObjForCore(ITER)->getinterface(&p);
				q->Add(p);
			}
			*pVal = q.Detach();
		} COMCATCH(; )
}

HRESULT FCO::get_Type( IMgaFCO **pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPAR(pVal);
;
			if(!(self[ATTRID_PERMISSIONS] & INSTANCE_FLAG)) COMTHROW(E_MGA_NOT_INSTANCE);
			CoreObj d = self[ATTRID_DERIVED], d2;
			if(d) {
				while((d2 = d[ATTRID_DERIVED]) != NULL) d <<= d2;
				ObjForCore(d)->getinterface(pVal);
			}
		} COMCATCH(; )
}

HRESULT FCO::get_BaseType( IMgaFCO **pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPAR(pVal);
			if(self[ATTRID_PERMISSIONS] & INSTANCE_FLAG) COMTHROW(E_MGA_INSTANCE);
			CoreObj d = self[ATTRID_DERIVED];
			if(d) ObjForCore(d)->getinterface(pVal);
		} COMCATCH(; )
}

HRESULT FCO::get_ArcheType( IMgaFCO **pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CoreObj d = self[ATTRID_DERIVED], d2;
			if(d) {
				while((d2 = d[ATTRID_DERIVED]) != NULL) d <<= d2;
				ObjForCore(d)->getinterface(pVal);
			}
		} COMCATCH(; )
}

HRESULT FCO::get_IsInstance( VARIANT_BOOL *pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPAR(pVal);
			*pVal = self[ATTRID_PERMISSIONS] & INSTANCE_FLAG ? VARIANT_TRUE : VARIANT_FALSE;
		} COMCATCH(; )
}

HRESULT FCO::get_IsLibObject( VARIANT_BOOL *pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPAR(pVal);
			*pVal = self[ATTRID_PERMISSIONS] & LIBRARY_FLAG ? VARIANT_TRUE : VARIANT_FALSE;
		} COMCATCH(; )
}

HRESULT FCO::get_IsPrimaryDerived( VARIANT_BOOL *pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPAR(pVal);
			CoreObj d = self[ATTRID_DERIVED];
			if(d && self[ATTRID_RELID] < RELIDSPACE) {
				*pVal = VARIANT_TRUE;
			}
			else *pVal = VARIANT_FALSE;
		} COMCATCH(; )
}


HRESULT FCO::get_AllBaseTypes(IMgaFCOs **bases) {       // retuns itself if root model
	COMTRY {
		CoreObj cur = self;
		CREATEEXCOLLECTION_FOR(MgaFCO, q);
		while(GetMetaID(cur) != DTID_FOLDER) {
			if(cur[ATTRID_RELID] < RELIDSPACE) {
				CoreObj base = cur[ATTRID_DERIVED];
				if(base) {
					CComPtr<IMgaFCO> ff;
					ObjForCore(base)->getinterface(&ff);
					q->Add(ff);
				}
			}
			cur = cur[ATTRID_FCOPARENT];
			ASSERT(cur);
		}
		*bases = q.Detach();
	} COMCATCH(;)
}


HRESULT FCO::get_ChildDerivedFrom(IMgaFCO *baseobj, IMgaFCO **pVal) {
	COMTRY {
		CHECK_MYINPTRPAR(baseobj);
		CHECK_OUTPTRPAR(pVal);
		CoreObj bchild(baseobj);
		CoreObj bparent = self[ATTRID_DERIVED];
		if(!bparent || !COM_EQUAL(bparent, CoreObj(bchild[ATTRID_FCOPARENT]))) COMTHROW(E_MGA_NOT_DERIVED);
		CoreObj selfchild;
		GetDerivedEquivalent(bchild, self, selfchild, 1);
		if(!selfchild)	COMTHROW(E_MGA_META_VIOLATION);
		ObjForCore(selfchild)->getinterface(pVal);
	} COMCATCH(;)
}



// ----------------------------------------
// NON-members that use CoreObjs 
// ----------------------------------------

// check if the original master of this relation is internal or external
// does not check the internality of references within connections!
bool IsInternalRelation(CoreObj src) {
	CoreObj m;
	while((m = src.GetMaster()) != NULL) src = m;
	attrid_type a = 0;
	switch(src.GetMetaID()) {
	case DTID_REFERENCE: a = ATTRID_REFERENCE; break;	
	case DTID_CONNROLE:	 a = ATTRID_XREF; break;	
	case DTID_SETNODE:	 a = ATTRID_XREF; break;	
	case DTID_REFATTR:	 a = ATTRID_XREF; break;	
	case DTID_CONNROLESEG:   a = ATTRID_SEGREF;	break;
	default: COMTHROW(E_MGA_DATA_INCONSISTENCY);
	}
	CoreObj target = src[a];
	if(!target) return false;
	CoreObj p1, p2;
	GetRootOfDeriv(target, p1);
	GetRootOfDeriv(src.GetMgaObj(), p2);
	return p1 && COM_EQUAL(p1,p2);
}

// in the diagram below a model M is instantiated/subtyped into Mi
// M contains reference r, pointing to A
// Mi will contain derived reference ri, and derived target object Ai
// Mj will contain derived reference rj, and derived target object Aj
// this is the internal reference redirection scenario, so when MGA detects
// that secondary derived references (r) have their targets set like here to 
// another secondary derived object (A) it must perform the redirect job
// i.e.: ri->Ai, rj->Aj
// external references (let's suppose p refers to B, p->B) can't be redirected
// automatically because there is no clue how Mi, Mj relate to B1, B2
//
//        M-------------                  B       
//        |             |               /   \     
//        | r--->A      |              /     \    
//        | p           |            / \     / \  
//         -------------            /I/S\   /I/S\ 
//          /           \           ----    ----- 
//         |             |           |        |   
//        / \           / \          B1       B2  
//       /I/S\         /I/S\              
//       -----         -----              
//         |              |               
//  Mi------------     Mj------------     
//  |             |    |             |    
//  | ri---->Ai   |    | rj---->Aj   |    
//  |             |    |             |    
//   -------------      -------------     
//
// this method GetDerivedEquivalent gives back/finds Ai [corresponding for A] in Mi
// and is called like:
// GetDerivedEquivalent( A, Mi, [out] Ai, [in] level = -1)
// the corresponding parameters are (names taken from the diagram above)
void GetDerivedEquivalent(CoreObj const &objinbase, CoreObj const &subtype, CoreObj &objinsubtype, int level) {
	CoreObj basetype= subtype[ATTRID_DERIVED];
	ASSERT(basetype);
	CoreObj o = objinbase;
	
	if(!objinbase || (level < 0 && !IsContained(o,basetype, &level)) ) {
		objinsubtype = objinbase;
		return;
	}
	CoreObjs subs = objinbase[ATTRID_DERIVED + ATTRID_COLLECTION];
	ITERATE_THROUGH(subs) {
		if( COM_EQUAL(subtype, ITER.FollowChain(ATTRID_FCOPARENT, level))) {
			objinsubtype = ITER;
			return;
		}
	}
	COMTHROW(E_MGA_DATA_INCONSISTENCY);
}


// ----------------------------------------
// DeriveTreeTask
// ----------------------------------------
void DeriveTreeTask::_DoWithDeriveds(CoreObj self, std::vector<CoreObj> *peers) {
		CoreObjs chds = self[masterattr + ATTRID_COLLECTION];  // do it first to enable delete operations...
		if(!Do(self, peers))
			return;
		ITERATE_THROUGH(chds) {
			if(peercnt) {
				CoreObj r = ITER.FollowChain(ATTRID_FCOPARENT, selfdepth);
				std::vector<CoreObj> subpeers(peercnt);
				int i;
				for(i = 0; i < internalpeercnt; i++) {			// first the internals
					GetDerivedEquivalent((*peers)[i], r, subpeers[i], peerdepths[i]);
				}
				for(; i < peercnt; i++) subpeers[i] = (*peers)[i];  // the rest are externals
				_DoWithDeriveds(ITER, &subpeers);
			}
			else  _DoWithDeriveds(ITER, peers);		
		}
}

// before starting with the initial object it is determined 
// which targets are 'internal' i.e. to be adjusted for the subtypes
// internal refs always preceed external refs in the peers list.
// 'false internals' (references into the same derive tree, but inherited 
// from a basetype) are rare, but they can be avoided through setting 
// the 'endreserve' data member (e.g. at RevertToBase) 

// masterattr the attribute followed recuresively. It can either be ATTRID_DERIVEDS
// or ATTRID_MASTEROBJ. It is usually determined by the objecttype, but in case of 
// references, it can be preset by the user.
void DeriveTreeTask::DoWithDeriveds(CoreObj self, std::vector<CoreObj> *peers) {
		if(GetMetaID(self) == DTID_FOLDER) Do(self, peers);  // no derived objects
		else {
			ASSERT(!masterattr || self.GetMetaID() == DTID_REFERENCE);
			if(!masterattr) {
				if(self.IsFCO()) masterattr = ATTRID_DERIVED;
				else {
					ASSERT(GetMetaID(self) == DTID_CONNROLE || GetMetaID(self) == DTID_SETNODE);
					masterattr = ATTRID_MASTEROBJ;
				}
			}
			CoreObjs chds = self[masterattr + ATTRID_COLLECTION];
			if(!peers) peercnt = internalpeercnt = 0;
			else if(!chds.Count()) {
				peercnt = (*peers).size();
				internalpeercnt = 0;
			}
			else {
				CoreObj r;
				GetRootOfDeriv(self, r, &selfdepth);
				ASSERT(r);
				peercnt = (*peers).size();
				peerdepths.resize(peercnt);   // only internalpeercnt will be used, but we do not know that number yet
				int i;
				for(i = 0; i < peercnt-endreserve; i++) {
					int l;
					CoreObj r2;
					if((*peers)[i] == NULL) break;
					GetRootOfDeriv((*peers)[i], r2, &l);
					if(COM_EQUAL(r,r2)) peerdepths[i] = l;
					else break; 
				}
				internalpeercnt = i;
			}
			_DoWithDeriveds(self, peers);
		}
}


// ----------------------------------------
// Attach/detach
// ----------------------------------------
HRESULT FCO::DetachFromArcheType ()
{
	COMTRY_IN_TRANSACTION {
		CheckWrite();
		VARIANT_BOOL prim_deriv;
		COMTHROW(this->get_IsPrimaryDerived(&prim_deriv));
		if (prim_deriv == VARIANT_FALSE) {
			COMTHROW(E_MGA_INVALID_ARG);
		}
		CoreObj d = self[ATTRID_DERIVED], d2;
		if(d) { // if self is really derived from something (d)
			while((d2 = d[ATTRID_DERIVED]) != NULL) 
				d <<= d2;

			// copy/merge/detach from parent to self (overwrite the derivation)
			// almost like ObjTreeCopy, copies from 'd' (the archetype) to 'self' (the derived)
			coreobjpairhash crealist2;
			ObjDetachAndMerge(mgaproject, d, self, crealist2, 0, true);

		}
		SelfMark(OBJEVENT_SUBT_INST);

		// Need to run this check: "inherited ref can only refer to a derived instance of the target of its base"
		{
			CoreObjs segs = self[ATTRID_REFERENCE + ATTRID_COLLECTION];
			ITERATE_THROUGH(segs) {
				CComPtr<IMgaFCO> ffco;
				ObjForCore(ITER)->getinterface(&ffco);
				COMTHROW(ffco->Check());
			}
		}

		CComPtr<IMgaFCO> selfFco;
		ObjForCore(self)->getinterface(&selfFco);
		COMTHROW(selfFco->Check());

		CoreObjMark(self[ATTRID_PARENT], OBJEVENT_LOSTCHILD);
	} COMCATCH_IN_TRANSACTION(;);

	return S_OK;
}

// if instance is needed all inner objects in 'self' must be attachable to a corresponding object in 'newtype'
// if subtype is needed then attachment happens with (name & kind) matchable elements [these become secondary derived]
// and any other object in 'self' will become additional plain object
HRESULT FCO::AttachToArcheType( IMgaFCO *newtype,  VARIANT_BOOL instance) {
	COMTRY_IN_TRANSACTION {
		CHECK_MYINPTRPAR(newtype);
		CoreObj ntype(newtype);
		coreobjpairhash crealist2;
		// attaches self to nettype (the new base) if possible
		ObjAttach( mgaproject, ntype, self, crealist2, instance, true );
		CoreObjMark( ntype, OBJEVENT_SUBT_INST);
	} COMCATCH_IN_TRANSACTION(;)
}
