
#ifndef MGA_COMMONSTL_H
#define MGA_COMMONSTL_H

#include <vector>
#include <string>

#ifndef MGA_COMMONSMART_H
#include "CommonSmart.h"
#endif

// --------------------------- string

// we use "const string &" for [in] and "string &" for [in, out]

// these THROW exceptions

/* needed for MS STL
inline void CopyTo(const std::string::const_iterator i, int l, BSTR * b){}
inline void CopyTo(const std::string::const_iterator i, int l, VARIANT *v){}
inline void CopyTo(const std::string::const_iterator i, int l, CComBstrObj &a){}
inline void CopyTo(const std::string::const_iterator i, int l, CComVariant &a){}
inline void CopyTo(BSTR b, std::string::iterator i, int l) {}
inline void CopyTo(VARIANT &v, std::string::iterator i, int l) {}
*/
inline void CopyTo(const std::string &s, BSTR *b) { CopyTo( s.c_str(), (int)s.length(), b); }
inline void CopyTo(const std::string &s, VARIANT *v) { CopyTo(s.c_str(), (int)s.length(), v); }
inline void CopyTo(const std::string &s, CComBstrObj &a) { CopyTo(s.c_str(), (int)s.length(), a); }
inline void CopyTo(const std::string &s, CComVariant &a) { CopyTo(s.c_str(), (int)s.length(), a); }

inline void CopyTo(BSTR b, std::string &s)
{
	int l = GetCharLength(b);
	char *tmp = (char*)malloc(l);
	if (tmp == NULL)
		COMTHROW(E_OUTOFMEMORY);
	CopyTo(b, tmp, l);
	s.assign(tmp, l);
	free(tmp);
}

inline void CopyTo(VARIANT &v, std::string &s)
{
	int l = GetCharLength(v);
	char *tmp = (char*)malloc(l);
	if (tmp == NULL)
		COMTHROW(E_OUTOFMEMORY);
	CopyTo(v, tmp, l);
	s.assign(tmp, l);
	free(tmp);
}

void vFormat(std::string &s, const char *format, va_list args);
void vFormat(std::wstring &s, const wchar_t *format, va_list args);

inline void Format(std::string &s, const char *format, ...)
{
	va_list args;
	va_start(args, format);

	vFormat(s, format, args);
}

inline void Format(std::wstring &s, const wchar_t *format, ...)
{
	va_list args;
	va_start(args, format);

	vFormat(s, format, args);
}

typedef wchar_t* bstr_iterator;
typedef const wchar_t* bstr_const_iterator;

inline bstr_const_iterator begin(BSTR p) { return reinterpret_cast<wchar_t*>(p); }
inline bstr_const_iterator end(BSTR p) { return reinterpret_cast<wchar_t*>(p) ? (reinterpret_cast<wchar_t*>(p) + SysStringLen(p)) : 0; }

inline void CopyTo(bstr_const_iterator i, bstr_const_iterator e, CComBstrObj &b)
{
	ASSERT( i <= e );
	BSTR tmpb = SysAllocStringLen(i, (UINT)(e - i));
	if (tmpb == NULL)
		COMTHROW(E_OUTOFMEMORY);

	b.Attach(tmpb);
}

// --------------------------- bindata

typedef std::vector<unsigned char> bindata;

inline void CopyTo(const bindata &b, VARIANT *p) 
{
	if(b.empty())
	{
		unsigned char* pnull=NULL;
		CopyTo(pnull,pnull, p);
	}
	else
	{
		CopyTo(&b[0], (&b[0]) + b.size(), p); 
	}
}
inline void CopyTo(const bindata &b, CComVariant &a) { CopyTo(&b[0], (&b[0]) + b.size(), a); }

inline void CopyTo(VARIANT &v, bindata &b)
{
	if( v.vt == (VT_I4 | VT_ARRAY) )
	{
		b.resize(sizeof(long) * GetArrayLength(v));
		CopyTo(v, (long*)&b[0], (long*)((&b[0]) + b.size()) );
	}
	else
	{
		if(GetArrayLength(v)==0)
		{
			b.clear();
		}
		else
		{
			b.resize(GetArrayLength(v));
			CopyTo(v, &b[0], (&b[0]) + b.size());
		}
	}
}

