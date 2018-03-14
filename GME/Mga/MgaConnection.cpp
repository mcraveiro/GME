// MgaConnection.cpp : Implementation of CMgaConnection
#include "stdafx.h"
#include "MgaConnection.h"

/////////////////////////////////////////////////////////////////////////////
// CMgaConnection
const CComBSTR FCO::SrcName="src", FCO::DstName="dst";


bool FCO::findroleobj(BSTR rolename, CoreObj &tt) {
			CHECK_INSTRPAR(rolename);
			CoreObjs roles = self[ATTRID_CONNROLE+ATTRID_COLLECTION];
			ITERATE_THROUGH(roles) {
					CComBSTR n = ITER[ATTRID_NAME];
					if(n == rolename) {
						tt <<= ITER;
						return true;
					}
			}
			return false;
}

HRESULT FCO::get_NamedRoleReferences(BSTR rolename, IMgaFCOs **pVal) {
		COMTRY {
			CHECK_INSTRPAR(rolename); 
			CHECK_OUTPTRPAR(pVal); 
			CoreObj r;
			if(!findroleobj(rolename, r))
			{
				COMRETURN(E_MGA_NAME_NOT_FOUND);
			}
			CoreObjs segs = r[ATTRID_CONNSEG+ATTRID_COLLECTION];
			segs.Sort();
			CREATEEXCOLLECTION_FOR(MgaFCO, q);
			ITERATE_THROUGH(segs) {
			  CoreObj seg = ITER[ATTRID_SEGREF];
			  ASSERT(seg);
			  CComPtr<IMgaFCO> ff;
		      ObjForCore(seg)->getinterface(&ff);
			  q->Add(ff);
			}
			*pVal = q.Detach();
		} COMCATCH(;)
}
	
HRESULT FCO::get_NamedRoleTarget(BSTR rolename, IMgaFCO **pVal) {
		COMTRY {
			CHECK_INSTRPAR(rolename); 
			CHECK_OUTPTRPAR(pVal); 
			CoreObj r;
			if(findroleobj(rolename, r)) { 
				CoreObj obj = r[ATTRID_XREF];
				if(obj) {
					ObjForCore(obj)->getinterface(pVal);
				}
			}
		} COMCATCH(;)
}	


HRESULT FCO::put_NamedRole(BSTR rolename,  IMgaFCOs *refs, IMgaFCO *endfco) {
	COMTRY_IN_TRANSACTION_MAYBE {
			CheckWrite();
			{
				booltempflag fl(mgaproject->checkofftemporary);
					
				CComPtr<IMgaConnPoints> p;
				COMTHROW(get_ConnPoints(&p)); 
				MGACOLL_ITERATE(IMgaConnPoint, p) {
					CComBSTR rn;
					COMTHROW(MGACOLL_ITER->get_ConnRole(&rn));
					if(rn == rolename) {
						COMTHROW(MGACOLL_ITER->Remove());
						break;
					}
				} MGACOLL_ITERATE_END;
			}
			if(endfco) {
				CComPtr<IMgaConnPoint> tt;
				COMRETURN_IN_TRANSACTION_MAYBE(AddConnPoint(rolename, 1, endfco, refs, &tt));
			}
	} COMCATCH_IN_TRANSACTION_MAYBE(;)
}	

HRESULT FCO::CreateSimpleConn( IMgaMetaRole *metar,  IMgaFCO *src,  IMgaFCO *dst, 
										 IMgaFCOs *srcrefs, IMgaFCOs *dstrefs, IMgaFCO **newobj) {
	COMTRY_IN_TRANSACTION {

		CComPtr<IMgaFCO> f;
		{
			booltempflag fl(mgaproject->checkofftemporary);
			COMTHROW(CreateChildObject(metar, &f));
			objtype_enum ot;
			COMTHROW(f->get_ObjType(&ot));
			if(ot != OBJTYPE_CONNECTION) COMTHROW(E_MGA_INVALID_ROLE);
			COMTHROW(ObjFor(f)->put_NamedRole(SrcName,  srcrefs, src));
		}	
		COMTHROW(ObjFor(f)->put_NamedRole(DstName,  dstrefs, dst));
		*newobj = f.Detach();
	} COMCATCH_IN_TRANSACTION(;)
}

