
#include "stdafx.h"
#include "GenParser.h"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/Locator.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework\MemBufInputSource.hpp>
#include <stdio.h>
#include <string>
#include "atlstr.h"
#include <comutil.h>

// --------------------------- XmlStr

#ifdef UNICODE
XmlStr::XmlStr(const XMLCh* const input) : std::tstring(input)
{
}

XmlStr::XmlStr(const XMLCh* const input, unsigned int len) : std::tstring(input)
{
	resize(len);
}
#else
XmlStr::XmlStr(const XMLCh* const input)
{
	resize(GetCharLength(input, -1));
	CopyTo(input, -1, &(operator[](0)), length());
	ASSERT( operator[](length()-1) == 0 );
	erase( length() - 1); // was: pop_back();
}

XmlStr::XmlStr(const XMLCh* const input, unsigned int len)
{
	resize(GetCharLength(input, len));
	CopyTo(input, len, &(operator[](0)), length());
}
#endif

void CGenParser::SetErrorInfo2(HRESULT hr)
{
	_bstr_t bstr;
	GetErrorInfo(hr, bstr.GetAddress());
	std::tstring str;
	Format(str, _T("in file %s at line %ld, char %ld"), 
		xmlfile.c_str(), err_line, err_column);

	if (bstr.length())
	{
		str = std::tstring(static_cast<const TCHAR*>(bstr)) + _T(": ") + str;
	}

	SetErrorInfo(hr, _bstr_t(str.c_str()));
}

void CGenParser::ThrowXmlError(const TCHAR *format, ...)
{
	ASSERT( format != NULL );

	va_list args;
	va_start(args, format);

	std::tstring desc;
	vFormat(desc, format, args);

	std::tstring str;
	if( locator != NULL )
		Format(str, 
			_T("XML parser error in file %s at line %d, char %d : %s"),
			xmlfile.c_str(), (int)locator->getLineNumber(),
			(int)locator->getColumnNumber(), desc.c_str());
	else
		Format(str,
			_T("XML parser error in file %s : %s"), xmlfile.c_str(), desc.c_str());

	errorinfo = str.c_str();
	SetErrorInfo(const_cast<TCHAR*>(str.c_str()));
	HR_THROW(E_XMLPARSER);
}

// ------- Handler Base

InputSource *CGenParser::resolveEntity(const XMLCh* const publicId, const XMLCh* const systemId)
{
	do
	{
		XmlStr sysid = systemId;
		std::tstring syssid = sysid;

		HMODULE hm = GetModuleHandle(_T("PARSER.DLL"));
		if( hm == NULL )
			break;

		HRSRC res = FindResource(hm, syssid.c_str(), _T("DTD"));
		if(res) {
			XMLByte *bytes = (XMLByte *)LockResource(LoadResource(hm, res));
			if (!bytes)
				COMTHROW(E_INVALID_DTD);
			return new MemBufInputSource(bytes, SizeofResource(hm, res), syssid.c_str());
		}

		
		TCHAR filename[MAX_PATH];
		int a = GetModuleFileName(hm, filename, MAX_PATH);
		a -= 10;
		if( a <= 0 )
			break;

		if( _tcsicmp(filename + a, _T("PARSER.DLL")) != 0 )
			break;

		_tcscpy(filename + a, sysid.c_str());

		FILE *file = _tfopen(filename, _T("r"));
		if( file == NULL )
			break;

		fclose(file);

		return new LocalFileInputSource(PutInBstr(filename));

	} while(false);

	return HandlerBase::resolveEntity(publicId, systemId);
}

void CGenParser::startDocument()
{
	counter = 0;
}

void CGenParser::endDocument()
{
	locator = NULL;
}