inline void MoveTo(SAFEARRAY *p, std::vector<CComBstrObj> &bstrobjs)
{
	ASSERT( p != NULL );

	bstrobjs.resize(GetArrayLength(p));
	MoveTo(p, &bstrobjs[0], (&bstrobjs[0]) + bstrobjs.size());
}

inline void MoveTo(std::vector<CComBstrObj> &bstrobjs, SAFEARRAY **p)
{
	ASSERT( p != NULL && *p == NULL );

	MoveTo(&bstrobjs[0], (&bstrobjs[0]) + bstrobjs.size(), p);
}

inline void CopyTo(const std::vector<CComBstrObj> &bstrobjs, SAFEARRAY **p)
{
	ASSERT( p != NULL && *p == NULL );

	CopyTo(&bstrobjs[0], (&bstrobjs[0]) + bstrobjs.size(), p);
}

inline void CopyTo(const std::vector<GUID> &guids, SAFEARRAY **p)
{
	ASSERT( p != NULL && *p == NULL );

	if(guids.empty())
	{
		GUID* pnull=NULL;
		CopyTo(pnull,pnull,p);
	}
	else
	{
		CopyTo(&guids[0], (&guids[0]) + guids.size(), p);
	}
}

// --------------------------- Iterator

#ifdef _DEBUG

template<class CONTAINER>
inline bool IsValidIterator(const CONTAINER &container, typename CONTAINER::const_iterator i)
{
	CONTAINER::const_iterator b = container.begin();
	CONTAINER::const_iterator e = container.end();

	while( b != e )
	{
		if( b == i )
			return true;
		++b;
	}

	return false;
}

#endif

template<class CONTAINER>
inline int limited_size(const CONTAINER &container, int limit)
{
	ASSERT( limit >= 1 );
	int counter = limit;

	CONTAINER::const_iterator i = container.begin();
	CONTAINER::const_iterator e = container.end();
	while( i != e && --counter > 0 )
		++i;

	ASSERT( ( (int) container.size() < limit ? (int) container.size() : limit) == (limit - counter) );
	return limit - counter;
}

// --------------------------- Collection

template<class INTERFACE>
class CComObjPtrVector : public std::vector< CComObjPtr<INTERFACE> >
{
};

template<class ELEM>
void CopyTo( TYPENAME_ELEM2COLL(ELEM) *p, std::vector< CComObjPtr<ELEM> > &q)
{
	ASSERT( p != NULL );
	ASSERT( &q != NULL );

	q.clear();

	long count = 0;
	COMTHROW( p->get_Count(&count) );
	ASSERT( count >= 0 );

	if(count > 0)
	{
		//q.insert(q.begin(), count, CComObjPtr<ELEM>());
		q.resize(count);

		COMTHROW( p->GetAll(count, (ELEM**)&q[0]) );
	}
}

template<class ELEM>
class PutOutCollObj; // forward declaration for the template class 


// this template function will be befriended with the PutOutCollObj class, in VS.NET 2003 sucha friend needs to be declared before the class declaration
template<class ELEM>
PutOutCollObj<ELEM> PutOut( std::vector< CComObjPtr<ELEM> > & coll_vector);


template<class ELEM>
class PutOutCollObj
{
public:
	typedef ELEM elem_interface;
	typedef TYPENAME_ELEM2COLL(ELEM) coll_interface;
	
	typedef std::vector< CComObjPtr<elem_interface> > vector_type;

	operator coll_interface** () { return PutOut(coll_ptr); }

	~PutOutCollObj()
	{
		// we need this because COM methods might
		// not fill in the [out] parameter

		if( coll_ptr != NULL )
			CopyTo(coll_ptr, coll_vector);
	}
	
private:
	PutOutCollObj(vector_type &v) : coll_vector(v) { }

	friend PutOutCollObj<ELEM> PutOut<ELEM>( std::vector< CComObjPtr<ELEM> > &coll_vector);

	// to prevent misuse
	PutOutCollObj();
	PutOutCollObj(const PutOutCollObj<ELEM> &a);

public:
	CComObjPtr<coll_interface> coll_ptr;
	vector_type &coll_vector;
};

// this template function (friend of PutOutCollObj) needs to be forward declared!
template<class ELEM>
PutOutCollObj<ELEM> PutOut( std::vector< CComObjPtr<ELEM> > & coll_vector)
{
	return PutOutCollObj<ELEM>( coll_vector);
}

#endif//MGA_COMMONSTL_H