static CComPtr<IMgaFCOs> GetRefChain(IMgaFCO* fco)
{
	CREATEEXCOLLECTION_FOR(MgaFCO, srcrefs);

	CComQIPtr<IMgaReference> ref = fco;
	while (srcrefs && ref)
	{
		srcrefs->Add(ref);
		CComPtr<IMgaFCO> referred;
		COMTHROW(ref->get_Referred(&referred));
		if (referred)
		{
			ref.Release();
			ref = referred;
		}
	}
	return CComQIPtr<IMgaFCOs>(srcrefs);
}

HRESULT FCO::CreateSimpleConnDisp( IMgaMetaRole *metar
                                  , IMgaFCO *srcobj, IMgaFCO *dstobj
                                  , IMgaFCO *srcref, IMgaFCO *dstref
                                  , IMgaFCO **newobj) 
{
	return CreateSimpleConn( metar, srcobj, dstobj, GetRefChain(srcref), GetRefChain(dstref), newobj);
}


/* ******************** CONNPOINT ********************************* */

void CMgaConnPoint::GetConnPoint(FCO *f, CoreObj &c, IMgaConnPoint **cp) {
	ASSERT(GetMetaID(c) == DTID_CONNROLE);
	FCO::cphash::iterator ii;
	objid_type ll = c.GetObjID();
	if((ii = f->connpointhash.find(ll)) != f->connpointhash.end()) {
		(*cp = (*ii).second)->AddRef();
	}
	else {
		CComPtr<CMgaConnPoint> p; 
		CreateComObject(p);
#ifdef DEBUG
		p->MARKSIG('1');
#endif
		p->fco = f;
		p->cobj = c;
		f->connpointhash.insert(FCO::cphash::value_type(ll, p));
		*cp = p.Detach();
	}
}


CMgaConnPoint::~CMgaConnPoint() {
	MARKSIG('9');
	// n.b. don't use GetObjId, it can COMTHROW when the project is closed
	objid_type t;
	HRESULT hr = cobj->get_ObjID(&t);
	if (SUCCEEDED(hr)) {
		FCO::cphash::iterator ii = fco->connpointhash.find(t);
		ASSERT(ii != fco->connpointhash.end());
		fco->connpointhash.erase(ii);
	}
}	


// the peers array is used in a tricky way here. 
// the first peercnt-2 members (if any) are references for connsegments
// the peer before the last is the target
// the last peer points back to the master object of the new connpoint
//    (it is protected by endreserve, and updated manually below)

