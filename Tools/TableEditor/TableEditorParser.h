// TableEditorParser.h: interface for the CTableEditorParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TABLEEDITORPARSER_H__1CB08981_9ED5_4AFF_862A_71AD347CCEF9__INCLUDED_)
#define AFX_TABLEEDITORPARSER_H__1CB08981_9ED5_4AFF_862A_71AD347CCEF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GMECOM.h"
#include "Mga.h"
#include <xercesc/sax/HandlerBase.hpp>
XERCES_CPP_NAMESPACE_USE

class CTableEditorParser : public HandlerBase  
{
public:
	CTableEditorParser(IMgaProject* theProject);
	virtual ~CTableEditorParser();

	//Handler Base
public:
	//virtual InputSource *resolveEntity (const XMLCh* const publicId, const XMLCh* const systemId);

    virtual void startDocument();
    virtual void endDocument();

    virtual void startElement(const XMLCh* const name, AttributeList& attributes);
    virtual void endElement(const XMLCh* const name);

	virtual void characters(const XMLCh* const chars, const XMLSize_t length);

    virtual void error(const SAXParseException& exception);
    virtual void fatalError(const SAXParseException& exception);

	virtual void setDocumentLocator(const Locator *const locator);
 
	IMgaFCOs* ParseExcelXML(IMgaProject *p, CString filename); //must be for current project open, and all FCOs must previously exist

private:
	CComPtr<IMgaFCO> currentFCO;
	CComPtr<IMgaProject> currentProject;
	CComPtr<IMgaFCOs> importedFCOs;
	int currentColNum; //state of parser info
	int currentRowNum; //state of parser info
	int firstAttrColNum; //the start of Attribute/Value pairs of columns
	int objIDColNum;
	int objNameColNum;
	CString currentFCOname;
	CString currentAttrDName;
	BOOL inData; //only interested in CDATA in the <Data> tag
	CString cdata;

	BOOL SetAttr(CString DisplayedName, CString Value);

};

#endif // !defined(AFX_TABLEEDITORPARSER_H__1CB08981_9ED5_4AFF_862A_71AD347CCEF9__INCLUDED_)
