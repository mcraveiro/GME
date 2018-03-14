
#ifndef INTERFACECOLL_INCLUDED
#define INTERFACECOLL_INCLUDED

// included as C++
cpp_quote("#ifndef INTERFACECOLL_INCLUDED") 
cpp_quote("#define INTERFACECOLL_INCLUDED") 
cpp_quote("extern \"C++\"											")
cpp_quote("{														")
cpp_quote("	template<class COLL> struct TypeName_MgaColl2Elem		")
cpp_quote("	{														")
cpp_quote("		typedef IUnknown element_type;						")
cpp_quote("		typedef IUnknown collection_type;					")
cpp_quote("	};														")
cpp_quote("	template<class ELEM> struct TypeName_MgaElem2Coll		")
cpp_quote("	{														")
cpp_quote("		typedef IUnknown element_type;						")
cpp_quote("		typedef IUnknown collection_type;					")
cpp_quote("	};														")
cpp_quote("}														")

cpp_quote("#define TYPENAME_COLL2ELEM(COLL) typename ::TypeName_MgaColl2Elem<COLL>::element_type")
cpp_quote("#define TYPENAME_ELEM2COLL(ELEM) typename ::TypeName_MgaElem2Coll<ELEM>::collection_type")

#define MAKESTRING(ARG) #ARG

#define MGACOLL_INTERFACE2(IID, COLL, ELEM) \
[ \
	object, \
	uuid(IID), \
	dual, \
	helpstring(MAKESTRING(COLL Interface)), \
	pointer_default(unique) \
] \
interface COLL : IDispatch \
{ \
	[propget, helpstring("property Count")] \
	HRESULT Count([out, retval] long *p); \
\
	[propget, id(DISPID_VALUE), helpstring("property Item")] \
	HRESULT Item([in] long n, [out, retval] ELEM **p); \
\
	[propget, id(DISPID_NEWENUM), helpstring("property NewEnum")] \
	HRESULT _NewEnum([out, retval] IUnknown **p); \
\
	[helpstring("method GetAll")] \
	HRESULT GetAll([in] long count, [out, size_is(count)] ELEM **p); \
\
    [helpstring("method Insert, index starts with 1, element may already be contained")] \
	HRESULT Insert([in] ELEM *p, [in] long at); \
\
	[helpstring("method Append, add to the end, may already be contained")] \
	HRESULT Append([in] ELEM *p); \
\
	[helpstring("method Find, returns 0 if none found after pos start")] \
	HRESULT Find([in] ELEM *p, [in] long start, [out, retval] long *res); \
\
	[helpstring("method Remove, index starts with 1")] \
	HRESULT Remove([in] long n); \
}; \
cpp_quote("#ifdef __cplusplus") \
cpp_quote("extern \"C++\"    ") \
cpp_quote("{            ") \
cpp_quote(MAKESTRING(template<> struct ::TypeName_MgaColl2Elem<COLL>)) \
cpp_quote(MAKESTRING({ typedef COLL collection_type; typedef ELEM element_type; };)) \
cpp_quote(MAKESTRING(template<> struct ::TypeName_MgaElem2Coll<ELEM>)) \
cpp_quote(MAKESTRING({ typedef COLL collection_type; typedef ELEM element_type; };)) \
cpp_quote("}            ") \
cpp_quote("#endif")



#define MGACOLL_COCLASS2(CLASSID, COCLASS, COLL) \
[ \
	uuid(CLASSID), \
	helpstring(MAKESTRING(COCLASS Class)), \
	noncreatable \
] \
coclass COCLASS \
{ \
	[default] interface COLL; \
	interface ISupportErrorInfo; \
};

#define MGACOLL_INTERFACE(IID, NAME) \
	MGACOLL_INTERFACE2(IID, I##NAME##s, I##NAME)

#define MGACOLL_COCLASS(CLASSID, NAME) \
	MGACOLL_COCLASS2(CLASSID, NAME##s, I##NAME##s)


cpp_quote("#endif")

#endif//INTERFACECOLL_INCLUDED
