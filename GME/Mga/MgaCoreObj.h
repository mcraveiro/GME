//////////////////////////////////////////////////////////////////////////////////
////////////////////////////// COREOBJ COREOBJS & COREATTRIBUTE //////////////////
///////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "MgaGeneric.h"

class CoreAttr;  // defined  below
class CoreObjs;



// ----------------------------------------
// CoreObj: supersmart ptr for ICoreObject
// ----------------------------------------
class CoreObj : public CComPtr<ICoreObject> {
public:
	CoreObj() {;};
	CoreObj(ICoreObject *ptr) : CComPtr<ICoreObject>(ptr) {	}

	// Create this object (it must be empty)
	// In both versions 'c' is used to access the core project.
	void Create(ICoreObject *c, metaid_type mm);
	void Create(ICoreProject *c, metaid_type mm);

	explicit CoreObj( IMgaObject *ptr);

	inline CoreAttr operator[](int  id) const;
	inline CoreAttr operator[](attrid_type  id) const;

	void operator= (ICoreObject *s) {
		CComPtr<ICoreObject>::operator=(s);
	}

	void operator <<= (CoreObj &s) {
		Attach(s.Detach());
	}

	CoreObj *operator &()  {
		return this;
	}
	bool IsEqualObject(IUnknown* pOther) const {
		if (p == NULL && pOther == NULL)
			return true; // They are both NULL objects

		if (p == NULL || pOther == NULL)
			return false; // One is NULL the other is not

		CComPtr<IUnknown> punk1;
		CComPtr<IUnknown> punk2;
		p->QueryInterface(IID_IUnknown, (void**)&punk1);
		pOther->QueryInterface(IID_IUnknown, (void**)&punk2);
		return punk1 == punk2;
	}

	ICoreObject *&ComPtr() { ASSERT(!p); return p; }

	metaid_type GetMetaID() const {
				metaid_type t;
				CComPtr<ICoreMetaObject> mmo;
				COMTHROW((*this)->get_MetaObject(&mmo));
				COMTHROW(mmo->get_MetaID(&t));
				return t;
	}

	objid_type GetObjID() const {
				objid_type t;
				COMTHROW((*this)->get_ObjID(&t));
				return t;
	}

	bool IsDeleted() const {
				VARIANT_BOOL pp;
				if((*this)->get_IsDeleted(&pp) != S_OK)
					return true;
				return pp ? true:false;
	}

	bool IsContainer() const { metaid_type s = GetMetaID(); return s == DTID_MODEL || s == DTID_FOLDER; }
	bool IsFCO() const { metaid_type s = GetMetaID(); return s >= DTID_MODEL && s <= DTID_SET; }
	bool IsRootFCO() const;
	bool IsRootFolder() const;
	bool IsObject()  const { metaid_type s = GetMetaID(); return s >= DTID_MODEL && s <= DTID_FOLDER; }
	bool IsSubObject() const { metaid_type s = GetMetaID(); return s >= DTID_FOLDER; }

	// if the object is a subobject, get the CoreObj of the object it belongs to
	CoreObj GetMgaObj();

	// get the master (if any) for a subobj (REFERENCE, SETMEMBER, CONNROLE, CONNROLESEG)
	// the master is the corresponding subobject in the immediate base FCO
	// for references, master is an object (if any)
	CoreObj GetMaster(int offset = 1);

	// FOLLOWS a chain (like ATTRID_FCOPARENT) returns NULL if list is shorter
	CoreObj FollowChain(attrid_type id, int offset);  

	// this object (an FCO) has outgoing inheritance relationships, 
	// but its FCO parent (if any) has none.
	bool IsRootOfDeriv() const;  
	
};



// ----------------------------------------
// CoreObjs: supersmart ptr for ICoreObjects
// ----------------------------------------
class CoreObjs : public CComPtr<ICoreObjects> {
public:
	CoreObjs() {;};
	// sor ascending by use for CONNROLESEG-s only
	void Sort();
	long Count() const {
		long l; 
		COMTHROW(p->get_Count(&l));
		return l;
	}

private:
	friend CoreAttr;

	explicit CoreObjs( IDispatch *ptr) {
		COMTHROW(ptr->QueryInterface(_uuidof(p),(void**)&p));
		ASSERT(p);
	}
};


