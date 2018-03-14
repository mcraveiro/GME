
#ifndef MGA_COREUTILITIES_H
#define MGA_COREUTILITIES_H

typedef OBJECTID OBJID;

#include <vector>

// --------------------------- ID Pair

struct metaobjidpair_type
{
	long metaid;
	long objid;
};

static bool operator==(const metaobjidpair_type& a, const metaobjidpair_type& b) { return a.objid == b.objid && a.metaid == b.metaid; }


struct metaobjidpair_hashfunc
{
	size_t operator()(const metaobjidpair_type &idpair) const
	{
		return (((size_t)idpair.metaid) << 8) + ((size_t)idpair.objid);
	}
};



struct metaobjidpair_equalkey
{
	bool operator()(const metaobjidpair_type &a, const metaobjidpair_type &b) const
	{
		return a.objid == b.objid && a.metaid == b.metaid;
	}
};

struct metaobjidpair_less
{
	bool operator()(const metaobjidpair_type& a, const metaobjidpair_type& b) const
	{
        if( a.metaid < b.metaid )
            return true;
        else if( a.metaid > b.metaid )
            return false;
        else if( a.objid < b.objid )
            return true;
        else
            return false;
	}
};

struct metaobjid2pair_hashfunc : public stdext::hash_compare<metaobjidpair_type, metaobjidpair_less>
{
	size_t operator()(const metaobjidpair_type &idpair) const
	{
		return (((size_t)idpair.metaid) << 8) + ((size_t)idpair.objid);
	}
	bool operator()(const metaobjidpair_type &a, const metaobjidpair_type &b) const
	{
		// implement < logic here !!!
		return metaobjidpair_less()( a, b);	
	}
};

inline void CopyTo(const metaobjidpair_type &idpair, VARIANT *v)
{ CopyTo((long*)&idpair, (long*)&idpair + 2, v); }

inline void CopyTo(const std::vector<metaobjidpair_type> &idpairs, VARIANT *v)
{ 
	if(idpairs.empty())
	{
		long*pnull=NULL;
		CopyTo(pnull,pnull, v); 
	}
	else
	{
		CopyTo((long*)&idpairs[0], (long*)(&idpairs[0] + idpairs.size()), v); 
	}
}

inline void CopyTo(const VARIANT &v, metaobjidpair_type &idpair)
{
	long bound = GetArrayLength(v);

	if( bound <= 0 )
	{
		idpair.metaid = METAID_NONE;
		idpair.objid = OBJID_NONE;
	}
	else if( bound == 2 )
		CopyTo(v, (long*)&idpair, (long*)&idpair + 2);
	else
		HR_THROW(E_INVALIDARG);
}

inline void GetArrayBounds(const VARIANT &v, metaobjidpair_type *&start, metaobjidpair_type *&end)
{
	GetArrayStart(v, *(long**)&start);
	
	ASSERT( GetArrayLength(v) % 2 == 0 );
	end = start + (GetArrayLength(v) / 2);
}

#endif//MGA_COREUTILITIES_H