bool AddConnPTask::Do(CoreObj self, std::vector<CoreObj> *peers) {
	CoreObj role;
	CMgaProject *mgaproject = ObjForCore(self)->mgaproject;
	if(maxinrole > 0) {
		long rolesofname = 0;
		ITERATE_THROUGH(self[ATTRID_CONNROLE+ATTRID_COLLECTION]) {
			if(rolename == CComBSTR(ITER[ATTRID_NAME])) rolesofname++;
		}
		if(rolesofname >= maxinrole) {
			if(!(*peers)[peercnt-1]) {  // if this is the master connection
				COMTHROW(E_MGA_CONNROLE_USED);  // indicate error
			}
			return false;			   // ... else silently ignore it
		}
	}
	COMTHROW(mgaproject->dataproject->CreateObject(DTID_CONNROLE,&role.ComPtr()));
	role[ATTRID_NAME] = rolename;
	role[ATTRID_CONNROLE] = self;
	role[ATTRID_XREF] = (*peers)[peercnt-2];
	role[ATTRID_MASTEROBJ] = (*peers)[peercnt-1];
	for(int i = 0; i < peercnt-2; i++) {
		CoreObj seg;
		COMTHROW(mgaproject->dataproject->CreateObject(DTID_CONNROLESEG,&seg.ComPtr()));
		seg[ATTRID_CONNSEG] = role;
		seg[ATTRID_SEGORDNUM] = i+1;
		seg[ATTRID_SEGREF] = (*peers)[i];
	}

	if( peercnt-2 >= 1) // additional check
	{
		CoreObj last_ref = (*peers)[peercnt - 3];
		CoreObj tgt_model= last_ref[ATTRID_REFERENCE];
		if (!last_ref.p)
			throw_com_error(E_MGA_INVALID_ARG, L"Cannot add refport to null reference");
		CoreObj tgt_elem = (*peers)[peercnt - 2];
		CoreObj parent   = tgt_elem[ATTRID_FCOPARENT];
		CoreObj bas_model= tgt_model[ATTRID_DERIVED]; // tgt_model must be a derived for GetDerivedEquivalent to succeed
		if( !COM_EQUAL(parent, tgt_model) && bas_model) { // JIRA:130?
			CoreObj new_tgt_elem;
			GetDerivedEquivalent( tgt_elem, tgt_model, new_tgt_elem);
			if( new_tgt_elem) // found tgt_elem's equivalent in tgt_model
				role[ATTRID_XREF] = new_tgt_elem; // overwrite the previous value: (*peers)[peercnt-2];
		}
	}

	IMgaObjectPtr iObject;
	ObjForCore(self)->getinterface(&iObject, 0);
	iObject->__Check();
	ObjForCore((*peers)[0])->SelfMark(OBJEVENT_CONNECTED);
	ObjForCore(self)->SelfMark(OBJEVENT_RELATION);
	(*peers)[peercnt-1] = role;
	return true;
}

HRESULT FCO::AddConnPoint(BSTR rolename, long maxinrole, IMgaFCO *target, IMgaFCOs *refs, IMgaConnPoint **pVal) {
	COMTRY_IN_TRANSACTION_MAYBE {
	   CheckWrite();
	   long c = 0;
	   if(refs) COMTHROW(refs->get_Count(&c));
	   std::vector<CoreObj> peers(c+2);
	   CHECK_INSTRPAR(rolename);
	   CHECK_MYINPTRPAR(target);
	   CHECK_OUTPTRPAR(pVal);
	   int pos = 0;
	   if(refs) {
		   CHECK_MYINPTRSPAR(refs);

		   MGACOLL_ITERATE(IMgaFCO, refs) {
			   CoreObj l(MGACOLL_ITER);
			   if(l.GetMetaID() != DTID_REFERENCE) COMTHROW(E_MGA_REFERENCE_EXPECTED);
			   peers[pos++] = l;
		   } MGACOLL_ITERATE_END;
	   } 
	   peers[pos++] = CoreObj(target);
	   peers[pos] = NULLCOREOBJ;		
	   AddConnPTask(maxinrole, rolename).DoWithDeriveds(self, &peers);
	   CMgaConnPoint::GetConnPoint(this, peers[pos], pVal);
	} COMCATCH_IN_TRANSACTION_MAYBE(;);
}

HRESULT FCO::get_ConnPoints(IMgaConnPoints **pVal) {
	CREATECOLLECTION_FOR(IMgaConnPoint, q);
	COMTRY {
		CoreObjs roles = self[ATTRID_CONNROLE + ATTRID_COLLECTION];
		ITERATE_THROUGH(roles) {
			CComPtr<IMgaConnPoint> p;
			CMgaConnPoint::GetConnPoint(this, ITER, &p);
			q->Add(p);
		}
		*pVal = q.Detach();
	} COMCATCH(;);
}


STDMETHODIMP CMgaConnPoint::get_Owner(IMgaConnection **pVal) {
	COMTRY { 
		CHECK_OUTPTRPAR(pVal);
		ObjForCore(CoreObj(cobj[ATTRID_CONNROLE]))->getinterface(pVal);
	} COMCATCH(;);
}