// ----------------------------------------
// CoreAttribute: Represents an attribute within a coreobject 
// This is intended to be used as a short-lived object (does not store references)
// ----------------------------------------
class CoreAttr {
	attrid_type i;
protected:
	CoreAttr(int attrid) {
		i = (short)attrid;
	};
	ICoreObject *p;   // not a smart pointer  
public:
	CoreAttr(int attrid, ICoreObject *ptr) {
		i = (short)attrid; p = ptr;
	};


	void operator= (const CoreAttr &s) {
		CComVariant k(s);
		COMTHROW(p->put_AttributeValue(i,k));
	}

	template <class C> 
	const C& operator= (const C &s) { //const removed
		CComVariant k(s);
		COMTHROW(p->put_AttributeValue(i,k));
		return s;
	};

	operator CComBSTR() const {
		CComBSTR v;				// It will be optimized by the compiler!!!!
		CComVariant k;
		COMTHROW(p->get_AttributeValue(i,&k));
		ASSERT(k.vt == VT_BSTR);
		k.vt = VT_EMPTY;
		v.Attach(k.bstrVal);
		return v;
	}; 

	operator long() const {
		CComVariant k;
		COMTHROW(p->get_AttributeValue(i,&k));
		ASSERT(k.vt == VT_I4);
		return k.lVal;
	}; 

	operator CComVariant() const {
		CComVariant k;
		COMTHROW(p->get_AttributeValue(i,&k));
		return k;
	}; 
	inline operator CoreObj () const {
		CComVariant k;
		COMTHROW(p->get_AttributeValue(i,&k));
		if(k.vt != VT_DISPATCH) COMTHROW(E_MGA_MODULE_INCOMPATIBILITY);
		CoreObj rr;
		if(k.pdispVal) COMTHROW(k.pdispVal->QueryInterface(_uuidof(ICoreObject), (void **)&rr));
		return rr;
	}; 
	inline operator CoreObjs () const {
		CComVariant k;
		COMTHROW(p->get_AttributeValue(i,&k));
		if(k.vt != VT_DISPATCH) COMTHROW(E_MGA_MODULE_INCOMPATIBILITY);
		ASSERT(k.pdispVal);
		return CoreObjs(k.pdispVal);
	}; 
};

inline CoreAttr CoreObj::operator[](int  id) const {
		return CoreAttr(id, p);
};

inline CoreAttr CoreObj::operator[](attrid_type  id) const {
		return CoreAttr(id, p);
};

inline bool CoreObj::IsRootFCO() const { return ((*this)[ATTRID_ROLEMETA] == METAREF_NULL); }
inline bool CoreObj::IsRootFolder() const { return this->GetMetaID() == DTID_ROOT; }


// ----------------------------------------
// Non-member CoreObj functions
// they are non-members mostly for historical reasons
// ----------------------------------------

metaid_type GetMetaID(const CoreObj &);

inline CoreObj &Clr(CoreObj &x) { x.Release(); return x; }

// get rootFCO with (optional) level
void GetRootFCO(CoreObj &fco,  /* out */ CoreObj &rootFCO, int *level = NULL);

// get the parent (and its distance from FCO) that is the root of current outgoing inheritance relationships
// if nothing is derived from fco, return NULL for rootFCO and -1 for level
void GetRootOfDeriv(CoreObj &fco,  /* out */ CoreObj &rootFCO, int *level = NULL);

// check if FCO is contained by parentFCO, also true if parent == fco
// level is only assigned if return value is true;
bool IsContained(CoreObj &fco, CoreObj &parentFCO, int *level = NULL);
bool IsFolderContained(CoreObj &fold, CoreObj &parentFold, int *level = NULL);

// get the derived equivalent of 'objinbase' in 'subtype'  
// level is the generation distance; not calculated if supplied by caller
void GetDerivedEquivalent(CoreObj const &objinbase, CoreObj const &subtype, 
						  /*out*/ CoreObj &objinsubtype, int level = -1);
// check if the original master of this relation is internal or external
// does not check the internality of references whithin connections!!!!!!
bool IsInternalRelation(CoreObj src);

