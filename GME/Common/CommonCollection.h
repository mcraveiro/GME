
#ifndef MGA_COMMONCOLLECTION_H
#define MGA_COMMONCOLLECTION_H

#include <vector>
#include <list>//slist

template<class ITFTYPE, class OBJTYPE>
struct CopyItfFromObj
{
	static HRESULT copy(ITFTYPE **p, OBJTYPE *const*q)
	{
		ASSERT( p != NULL && q != NULL);
		if( *p != NULL )
			COMRETURN(E_POINTER);

		if( *q == NULL )
		{
			*p = NULL; 
			return S_OK;
		}

		return (*q)->QueryInterface(__uuidof(ITFTYPE), (void**)p);
	}

	static void init(ITFTYPE **p) 
	{
		ASSERT( p != NULL );

		*p = NULL;
	}

	static void destroy(ITFTYPE **p) 
	{
		ASSERT( p != NULL );

		if( *p )
		{
			(*p)->Release();
			*p = NULL;
		}
	}
};

template<class ITFTYPE>
struct CopyItfFromItf
{
	static HRESULT copy(ITFTYPE **p, ITFTYPE *const*q)
	{
		ASSERT( p != NULL && q != NULL);

		if( *p != NULL )
			COMRETURN(E_POINTER);

		*p = *q;
		if(*p) 
			(*p)->AddRef();

		return S_OK;
	}

	static void init(ITFTYPE **p)
	{
		ASSERT( p != NULL );

		*p = NULL;
	}

	static void destroy(ITFTYPE **p)
	{
		ASSERT( p != NULL );

		if( *p )
		{
			(*p)->Release();
			*p = NULL;
		}
	}
};

template<class OBJTYPE>
struct CopyDispVariantFromObj
{
	static HRESULT copy(VARIANT *p, OBJTYPE *const*q)
	{
		ASSERT( p != NULL && q != NULL);

		if( p->vt != VT_EMPTY )
			COMRETURN(E_POINTER);

		if( *q == NULL )
			return S_OK;		// leave as VT_EMPTY !!

		HRESULT hr = (*q)->QueryInterface(IID_IDispatch, (void**)&p->pdispVal);
		if( SUCCEEDED(hr) )
			p->vt = VT_DISPATCH;

		return hr;
	}

	static void init(VARIANT *p) { VariantInit(p); }
	static void destroy(VARIANT *p) { VariantClear(p); }
};

template<class ITFTYPE>
struct CopyDispVariantFromItf
{
	static HRESULT copy(VARIANT *p, ITFTYPE *const * q)
	{
		ASSERT( p != NULL && q != NULL);

		if( p->vt != VT_EMPTY )
			COMRETURN(E_POINTER);

		if(*q == NULL)
			return S_OK;		// leave as VT_EMPTY !!

		HRESULT hr = (*q)->QueryInterface(IID_IDispatch, (void**)&p->pdispVal);
		if( SUCCEEDED(hr) )
			p->vt = VT_DISPATCH;

		return hr;
	}

	static void init(VARIANT *p) { VariantInit(p); }
	static void destroy(VARIANT *p) { VariantClear(p); }
};

template<class COLLTYPE, class OBJTYPE>
class ATL_NO_VTABLE CComEnumDispVariantOnObjColl :
	public CComEnumOnSTL<IEnumVARIANT, &IID_IEnumVARIANT,
	VARIANT, CopyDispVariantFromObj<OBJTYPE>,
	COLLTYPE>
{
};

template<class COLLTYPE, class ITFTYPE>
class ATL_NO_VTABLE CComEnumDispVariantOnItfColl :
	public CComEnumOnSTL<IEnumVARIANT, &IID_IEnumVARIANT,
	VARIANT, CopyDispVariantFromItf<ITFTYPE>, COLLTYPE>
{
};


template<class ITFTYPE> 
void push_best(std::list<ITFTYPE*> &into, ITFTYPE *o) { into.push_front(o); }//slist
template<class ITFTYPE> 
void push_best(std::vector<ITFTYPE*> &into, ITFTYPE *o) { into.push_back(o); }