STDMETHODIMP CMgaConnPoint::get_ConnRole(BSTR *pVal) {
	COMTRY {
		CHECK_OUTSTRPAR(pVal);
		*pVal = CComBSTR(cobj[ATTRID_NAME]).Detach();
	} COMCATCH(;);
}




class put_ConnRoleTask : public DeriveTreeTask {
	CComBSTR newrname;

	bool Do(CoreObj self, std::vector<CoreObj> *peers = NULL) {
		self[ATTRID_NAME] = newrname;
		COMTHROW(ObjForCore(CoreObj(self[ATTRID_CONNROLE]))->Check());
		return true;
	}

public:
	put_ConnRoleTask(BSTR nname) : newrname(nname) { ; }
};


STDMETHODIMP CMgaConnPoint::put_ConnRole(BSTR newVal) {
	CMgaProject *mgaproject = fco->mgaproject;
	COMTRY_IN_TRANSACTION {
		CHECK_INSTRPAR(newVal);
		fco->CheckWrite();
		//if(CComBSTR( cobj[ATTRID_NAME] ) = newVal) ... //zolmol: rewritten for VC7
		//if( (CComBSTR) cobj[ATTRID_NAME] = newVal) ... // also would be ok for vc7
		if( newVal) { // ... but this is more clear
			// FIXME: doesn't look good; we don't need a & ?
			(CComBSTR) cobj[ATTRID_NAME] = newVal;
			cobj[ATTRID_MASTEROBJ] = NULLCOREOBJ;
			put_ConnRoleTask(newVal).DoWithDeriveds(cobj);
		}
	} COMCATCH_IN_TRANSACTION(;);
}

STDMETHODIMP CMgaConnPoint::get_Target(IMgaFCO **pVal) {
	COMTRY { 
		CHECK_OUTPTRPAR(pVal);
		CoreObj t = cobj[ATTRID_XREF];
		ASSERT(t);
		ObjForCore(t)->getinterface(pVal);
	} COMCATCH(;);
}

STDMETHODIMP CMgaConnPoint::get_References(IMgaFCOs **pVal) {
	COMTRY { 
		CHECK_OUTPTRPAR(pVal);
		CoreObjs refs = cobj[ATTRID_CONNSEG + ATTRID_COLLECTION];
		refs.Sort();
		CREATEEXCOLLECTION_FOR(MgaFCO, q);
		ITERATE_THROUGH(refs) {
			CComPtr<IMgaFCO> p;
			CoreObj t = ITER[ATTRID_SEGREF];
			ASSERT(t);
			ObjForCore(t)->getinterface(&p);
			q->Add(p);
		}
		*pVal = q.Detach();
	} COMCATCH(;);
}


void SingleObjTreeDelete(CoreObj &self, bool deleteself = true);


void MgaConnPointDelete(CoreObj& cobj)
{
		CoreObj fco = cobj[ATTRID_CONNROLE];
		CoreObj target = cobj[ATTRID_XREF];
		CoreObjs children = cobj[ATTRID_MASTEROBJ + ATTRID_COLLECTION];
		ITERATE_THROUGH(children)
		{
			MgaConnPointDelete(ITER);
		}
		SingleObjTreeDelete(cobj);
		ObjForCore(fco)->SelfMark(OBJEVENT_RELATION);
		ObjForCore(target)->SelfMark(OBJEVENT_DISCONNECTED);
}

STDMETHODIMP CMgaConnPoint::Remove() {
	CMgaProject *mgaproject = fco->mgaproject;
	COMTRY_IN_TRANSACTION {
		fco->CheckWrite();

		CoreObj fco = cobj[ATTRID_CONNROLE];
		MgaConnPointDelete(cobj);
		COMTHROW(ObjForCore(fco)->Check());
	} COMCATCH_IN_TRANSACTION(;);
}


