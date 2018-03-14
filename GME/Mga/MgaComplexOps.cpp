// MgaComplexOps.cpp : Implementation of FCO Copy/Move/Derive/Delete operations
#include "stdafx.h"
#include "MgaFCO.h"
#include <map>
#include <unordered_set>
#include "MgaComplexOps.h"
#include "MgaSet.h"
#include "limits.h"
#define DETACHED_FROM "_detachedFrom"
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// DELETE //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


void SingleObjTreeDelete(CoreObj &self, bool deleteself) {
	CComPtr<ICoreAttributes> atts;
	COMTHROW(self->get_Attributes(&atts));
	MGACOLL_ITERATE(ICoreAttribute, atts) {
		attrid_type ai;
		CComPtr<ICoreMetaAttribute> mattr;
		COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
		COMTHROW(mattr->get_AttrID(&ai));
		if(ai >= ATTRID_COLLECTION) {
			ai -= ATTRID_COLLECTION;
			if(LINKREF_ATTR(ai) && ai != ATTRID_PARENT) {
				CoreObjs collmembers = self[ai + ATTRID_COLLECTION];
				ITERATE_THROUGH(collmembers) {
					SingleObjTreeDelete(ITER);
				}
			}
		}
	} MGACOLL_ITERATE_END;
	if(deleteself) COMTHROW(self->Delete());
}


void FCO::inDeleteObject() { 
			long status;
			COMTHROW(get_Status(&status));
			metaid_type typ = GetMetaID(self);
			if(status == OBJECT_DELETED)
				return;  // since collections contain objects, it may happen!!!
// Step 1: delete structure of this object, but not the object itself
			if(typ == DTID_CONNECTION) {
				ITERATE_THROUGH(self[ATTRID_CONNROLE+ATTRID_COLLECTION]) CoreObjMark(ITER[ATTRID_XREF], OBJEVENT_DISCONNECTED);
			}
			else if(typ == DTID_SET) {
				ITERATE_THROUGH(self[ATTRID_SETMEMBER+ATTRID_COLLECTION]) CoreObjMark(ITER[ATTRID_XREF], OBJEVENT_SETEXCLUDED);
			}
			else if(typ == DTID_REFERENCE) {
				CoreObj h = self[ATTRID_REFERENCE]; if(h) CoreObjMark(h, OBJEVENT_REFRELEASED);
			}

			if(typ != DTID_FOLDER) {
				CoreObjs children = self[ATTRID_DERIVED + ATTRID_COLLECTION];
				ITERATE_THROUGH(children) ObjForCore(ITER)->inDeleteObject();
			}

			SingleObjTreeDelete(self, false);

// Step 2: delete children, and derivated objects
			if(typ == DTID_MODEL || typ == DTID_FOLDER) {
				CoreObjs children = self[ATTRID_FCOPARENT + ATTRID_COLLECTION];
				ITERATE_THROUGH(children) ObjForCore(ITER)->inDeleteObject();
			}
// Step 3: mark or delete objects that are affected
			CoreObj parent;   
			if(typ != DTID_FOLDER) {
				parent = self[ATTRID_FCOPARENT];
// REFERENCES	set references to empty
				CoreObj const nil;  // Null COM pointer
				{
					CoreObjs referringobjs = self[ATTRID_REFERENCE + ATTRID_COLLECTION];
					ITERATE_THROUGH(referringobjs) {
						if(ITER.IsDeleted()) continue;   // object was deleted during a previous iteration of the same cycle
						int todo = MODEMASK(MM_REF, MM_INTO);
						if(todo != MM_ERROR && MODEFLAG(MM_REF,MM_FULLDELETE)) {
							ObjForCore(ITER)->inDeleteObject();
						}
						else 	{
							setcheck(mgaproject, ITER, todo == MM_ERROR ? CHK_ILLEGAL : CHK_CHANGED);
							ITER[ATTRID_REFERENCE] = nil;
							CoreObjMark(ITER, OBJEVENT_RELATION);
						}
					}
				}
// OTHER types of xrefs:
				CoreObjs xrefs = self[ATTRID_XREF + ATTRID_COLLECTION];
				ITERATE_THROUGH(xrefs) {
					if(ITER.IsDeleted()) continue; // object was deleted during a previous iteration of the same cycle
					switch(GetMetaID(ITER)) {
					case DTID_CONNROLE:
// Connection role
						{
						CoreObj fco = ITER.GetMgaObj();
						int todo = MODEMASK(MM_CONN, MM_INTO);
						if(todo != MM_ERROR && MODEFLAG(MM_CONN,MM_FULLDELETE)) {
							ObjForCore(fco)->inDeleteObject();
						}
						else 	{
							setcheck(mgaproject, fco, todo == MM_ERROR ? CHK_ILLEGAL : CHK_CHANGED);
							SingleObjTreeDelete(ITER);
							CoreObjMark(fco, OBJEVENT_RELATION);
						}
						break;
						}
					case DTID_SETNODE:
// Set
						{
						CoreObj fco = ITER.GetMgaObj();
						int todo = MODEMASK(MM_SET, MM_INTO);
						if(todo != MM_ERROR && MODEFLAG(MM_SET,MM_FULLDELETE)) {
							ObjForCore(fco)->inDeleteObject();
						}
						else 	{
							setcheck(mgaproject, fco, todo == MM_ERROR ? CHK_ILLEGAL : CHK_CHANGED);
							SingleObjTreeDelete(ITER);
							CoreObjMark(fco, OBJEVENT_RELATION);
						}
						break;
						}
					case DTID_REFATTR:
// REF attribute:  set it to NIL
						ITER[ATTRID_XREF] = nil;
						CoreObjMark(ITER[ATTRID_ATTRPARENT], OBJEVENT_ATTR);
						break;
					default:	
						COMTHROW(E_MGA_META_INCOMPATIBILITY);
					}
				}
				if(typ == DTID_REFERENCE) {
					CoreObjs segrefs = self[ATTRID_SEGREF + ATTRID_COLLECTION];
					ITERATE_THROUGH(segrefs) {
					    if(ITER.IsDeleted()) continue; // object was deleted during a previous iteration of the same cycle
						CoreObj fco = ITER.GetMgaObj();
						int todo = MODEMASK(MM_CONN, MM_INTO);
						if(todo != MM_ERROR && MODEFLAG(MM_CONN,MM_FULLDELETE)) {
							ObjForCore(fco)->inDeleteObject();
						}
						else 	{
							setcheck(mgaproject, fco, todo == MM_ERROR ? CHK_ILLEGAL : CHK_CHANGED);
							SingleObjTreeDelete(CoreObj(ITER[ATTRID_CONNSEG]));
							CoreObjMark(fco, OBJEVENT_RELATION);
						}
					}
				}
			} 
			else parent = self[ATTRID_FPARENT];

//Step4: delete the object itself
			COMTHROW(self->Delete());
			SelfMark(OBJEVENT_DESTROYED);
			CoreObjMark(parent, OBJEVENT_LOSTCHILD);
}

// PreDelete Notification by Tihamer for the PAMS SynchTool
void FCO::PreDeleteNotify()
{
/*
	
	long status;
	COMTHROW(get_Status(&status));
	metaid_type typ = GetMetaID(self);
	if(status == OBJECT_DELETED) return;  // since collections contain objects, it may happen!!!
*/
	metaid_type typ = GetMetaID(self);

	if(typ == DTID_MODEL || typ == DTID_FOLDER) 
	{
		CoreObjs children = self[ATTRID_FCOPARENT + ATTRID_COLLECTION];
		ITERATE_THROUGH(children) ObjForCore(ITER)->PreDeleteNotify();
	}

	// Notification
	PreNotify(OBJEVENT_PRE_DESTROYED, CComVariant());
}

// Added by lph (Taken from PreDeleteNotify) Notification service for precursory object events
HRESULT FCO::PreNotify(unsigned long changemask, CComVariant param) {
	COMTRY {
		CMgaProject::addoncoll::iterator ai, abeg = mgaproject->alladdons.begin(), aend = mgaproject->alladdons.end();
		if(abeg != aend) 
		{
			bool push_terr = mgaproject->activeterr != mgaproject->reserveterr; // this method can be reentrant
			if (push_terr)
				COMTHROW(mgaproject->pushterr(*mgaproject->reserveterr));
			for(ai = abeg; ai != aend; ) 
			{
				CComPtr<CMgaAddOn> t = *ai++;	
				unsigned long mmask;
				if((mmask = (t->eventmask & changemask)) != 0) {
					CComPtr<IMgaObject> tt;
					getinterface(&tt);

					if(t->handler->ObjectEvent(tt, mmask, param) != S_OK) {
						ASSERT(("Notification failed", false));
					}
				    t->notified = true;
				}
			}
			if (push_terr)
				COMTHROW(mgaproject->popterr());
		}
	} COMCATCH(;)
}

HRESULT FCO::DeleteObject() { 
		COMTRY_IN_TRANSACTION {
			CheckWrite();

			if(self[ATTRID_PERMISSIONS] & LIBRARY_FLAG) {
				SetErrorInfo(L"Object is in a library. Library objects cannot be deleted.");
				COMRETURN_IN_TRANSACTION(E_MGA_OP_REFUSED);
			}
			if(self[ATTRID_PERMISSIONS] & READONLY_FLAG)
			{
				SetErrorInfo(L"Object is read-only");
				COMRETURN_IN_TRANSACTION(E_MGA_OP_REFUSED);
			}
			// check for non-primary derived
			if(self[ATTRID_RELID] >= RELIDSPACE) {
				SetErrorInfo(L"Object is derived.");
				COMRETURN_IN_TRANSACTION(E_MGA_OP_REFUSED);
			}
			// check for rootfolder
			if(!CoreObj(self[ATTRID_PARENT]).IsContainer()) {  
				COMTHROW(E_MGA_OP_REFUSED);  
			}

			PreDeleteNotify();

			inDeleteObject();

			docheck(mgaproject);

		} COMCATCH_IN_TRANSACTION(;);
}


/////////////////////////////////////////////////////////////////////////////////
///////////////////////// COPY, MOVE, DERIVE ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// take second objects from list1, and put them to the first place into list2
void shiftlist(coreobjpairhash &list1, coreobjhash &list2) {
		coreobjpairhash::iterator ii, beg = list1.begin(), end = list1.end();
		for(ii = beg; ii != end; ++ii) list2.insert(coreobjhash::value_type((*ii).second, 0));
}


