// MgaModel.cpp : Implementation of CMgaModel
#include "stdafx.h"
#include "MgaFCO.h"
#include "MgaFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CMgaModel

HRESULT FCO::CreateChildObject(IMgaMetaRole *metar, IMgaFCO **newobj)
{
	COMTRY_IN_TRANSACTION_MAYBE { // FIXME: with _MAYBE, e.g. GMEView doesn't get notification that the object was created
		CheckWrite();
		CHECK_INPTRPAR(metar);
		CHECK_OUTPTRPAR(newobj);
		if((long)self[ATTRID_PERMISSIONS] & ~EXEMPT_FLAG) COMTHROW(E_MGA_NOT_CHANGEABLE);
		CoreObj  nobj;
		CComPtr<IMgaMetaFCO> meta;
		metaref_type rr;
		COMTHROW(metar->get_MetaRef(&rr));
		COMTHROW(metar->get_Kind(&meta));

		COMTHROW(ContainerCreateFCO(meta, nobj));

		nobj[ATTRID_ROLEMETA]=rr;
		auto nfco = ObjForCore(nobj);
		nfco->initialname();

		setcheck(mgaproject, nobj, CHK_NEW);

		int targetlevel = 0;
		CoreObj rootp;

		GetRootOfDeriv(self, rootp, &targetlevel);
		if(targetlevel >= 0) ReDeriveNewObj(mgaproject, nobj, targetlevel+1);
		docheck(mgaproject);

		nfco->SelfMark(OBJEVENT_CREATED);
		SelfMark(OBJEVENT_NEWCHILD);
		nfco->getinterface(newobj);
	}
	COMCATCH_IN_TRANSACTION_MAYBE(;);	

}


HRESULT FCO::get_AspectParts(IMgaMetaAspect * asp, unsigned int filter, IMgaParts **pVal) {
			COMTRY {
				CHECK_OUTPTRPAR(pVal);
				CHECK_INPTRPAR(asp);

				CComPtr<IMgaMetaModel> pmm;
				COMTHROW(asp->get_ParentModel(&pmm));
				CComPtr<IMgaMetaFCO> mm;
				COMTHROW(get_Meta(&mm));
				if(!mm.IsEqualObject(pmm)) return E_META_INVALIDASPECT;


#ifdef OWN_META				
				CComPtr<IMgaMetaRoles> metars;	
				COMTHROW(asp->get_Roles(filter, &metars));
				std::set<metaref_type> mrefs;
				MGACOLL_ITERATE(IMgaMetaRole, metars) {
						metaref_type r;	            				
						COMTHROW(MGACOLL_ITER->get_MetaRef(&r));
						mrefs.insert(r);
				}
				MGACOLL_ITERATE_END;
#else
				CComPtr<IMgaMetaParts> metaps;	
				COMTHROW(asp->get_Parts(&metaps));
#pragma warning(push,3)
				std::set<metaref_type> mrefs;
#pragma warning(pop)
				MGACOLL_ITERATE(IMgaMetaPart, metaps) {
						metaref_type r;	            				
						CComPtr<IMgaMetaRole> rr;
						COMTHROW(MGACOLL_ITER->get_Role(&rr));
						COMTHROW(rr->get_MetaRef(&r));
						mrefs.insert(r);
				}
				MGACOLL_ITERATE_END;
#endif

				CComPtr<IMgaFCOs> children;					
				COMTHROW(get_ChildFCOs(&children));

				CREATECOLLECTION_FOR(IMgaPart,q);
				MGACOLL_ITERATE(IMgaFCO, children) {
						CComPtr<IMgaMetaRole> r;
						COMTHROW(MGACOLL_ITER->get_MetaRole(&r));
						metaref_type mref;           				
						COMTHROW(r->get_MetaRef(&mref));

					    if(mrefs.find(mref) != mrefs.end()) {
						  CComPtr<IMgaPart> pp;
						  COMTHROW(MGACOLL_ITER->get_Part(asp, &pp));
						  q->Add(pp);
						}
				}
				MGACOLL_ITERATE_END;
				*pVal = q.Detach();

			} COMCATCH(;);
}