// get conn points pointed at by this object
HRESULT FCO::get_PartOfConns(IMgaConnPoints **pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CoreObjs segs = self[ATTRID_XREF+ATTRID_COLLECTION];
			CREATECOLLECTION_FOR(IMgaConnPoint, q);
			ITERATE_THROUGH(segs) {
				if(GetMetaID(ITER) != DTID_CONNROLE) continue;
				CoreObj conn = ITER[ATTRID_CONNROLE];
				CComPtr<IMgaConnPoint> p;
			    CMgaConnPoint::GetConnPoint(ObjForCore(conn), ITER, &p);
				q->Add(p); 
			}
			*pVal = q.Detach();
		} COMCATCH(;)
}


// get conn points running through this reference
HRESULT FCO::get_UsedByConns(IMgaConnPoints **pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CoreObjs segs = self[ATTRID_SEGREF+ATTRID_COLLECTION];
			CREATECOLLECTION_FOR(IMgaConnPoint, q);
			ITERATE_THROUGH(segs) {
				CoreObj connrole = ITER[ATTRID_CONNSEG];
				ASSERT(connrole);
				CoreObj conn = connrole[ATTRID_CONNROLE];
				CComPtr<IMgaConnPoint> p;
			    CMgaConnPoint::GetConnPoint(ObjForCore(conn), connrole, &p);
				q->Add(p); 
			}
			*pVal = q.Detach();
		} COMCATCH(;)
}

HRESULT FCO::ConnCompareToBase(IMgaConnPoint *p, short *status) { 
	COMTRY {
		CheckRead();
		if(p) {
			CComPtr<IMgaConnection> z;
			COMTHROW(p->get_Owner(&z));
			CHECK_MYINPTRPAR(z); 
		}
		CHECK_OUTPAR(status);
		CoreObj base = self[ATTRID_DERIVED];
		if(!base) COMTHROW(E_MGA_NOT_DERIVED);

		if(p) {
			CoreObj l = static_cast<CMgaConnPoint *>(p)->cobj;
			*status = CoreObj(l[ATTRID_MASTEROBJ]) ? 0 : 1;
		}
		else {
			CoreObjs cpoints = self[ATTRID_CONNROLE + ATTRID_COLLECTION];
			*status = 1;
			ITERATE_THROUGH(cpoints) {
				if(!CoreObj(ITER[ATTRID_MASTEROBJ])) break;
			}
			if(!ITER_BROKEN) *status = 0;
		}
	} COMCATCH(;);
}


HRESULT FCO::ConnRevertToBase(IMgaConnPoint *p)  { 
	COMTRY_IN_TRANSACTION {
		CheckWrite();
		if(p) {
			CComPtr<IMgaConnection> z;
			COMTHROW(p->get_Owner(&z));
			CHECK_MYINPTRPAR(z); 
		}
		CoreObj base = self[ATTRID_DERIVED];
		if(!base) COMTHROW(E_MGA_NOT_DERIVED);

		if(p) {
			COMTHROW(E_MGA_NOT_IMPLEMENTED);
		}
		else {
			std::set<CoreObj> vv;
			{
				ITERATE_THROUGH(self[ATTRID_CONNROLE + ATTRID_COLLECTION]) {
					if (CoreObj(ITER[ATTRID_MASTEROBJ]))
						vv.insert(ITER[ATTRID_MASTEROBJ]);
					else
						MgaConnPointDelete(ITER);
				}
			}
			ITERATE_THROUGH(base[ATTRID_CONNROLE + ATTRID_COLLECTION]) {
				if(vv.find(ITER) == vv.end()) {
						coreobjpairhash crealist;
						CoreObj newcoreobj;
						ObjTreeDerive(mgaproject, ITER, newcoreobj, crealist, 0);  // copy
						newcoreobj[ATTRID_CONNROLE] = self;
						int level;
						CoreObj rfco;
						GetRootOfDeriv(base, rfco, &level);
						ObjTreeReconnect(newcoreobj, crealist, self.FollowChain(ATTRID_FCOPARENT, level));			
				}
			}
		}
	} COMCATCH_IN_TRANSACTION(;);
}