void CGenParser::startElement(const XMLCh* const name, AttributeList& attrlist)
{
#ifdef _DEBUG
	int cur_line, cur_column;
	if( locator != NULL )
	{
		cur_line = (int)locator->getLineNumber();
		cur_column = (int)locator->getColumnNumber();
		// CString msg;
		// msg.Format(_T("Line %d, Col: %d\n"), cur_line, cur_column);
		// OutputDebugString(msg);
	}
#endif

	try
	{
		attributes_type attributes;

		unsigned int len = (unsigned int)attrlist.getLength();
		for(unsigned int index = 0; index < len; index++)
		{
			attributes.push_back( std::pair<std::tstring,std::tstring>(
				XmlStr(attrlist.getName(index)), XmlStr(attrlist.getValue(index))) );
		}

		XmlStr namestr(name);

		elements.push_back(element_type());
		elements.back().name = namestr;
		elements.back().begin = counter;
/*
		for(unsigned int index = 0; !elementfuncs[index].name.empty(); index++)
		{
			if( namestr == elementfuncs[index].name )
			{
				elementfuncs[index].Start(this, attributes);
				break;
			}
		}
		*/
	
	fireStartFunction(namestr, attributes);
	}
	catch (hresult_exception &)
	{
		if( locator != NULL )
		{
			err_line = locator->getLineNumber();
			err_column = locator->getColumnNumber();

			// we compose and set the error message for exceptions
			// [which come from the MGA layer because of meta incompatibility]
			std::tstring str;
			Format(str, _T("Improper use of object error in file %s at line %d, char %d."), xmlfile.c_str(), err_line, err_column);
			errorinfo = str.c_str();
		}

		throw;
	}
	catch (_com_error& e)
	{
		std::tstring str;
		if( locator != NULL )
		{
			err_line = locator->getLineNumber();
			err_column = locator->getColumnNumber();

			// we compose and set the error message for exceptions
			// [which come from the MGA layer because of meta incompatibility]
			Format(str, _T("In file %s at line %d, char %d: "), xmlfile.c_str(), err_line, err_column);
			errorinfo = str.c_str();
		}
		if (e.Description() != _bstr_t())
		{
			errorinfo = (str + static_cast<const TCHAR*>(e.Description())).c_str();
			throw_com_error(e.Error(), errorinfo);
		}
		throw hresult_exception(e.Error());
	}
}


void CGenParser::endElement(const XMLCh* const name)
{
	try
	{
		XmlStr namestr(name);

		ASSERT( !elements.empty() );

//	we modify the names, see clipboard
//	ASSERT( elements.back().name == namestr );

		elements.back().end = counter;
/*
		for(int index = 0; !elementfuncs[index].name.empty(); index++)
		{
			if( namestr == elementfuncs[index].name )
			{
				elementfuncs[index].End(this);
				break;
			}
		}
		*/

		fireEndFunction(namestr);

		elements.pop_back();
	}
	catch(hresult_exception &)
	{
		if( locator != NULL )
		{
			err_line = locator->getLineNumber();
			err_column = locator->getColumnNumber();

			// we compose and set the error message for exceptions
			// [which come from the MGA layer because of meta incompatibility]
			std::tstring str;
			Format(str, _T("Improper use of object error in file %s at line %ld, char %ld."), xmlfile.c_str(), err_line, err_column);
			errorinfo = str.c_str();
		}

		throw;
	}
	catch (_com_error& e)
	{
		std::tstring str;
		if( locator != NULL )
		{
			err_line = locator->getLineNumber();
			err_column = locator->getColumnNumber();

			// we compose and set the error message for exceptions
			// [which come from the MGA layer because of meta incompatibility]
			Format(str, _T("In file %s at line %d, char %d: "), xmlfile.c_str(), err_line, err_column);
			errorinfo = str.c_str();
		}
		if (e.Description() != _bstr_t())
		{
			errorinfo = (str + static_cast<const TCHAR*>(e.Description())).c_str();
			throw_com_error(e.Error(), errorinfo);
		}
		throw hresult_exception(e.Error());
	}
}

void CGenParser::characters(const XMLCh* const chars, const XMLSize_t length)
{
	if( !elements.empty() )
		elements.back().chardata += XmlStr(chars, length);
}

void CGenParser::error(const SAXParseException &e)
{
	ThrowXmlError(
		_T("(at line %d, char %d) %s"),
		(int)e.getLineNumber(),(int)e.getColumnNumber(),
		XmlStr(e.getMessage()).c_str());
}

void CGenParser::fatalError(const SAXParseException &e)
{
	error(e);
}

void CGenParser::setDocumentLocator(const Locator *const loc)
{
	locator = loc;
}

// ------- Attributes

const std::tstring *CGenParser::GetByNameX(const attributes_type &attributes, const TCHAR *name)
{
	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		if( (*i).first == name )
			return &(*i).second;

		++i;
	}

	return NULL;
}

long CGenParser::toLong(std::tstring s)
{
	const TCHAR *c = s.c_str();
	TCHAR *e;

	long a = _tcstol(c, &e, 0);

	if( (e - c) != (int) s.length() )
		HR_THROW(E_INVALID_XML_LONG);

	return a;
}

unsigned long CGenParser::toULong(std::tstring s)
{
	const TCHAR *c = s.c_str();
	TCHAR *e;

	long a = _tcstoul(c, &e, 0);

	if( (e - c) != (int) s.length() )
		HR_THROW(E_INVALID_XML_LONG);

	return a;
}