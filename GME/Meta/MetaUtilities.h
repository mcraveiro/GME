
#ifndef MGA_METAUTILITIES_H
#define MGA_METAUTILITIES_H

#ifndef MGA_CORECOLLECTION_H
#include "../Common/CommonCollection.h"
#endif

#include <vector>
#include <algorithm>

class CMgaMetaProject;
class CMgaMetaBase;

// --------------------------- CCoreObjectPtr

class CCoreObjectPtr;
typedef std::vector<CCoreObjectPtr> coreobjects_type;
typedef coreobjects_type::iterator coreobjects_iterator;

class CCoreObjectPtr
{
public:
	CCoreObjectPtr() { }
	explicit CCoreObjectPtr(IUnknown *q) { COMTHROW( ::QueryInterface(q, p) ); }
	explicit CCoreObjectPtr(ICoreObject *q) : p(q) { }
	CCoreObjectPtr(const CCoreObjectPtr &q) : p(q.p) { }

	operator ICoreObject*() const { return p; }
	ICoreObject* operator->() const { ASSERT( p != NULL ); return p; }

	ICoreObject* operator=(ICoreObject *q) { p = q; return q; }
	ICoreObject* operator=(const CCoreObjectPtr &q) { p = q.p; return p; }

public:
	void GetCollectionValue(attrid_type attrid, coreobjects_type &ret) const;
	void GetStringValue(attrid_type attrid, BSTR *ret) const;
	void GetVariantValue(attrid_type attrid, VARIANT *ret) const;
	long GetLongValue(attrid_type attrid) const;
	VARIANT_BOOL GetBoolValue(attrid_type attrid) const;
	void GetPointerValue(attrid_type attrid, CCoreObjectPtr &ret) const;
	metaid_type GetMetaID() const;
	objid_type GetObjID() const;

	void PutStringValue(attrid_type attrid, BSTR a);
	void PutLongValue(attrid_type attrid, long a);
	void PutVariantValue(attrid_type attrid, VARIANT a);
	void PutBoolValue(attrid_type attrid, VARIANT_BOOL l);
	void PutPointerValue(attrid_type attrid, ICoreObject *a);
	void PutLockValue(attrid_type attrid, locking_type locking);

	void Load() { PutLockValue(ATTRID_LOCK, LOCKING_READ); }
	void LoadCollectionValue(attrid_type attrid, coreobjects_type &ret);

public:
	CComObjPtr<ICoreObject> p;
};

// --------------------------- PutOut

inline ICoreObject **PutOut(CCoreObjectPtr &coreobj)
{
	return PutOut(coreobj.p);
}

// --------------------------- ComGetAttrValue

struct objid_lessthan
{
	bool operator()(const CCoreObjectPtr &a, const CCoreObjectPtr &b) const
	{
		metaid_type am = a.GetMetaID();
		metaid_type bm = b.GetMetaID();

		if( am != bm )
			return am > bm;

		return a.GetObjID() > b.GetObjID();
	}
};



struct objid_morethan
{
	bool operator()(const CCoreObjectPtr &a, const CCoreObjectPtr &b) const
	{
		metaid_type am = a.GetMetaID();
		metaid_type bm = b.GetMetaID();

		if( am != bm )
			return am > bm;

		return a.GetObjID() < b.GetObjID();
	}
};






template<class ITFTYPE, class COLLITF>
HRESULT ComGetCollectionValue(IUnknown *me, attrid_type attrid, COLLITF **p) {
	CHECK_OUT(p);

	COMTRY
	{
		ASSERT( me != NULL );
		CCoreObjectPtr self(me);

		coreobjects_type coreobjects;
		self.GetCollectionValue(attrid, coreobjects);
	
		typedef CCoreCollection<COLLITF, std::vector<ITFTYPE*>, ITFTYPE, ITFTYPE> COMTYPE;

		CComObjPtr<COMTYPE> coll;
		CreateComObject(coll);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComObjPtr<ITFTYPE> q;
			COMTHROW( ::QueryInterface(*i, q) );

			coll->Add(q);

			++i;
		}

		MoveTo(coll, p);
	}
	COMCATCH(;)
}