void ObjTreeCollect(CMgaProject *mgaproject, CoreObj &self, coreobjhash &crealist, int code ) {
	metaid_type s = s = GetMetaID(self);
	if(s >= DTID_MODEL && s <= DTID_SET) {
		crealist.insert(coreobjhash::value_type(self, 0));
		setcheck(mgaproject, self, code);
	}
	if(s == DTID_MODEL) {
			CoreObjs children = self[ATTRID_FCOPARENT + ATTRID_COLLECTION];
			ITERATE_THROUGH(children) {
				ObjTreeCollect(mgaproject, ITER, crealist, code);
			}
	}
}

// by ZolMol -invented by
void ObjTreeCollectFoldersToo(CMgaProject *mgaproject, CoreObj &self, coreobjhash &crealist, int code ) {
	metaid_type s = s = GetMetaID(self);
	if(s >= DTID_MODEL && s <= DTID_FOLDER) {
		crealist.insert(coreobjhash::value_type(self, 0));
		setcheck(mgaproject, self, code);
	}
	if(s == DTID_MODEL) {
			CoreObjs children = self[ATTRID_FCOPARENT + ATTRID_COLLECTION];
			ITERATE_THROUGH(children) {
				ObjTreeCollectFoldersToo(mgaproject, ITER, crealist, code);
			}
	}
	if(s == DTID_FOLDER) {
			CoreObjs children = self[ATTRID_FCOPARENT + ATTRID_COLLECTION];
			ITERATE_THROUGH(children) {
				ObjTreeCollectFoldersToo(mgaproject, ITER, crealist, code);
			}
	}
}

void ObjTreeCopy(CMgaProject *mgaproject, CoreObj self, CoreObj &nobj, coreobjpairhash &crealist)
{
	metaid_type s;
	COMTHROW(mgaproject->dataproject->CreateObject(s = GetMetaID(self), &nobj.ComPtr()));
	if( s>= DTID_MODEL && s <= DTID_FOLDER)
		assignGuid( mgaproject, nobj);
	if(s >= DTID_MODEL && s <= DTID_SET) {
		crealist.insert(coreobjpairhash::value_type(self, nobj));
		setcheck(mgaproject, nobj, CHK_NEW);
		CoreObjMark(nobj, OBJEVENT_CREATED);
	}

	CComPtr<ICoreAttributes> atts;
	COMTHROW(self->get_Attributes(&atts));
	MGACOLL_ITERATE(ICoreAttribute, atts) {
			attrid_type ai;
			CComPtr<ICoreMetaAttribute> mattr;
			COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
			COMTHROW(mattr->get_AttrID(&ai));
			if (ai == ATTRID_LOCK)
				continue;
			if(ai < ATTRID_COLLECTION) {
				// remove library flags from a copy
				// FIXME this looks wrong
				if(ai == ATTRID_PERMISSIONS) nobj[ai] = self[ai] & INSTANCE_FLAG;
				else if( ai == ATTRID_GUID1 // don't copy these
					|| ai == ATTRID_GUID2
					|| ai == ATTRID_GUID3 
					|| ai == ATTRID_GUID4) {}
				else nobj[ai] = static_cast<CComVariant>(self[ai]);
			}
			else {
				ai -= ATTRID_COLLECTION;
				if(LINKREF_ATTR(ai)) {
					CoreObjs collmembers = self[ai + ATTRID_COLLECTION];
					ITERATE_THROUGH(collmembers) {
						CoreObj nchild;
						ObjTreeCopy(mgaproject, ITER, nchild, crealist);
						nchild[ai] = nobj;
					}
				}
			}

	} MGACOLL_ITERATE_END;
}


void ObjTreeCopyFoldersToo(CMgaProject *mgaproject, CoreObj self, CoreObj &nobj, coreobjpairhash &crealist) {
	metaid_type s;
	COMTHROW(mgaproject->dataproject->CreateObject(s = GetMetaID(self), &nobj.ComPtr()));
	if(s >= DTID_MODEL && s <= DTID_FOLDER) {
		assignGuid( mgaproject, nobj);
		crealist.insert(coreobjpairhash::value_type(self, nobj));
		setcheck(mgaproject, nobj, CHK_NEW);
		CoreObjMark(nobj, OBJEVENT_CREATED);
	}

	CComPtr<ICoreAttributes> atts;
	COMTHROW(self->get_Attributes(&atts));
	MGACOLL_ITERATE(ICoreAttribute, atts) {
			attrid_type ai;
			CComPtr<ICoreMetaAttribute> mattr;
			COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
			COMTHROW(mattr->get_AttrID(&ai));
			if(ai < ATTRID_COLLECTION) {
				if (ai == ATTRID_LOCK)
					continue;
				// remove library flags from a copy
				// FIXME this looks wrong
				if(ai == ATTRID_PERMISSIONS) nobj[ai] = self[ai] & INSTANCE_FLAG;
				else if( ai == ATTRID_GUID1 // don't copy these
					|| ai == ATTRID_GUID2
					|| ai == ATTRID_GUID3 
					|| ai == ATTRID_GUID4) {}
				else nobj[ai] = static_cast<CComVariant>(self[ai]);
			}
			else {
				ai -= ATTRID_COLLECTION;
				if(LINKREF_ATTR(ai)) {
					CoreObjs collmembers = self[ai + ATTRID_COLLECTION];
					ITERATE_THROUGH(collmembers) {
						CoreObj nchild;
						ObjTreeCopyFoldersToo(mgaproject, ITER, nchild, crealist);
						nchild[ai] = nobj;
					}
				}
			}

	} MGACOLL_ITERATE_END;
}

// get the derived-chain distance of the closest real basetype (or if there is none, the length of the full base chain) 
// if the object is not derived, or it is a real subtype/instance (i.e derived not because of its parent) 0 is returned

int GetRealSubtypeDist(CoreObj oldobj) {
			CoreObj pp = oldobj;
			int derdist = 0;
			while(pp[ATTRID_RELID] >= RELIDSPACE) { 
				pp = pp[ATTRID_DERIVED];
				ASSERT(pp);
				derdist++;
			}
			return derdist;
}

void ObjTreeDist(CoreObj self, int derdist) {
	CComPtr<ICoreAttributes> atts;
	COMTHROW(self->get_Attributes(&atts));
	MGACOLL_ITERATE(ICoreAttribute, atts) {
			attrid_type ai;
			CComPtr<ICoreMetaAttribute> mattr;
			COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
			COMTHROW(mattr->get_AttrID(&ai));
			if(ai == ATTRID_DERIVED) {
				CoreObj h = self;
				for(int i = 0; i < derdist && (h = h[ATTRID_DERIVED]); i++) {
					MergeRegs(h,self);
					MergeAttrs(h,self);
				}
				CoreObj p;
				self[ai] = p = self.FollowChain(ai, derdist+1);
				// FIXME this looks wrong
				if(p) self[ATTRID_PERMISSIONS] = p[ATTRID_PERMISSIONS] & INSTANCE_FLAG;
				else self[ATTRID_PERMISSIONS] = 0;
			}
			else if(ai == ATTRID_MASTEROBJ) {
					self[ai] = self.FollowChain(ai, derdist+1);
			}
			if(ai == ATTRID_FCOPARENT + ATTRID_COLLECTION ||
			   ai == ATTRID_CONNROLE + ATTRID_COLLECTION ||
			   ai == ATTRID_SETMEMBER + ATTRID_COLLECTION) {
				CoreObjs collmembers = self[ai];
				ITERATE_THROUGH(collmembers) {
							ObjTreeDist(ITER, derdist);
				}
			}

	} MGACOLL_ITERATE_END;
}	


/////////////////
// ObjTreeDerive
/////////////////
// Create a derived deep copy of 'origobj' to 'newobj'
// -- 'newobj' tree is exact copy of 'origobj' tree, except 
//			ATTRID_INSTANCE: set if instance, copied from 'origobj' tree othervise
//			ATTRID_DERIVED & ATTRID_MASTEROBJ: point to the corresponding object in 'origobj' tree
//			ATTRID_GUID1..4, which are assigned new value
// -- on return the new root object 'newobj' will also be contained in the container of 'origobj'.
// -- new FCO-s are inserted in 'crealist'
///////////////////
// !!! RECURSIVE 
///////////////////
void ObjTreeDerive(CMgaProject *mgaproject, const CoreObj &origobj, CoreObj &newobj, coreobjpairhash &crealist, long instance) {
	metaid_type s;
	COMTHROW(mgaproject->dataproject->CreateObject(s = origobj.GetMetaID(), &newobj.ComPtr()));
	if( s >= DTID_MODEL && s <= DTID_FOLDER)
		assignGuid( mgaproject, newobj); // assigns new value to ATTRID_GUID1..4
	if(s >= DTID_MODEL && s <= DTID_SET) {
		crealist.insert(coreobjpairhash::value_type(origobj, newobj));
		setcheck(mgaproject, newobj, CHK_NEW);
		CoreObjMark(newobj, OBJEVENT_CREATED);
	}
	
	CComPtr<ICoreAttributes> atts;
	COMTHROW(origobj->get_Attributes(&atts));
	MGACOLL_ITERATE(ICoreAttribute, atts) {
			attrid_type ai;
			CComPtr<ICoreMetaAttribute> mattr;
			COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
			COMTHROW(mattr->get_AttrID(&ai));
			if(ai < ATTRID_COLLECTION) {
				switch(ai) {
				case ATTRID_DERIVED: 
				case ATTRID_MASTEROBJ:
					newobj[ai] = origobj;
					break;
				case ATTRID_PERMISSIONS:
						// overwrite copy only if instance
					// PETER'S FIX 
					// newobj[ai] = (origobj[ai] | long(instance)) & INSTANCE_FLAG;
					newobj[ai] = (origobj[ai] | long(instance)) & INSTANCE_FLAG;
					// PETER END
					break;
				case ATTRID_RELID: 
					{
					// if newobj[ai] + RELIDSPACE > LONG_MAX
					if (newobj[ai] > LONG_MAX - RELIDSPACE) 
						COMTHROW(E_MGA_LONG_DERIVCHAIN);
					long relid = origobj[ai]+RELIDSPACE;
					ASSERT(relid > 0);
					newobj[ai] = relid;
					}
					break;
				case ATTRID_LASTRELID: 
					newobj[ai] = 0L;
					break;
				case ATTRID_REGNODE:
				case ATTRID_GUID1: // don't copy these
				case ATTRID_GUID2:
				case ATTRID_GUID3:
				case ATTRID_GUID4:
				case ATTRID_LOCK: break;
				default:
					newobj[ai] = static_cast<CComVariant>(origobj[ai]);
				}
			}
			else {
				ai -= ATTRID_COLLECTION;
				if(LINKREF_ATTR(ai) && ai != ATTRID_ATTRPARENT) {
					CoreObjs collmembers = origobj[ai + ATTRID_COLLECTION];
					ITERATE_THROUGH(collmembers) {
						CoreObj nchild;
						ObjTreeDerive(mgaproject, ITER, nchild, crealist, instance);
						nchild[ai] = newobj;
					}
					if(ai == ATTRID_FCOPARENT && collmembers.Count() > 0) {
							CoreObjMark(newobj, OBJEVENT_NEWCHILD);
					}
				}
			}

	} MGACOLL_ITERATE_END;
}