template<class COLLITF, class COLLTYPE, class ITFTYPE, class OBJTYPE>
class ATL_NO_VTABLE CCoreCollection;

class __declspec(uuid("9FE3BB80-B596-41BA-910F-0FF9C3B4F38A")) CoreCollectionHandlerCLSID;

template<class COLLITF, class COLLTYPE, class ITFTYPE, class OBJTYPE>
class CCoreCollectionMarshalTearOff : 
	public CComTearOffObjectBase<CCoreCollection<COLLITF, COLLTYPE, ITFTYPE, OBJTYPE>>,
	IStdMarshalInfo
{
BEGIN_COM_MAP(CCoreCollectionMarshalTearOff)
	COM_INTERFACE_ENTRY(IStdMarshalInfo)
END_COM_MAP()

    public:
        virtual HRESULT STDMETHODCALLTYPE GetClassForHandler( 
            __in  DWORD dwDestContext,
            __reserved  void *pvDestContext,
            __out  CLSID *pClsid)
	{
		*pClsid = __uuidof(CoreCollectionHandlerCLSID);
		return S_OK;
	}
};


template<class COLLITF, class COLLTYPE, class ITFTYPE, class OBJTYPE>
class ATL_NO_VTABLE CCoreCollection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public ICollectionOnSTLImpl<
	IDispatchImpl<COLLITF, &__uuidof(COLLITF)>,
	COLLTYPE, ITFTYPE*, CopyItfFromItf<ITFTYPE>,
	CComEnumDispVariantOnItfColl<COLLTYPE, ITFTYPE> >,
	public ISupportErrorInfoImpl<&__uuidof(COLLITF)>
{
public:
	typedef CCoreCollection<COLLITF, COLLTYPE, ITFTYPE, OBJTYPE> CLASS;
	typedef typename COLLTYPE::iterator ITERTYPE;
	typedef CopyItfFromObj<ITFTYPE, OBJTYPE> FILL_COPYTYPE;
	typedef CopyItfFromItf<ITFTYPE> GETALL_COPYTYPE;
	typedef typename CCoreCollectionMarshalTearOff<COLLITF, COLLTYPE, ITFTYPE, OBJTYPE> ColTearOff;

BEGIN_COM_MAP(CLASS)
	COM_INTERFACE_ENTRY(COLLITF)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_TEAR_OFF(IID_IStdMarshalInfo, ColTearOff)
END_COM_MAP()

public:
	~CCoreCollection()
	{
		ITERTYPE i = m_coll.begin();
		ITERTYPE e = m_coll.end();

		while( i != e) 
			FILL_COPYTYPE::destroy(&*i++);
	}

	COLLTYPE &GetCollection() { return m_coll; }

	STDMETHODIMP GetAll(long count, ITFTYPE **p)
	{
		ITERTYPE i = m_coll.begin();
		ITERTYPE e = m_coll.end();
		long copied = 0;
		HRESULT hr = S_OK;

		while( i != e && copied < count )
		{
			ASSERT( *(p+copied) == NULL );
			hr = GETALL_COPYTYPE::copy(p+copied, &*i);
			if( FAILED(hr) )
				break;

			++copied;
			++i;
		}

		if( FAILED(hr) || i != e || copied != count )
		{
			while( copied > 0 )
			{
				GETALL_COPYTYPE::destroy(p+copied);
				--copied;
			}

			if( SUCCEEDED(hr) )
				hr = E_GETALL;
		}

		if( FAILED(hr) )
			SetErrorInfo(hr);

		return hr;
	}

	template<class ITERTYPE2>
	void Fill(ITERTYPE2 i, ITERTYPE2 e)
	{
		while( i != e )
		{
			ITFTYPE *o = NULL;
			COMTHROW( FILL_COPYTYPE::copy(&o, &*i) );
			push_best<ITFTYPE>(m_coll, o);
			++i;
		}
	}

	template<class COLLTYPE_>
	void Reserve(COLLTYPE_& col, size_t num_additional) { }
	template<>
	void Reserve(std::vector<ITFTYPE*>& col, size_t num_additional)
	{
		col.reserve(col.size() + num_additional);
	}

	template<class COLLTYPE2>
	void FillAll(COLLTYPE2& col)
	{
		Reserve(m_coll, col.size());
		Fill(col.begin(), col.end());
	}

	template<class COLLTYPE2>
	void FillAllNoReserve(COLLTYPE2& col)
	{
		Fill(col.begin(), col.end());
	}

	void Add(OBJTYPE *i)
	{
		ASSERT( i != NULL );

		ITFTYPE *o = NULL;
		COMTHROW( FILL_COPYTYPE::copy(&o, &i) );
		push_best<ITFTYPE>(m_coll, o);
	}

//start and index are 1-based, index is 0 if not found
	STDMETHOD(Find)(ITFTYPE* pvar, long start, long *index) {

		if(start == 0) start = (long) m_coll.size();
		else start--;
		if((unsigned long)start > m_coll.size()) return E_FAIL;
		if (pvar == NULL)
			return E_POINTER;
		CComPtr<ITFTYPE> pp = pvar;
		COLLTYPE::iterator iter = m_coll.begin()+start;
		while (iter != m_coll.end()) {
			if(pvar == *iter) {
				*index = (long)(m_coll.begin() - iter + 1);
				return S_OK;
			}
			++iter;
		}
		*index = 0;
		return E_NOTFOUND;
	}

//pos is 1-based  0 means 'to the end'
	STDMETHOD(Insert)(ITFTYPE* pvar, long pos) {
		// this fuction must be supplied by the user for CollectionEx to compile
		// extern HRESULT check_location_compatibility(ITFTYPE *newobj, ITFTYPE *oldobj);
		if(pos == 0) pos = (long)m_coll.size();
		else pos--;
		if((unsigned long)pos > m_coll.size()) return E_FAIL;
		if (pvar == NULL)
			return E_POINTER;

		// HRESULT hr = E_FAIL;
		HRESULT hr = check_location_compatibility(pvar, m_coll.size() ? m_coll.front() : NULL);
		if(hr != S_OK) return hr;

		COLLTYPE::iterator iter = m_coll.insert(m_coll.begin()+ pos, static_cast<ITFTYPE*>(NULL));
		hr = GETALL_COPYTYPE::copy(&*iter, &pvar);
		return hr;
	}

	STDMETHOD(Append)(ITFTYPE* pvar) {
		return Insert(pvar,0);
	}

	STDMETHOD(Remove)(long pos) {
		if(pos == 0) pos = (long)m_coll.size();
		else pos--;
		if((unsigned long)pos > m_coll.size()) return E_FAIL;
		GETALL_COPYTYPE::destroy(&(m_coll[0])+pos);
		m_coll.erase(m_coll.begin()+pos);
		return S_OK;
	}

};