template<class ITFTYPE, class COLLITF>
HRESULT ComGetSortedCollValue(IUnknown *me, attrid_type attrid, COLLITF **p) {
	CHECK_OUT(p);

	COMTRY
	{
		ASSERT( me != NULL );
		CCoreObjectPtr self(me);

		coreobjects_type coreobjects;
		self.GetCollectionValue(attrid, coreobjects);
	
		std::sort(coreobjects.begin(), coreobjects.end(), objid_morethan());

		typedef CCoreCollection<COLLITF, std::vector<ITFTYPE*>, ITFTYPE, ITFTYPE> COMTYPE;

		CComObjPtr<COMTYPE> coll;
		if(!coll) CreateComObject(coll);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComObjPtr<ITFTYPE> q;
			COMTHROW( ::QueryInterface(*i, q) );

			coll->Add(q);

			++i;
		}

		MoveTo(coll, p);
	}
	COMCATCH(;)
}

template<class ITFTYPE>
HRESULT ComGetCollValueByName(BSTR name, IUnknown *me, attrid_type attrid, attrid_type nameid, ITFTYPE **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		ASSERT( me != NULL );
		CCoreObjectPtr self(me);

		coreobjects_type coreobjects;
		self.GetCollectionValue(attrid, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComBstrObj n;
			(*i).GetStringValue(nameid, PutOut(n));
			
			if( n == name )
			{
				COMTHROW( ::QueryInterface(*i, p) );
				return S_OK;
			}

			++i;
		}

		COMRETURN(E_NOTFOUND);
	}
	COMCATCH(;)
}

template<class ITFTYPE, class COLLITF>
HRESULT ComGetLinkCollectionValue(IUnknown *me, attrid_type collid, attrid_type ptrid, COLLITF **p) {
	CHECK_OUT(p);

	COMTRY
	{
		ASSERT( me != NULL );
		CCoreObjectPtr self(me);

		coreobjects_type coreobjects;
		self.GetCollectionValue(collid, coreobjects);

		typedef CCoreCollection<COLLITF, std::vector<ITFTYPE*>, ITFTYPE, ITFTYPE> COMTYPE;

		CComObjPtr<COMTYPE> coll;
		CreateComObject(coll);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CCoreObjectPtr obj;
			(*i).GetPointerValue(ptrid, obj);

			CComObjPtr<ITFTYPE> q;
			COMTHROW( ::QueryInterface(obj, q) );

			coll->Add(q);

			++i;
		}

		MoveTo(coll, p);
	}
	COMCATCH(;)
}

template<class ITFTYPE>
HRESULT ComGetLinkCollValueByName(BSTR name, IUnknown *me, attrid_type collid, attrid_type ptrid, ITFTYPE **p) {
	CHECK_OUT(p);

	COMTRY
	{
		ASSERT( me != NULL );
		CCoreObjectPtr self(me);

		CComBstrObj_lightequal equal( me);
		coreobjects_type coreobjects;
		self.GetCollectionValue(collid, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CCoreObjectPtr obj;
			(*i).GetPointerValue(ptrid, obj);

			CComBstrObj n;
			obj.GetStringValue(ATTRID_NAME, PutOut(n));
			
			if( equal( n, name))//if( n == name )
			{
				COMTHROW( ::QueryInterface(obj, p) );
				return S_OK;
			}

			++i;
		}

		COMRETURN(E_NOTFOUND);
	}
	COMCATCH(;)
}