HRESULT FCO::GetDescendantFCOs(struct IMgaFilter *filter,struct IMgaFCOs **fcos) {
	COMTRY {
		CHECK_OUTPTRPAR(fcos);
		CHECK_INPTRPAR(filter);  // checks project in the following lines
		CComPtr<IMgaProject> p;
		COMTHROW(filter->get_Project(&p));
		if(!COM_EQUAL(p, (IMgaProject *)mgaproject)) COMTHROW(E_MGA_FOREIGN_PROJECT);

		CREATEEXCOLLECTION_FOR(MgaFCO, q);
		reinterpret_cast<CMgaFilter *>(filter)->searchFCOs(self, q);
		*fcos = q.Detach();
    }
    COMCATCH(;);
}




HRESULT FCO::AddInternalConnections(struct IMgaFCOs *ins, struct IMgaFCOs **objs) {
  COMTRY {
	bool sameobj = false;
	CHECK_MYINPTRSPAR(ins);
	if(objs && *objs && COM_EQUAL(CComPtr<IMgaFCOs>(ins), *objs)) sameobj = true;
	else CHECK_OUTPTRPAR(objs);

	CREATEEXCOLLECTION_FOR(MgaFCO, q); 

	typedef std::set<FCO*> colltype;
	colltype coll;
	{
		MGACOLL_ITERATE(IMgaFCO, ins) {
			coll.insert(ObjFor(MGACOLL_ITER));
			q->Add(MGACOLL_ITER);
		}
		MGACOLL_ITERATE_END;
	}
	CComPtr<IMgaFCOs> childfcos;
	get_ChildFCOs(&childfcos);
	MGACOLL_ITERATE(IMgaFCO, childfcos) {
		objtype_enum t;
		COMTHROW(MGACOLL_ITER->get_ObjType(&t));
		if(t == OBJTYPE_CONNECTION) {
			bool skip =false;
			if(coll.find(ObjFor(MGACOLL_ITER)) != coll.end()) skip = true; 
			else {
				CComQIPtr<IMgaSimpleConnection> conn = MGACOLL_ITER;
				ASSERT(conn);
				{
					CComPtr<IMgaFCOs> ff;
					COMTHROW(conn->get_SrcReferences(&ff));
					long ll;
					COMTHROW(ff->get_Count(&ll));
					CComPtr<IMgaFCO> ntgt;
					if(ll == 0) { COMTHROW(conn->get_Src(&ntgt)); }
					else { COMTHROW(ff->get_Item(1, &ntgt)); }
					colltype::iterator colli, collb = coll.begin(), colle = coll.end();
					for(colli = collb; colli != colle; ++colli) {
						CComPtr<IMgaFCO> pp;
						(*colli)->getinterface(&pp);
						if(ObjFor(ntgt)->IsDescendantOf(pp)) {
							break;
						}
					}
					if(colli == colle) skip = true;
				}
				if(!skip) {
					CComPtr<IMgaFCOs> ff;
					COMTHROW(conn->get_DstReferences(&ff));
					long ll;
					COMTHROW(ff->get_Count(&ll));
					CComPtr<IMgaFCO> ntgt;
					if(ll == 0) { COMTHROW(conn->get_Dst(&ntgt)); }
					else { COMTHROW(ff->get_Item(1, &ntgt)); }
					colltype::iterator colli, collb = coll.begin(), colle = coll.end();
					for(colli = collb; colli != colle; ++colli) {
						CComPtr<IMgaFCO> pp;
						(*colli)->getinterface(&pp);
						if(ObjFor(ntgt)->IsDescendantOf(pp)) {
							break;
						}
					}
					if(colli == colle) skip = true;
				}
			}
			if(!skip) q->Add(MGACOLL_ITER);
		}
	}
	MGACOLL_ITERATE_END;
	if(!sameobj) *objs = q.Detach();
  }COMCATCH(;);
}