/////////////////
// ObjTreeReconnect
/////////////////
// Reconnects relations in a newly copied/derived tree
// 
// -- 'crealist'contains old-new equivalent pairs for quick access
// -- if object is not in crealist, it may still be reconnected 
//		if it is an internal reference, (i.e., the ends of the pointer belong to the same rootFCO).
//	  -- if the relation has a masterobj, then the relation of the root masterobj is checked
//	  -- otherwise the target of the ralation must be contained in the crealist (list of newly created objects)
// -- return value used internally to indicate that the tree contained relations which were not reconnected
///////////////////
// !!! RECURSIVE 
///////////////////
bool ObjTreeReconnect(CoreObj self, coreobjpairhash &crealist, CoreObj const &derivtgt) {
	typedef struct { metaid_type mid; int search, reconnect; } table;
	static const table tab[] = {
		{DTID_MODEL,	ATTRID_FCOPARENT,	0},
		{DTID_REFERENCE, 0,					ATTRID_REFERENCE},
		{DTID_SET,		ATTRID_SETMEMBER,	0},
		{DTID_SETNODE,	0,					ATTRID_XREF },
		{DTID_CONNECTION, ATTRID_CONNROLE,	0},
		{DTID_CONNROLE, ATTRID_CONNSEG,		ATTRID_XREF},
		{DTID_CONNROLESEG, 0,				ATTRID_SEGREF}
	};


	bool containedexternal = false;
	metaid_type n = GetMetaID(self);

	for(int i = 0; i < sizeof(tab) / sizeof(tab[0]); i++) {
		if(tab[i].mid == n) {
			const table *ll = &tab[i];
			if(ll->search) {
				CoreObjs children = self[ll->search+ATTRID_COLLECTION];
				if(n == DTID_CONNROLE) {
					children.Sort();
				}
				ITERATE_THROUGH(children) {
					if(ObjTreeReconnect(ITER,crealist, derivtgt)) {
						containedexternal = true;
						if(n == DTID_CONNROLE) break; // stop processing rolesegments after an ext ref.
					}
				}
			}
			if(!containedexternal && ll->reconnect) {
			  // the relationship pointer will to be reconnected if
				// - the object has no master (only possible with copied objects)
				// - or the relation in the most original base is internal
			  if(!self.GetMaster() || IsInternalRelation(self)) {  
				    CoreObj oldr = self[ll->reconnect];
					coreobjpairhash::iterator i = crealist.find(oldr);
					if(i != crealist.end()) {  
// if it is a reference to a newly created object: use lookup
						self[ll->reconnect] = (*i).second;
					}
					else if(derivtgt) {  
						ASSERT(CoreObj(derivtgt[ATTRID_DERIVED]));
						ASSERT(derivtgt[ATTRID_RELID] < RELIDSPACE);
						CoreObj newr;
						GetDerivedEquivalent(oldr, derivtgt, newr);
						self[ll->reconnect] = newr;
					}
					else if( n == DTID_SETNODE) // if setnode not found while copying a set, 
					{   // delete the membership entry, thus allow a set to be copied without its members
						// previously Sets could not be copied&pasted only along with their members
						removemember().DoWithDeriveds(self);
					}
			  }
			  else containedexternal = true;
			}
			break;
		}
	}
	return containedexternal;
}

bool ObjTreeReconnectFoldersToo(CoreObj self, coreobjpairhash &crealist, CoreObj const &derivtgt) {
	typedef struct { metaid_type mid; int search, reconnect; } table;
	static const table tab[] = {
		{DTID_FOLDER,	ATTRID_FCOPARENT,	0},
		{DTID_MODEL,	ATTRID_FCOPARENT,	0},
		{DTID_REFERENCE, 0,					ATTRID_REFERENCE},
		{DTID_SET,		ATTRID_SETMEMBER,	0},
		{DTID_SETNODE,	0,					ATTRID_XREF },
		{DTID_CONNECTION, ATTRID_CONNROLE,	0},
		{DTID_CONNROLE, ATTRID_CONNSEG,		ATTRID_XREF},
		{DTID_CONNROLESEG, 0,				ATTRID_SEGREF}
	};


	bool containedexternal = false;
	metaid_type n = GetMetaID(self);

	for(int i = 0; i < sizeof(tab) / sizeof(tab[0]); i++) {
		if(tab[i].mid == n) {
			const table *ll = &tab[i];
			if(ll->search) {
				CoreObjs children = self[ll->search+ATTRID_COLLECTION];
				if(n == DTID_CONNROLE) {
					children.Sort();
				}
				ITERATE_THROUGH(children) {
					if (GetMetaID(ITER) == DTID_FOLDER) {
						if(ObjTreeReconnectFoldersToo(ITER, crealist, derivtgt)) {
							containedexternal = true;
						}
					} else {
						if(ObjTreeReconnect(ITER, crealist, derivtgt)) {
							containedexternal = true;
							if(n == DTID_CONNROLE) break; // stop processing rolesegments after an ext ref.
						}
					}
				}
			}
			if(!containedexternal && ll->reconnect) {
			  // the relationship pointer will to be reconnected if
				// - the object has no master (only possible with copied objects)
				// - or the relation in the most original base is internal
			  if(!self.GetMaster() || IsInternalRelation(self)) {  
				    CoreObj oldr = self[ll->reconnect];
					coreobjpairhash::iterator i = crealist.find(oldr);
					if(i != crealist.end()) {  
// if it is a reference to a newly created object: use lookup
						self[ll->reconnect] = (*i).second;
					}
					else if(derivtgt) {  
						ASSERT(CoreObj(derivtgt[ATTRID_DERIVED]));
						ASSERT(derivtgt[ATTRID_RELID] < RELIDSPACE);
						CoreObj newr;
						GetDerivedEquivalent(oldr, derivtgt, newr);
						self[ll->reconnect] = newr;
					}
			  }
			  else containedexternal = true;
			}
			break;
		}
	}
	return containedexternal;
}



/////////////////
// ObjTreeCheckRelations
/////////////////
// Removes outgoing/internal relations (or whole objects) in a newly copied/derived/moved tree
// The policy is based on the project-wide operationsmask queried with MM_OUTOF or MM_INTERNAL masks
// 
// -- 'self' is the root of the tree, 
// -- 'internals' are all the objects that are considered internal for this operation
// -- all relations hold by references, sets and connections are checked
// -- if the required operation is
//	- MM_ERROR: error is signaled
//  - MM_CLEAR: the corresponding pointer is cleared
//  - MM_CLEAR & MM_FULLDELETE: the object itself is cleared
//  - MM_DO:    nothing happens 
///////////////////
// !!! RECURSIVE 
///////////////////
void ObjTreeCheckRelations(CMgaProject *mgaproject, CoreObj &self, coreobjhash &internals) {
	typedef struct { metaid_type mid; int search, check, mm; } table;
	static const table tab[] = {
		{DTID_MODEL, ATTRID_FCOPARENT, 0},
		{DTID_REFERENCE, 0, ATTRID_REFERENCE, MM_REF},
		{DTID_SET, ATTRID_SETMEMBER, 0},
		{DTID_SETNODE, 0, ATTRID_XREF, MM_SET },
		{DTID_CONNECTION, ATTRID_CONNROLE, 0},
		{DTID_CONNROLE, ATTRID_CONNSEG, ATTRID_XREF, MM_CONN},
		{DTID_CONNROLESEG, 0, ATTRID_SEGREF}
	};

	metaid_type n = GetMetaID(self);

	for(int i = 0; i < sizeof(tab) / sizeof(tab[0]); i++) {
		if(tab[i].mid == n) {
			const table *ll = &tab[i];
			if(ll->search) {
				CoreObjs children = self[ll->search+ATTRID_COLLECTION];
				ITERATE_THROUGH(children) {
					if(ITER.IsDeleted()) continue;
					ObjTreeCheckRelations(mgaproject, ITER, internals);
				}
			}
			if(ll->check && !self.IsDeleted()) {
				CoreObj tgt = self[ll->check];
				int mm2 = (internals.find(tgt) != internals.end()) ? MM_INTERNAL : MM_OUTOF;
				switch(MODEMASK(ll->mm, mm2)) {
				case MM_ERROR: COMTHROW(E_MGA_OP_REFUSED);
				case MM_CLEAR: 
					self[ll->check] = NULLCOREOBJ;
					CoreObj selfobj = self.GetMgaObj();
					if( (GetMetaID(selfobj) == DTID_CONNECTION && ObjForCore(selfobj)->simpleconn()) ||
						MODEFLAG(ll->mm, MM_FULLDELETE)) {
							ObjForCore(selfobj)->inDeleteObject();
					}
					else {
						if(n != DTID_REFERENCE) {
							if(n == DTID_CONNROLESEG) self = self[ATTRID_CONNSEG];
							SingleObjTreeDelete(self);
						}
					}
				}
			}
			break;
		}
	}
}

void ObjTreeCheckRelationsFoldersToo(CMgaProject *mgaproject, CoreObj &self, coreobjhash &internals) {
	typedef struct { metaid_type mid; int search, check, mm; } table;
	static const table tab[] = {
		{DTID_FOLDER, ATTRID_FCOPARENT, 0},
		{DTID_MODEL, ATTRID_FCOPARENT, 0},
		{DTID_REFERENCE, 0, ATTRID_REFERENCE, MM_REF},
		{DTID_SET, ATTRID_SETMEMBER, 0},
		{DTID_SETNODE, 0, ATTRID_XREF, MM_SET },
		{DTID_CONNECTION, ATTRID_CONNROLE, 0},
		{DTID_CONNROLE, ATTRID_CONNSEG, ATTRID_XREF, MM_CONN},
		{DTID_CONNROLESEG, 0, ATTRID_SEGREF}
	};

	metaid_type n = GetMetaID(self);

	for(int i = 0; i < sizeof(tab) / sizeof(tab[0]); i++) {
		if(tab[i].mid == n) {
			const table *ll = &tab[i];
			if(ll->search) {
				CoreObjs children = self[ll->search+ATTRID_COLLECTION];
				ITERATE_THROUGH(children) {
					if(ITER.IsDeleted()) continue;
					ObjTreeCheckRelationsFoldersToo(mgaproject, ITER, internals);
				}
			}
			if(ll->check && !self.IsDeleted()) {
				CoreObj tgt = self[ll->check];
				int mm2 = (internals.find(tgt) != internals.end()) ? MM_INTERNAL : MM_OUTOF;
				switch(MODEMASK(ll->mm, mm2)) {
				case MM_ERROR: COMTHROW(E_MGA_OP_REFUSED);
				case MM_CLEAR: 
					self[ll->check] = NULLCOREOBJ;
					CoreObj selfobj = self.GetMgaObj();
					if( (GetMetaID(selfobj) == DTID_CONNECTION && ObjForCore(selfobj)->simpleconn()) ||
						MODEFLAG(ll->mm, MM_FULLDELETE)) {
							ObjForCore(selfobj)->inDeleteObject();
					}
					else {
						if(n != DTID_REFERENCE) {
							if(n == DTID_CONNROLESEG) self = self[ATTRID_CONNSEG];
							SingleObjTreeDelete(self);
						}
					}
				}
			}
			break;
		}
	}
}

