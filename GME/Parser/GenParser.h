
#ifndef MGA_GENPARSER_H
#define MGA_GENPARSER_H

#include <xercesc/sax/HandlerBase.hpp>
#include <comutil.h>

XERCES_CPP_NAMESPACE_USE

template<class DERIVED>
struct CGenParserFunc ;

class CMgaMetaParser;


// --------------------------- XmlStr

class XmlStr : public std::tstring
{
public:
	XmlStr() { };
	XmlStr(const XMLCh* const input);
	XmlStr(const XMLCh* const input, unsigned int len);
	XmlStr(XmlStr&& input) : std::tstring(std::move(input)) { }
};

// --------------------------- CGenParser

//struct CGenParserFunc_Base;

class CGenParser : public HandlerBase
{
public:
	typedef std::vector< std::pair<std::tstring, std::tstring> > attributes_type;
	typedef attributes_type::const_iterator attributes_iterator;


	CGenParser() : locator(NULL),/* elementfuncs(NULL),*/ err_line(0), err_column(0) { }

// ------- Erros

public:
	void SetErrorInfo2(HRESULT hr);
	void ThrowXmlError(const TCHAR *format, ...);

// ------- Handler Base

public:
	virtual InputSource *resolveEntity (const XMLCh* const publicId, const XMLCh* const systemId);

    virtual void startDocument();
    virtual void endDocument();

    virtual void startElement(const XMLCh* const name, AttributeList& attributes);
    virtual void endElement(const XMLCh* const name);

	virtual void characters(const XMLCh* const chars,const XMLSize_t);

    virtual void error(const SAXParseException& exception);
    virtual void fatalError(const SAXParseException& exception);

	virtual void setDocumentLocator(const Locator *const locator);
	virtual void fireStartFunction(const std::tstring& name, const attributes_type& attributes) = 0;
	virtual void fireEndFunction(const std::tstring& name) = 0;
	virtual void resetDocument() {
		// GME-441
		locator = NULL;
	}

public:
	const Locator *locator;

// ------- Attributes


	


	static const std::tstring *GetByNameX(const attributes_type &attributes, const TCHAR *name);

	static const std::tstring &GetByName(const attributes_type &attributes, const TCHAR *name)
	{
		const std::tstring *p = GetByNameX(attributes, name);
		const static std::tstring nullstr;

		if( p == NULL ) p = &nullstr;
//			HR_THROW(E_INVALID_DTD);

		return *p;
	}

	template<class INTERFACE, class FUNC_INTERFACE>
	void Attr(attributes_iterator i, const TCHAR *name, INTERFACE p, 
		HRESULT (__stdcall FUNC_INTERFACE::*func)(BSTR))
	{
		if( i->first == name )
		{
			FUNC_INTERFACE *q = p;
			ASSERT( q != NULL );

			COMTHROW( (q->*func)(PutInBstr(i->second)) );
		}
	}

	static long toLong(std::tstring s);
	static unsigned long toULong(std::tstring s);

	template<class INTERFACE, class FUNC_INTERFACE>
	void Attr(attributes_iterator i, const TCHAR *name, INTERFACE p,
		HRESULT (__stdcall FUNC_INTERFACE::*func)(long))
	{
		if( i->first == name )
		{
			FUNC_INTERFACE *q = p;
			ASSERT( q != NULL );

			HRESULT hr = (q->*func)(toLong(i->second));
			if (FAILED(hr))
			{
				throw_last_com_error(hr);
			}
		}
	}

	template<class INTERFACE, class FUNC_INTERFACE>
	void Attr(attributes_iterator i, const TCHAR *name, INTERFACE p,
	HRESULT (__stdcall FUNC_INTERFACE::*func)(unsigned long))
	{
		if( i->first == name )
		{
			FUNC_INTERFACE *q = p;
			ASSERT( q != NULL );

			COMTHROW( (q->*func)(toULong(i->second)) );
		}
	}

// ------- ElementFuncs

public:

	// Used to be: CGenParserFunc_Base *elementfuncs;
	//CGenParserFunc <CMgaMetaParser> *elementfuncs;

	void StartNone(const attributes_type &attributes) { }
	void EndNone() { }

// ------- Properties

public:
	std::tstring xmlfile;
	_bstr_t errorinfo;
	long err_line;
	long err_column;

	typedef unsigned long counter_type;
	counter_type counter;		// we do not advance the counter in GenParser

	struct element_type
	{
		std::tstring name;
		std::tstring chardata;
		CComObjPtr<IUnknown> object;
		std::tstring exstrinfo;
		long   exnuminfo;

		counter_type begin;
		counter_type end;
	};

	std::vector<element_type> elements;

public:
	element_type &GetCurrent() { ASSERT( !elements.empty() ); return elements.back(); }
	const element_type &GetPrevious() const
	{
		ASSERT( elements.size() >= 2 );

		std::vector<element_type>::const_iterator i = elements.end();
		--i; --i;
		return *i;
	}

	std::tstring &GetCurrData() { return GetCurrent().chardata; }
	const std::tstring &GetPrevName() const { return GetPrevious().name; }
	
	template<class T>
	void GetPrevObj(CComObjPtr<T> &obj) const
	{
		ASSERT( GetPrevious().object != NULL );
		COMTHROW( GetPrevious().object.QueryInterface(obj) );
		ASSERT( obj != NULL );
	}

	_bstr_t PutInBstrAttr(const attributes_type &attributes, const TCHAR *name)
	{
		const std::tstring *s = GetByNameX(attributes, name);
		if( s == NULL )
			return _bstr_t("");

		return _bstr_t(s->c_str());
	}
};

// --------------------------- CGenParserFunc
/*
struct CGenParserFunc_Base
{
	CGenParserFunc_Base(const char *n) : name(n) { }

	std::tstring name;

	virtual void Start(CGenParser *parser, const CGenParser::attributes_type &attributes) = 0;
	virtual void End(CGenParser *parser) = 0;
};
*/
template<class DERIVED>
struct CGenParserFunc //: public CGenParserFunc_Base
{
	typedef void (DERIVED::*StartFunc)(const CGenParser::attributes_type &);
	typedef void (DERIVED::*EndFunc)();

	CGenParserFunc(const TCHAR *n) : name(n) { }

	std::tstring name;

	virtual void Start(CGenParser *parser, const CGenParser::attributes_type &attributes)
	{ 
		(static_cast<DERIVED*>(parser)->*start)(attributes);  
	}

	virtual void End(CGenParser *parser)
	{
		(static_cast<DERIVED*>(parser)->*end)();
	}

	CGenParserFunc(const TCHAR *n, StartFunc s, EndFunc e) : 
		name(n), start(s), end(e) { }

	StartFunc start;
	EndFunc end;
};

#endif//MGA_GENPARSER_H