template<class ITFTYPE, class COLLITF>
HRESULT ComGetSortedLinkCollValue(IUnknown *me, attrid_type collid, attrid_type ptrid, COLLITF **p) {

	CHECK_OUT(p);

	COMTRY
	{
		ASSERT( me != NULL );
		CCoreObjectPtr self(me);

		coreobjects_type coreobjects;
		self.GetCollectionValue(collid, coreobjects);

		std::sort(coreobjects.begin(), coreobjects.end(), objid_morethan());

		typedef CCoreCollection<COLLITF, std::vector<ITFTYPE*>, ITFTYPE, ITFTYPE> COMTYPE;

		CComObjPtr<COMTYPE> coll;
		CreateComObject(coll);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CCoreObjectPtr obj;
			(*i).GetPointerValue(ptrid, obj);

			CComObjPtr<ITFTYPE> q;
			COMTHROW( ::QueryInterface(obj, q) );

			coll->Add(q);

			++i;
		}

		MoveTo(coll, p);
	}
	COMCATCH(;)
}

template<class ITFTYPE>
HRESULT ComGetPointerValue(IUnknown *me, attrid_type attrid, ITFTYPE **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		ASSERT( me != NULL );
		CCoreObjectPtr self(me);

		CCoreObjectPtr pointer;
		self.GetPointerValue(attrid, pointer);

		if( pointer != NULL )
			COMTHROW( ::QueryInterface(pointer, p) );
	}
	COMCATCH(;)
}

HRESULT ComGetAttrValue(IUnknown *me, attrid_type attrid, BSTR *p);
HRESULT ComGetAttrValue(IUnknown *me, attrid_type attrid, VARIANT *p);
HRESULT ComGetAttrValue(IUnknown *me, attrid_type attrid, long *p);

HRESULT ComGetDisplayedName(IUnknown *me, attrid_type dispattrid, attrid_type nameattrid, BSTR *p);
HRESULT ComGetObjType(IUnknown *me, objtype_enum *p);

// --------------------------- ComCreateObject

template<class ITFTYPE>
HRESULT ComCreateMetaObj(IUnknown *me, metaid_type metaid, attrid_type pointer, ITFTYPE **p)
{
	CHECK_OUT(p);
	ASSERT( me != NULL );

	COMTRY
	{
		CCoreObjectPtr self(me);

		CComObjPtr<ICoreProject> coreproject;
		COMTHROW( self->get_Project(PutOut(coreproject)) );
		ASSERT( coreproject != NULL );

		CCoreObjectPtr subobject;
		COMTHROW( coreproject->CreateObject(metaid, PutOut(subobject)) );

		subobject.PutPointerValue(pointer, self);

		COMTHROW( ::QueryInterface(subobject, p) );
	}
	COMCATCH(;)
}

template<class ITFTYPE>
HRESULT ComDefineBase(CMgaMetaBase *me, metaid_type metaid, attrid_type pointer, ITFTYPE **p)
{
	CHECK_OUT(p);
	ASSERT( me != NULL );

	CMgaMetaProject *metaproject = me->metaproject;
	ASSERT( metaproject != NULL );

	COMTRY
	{
		CCoreObjectPtr self(me);

		CCoreObjectPtr subobject;
		metaproject->CreateMetaBase(metaid, subobject);

		subobject.PutPointerValue(pointer, self);

		COMTHROW( ::QueryInterface(subobject, p) );
	}
	COMCATCH(;)
}

template<class ITFTYPE>
HRESULT ComAddLink(CMgaMetaBase *me, metaid_type metaid, 
	attrid_type here, attrid_type there, ITFTYPE *p)
{
	if( p == NULL )
		COMRETURN(E_POINTER);

	ASSERT( me != NULL );

	CMgaMetaProject *metaproject = me->metaproject;
	ASSERT( metaproject != NULL );

	COMTRY
	{
		CCoreObjectPtr self(me);
		CCoreObjectPtr other(p);

		CCoreObjectPtr link;
		metaproject->CreateMetaObj(metaid, link);

		link.PutPointerValue(here, self);
		link.PutPointerValue(there, other);
	}
	COMCATCH(;)
}

template<class ITFTYPE>
HRESULT ComRemoveLink(CMgaMetaBase *me, metaid_type metaid, 
	attrid_type here, attrid_type there, ITFTYPE *p)
{
	return E_NOTIMPL;
}

// --------------------------- ComPutAttrValue