/////////////////
// ObjTreeCheckINTORelations
/////////////////
// Removes incoming relations (or whole objects) that point into a newly copied/derived/moved tree
// The policy is based on the project-wide operationsmask queried with MM_INTO mask
// 
// -- 'self' is the root of the tree, 
// -- 'internals' are all the objects that are considered internal for this operation
// -- all relations that point to objects in the tree are checked.
// -- if the required operation is
//	- MM_ERROR: error is signaled
//  - MM_CLEAR: the corresponding pointer is cleared
//  - MM_CLEAR & MM_FULLDELETE: the object itself is cleared
//  - MM_DO:    nothing happens 
void ObjCheckINTORelations(CMgaProject* mgaproject, CoreObj& self, coreobjhash& internals) {
	metaid_type n = GetMetaID(self);
	ASSERT(n >= DTID_MODEL && n <= DTID_SET);
	if (n == DTID_REFERENCE) {
		// GME-311: need to delete connections into refport 'conn_seg' iff 
		//   connection 'rel_owner' is not in internals and 'conn_seg' is the actual connection end (not an intermediary)
		CoreObjs conn_segs = self[ATTRID_SEGREF + ATTRID_COLLECTION];
		ITERATE_THROUGH(conn_segs) {
			CoreObj conn_seg = ITER;
			metaid_type st = GetMetaID(conn_seg);
			ASSERT(st == DTID_CONNROLESEG);
			if (st != DTID_CONNROLESEG) {
				continue;
			}
			CoreObj rel_owner = conn_seg.GetMgaObj();
			if (!rel_owner) {
				continue;	// connection might be deleted due to a previous relation
			}
			ASSERT(GetMetaID(rel_owner) == DTID_CONNECTION);
			#ifdef _DEBUG
			CoreObj role = conn_seg[ATTRID_CONNSEG];
			CComBSTR conn_name = rel_owner[ATTRID_NAME], role_name = role[ATTRID_NAME];
			#endif
			ASSERT(ObjForCore(rel_owner)->simpleconn()); // KMS: don't think we can get here without a simpleconn
			if (internals.find(rel_owner) == internals.end() && ObjForCore(rel_owner)->simpleconn()) {
				setcheck(mgaproject, rel_owner, CHK_CHANGED);
				switch(MODEMASK(MM_CONN, MM_INTO)) {
				case MM_ERROR: COMTHROW(E_MGA_OP_REFUSED);
					break;
				case MM_CLEAR:
					if (conn_seg[ATTRID_SEGORDNUM] == 1) {
						ObjForCore(rel_owner)->inDeleteObject();
						break;
					}
				}
			}
		}
	}

	CoreObjs xrefs = self[ATTRID_XREF + ATTRID_COLLECTION]; 
	ITERATE_THROUGH(xrefs) {
		metaid_type st = GetMetaID(ITER);
		if(st != DTID_SETNODE && st != DTID_CONNROLE) {
			continue;
		}
		CoreObj rel_owner = ITER.GetMgaObj();
		if (!rel_owner) {
			continue;	// connection/set might be deleted due to a previous relation
		}

		if(internals.find(rel_owner) == internals.end()) {
			int ttt = st == DTID_CONNROLE ? MM_CONN : MM_SET;
			setcheck(mgaproject, rel_owner, CHK_CHANGED);
			switch(MODEMASK(ttt, MM_INTO)) {
			case MM_ERROR: COMTHROW(E_MGA_OP_REFUSED);
				break;
			case MM_CLEAR: 
				if (st == DTID_CONNROLE && ObjForCore(rel_owner)->simpleconn()) {
					// GME-297: don't delete connections connecting to refports
					// (outside connections to inside refports are deleted above)
					long count = 0;
					CoreObjs refport_refs = ITER[ATTRID_CONNSEG+ATTRID_COLLECTION]; // i.e. refport containers
					COMTHROW(refport_refs->get_Count(&count));
					if (count == 0) {
						// this connection role is connected directly; it is not connected to a refport
						ObjForCore(rel_owner)->inDeleteObject();
					}
				} else if (MODEFLAG(ttt, MM_FULLDELETE)) {
					ObjForCore(rel_owner)->inDeleteObject();
				} 
				else {
					CoreObjMark(self, st == DTID_CONNROLE ? OBJEVENT_DISCONNECTED : OBJEVENT_SETEXCLUDED);
					CoreObjMark(rel_owner, OBJEVENT_RELATION);
					SingleObjTreeDelete(ITER);
				}
				break;
			}
		}	
	}
	{
	CoreObjs refs = self[ATTRID_REFERENCE + ATTRID_COLLECTION]; 
	ITERATE_THROUGH(refs) {
		CoreObj rel_owner = ITER;
		if(internals.find(rel_owner) == internals.end()) {
			setcheck(mgaproject, rel_owner, CHK_CHANGED);
			switch(MODEMASK(MM_REF, MM_INTO)) {
			case MM_ERROR: COMTHROW(E_MGA_OP_REFUSED);
			case MM_CLEAR: 
				if(MODEFLAG(MM_REF, MM_FULLDELETE)) {
					ObjForCore(rel_owner)->inDeleteObject();
				}
				else { 
					rel_owner[ATTRID_REFERENCE] = NULLCOREOBJ;
					CoreObjMark(self, OBJEVENT_REFRELEASED);
					CoreObjMark(rel_owner, OBJEVENT_RELATION);
				}
			}
		}
	}
	}
}


///////////////////
// !!! RECURSIVE 
///////////////////
void ObjTreeCheckINTORelations(CMgaProject *mgaproject, CoreObj &self, coreobjhash &internals) {
	metaid_type n = GetMetaID(self);
	ASSERT(n >= DTID_MODEL && n <= DTID_SET);
	ObjCheckINTORelations(mgaproject, self, internals);
	
	if(n == DTID_MODEL) {
		CoreObjs children = self[ATTRID_FCOPARENT + ATTRID_COLLECTION];
		ITERATE_THROUGH(children) {
			ObjTreeCheckINTORelations(mgaproject, ITER, internals);
		}
	}
}


///////////////////
// !!! RECURSIVE 
///////////////////
void ObjTreeCheckINTORelationsFoldersToo(CMgaProject *mgaproject, CoreObj &self, coreobjhash &internals) {
	metaid_type n = GetMetaID(self);
	ASSERT(n >= DTID_MODEL && n <= DTID_FOLDER);
	if ( n >= DTID_MODEL && n <= DTID_SET)
	{
		ObjCheckINTORelations(mgaproject, self, internals);
	}
	if(n == DTID_MODEL || n == DTID_FOLDER) {
		CoreObjs children = self[ATTRID_FCOPARENT + ATTRID_COLLECTION];
		ITERATE_THROUGH(children) {
			ObjTreeCheckINTORelationsFoldersToo(mgaproject, ITER, internals);
		}
	}
}

void ObjTreeNotify(CMgaProject *mgaproject, CoreObj &self) {
	int k = OBJEVENT_CREATED;
	metaid_type typ = GetMetaID(self);
	if(typ == DTID_FOLDER || typ == DTID_MODEL) {
			CoreObjs children = self[ATTRID_FCOPARENT + ATTRID_COLLECTION];
			ITERATE_THROUGH(children) {
				k |= OBJEVENT_NEWCHILD;
				ObjTreeNotify(mgaproject, ITER);
			}
	}
	else if(typ == DTID_REFERENCE) {
			CoreObj rr = self[ATTRID_REFERENCE];
			if(rr) { 
				CComPtr<IMgaFCO> fco;
				ObjForCore(rr)->getinterface(&fco);
				mgaproject->ObjMark(fco, OBJEVENT_REFERENCED);
			}
	}
	else if(typ == DTID_CONNECTION || typ == DTID_SET) {
			
			CoreObjs rrs;
			long msg;
			if(typ == DTID_SET) {
				rrs = self[ATTRID_SETMEMBER+ATTRID_COLLECTION];
				msg = OBJEVENT_SETINCLUDED;
			}
			else {
				rrs = self[ATTRID_CONNROLE+ATTRID_COLLECTION];
				msg = OBJEVENT_CONNECTED;
			}
			ITERATE_THROUGH(rrs) { 
				CoreObj fff = ITER[ATTRID_XREF];
				if(fff) {
					CComPtr<IMgaFCO> fco;
					ObjForCore(fff)->getinterface(&fco);
					mgaproject->ObjMark(fco, msg);
				}
			}
	}
	ObjForCore(self)->SelfMark(k);
}

// check of obj or any of its children is derived from root
void CheckConflict(CoreObj &b, CoreObj &root) {

	if(GetMetaID(b) == DTID_MODEL) {
		CoreObj base = b[ATTRID_DERIVED];
		while(base) {
			if(COM_EQUAL(base, root)) COMTHROW(E_MGA_OP_REFUSED);
			base = base[ATTRID_DERIVED];
		}
		CoreObjs children = b[ATTRID_FCOPARENT + ATTRID_COLLECTION];
		ITERATE_THROUGH(children) {
			CheckConflict(ITER, root);
		}
	}		
}