template<class COLLITF, class COLLTYPE, class ITFTYPE, class OBJTYPE, const CLSID *CLSIDP, const int IDR>
class ATL_NO_VTABLE CCoreCollectionEx : public CCoreCollection<COLLITF, COLLTYPE, ITFTYPE, OBJTYPE>,
	public CComCoClass<CCoreCollectionEx<COLLITF, COLLTYPE, ITFTYPE, OBJTYPE, CLSIDP, IDR>, CLSIDP> {
public:
	typedef CCoreCollectionEx<COLLITF, COLLTYPE, ITFTYPE, OBJTYPE, CLSIDP, IDR> EXCLASS;

DECLARE_REGISTRY_RESOURCEID(IDR)

};

template<class COLLITF, class ITFTYPE>
void GetAll(COLLITF *coll, std::vector< CComObjPtr<ITFTYPE> > &ret)
{
	ASSERT( coll != NULL );
	ASSERT( ret.empty() );

	long count = 0;
	COMTHROW( coll->get_Count(&count) );
	ASSERT( count >= 0 );

	ret.clear();
	ret.insert(ret.begin(), count, NULL);

	COMTHROW( coll->GetAll(count, (ITFTYPE**)&ret[0]) );
}

inline HRESULT check_location_compatibility(IUnknown *, IUnknown *) { return S_OK; }


#endif//MGA_COMMONCOLLECTION_H