HRESULT ComPutAttrValue(IUnknown *self, attrid_type attrid, BSTR p);
HRESULT ComPutAttrValue(IUnknown *self, attrid_type attrid, long p);
HRESULT ComPutAttrValue(IUnknown *self, attrid_type attrid, VARIANT p);

// try to avoid it, not nice interface
//HRESULT ComPutPointerValue(IUnknown *me, attrid_type attrid, IDispatch *p);

// --------------------------- Others

// this changes the variant type from BSTR
void ChangeAttrValueType(CComVariant &v, attval_enum attval);

HRESULT ComDeleteObject(IUnknown *self);

// --------------------------- Traverse

// these functions are implemented in the Meta.cpp
void TraverseObject(CMgaMetaProject *metaproject, CCoreObjectPtr &me);
void TraverseCollection(CMgaMetaProject *metaproject, CCoreObjectPtr &me, attrid_type attrid);

// --------------------------- PathItem

typedef struct pathitem_type
{
	CComBstrObj terminal_name;
	CComBstrObj continual_name;
} pathitem_type;

typedef std::vector<pathitem_type> pathitems_type;

typedef struct jointpath_type
{
	CComBstrObj ptrspec_name;
	pathitems_type pathitems;
} jointpath_type;

typedef std::vector<jointpath_type> jointpaths_type;

// --------------------------- STL function objects

struct BSTR_hashfunc 
{
	size_t operator()(const CComBstrObj &str) const 
	{
		if(str.p == NULL) return 0;
		return *str.p;
	}
};

struct BSTR_equalkey 
{
	bool operator()(const CComBstrObj &str1, const CComBstrObj &str2) const	
	{
		return str1 == str2;	
	}
};

struct metaid_hashfunc : public stdext::hash_compare<metaref_type>
{
	size_t operator()(metaref_type metaref) const
	{
		return metaref;
	}
	bool operator()(metaref_type id1, metaref_type id2) const
	{
		// this must be < logic
		//return id1 == id2;
		return id1 < id2;
	}
};

CComBSTR truncateName( IUnknown *p_bs, CComBSTR pIn);

struct CComBstrObj_lightequal
{
	CComBSTR m_nmspc;
	//CComBstrObj_lightequal() {} // we do not allow this

	template<class TMETABASE>
	CComBstrObj_lightequal( TMETABASE p_bs) 
	{
		CComQIPtr<IMgaMetaBase> bs( p_bs);
		if( bs)
		{
			CComPtr<IMgaMetaProject> pr;
			COMTHROW( bs->get_MetaProject( &pr));
			COMTHROW( pr->GetNmspc( &m_nmspc)); // fetch namespace into m_nmspc
		}
	}

	CComBstrObj_lightequal( const CComBSTR pNmspc)
		: m_nmspc( pNmspc)
	{
	}

	//previously p1 was pUnknown, p2 was pFullName, and we thought that the user would give Fullnames
	bool operator()( const CComBstrObj &pFullNmInParadigm, const CComBstrObj &pUnknownFormat) const
	{
		if( pFullNmInParadigm == pUnknownFormat)
			return true;
		else if( m_nmspc.Length() > 0)
		{
			CComBSTR longername;
			COMTHROW(longername.AppendBSTR( m_nmspc));
			COMTHROW(longername.Append( _T("::")));
			COMTHROW(longername.AppendBSTR( pUnknownFormat));
			return pFullNmInParadigm == longername;
		}
		else
		{
			return false;
		}
	}

	bool operator()( const CComBstrObj &pFullNmInParadigm, BSTR pUnknownFormat) const
	{
		return operator()(pFullNmInParadigm, CComBstrObj(pUnknownFormat));
	}
};


// --------------------------- TRACE

#ifdef _DEBUG

#define PATH_TRACE AtlTrace

#endif

inline void NOOP_TRACE(LPCSTR, ...) { }

#ifndef PATH_TRACE
#define PATH_TRACE 1 ? void(0) : NOOP_TRACE
#endif

#endif//MGA_METAUTILITIES_H