/////////////////
// DeriveNewObjs
/////////////////
///////////////////
// !!! RECURSIVE 
///////////////////
void ReDeriveNewObjs(CMgaProject *mgaproject, std::vector<CoreObj> &orignobjs, int cnt, int targetlevel) {
	ASSERT(targetlevel >= 1);
	if(cnt < 1) return;
	CoreObj orig = orignobjs[0][ATTRID_PARENT];
	if(!orig.IsFCO()) return;
	CoreObjs deriveds = orig[ATTRID_DERIVED+ATTRID_COLLECTION];
	ITERATE_THROUGH(deriveds) {
		int i;
		// we look for the object (subtypebase) which has been derived primarily (a parent of ITER most probably)
		// subtypebase will be used when internal references need to be redirected [ObjTreeReconnect call]
		// an internal reference is:
		//  - part of a model which is derived
		//  - its target is also part of that model
		// when such a reference is found in a derived model it is redirected to 
		// the target's derived counterpart. more details: GetDerivedEquivalent() [in MgaDeriveOps.cpp]
		// WAS: CoreObj subtypebase = ITER.FollowChain(ATTRID_PARENT, targetlevel); 
		CoreObj subtypebase = ITER.FollowChain(ATTRID_PARENT, targetlevel-1);
		coreobjpairhash derivcrealist;
		coreobjhash derivmovedlist;
		long instance = ITER[ATTRID_PERMISSIONS] & INSTANCE_FLAG;
		std::vector<CoreObj> newderiveds(cnt);
		for(i = 0; i< cnt; i++) {
			ObjTreeDerive(mgaproject, orignobjs[i], newderiveds[i], derivcrealist, instance);
			newderiveds[i][ATTRID_FCOPARENT]=ITER;
		}
		if(cnt) CoreObjMark(ITER, OBJEVENT_NEWCHILD);
// Reroute references
		for(i = 0; i< cnt; i++) {
			ObjTreeReconnect(newderiveds[i], derivcrealist, subtypebase);
		}
		shiftlist(derivcrealist, derivmovedlist);
		for(i = 0; i< cnt; i++) {
			ObjTreeCheckRelations(mgaproject, newderiveds[i], derivmovedlist);			
		}
		ReDeriveNewObjs(mgaproject, newderiveds, cnt, targetlevel);
	}
}


void ReDeriveNewObj(CMgaProject *mgaproject, CoreObj &orignobj, int targetlevel) {
   std::vector<CoreObj> vv(1);
   vv[0] = orignobj;
   ReDeriveNewObjs(mgaproject, vv, 1, targetlevel);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// COPY //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


HRESULT FCO::CopyFCOs(IMgaFCOs *copylist, IMgaMetaRoles *rlist,IMgaFCOs **objs) {
	COMTRY_IN_TRANSACTION_MAYBE {
		CheckWrite();
		CHECK_MYINPTRSPAR(copylist);
		long cnt;
		COMTHROW(copylist->get_Count(&cnt));
		if(rlist) {
			long rlcnt;
			COMTHROW(rlist->get_Count(&rlcnt));
			if(rlcnt != cnt) COMTHROW(E_MGA_BAD_COLLENGTH);
		}
		metaid_type targettype = GetMetaID(self);
		int targetlevel = 0;
		CoreObj rootp;
// Pre check:
		if(targettype == DTID_MODEL) {
			if(self[ATTRID_PERMISSIONS] & ~EXEMPT_FLAG) COMTHROW(E_MGA_NOT_CHANGEABLE);
			GetRootOfDeriv(self, rootp, &targetlevel);
			if(rootp) {
				MGACOLL_ITERATE(IMgaFCO, copylist) {
					CheckConflict(CoreObj(MGACOLL_ITER), rootp);
				} MGACOLL_ITERATE_END;
			}
		}

// Copy trees
		coreobjpairhash crealist;
		int i = 0;

		std::vector<CoreObj> nobjs(cnt);
		MGACOLL_ITERATE(IMgaFCO, copylist) {
			long relId;
			COMTHROW(MGACOLL_ITER->get_RelID(&relId)); // GME-335:essentially CheckRead
			CoreObj oldobj = CoreObj(MGACOLL_ITER);
			ObjForCore(oldobj)->SelfMark(OBJEVENT_COPIED);
			int derdist = GetRealSubtypeDist(oldobj);
			ObjTreeCopy(mgaproject, oldobj, nobjs[i], crealist);  // copy
			if(derdist) ObjTreeDist(nobjs[i], derdist);
			// assigns a new relid to nobjs[i]
			assignnewchild(nobjs[i]);
			// derdist is >0 for previously secondary derived objects so if such an object
			// was copied (and detached above by ObjTreeDist) then some of its children
			// have become archetypes now, but their relid still reflects the secondary 
			// derived status
			//
			// we must assign a new relid for the children of nobjs[i]
			if(derdist) newrelidforchildren(nobjs[i]);
			metaref_type trole = METAREF_NULL;
			if(targettype != DTID_FOLDER) {
				ASSERT(targettype == DTID_MODEL);
				CComPtr<IMgaMetaRole> r;
				if(rlist) COMTHROW(rlist->get_Item(i+1, &r));
				CComPtr<IMgaMetaFCO> mf; 
				COMTHROW(get_Meta(&mf));
				if(!r) {   // NO metarole given, inherit that of original object
					CComQIPtr<IMgaMetaModel> parentmeta = mf;
					if(!parentmeta) COMTHROW(E_MGA_META_INCOMPATIBILITY);
					metaref_type t;
					t = (nobjs[i])[ATTRID_ROLEMETA];
					if(!t) COMTHROW(E_MGA_NO_ROLE);
					CComQIPtr<IMgaMetaRole> metar = mgaproject->FindMetaRef(t);
					if(!metar) COMTHROW(E_MGA_META_INCOMPATIBILITY);
					CComBSTR rolename;
					COMTHROW(metar->get_Name(&rolename));
					COMTHROW(parentmeta->get_RoleByName(rolename, &r));
					if(!r) COMTHROW(E_MGA_NO_ROLE);
				}
				{
					metaref_type kt;
					CComPtr<IMgaMetaFCO> mfco;
					COMTHROW(r->get_Kind(&mfco));
					COMTHROW(mfco->get_MetaRef(&kt));
					if(kt != (nobjs[i])[ATTRID_META])
						COMTHROW(E_MGA_NO_ROLE);
					CComPtr<IMgaMetaModel> mmodel;
					COMTHROW(r->get_ParentModel(&mmodel));
					if (!mf.IsEqualObject(mmodel))
						COMTHROW(E_MGA_INVALID_ROLE);
				}

				COMTHROW(r->get_MetaRef(&trole));
				if(trole == METAREF_NULL)  COMTHROW(E_MGA_INVALID_ROLE);
			}
			(nobjs[i])[ATTRID_ROLEMETA] = trole;
			i++;
		} MGACOLL_ITERATE_END;

// Reroute references
		for(i = 0; i< cnt; i++) {
			ObjTreeReconnect(nobjs[i], crealist);			
		}

		coreobjhash newcrealist;
		shiftlist(crealist, newcrealist);

		for(i = 0; i< cnt; i++) {
			ObjTreeCheckRelations(mgaproject, nobjs[i], newcrealist);			
		}

		if(targetlevel >= 0) ReDeriveNewObjs(mgaproject, nobjs, cnt, targetlevel+1);

		docheck(mgaproject);

// Assemble return array:
		if(objs) {
			CREATEEXCOLLECTION_FOR(MgaFCO, q);
			for(i = 0; i< cnt; i++) {
				CComPtr<IMgaFCO> ff;
				ObjForCore(nobjs[i])->getinterface(&ff);
				q->Add(ff); 
			}
			*objs = q.Detach();
		}



		SelfMark(OBJEVENT_NEWCHILD);

	} COMCATCH_IN_TRANSACTION_MAYBE(;);
}

HRESULT FCO::CopyFCODisp(IMgaFCO *copiedobj, IMgaMetaRole *role, IMgaFCO **nobj)
{
	COMTRY_IN_TRANSACTION_MAYBE {
		CComPtr<IMgaFCO> new_fco;

		// copy in param to a folder coll
		CREATEEXCOLLECTION_FOR(MgaFCO, q);
		q->Add( CComPtr<IMgaFCO>( copiedobj));

		CComPtr<IMgaMetaRoles> roles;
		if (role)
		{
			COMTHROW(roles.CoCreateInstance(L"Mga.MgaMetaRoles", NULL, CLSCTX_INPROC));
			COMTHROW(roles->Append(CComPtr<IMgaMetaRole>(role)));
		}

		CComPtr<IMgaFCOs> newfcos;
		COMTHROW(CopyFCOs( q, roles, &newfcos));
		
		// extract ret value from returned coll
		long cnt = 0; 
		if( newfcos)  COMTHROW( newfcos->get_Count( &cnt));
		if( cnt == 1) COMTHROW( newfcos->get_Item( 1, &new_fco));
		
		if( nobj) {
			*nobj = new_fco.Detach();
		}
	} COMCATCH_IN_TRANSACTION_MAYBE(;);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// MOVE //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


// give new value to subobj, and also give the same value to all its slave subobjs
void RedirectMasterTree(CoreObj &subobj, attrid_type ai, CoreObj &newval) {
	subobj[ai] = newval;
	CoreObjs slso;
	slso = subobj[ATTRID_MASTEROBJ+ATTRID_COLLECTION];
	ITERATE_THROUGH(slso) {
		RedirectMasterTree(ITER, ai, newval);
	}
}

// An object with tree has been moved into a type, fixup relations that used to point from inside
// to the object just moved, i.e. which have just became 'internal'
// self = the target object that may have too many pointers pointing at itself
// selfbase = the base of the subtype/instance (self is contained by that)
// level = the distance between self and selfbase

void ObjTreeInternalize(CMgaProject *mgaproject, CoreObj &self, CoreObj &selfbase, int tlevel) {
	CoreObj origbase = selfbase[ATTRID_DERIVED];
	CoreObj orig = self[ATTRID_DERIVED];
	static attrid_type ais[] = {ATTRID_SEGREF, ATTRID_REFERENCE, ATTRID_XREF };
// do loop for ATTRID_REFERENCE, ATTRID_XREF, and also for ATTRID_SEGREF in case of ref objs
	for(int i = (self.GetMetaID() == DTID_REFERENCE) ? 0 : 1; i < DIM(ais); i++) {
		attrid_type ai = ais[i];
		CoreObjs rels = orig[ai + ATTRID_COLLECTION];
		ITERATE_THROUGH(rels) {
			metaid_type typ = ITER.GetMetaID();
// 1. The object must be CONNROLE or SETNODE
			if(typ != DTID_CONNROLE && typ != DTID_SETNODE) continue;
// 2. The object must not have a master pointing to the same object
			CoreObj obmaster = ITER[ATTRID_MASTEROBJ];
			if(obmaster && COM_EQUAL(orig, CoreObj(obmaster[ai]))) continue;
			CoreObj srcobj = ITER.GetMgaObj();
			int srclevel;
// 3. The relation object must be part of the same type (i.e common derivation parent)
			if(!IsContained(srcobj, origbase, &srclevel)) continue;	 // it is an external reference

			CoreObjs slso = ITER[ATTRID_MASTEROBJ+ATTRID_COLLECTION];
	
			ITERATE_THROUGH(slso) {
				CoreObj derivedsrc = ITER.GetMgaObj();
				if(COM_EQUAL(selfbase, derivedsrc.FollowChain(ATTRID_FCOPARENT, srclevel))) {
					ASSERT(COM_EQUAL(orig, CoreObj(ITER[ai])));
					RedirectMasterTree(ITER,ai,self);
					setcheck(mgaproject, derivedsrc, CHK_CHANGED);
					break;
				}
			}
		}
	}
// descend to children, increase level
	if(self.GetMetaID() == DTID_MODEL) {
		CoreObjs children = self[ATTRID_FCOPARENT + ATTRID_COLLECTION];
		ITERATE_THROUGH(children) {
			ObjTreeInternalize(mgaproject, ITER, selfbase, tlevel+1);
		}
	}
}




/////////////////
// DeriveMoveds
/////////////////
//   We have set of objects ('orignobjs') moved into a single model in a (base)type. 
//   Reflect changes in its deriveds. 
//   -- 'extmoved' objects must be 
//		- newly derived,  
//		- 'internalized'
//		- reconnected
//   -- others must be located and moved here
//   They all need to be checked.
//   
///////////////////
// !!! RECURSIVE 
///////////////////
void DeriveMoveds(CMgaProject *mgaproject, std::vector<CoreObj> &orignobjs, std::vector<int> &extmoved, int cnt, int targetlevel)
{
	if(cnt < 1) return;

	CoreObj orig = orignobjs[0][ATTRID_FCOPARENT];
	metaid_type n = GetMetaID( orig);

	if (n >= DTID_MODEL && n <= DTID_SET)
	{
		CoreObjs deriveds = orig[ATTRID_DERIVED+ATTRID_COLLECTION];
		ITERATE_THROUGH(deriveds) {
			int i;
			CoreObj subtypebase = ITER.FollowChain(ATTRID_FCOPARENT, targetlevel);
			coreobjpairhash derivcrealist;
			coreobjhash derivmovedlist;
			long instance = ITER[ATTRID_PERMISSIONS] & INSTANCE_FLAG;
			std::vector<CoreObj> newderiveds(cnt);
			for(i = 0; i< cnt; i++) {
				if(extmoved[i] < 0) {	// the movedobject is a newcomer in the basetype, derive new obj in subtype
					ObjTreeDerive(mgaproject, orignobjs[i], newderiveds[i], derivcrealist, instance);
					newderiveds[i][ATTRID_FCOPARENT]=ITER;
					ObjTreeInternalize(mgaproject, newderiveds[i], subtypebase, targetlevel+1);
				}
				else {
					CoreObj tgt(ITER);	// locate the corresponding object in subtype and move it
					CoreObjs ders = orignobjs[i][ATTRID_DERIVED+ATTRID_COLLECTION];
					ITERATE_THROUGH(ders) {
						CoreObj der(ITER);
						if(COM_EQUAL(subtypebase, ITER.FollowChain(ATTRID_FCOPARENT, extmoved[i]))) {
								newderiveds[i] = ITER;
								CoreObjMark(ITER[ATTRID_FCOPARENT], OBJEVENT_LOSTCHILD);
								ITER[ATTRID_FCOPARENT] = tgt;
								ITER[ATTRID_ROLEMETA] = orignobjs[i][ATTRID_ROLEMETA];
								ObjTreeCollect(mgaproject, ITER, derivmovedlist, CHK_MOVED);
								break;
						}
					}
					ASSERT(ITER_BROKEN);
				}
			}
			CoreObjMark(ITER, OBJEVENT_NEWCHILD);
	// Reroute references
			for(i = 0; i< cnt; i++) {
				ObjTreeReconnect(newderiveds[i], derivcrealist, subtypebase);
			}
			shiftlist(derivcrealist, derivmovedlist);
			for(i = 0; i< cnt; i++) {
				ObjTreeCheckRelations(mgaproject, newderiveds[i], derivmovedlist);			
			}
			DeriveMoveds(mgaproject, newderiveds, extmoved, cnt, targetlevel);

			for(i = 0; i< cnt; i++) {
				ObjTreeCheckINTORelations(mgaproject, newderiveds[i], derivmovedlist);			
			}
		}
	}
}


HRESULT FCO::MoveFCOs(IMgaFCOs *movelist, IMgaMetaRoles *rlist,IMgaFCOs **objs) {
	COMTRY_IN_TRANSACTION_MAYBE {
		CheckWrite();
		CHECK_MYINPTRSPAR(movelist);
		long cnt;
		COMTHROW(movelist->get_Count(&cnt));

		bool valid = ( cnt > 0);
		if ( cnt == 1)
		{
			valid = false;
			// check whether the source folder is the target as well
			CComPtr<IMgaFCO> mf;
			MGACOLL_ITERATE( IMgaFCO, movelist) {
				mf = MGACOLL_ITER;
			} MGACOLL_ITERATE_END;

			VARIANT_BOOL is_equal;
			CComPtr<IMgaFCO> thisptr;
			getinterface( &thisptr);
			COMTHROW( mf->get_IsEqual( thisptr, &is_equal));

			if (is_equal == VARIANT_FALSE) // not equal
				//COMTHROW( E_MGA_INVALID_ARG); // do not copy/move onto itself
				valid = true;
		}

		if ( valid) 
		{
			if(rlist) {
				long rlcnt;
				COMTHROW(rlist->get_Count(&rlcnt));
				if(rlcnt != cnt) COMTHROW(E_MGA_BAD_COLLENGTH);
			}
			metaid_type targettype = GetMetaID(self);
			int targetlevel = 0;
			CoreObj rootp;
// Pre check:
			if(targettype == DTID_MODEL) {
				if(self[ATTRID_PERMISSIONS]  & ~EXEMPT_FLAG) COMTHROW(E_MGA_NOT_CHANGEABLE);
				GetRootOfDeriv(self, rootp, &targetlevel);
				MGACOLL_ITERATE(IMgaFCO, movelist) {
					CoreObj cur = CoreObj(MGACOLL_ITER);
					if(IsContained(self, cur)) COMTHROW(E_MGA_OP_REFUSED);
					if(cur[ATTRID_RELID] >= RELIDSPACE) COMTHROW(E_MGA_OP_REFUSED); 
					if(rootp) {
						CheckConflict(cur, rootp);
					}
				} MGACOLL_ITERATE_END;
			}

// move trees
			coreobjhash moveslist;
			int i = 0;

			std::vector<CoreObj> nobjs(cnt);
			std::vector<int> moved_into(cnt);  // >= 0 for objects moved inside the tree, -1 for newcomers
			MGACOLL_ITERATE(IMgaFCO, movelist) {
				CoreObj cur = nobjs[i] = CoreObj(MGACOLL_ITER);
				CoreObj movedobjrootp;
				int movedobjlevel;
				GetRootOfDeriv(cur, movedobjrootp, &movedobjlevel);
				int derdist = GetRealSubtypeDist(cur);
				if(derdist) COMTHROW(E_MGA_OP_REFUSED);
	//			if(derdist)	ObjTreeDist(cur, derdist);  // move

				CoreObj curp = cur[ATTRID_FCOPARENT];
				if(!COM_EQUAL(curp, self)) {
					ObjForCore(curp)->CheckWrite(); // parent of cur will change, so check for WRITE permission
					CoreObjMark(curp, OBJEVENT_LOSTCHILD);
					assignnewchild(cur);
					CoreObjMark(cur, OBJEVENT_PARENT);
				}
				ObjTreeCollect(mgaproject, cur, moveslist, CHK_MOVED);
				metaref_type trole = METAREF_NULL;
				if(targettype != DTID_FOLDER) {
					ASSERT(targettype == DTID_MODEL);
					CComPtr<IMgaMetaRole> r;
					if(rlist) COMTHROW(rlist->get_Item(i+1, &r));
					if(!r) {   // NO metaname given, inherit that of original object
						CComPtr<IMgaMetaFCO> mf; 
						COMTHROW(get_Meta(&mf));
						CComQIPtr<IMgaMetaModel> parentmeta = mf;
						if(!parentmeta) COMTHROW(E_MGA_META_INCOMPATIBILITY);
						cur[ATTRID_ROLEMETA]; // FIXME what is this for
						metaref_type t;
						t = (nobjs[i])[ATTRID_ROLEMETA];
						if(!t) COMTHROW(E_MGA_NO_ROLE);
						CComQIPtr<IMgaMetaRole> metar = mgaproject->FindMetaRef(t);
						if(!metar) COMTHROW(E_MGA_META_INCOMPATIBILITY);
						CComBSTR rolename;
						COMTHROW(metar->get_Name(&rolename));
						COMTHROW(parentmeta->get_RoleByName(rolename, &r));
						if(!r) COMTHROW(E_MGA_NO_ROLE);
					}
					{
						metaref_type kt;
						CComPtr<IMgaMetaFCO> mfco;
						COMTHROW(r->get_Kind(&mfco));
						COMTHROW(mfco->get_MetaRef(&kt));
						if(kt != cur[ATTRID_META]) COMTHROW(E_MGA_META_INCOMPATIBILITY);
					}

					COMTHROW(r->get_MetaRef(&trole));
					if(trole == METAREF_NULL)  COMTHROW(E_MGA_INVALID_ROLE);
				}
				cur[ATTRID_ROLEMETA] = trole;

				// Determine what to do with subtypes of the objects moved 
				CoreObjs ders = cur[ATTRID_DERIVED+ATTRID_COLLECTION];
				if(!rootp || !COM_EQUAL(movedobjrootp, rootp)) {     // ********* moving out of a type
					moved_into[i] = -1;				   // ******** true if moving into a type
					if(movedobjlevel < 1) {	// not derived, or derived as self
											// if the object to be moved is a root of derivs,
											// and not moved to a model, thst model must not have any subtypes/instances
						if(ders.Count() &&
							targettype == DTID_MODEL && 
							CoreObjs(self[ATTRID_DERIVED+ATTRID_COLLECTION]).Count()) {
								COMTHROW(E_MGA_OP_REFUSED);  
						}
					}
					else {
						ITERATE_THROUGH(ders) {				   // ******* erase all previous subtypes
							ObjForCore(ITER)->inDeleteObject();
						}
					}
				}
				else moved_into[i] = movedobjlevel;        // ***** the object remains under the same rootmodel
				i++;
			} MGACOLL_ITERATE_END;

			for(i = 0; i< cnt; i++) {
				ObjTreeCheckRelations(mgaproject, nobjs[i], moveslist);			
			}

			if(targettype == DTID_MODEL) {
				DeriveMoveds(mgaproject, nobjs, moved_into, cnt, targetlevel);
			}

// Reroute references
			for(i = 0; i< cnt; i++) {
				ObjTreeCheckINTORelations(mgaproject, nobjs[i], moveslist);			
			}


			docheck(mgaproject);

// Assemble return array:
			CREATEEXCOLLECTION_FOR(MgaFCO, q);
			for(i = 0; i< cnt; i++) {
				CComPtr<IMgaFCO> n;
				ObjForCore(nobjs[i])->getinterface(&n);
				q->Add(n); 
			}

			
			if(objs) {
				*objs = q.Detach();
			}


			SelfMark(OBJEVENT_NEWCHILD);
		} // if valid

	} COMCATCH_IN_TRANSACTION_MAYBE(;);
}

HRESULT FCO::MoveFCODisp(IMgaFCO *movedobj, IMgaMetaRole *role, IMgaFCO **nobj)
{
	COMTRY_IN_TRANSACTION_MAYBE {
		CComPtr<IMgaFCO> new_fco;

		// copy in param to a folder coll
		CREATEEXCOLLECTION_FOR(MgaFCO, q);
		q->Add( CComPtr<IMgaFCO>( movedobj));

		CComPtr<IMgaMetaRoles> roles;
		if (role)
		{
			COMTHROW(roles.CoCreateInstance(L"Mga.MgaMetaRoles", NULL, CLSCTX_INPROC));
			COMTHROW(roles->Append(CComPtr<IMgaMetaRole>(role)));
		}

		CComPtr<IMgaFCOs> newfcos;
		COMTHROW(MoveFCOs( q, roles, &newfcos));
		
		// extract ret value from returned coll
		long cnt = 0; 
		if( newfcos)  COMTHROW( newfcos->get_Count( &cnt));
		if( cnt == 1) COMTHROW( newfcos->get_Item( 1, &new_fco));
		
		if( nobj) {
			*nobj = new_fco.Detach();
		}
	} COMCATCH_IN_TRANSACTION_MAYBE(;);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// DERIVE //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

// do not use with folders
bool IsAnythingDerivedFromChildren(CoreObj cbase) {
	if(GetMetaID(cbase) == DTID_MODEL) {
		ITERATE_THROUGH(CoreObjs(cbase[ATTRID_FCOPARENT + ATTRID_COLLECTION])) {
			if(CoreObjs(ITER[ATTRID_DERIVED + ATTRID_COLLECTION]).Count() ||
				IsAnythingDerivedFromChildren(ITER) ) return true;
		}
	}
	return false;
}



HRESULT FCO::DeriveFCO(IMgaFCO *base, IMgaMetaRole *role, VARIANT_BOOL binstance, IMgaFCO** newobj) {
	COMTRY_IN_TRANSACTION {
		CheckWrite();
		CHECK_INBOOLPAR(binstance);
		CHECK_MYINPTRPAR(base);
		CHECK_OUTPTRPAR(newobj);
		metaid_type targettype = GetMetaID(self);

// Pre check:
// ERROR if object is not rootobject
		objtype_enum l;
		CoreObj cbase(base);
		COMTHROW(base->get_ObjType(&l));
		if(l == OBJTYPE_FOLDER) COMTHROW(E_MGA_NOT_DERIVABLE);
//NCH
//		COMTHROW(base->GetParent(NULL, &l));
//		if(l != OBJTYPE_FOLDER) COMTHROW(E_MGA_NOT_DERIVABLE);

//		Derivable: the FCO parent of base has 0 deriveds
//				   base already has deriveds, or all the children have 0 deriveds too.
		CoreObj parent = cbase[ATTRID_FCOPARENT];
		int lev;
		CoreObj rr;
		GetRootOfDeriv(cbase, rr, &lev);
		if( lev > 0 ||
			lev == -1 && IsAnythingDerivedFromChildren(cbase) ) {
				// COMTHROW(E_MGA_NOT_DERIVABLE);
		}


// Check if object to be created or any of its children will become children of their own basetypes
		int targetlevel = 0;
		if(targettype != DTID_FOLDER) {
			if(self[ATTRID_PERMISSIONS] & ~EXEMPT_FLAG) COMTHROW(E_MGA_NOT_CHANGEABLE);
			CoreObj root;
			GetRootOfDeriv(self, root, &targetlevel);
			if(root) {
				CoreObj c1(base);
				if(COM_EQUAL(root, c1)) COMTHROW(E_MGA_NOT_DERIVABLE);
				CheckConflict(c1, root);
			}
		}


		
// Copy trees
		CoreObj newcoreobj;
		coreobjpairhash crealist;
		ObjTreeDerive(mgaproject, cbase, newcoreobj, crealist, binstance ? INSTANCE_FLAG : 0);  // copy
		assignnewchild(newcoreobj);

		if (targettype == DTID_FOLDER)
			newcoreobj[ATTRID_ROLEMETA] = METAREF_NULL;
		else {
			metaref_type t;
			if(!role)  COMTHROW(E_MGA_INVALID_ROLE);
			COMTHROW(role->get_MetaRef(&t));
			if(t == METAREF_NULL)  COMTHROW(E_MGA_INVALID_ROLE);
			{
				metaref_type kt;
				CComPtr<IMgaMetaFCO> mfco;
				COMTHROW(role->get_Kind(&mfco));
				COMTHROW(mfco->get_MetaRef(&kt));
				if(kt != newcoreobj[ATTRID_META])COMTHROW(E_MGA_META_INCOMPATIBILITY);
			}

			newcoreobj[ATTRID_ROLEMETA] = t;
		}


// Reroute references
		ObjTreeReconnect(newcoreobj, crealist);			
// Post check:
		coreobjhash newcrealist;
		shiftlist(crealist, newcrealist);

		ObjTreeCheckRelations(mgaproject, newcoreobj, newcrealist);			

		ObjTreeNotify(mgaproject, newcoreobj);

		if(targetlevel >= 0) ReDeriveNewObj(mgaproject, newcoreobj, targetlevel+1);

		SelfMark(OBJEVENT_NEWCHILD);
		ObjFor(base)->SelfMark(OBJEVENT_SUBT_INST);

		docheck(mgaproject);


// Notification
		ObjForCore(newcoreobj)->getinterface(newobj);
	} COMCATCH_IN_TRANSACTION(;)
}





// this recursive method is different from ObjTreeCopy in that it doesn't
// create a new object into 'nobj' as its first step
// some inner collections are copied with the help of ObjTreeCopy
// it doesn't create anything except RegNodes and Attributes which are still tied to the base
void ObjDetachAndMerge( CMgaProject *mgaproject, CoreObj orig, CoreObj &nobj, coreobjpairhash& crealist, unsigned long nextrelid, bool prim = true)
{
	bool has_last_relid_attr = false;
	unsigned long nb_of_children = 0;
	unsigned long last_relid_set = 0;

	CComPtr<ICoreAttributes> atts;
	COMTHROW(orig->get_Attributes(&atts));
	MGACOLL_ITERATE(ICoreAttribute, atts) {
			attrid_type ai;
			CComPtr<ICoreMetaAttribute> mattr;

			COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
			COMTHROW(mattr->get_AttrID(&ai));
			if(ai < ATTRID_COLLECTION) {
				switch(ai) {
					case ATTRID_DERIVED: 
					case ATTRID_MASTEROBJ:
					{
						CoreObj nmas = nobj[ai];
						if( nobj.IsFCO() && nmas && nmas.IsFCO()) // save its master's guid into registry
						{
							CComBSTR bstr;
							ObjForCore( nmas)->GetGuidDisp( &bstr);
							if( bstr) ObjForCore( nobj)->put_RegistryValue( CComBSTR( DETACHED_FROM), bstr);
						}
						CComPtr<ICoreAttribute> regattr;
						HRESULT hr = orig->get_Attribute(ATTRID_REGNODE, &regattr);
						if (SUCCEEDED(hr))
							MergeRegs(orig, nobj);
						else if (hr != E_INVALIDARG)
							COMTHROW(hr);
						nobj[ai] = CComVariant( (IDispatch*) 0);//an empty value;
						break;
					}
					case ATTRID_PERMISSIONS:
					{
						nobj[ai] = nobj[ai] & ~INSTANCE_FLAG; // remove INSTANCE_FLAG
						// do not modify LIBROOT_FLAG LIBRARY_FLAG READONLY_FLAG EXEMPT_FLAG as these are not inherited
						//   (though LIBROOT_FLAG LIBRARY_FLAG READONLY_FLAG should be false here or CheckWrite would have failed)

						break;
					}
					case ATTRID_RELID:
					{
						if( !prim) // it was a child of a primary derived, being detached
						{
							// since it was called recursively we know for sure that
							// this object is a secondary derived object along with its
							// parent. 
							// (the level of the derivation in its case might have been 
							// higher than the parent's level of derivation)
							// but since the parent is detached totally from its
							// base, these kinds of children (for which the method
							// has been invoked recursively) will become archetypes
							// no matter how they have been derived originally
							// that is why the relid has to be decreased to the
							// [0, RELIDSPACE) interval
							
							// revert a secondary derived objects relid to a normal relid
							//long relid = nobj[ai];
							//ASSERT( relid >= 0);
							//while( relid >= RELIDSPACE) relid -= RELIDSPACE;

							if( nextrelid >= 0 && nextrelid < RELIDSPACE)
							{
								// this command would cause ambiguity if the parent has plain children
								//nobj[ai] = relid; // object will be no more secondary derived!
								// that is why we use new relids
								nobj[ai] = nextrelid;
							}
							else
							{
								ASSERT(0); // problem
								COMTHROW(E_MGA_LONG_DERIVCHAIN);
							}
						}
						break;
					}
					case ATTRID_LASTRELID:
					{
						// unsigned long old_val = nobj[ai];
						// the old value 'nobj[ai]' contains only the number of the plain objects
						// this is a temp value (the correct value is inserted upon exit)
						//nobj[ai] = 0L;
						has_last_relid_attr = true;
						break;
					}
					default:
						break; // no copy/change needed in other plain cases
				}; // endswitch
			}
			else if( LINKREF_ATTR(ai-ATTRID_COLLECTION)){
				ai -= ATTRID_COLLECTION;

				switch( ai) {
					case ATTRID_ATTRPARENT: // copy the unfilled attributes
					{
#ifdef _DEBUG
						unsigned int owned_attrs(0), inherited_attrs(0), l3(0);
						CComBSTR nm;
						ObjForCore(nobj)->get_Name( &nm);
#endif

						std::unordered_set<metaref_type> setAttrs;
						{
							CoreObjs my_attrs = nobj[ai + ATTRID_COLLECTION];
							ITERATE_THROUGH(my_attrs) { 
#ifdef _DEBUG
								++owned_attrs;
#endif
								metaref_type attr_i = ITER[ATTRID_META];
								bool inserted = setAttrs.insert(attr_i).second;
								ASSERT(inserted); // can an attribute belong to self twice?
							}
						}
						// owned_attrs is the number of owned attributes
						// in case the user overwrites the inherited attribute values 
						// the instance or subtype gets an additional attribute (initially had none)
						// thus in case of detaching, we need to distinguish between owned values
						// and inherited values (owned preferred)
						CoreObj baseObject = orig;
						do {
							CoreObjs base_attrs = baseObject[ai + ATTRID_COLLECTION]; // copy the base's values selectively
							ITERATE_THROUGH(base_attrs) {

								if (setAttrs.insert(ITER[ATTRID_META]).second == true)
								{
#ifdef _DEBUG
									++inherited_attrs;
#endif
									CoreObj nchild;
									ObjTreeCopy(mgaproject, ITER, nchild, crealist);
									nchild[ai] = nobj;
									setAttrs.insert(ITER[ATTRID_META]);
								}
							}

							// folders may not be derived
							ASSERT(baseObject.IsFCO());
						} while (baseObject = baseObject[ATTRID_DERIVED]);

#ifdef _DEBUG
						CoreObjs mine = nobj[ai + ATTRID_COLLECTION];
						ITERATE_THROUGH(mine) {
							++l3;
						}
						ASSERT( owned_attrs + inherited_attrs == l3);
#endif

						break;
					}
					case ATTRID_FCOPARENT: // for all secondary derived fco children-> detach
					{
						// we will calc the max_relid of those child objects which are not secondary, tertiary, ... objects
						// because in case of subtypes not all children originate from the current base:
						// some children might be plain objects, and some others may be primary subtypes of other bases
						long cur_max_relid = nobj[ATTRID_LASTRELID];
						CoreObjs children = nobj[ai + ATTRID_COLLECTION];
						ITERATE_THROUGH(children) {
							// this will help set the last relid correctly
							++nb_of_children;
							CoreObj base = (ITER)[ATTRID_DERIVED];
							if( base) // child is also derived
							{
								VARIANT_BOOL primDer = VARIANT_FALSE;
								if( nobj.IsFCO()) ObjForCore( ITER)->get_IsPrimaryDerived( &primDer);
								if( primDer != VARIANT_TRUE) // it is a child derived along with 'orig'
								{
									// detach it as well from its base
									ObjDetachAndMerge(mgaproject, base, ITER, crealist, ++cur_max_relid, false);
									// increase the cur_max_relid for the next child
									// FIXME: should this be max?
									nobj[ATTRID_LASTRELID] = last_relid_set = cur_max_relid;
								}
								else 
								{	// it might be a primary subtype/instance placed into this subtype
									// leave this object untouched
								}
							}
							else
							{	// additional child->no action needed
							}
						}
						break;
					}
					default: // for all others -> detach
					{ 
						CoreObjs collmembers = nobj[ai + ATTRID_COLLECTION];
						ITERATE_THROUGH(collmembers) {
							// remove the ATTRID_DERIVED attrs if any?
							ObjDetachAndMerge(mgaproject, ITER, ITER, crealist, 0, false);
						}
					}
				}; // endswitch
			}

	} MGACOLL_ITERATE_END;

	if( has_last_relid_attr) // if this CoreObj has RELID_LAST attribute
	{
		unsigned long r = nobj[ATTRID_LASTRELID];
		ASSERT( last_relid_set >= nb_of_children);
		if( r < last_relid_set)
			nobj[ATTRID_LASTRELID] = last_relid_set; // fill it with the correct value
	}
}

void ObjAttach(CMgaProject *mgaproject, const CoreObj &origobj, CoreObj &newobj, coreobjpairhash &crealist, long instance, bool prim) 
{
	// this variable will make sure that if an instance is met inside a base (origobj) which is subtyped (!)
	// then assure that though a subtype may have additional children, but the instance brach might not 
	// contain additional children
	bool instance_met = false;
	CComPtr<ICoreAttributes> atts;
	COMTHROW(origobj->get_Attributes(&atts));
	MGACOLL_ITERATE(ICoreAttribute, atts) {
			attrid_type ai;
			CComPtr<ICoreMetaAttribute> mattr;
			COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
			COMTHROW(mattr->get_AttrID(&ai));
			if(ai < ATTRID_COLLECTION) {
				switch(ai) {
				case ATTRID_DERIVED: 
				case ATTRID_MASTEROBJ:
					newobj[ai] = origobj;
					break;
				case ATTRID_PERMISSIONS:
					// if it was instance or orig is instance or it is supposed to be an instance:
					newobj[ai] = (newobj[ai] | origobj[ai] | long(instance)) & INSTANCE_FLAG;
					instance_met = newobj[ai] & INSTANCE_FLAG;
					break;
				case ATTRID_RELID: 
					{
						if( !prim)
						{
							// if newobj[ai] + RELIDSPACE > LONG_MAX
							if (newobj[ai] > LONG_MAX - RELIDSPACE)
								COMTHROW(E_MGA_LONG_DERIVCHAIN);
							newobj[ai] = newobj[ai]+RELIDSPACE; //shift its own relid
							ASSERT(newobj[ai] >= 0);
						}
					}
					break;
				case ATTRID_LASTRELID: 
					newobj[ai] = 0L;
					break;
				//default:
				//	newobj[ai] = static_cast<CComVariant>(origobj[ai]);
				}
			}
			else {
				ai -= ATTRID_COLLECTION;
				if(LINKREF_ATTR(ai))  {
					if( ai == ATTRID_ATTRPARENT) { } // no need to copy attr values since the newobj already had its own
					else if( ai == ATTRID_CONSTROWNER) { }
					else if( ai == ATTRID_CONNROLE) { }
					else if( ai == ATTRID_SETMEMBER) { }
					else if(ai == ATTRID_FCOPARENT) {
						
						std::map< int, int> match;
						std::map< CoreObj, CoreObj> matchO;
						//std::vector<metaref_type> kidsVec, kidsVec2;
						//std::vector<std::string> nameVec, nameVec2;
						
						unsigned int kids_o = 0;
						{CoreObjs children = origobj[ai + ATTRID_COLLECTION];
						ITERATE_THROUGH(children) {
							++kids_o;
							metaref_type role_i = ITER[ATTRID_META];
							//kidsVec.push_back( role_i);
							
							CComBSTR nm_i;
							ObjForCore(ITER)->get_Name( &nm_i);
							//std::string nm_ii; CopyTo( nm_i, nm_ii);
							//nameVec.push_back( nm_ii);

							CComBSTR gd_i;
							ObjForCore(ITER)->GetGuidDisp( &gd_i);

							CoreObj outer_ITER = ITER;
							unsigned int kids_n = 0;
							bool fnd = false;
							{CoreObjs children2 = newobj[ai + ATTRID_COLLECTION];
							ITERATE_THROUGH(children2) {
								++kids_n;
								metaref_type role_j = ITER[ATTRID_META];
								//kidsVec2.push_back( role_j);
								
								CComBSTR nm_j;
								ObjForCore(ITER)->get_Name( &nm_j);
								//std::string nm_jj; CopyTo( nm_j, nm_jj);
								//nameVec2.push_back( nm_jj);

								CComBSTR gd_j;
								ObjForCore(ITER)->get_RegistryValue( CComBSTR( DETACHED_FROM), &gd_j);
								
								// name based equality is considered only if 
								// no stored master guid (Detached from) was found
								if( ( gd_j == gd_i && gd_j.Length() > 0 || nm_j == nm_i && gd_j.Length() == 0) 
									&& role_i == role_j 
									&& match.find(kids_n) == match.end()) // not found <==> not assigned
								{
									// kids_o <---> kids_n
									//ObjAttach( mgaproject, kids_o, kids_n)
									ObjAttach( mgaproject, outer_ITER, ITER, crealist, instance, false);
									match[kids_n] = kids_o;
									matchO[ITER] = outer_ITER;
									fnd = true;
									break;
								}
							}}
							
							if( !fnd)//!match.has_key(kids_o)) 
							{
								CoreObj i;
								ObjTreeCopy( mgaproject, ITER, i, crealist);//, instance, false);
								ObjAttach( mgaproject, ITER, i, crealist, instance, false);
								i[ATTRID_FCOPARENT] = newobj;
							}
						}}

						if( instance || instance_met) 
						{
							unsigned int kids_o( 0), kids_n( 0);
							{CoreObjs children = origobj[ai + ATTRID_COLLECTION];
							ITERATE_THROUGH(children) {
								++kids_o;
							}}
							{CoreObjs children = newobj[ai + ATTRID_COLLECTION];
							ITERATE_THROUGH(children) {
								++kids_n;
							}}

							if( kids_o != kids_n) // at this point the instance should have the same number of elements like its base
								COMTHROW(E_FAIL);
						}
					}
					else {
						CoreObjs collmembers = origobj[ai + ATTRID_COLLECTION];
						ITERATE_THROUGH(collmembers) {
							CoreObj nchild;
							ObjAttach(mgaproject, ITER, nchild, crealist, instance, false);
							nchild[ai] = newobj;
						}
					}
				}
			}

	} MGACOLL_ITERATE_END;
}

void getMeAGuid( long *p_l1, long *p_l2, long *p_l3, long *p_l4)
{
	GUID t_guid = GUID_NULL;
	COMTHROW(CoCreateGuid(&t_guid));
		
	ASSERT(t_guid != GUID_NULL);
	//char buff[39];
	//sprintf( buff, "{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}",
	//	t_guid.Data1, t_guid.Data2, t_guid.Data3,
	//	t_guid.Data4[0], t_guid.Data4[1], t_guid.Data4[2], t_guid.Data4[3],
	//	t_guid.Data4[4], t_guid.Data4[5], t_guid.Data4[6], t_guid.Data4[7]);

	// thus replace the old guid with a new one
	*p_l1 = t_guid.Data1; // Data1: 32 b, Data2, Data 3: 16 b, Data4: 64 bit
	*p_l2 = (t_guid.Data2 << 16) + t_guid.Data3;
	*p_l3 = (((((t_guid.Data4[0] << 8) + t_guid.Data4[1]) << 8) + t_guid.Data4[2]) << 8) + t_guid.Data4[3];
	*p_l4 = (((((t_guid.Data4[4] << 8) + t_guid.Data4[5]) << 8) + t_guid.Data4[6]) << 8) + t_guid.Data4[7];
}

void assignGuid( CMgaProject *mgaproject, CoreObj& ss)
{
	ASSERT( ss.IsFCO() || ss.IsContainer());
	if( !ss.IsFCO() && !ss.IsContainer()) return;
	//if( mgaproject->mgaversion <= 1) return;

	long l1(0), l2(0), l3(0), l4(0);
	getMeAGuid( &l1, &l2, &l3, &l4);

	ss[ATTRID_GUID1] = l1;
	ss[ATTRID_GUID2] = l2;
	ss[ATTRID_GUID3] = l3;
	ss[ATTRID_GUID4] = l4;
}


