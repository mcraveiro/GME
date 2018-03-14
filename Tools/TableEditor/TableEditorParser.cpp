// TableEditorParser.cpp: implementation of the CTableEditorParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TableEditorParser.h"
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
XERCES_CPP_NAMESPACE_USE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTableEditorParser::CTableEditorParser(IMgaProject* theProject):HandlerBase()
{
	currentProject = theProject;
	importedFCOs = NULL;
	cdata = "";

}

CTableEditorParser::~CTableEditorParser()
{

}

//initiate parsing
IMgaFCOs* CTableEditorParser::ParseExcelXML(IMgaProject *p, CString filename)
{
	try //catch moved up into GridDlg.cpp
	{
		XMLPlatformUtils::Initialize();

		SAXParser parser;
		//parser.setDoValidation(true);
		parser.setDocumentHandler(this);
		parser.setErrorHandler(this);
		parser.setEntityResolver(this);

		parser.parse(filename);

		//XMLPlatformUtils::Terminate(); 
	}
	catch(const XMLException &e)
	{
		CString desc(e.getMessage());
		AfxMessageBox(desc);
		throw;
	}

	return importedFCOs;

}

//XML Doc Handler stuff

void CTableEditorParser::startDocument()
{
	currentRowNum = 0;
	currentColNum = 0;
	firstAttrColNum = -1;
	objIDColNum = -1;
	objNameColNum = -1;
	inData = FALSE;
	cdata = "";

	COMTHROW(importedFCOs.CoCreateInstance(L"Mga.MgaFCOs"));
}

void CTableEditorParser::endDocument()
{
	//AfxMessageBox("EndDoc");
}

void CTableEditorParser::startElement(const XMLCh* const name, AttributeList& attributes)
{
	CString elementName(name);

	//if(elementName.Compare("Table")==0)
	if(elementName.Compare("Row")==0)
	{
		currentColNum = 0;
	}
	if(elementName.Compare("Data")==0)
	{
		inData = TRUE;
		cdata = "";
	}
	if(elementName.Compare("Cell")==0)
	{
		int len = attributes.getLength();
		for(int index = 0; index < len; index++)
		{
			CString name = attributes.getName(index);
			CString value = attributes.getValue(index);
			if(name.Compare("ss:Index")==0)//excel will not make <Cell> tags for empty cells
			{                              //including ss:Index means that some cells have been skipped over
				if(currentColNum>firstAttrColNum)//skipped some columns, so there was an empty attribute.
				{	
					SetAttr(currentAttrDName,"");
				}
				
				currentColNum=atoi(value)-1;//this code is 0-based, the ss:Index is 1-based
			}
		}
	}

}

void CTableEditorParser::endElement(const XMLCh* const name)
{
	CString elementName(name);

	if(elementName.Compare("Row")==0)//end of row, advance to next row, set col to 0
	{
		++currentRowNum;
		currentColNum = 0;
	}
	if(elementName.Compare("Cell")==0)
	{
		++currentColNum;
	}
	if(elementName.Compare("Data")==0)
	{
		inData = FALSE;
		cdata = "";
	}
}

void CTableEditorParser::characters(const XMLCh* const chars, const XMLSize_t length)
{

	CString newdata(chars);
//	newdata.Remove('\n'); //it appeared as if '\n' was added at the front and back of every string

//	if(cdata != "") //if multiline
//		cdata = cdata+"\n";

	cdata = cdata + newdata; //if it's multiline, this func gets called multiple times


	if(currentColNum == objNameColNum && inData)
	{
		currentFCOname = cdata;
	}

	if(currentColNum == objIDColNum && inData)
	{
		CComBSTR ID(cdata);
		currentFCO = 0;
		COMTHROW(currentProject->GetFCOByID(ID,&currentFCO));
		CComBSTR name(currentFCOname);
		COMTHROW(currentFCO->put_Name(name));

		COMTHROW(importedFCOs->Append(currentFCO));
	}


	if(currentRowNum==0) //special stuff for first row, need to find what col ID, Name and First attr are in
	{
		if(cdata.Compare("Object Name")==0)
			objNameColNum = currentColNum;
		if(cdata.Compare("Object ID")==0)
			objIDColNum = currentColNum;
		if(cdata.Compare("Attribute")==0 && firstAttrColNum == -1)
			firstAttrColNum = currentColNum;
	}

	//finally, the attributes
	if(currentRowNum > 0 && currentColNum >= firstAttrColNum && inData)
	{

		if((currentColNum-firstAttrColNum)%2==0)//Attribute Column
		{
			currentAttrDName = cdata;
		}
		else //Value Column
		{
			CString tmp = cdata;
			tmp.Replace("\n\n","\n");//somehow the newlines are doubling
			SetAttr(currentAttrDName,tmp);
		}
	}

}

void CTableEditorParser::error(const SAXParseException& exception)
{
	CString desc(exception.getMessage());
	AfxMessageBox(desc);
}

void CTableEditorParser::fatalError(const SAXParseException& exception)
{
	CString desc(exception.getMessage());
	AfxMessageBox(desc);
}

void CTableEditorParser::setDocumentLocator(const Locator *const locator)
{
}

BOOL CTableEditorParser::SetAttr(CString DisplayedName, CString Value)
{

	CComPtr<IMgaAttributes> attrs; //we only know the displayed name, so have to check through the meta on each attr
	COMTHROW(currentFCO->get_Attributes(&attrs));
	// TODO: memoize

	MGACOLL_ITERATE(IMgaAttribute,attrs)
	{

		CComPtr<IMgaAttribute> attr = MGACOLL_ITER;
		CComPtr<IMgaMetaAttribute> metaAttr;
		COMTHROW(attr->get_Meta(&metaAttr));

		CComBSTR metaDisplayedName;
		COMTHROW(metaAttr->get_DisplayedName(&metaDisplayedName));

		if(DisplayedName.Compare(CString(metaDisplayedName))==0)
		{
			attval_enum AttrType;
			COMTHROW(metaAttr->get_ValueType(&AttrType));

			CComBSTR StringVal = Value;
			CComBSTR prevStringVal;
			long IntVal = atoi(Value);
			long prevIntVal;
			double DoubleVal = atof(Value);
			double prevDoubleVal;
			VARIANT_BOOL BoolVal = (atoi(Value)==1);
			VARIANT_BOOL prevBoolVal;
			switch(AttrType)
			{
				case ATTVAL_NULL:
					break;
				case ATTVAL_STRING:
					COMTHROW(attr->get_StringValue(&prevStringVal));
					if(CString(prevStringVal).Compare(CString(StringVal))!=0)//only put if there's a change
						COMTHROW(attr->put_StringValue(StringVal));
					break;
				case ATTVAL_INTEGER:
					COMTHROW(attr->get_IntValue(&prevIntVal));
					if(prevIntVal != IntVal)
						COMTHROW(attr->put_IntValue(IntVal));
					break;
				case ATTVAL_DOUBLE:
					COMTHROW(attr->get_FloatValue(&prevDoubleVal));
					if(prevDoubleVal != DoubleVal)
						COMTHROW(attr->put_FloatValue(DoubleVal));
					break;
				case ATTVAL_BOOLEAN:
					COMTHROW(attr->get_BoolValue(&prevBoolVal));
					if(((prevBoolVal == 0) != (BoolVal == 0))) //Variant_bools are 0 for false, nonzero for true, 
						COMTHROW(attr->put_BoolValue(BoolVal)); //I want this to evaluate if prevBoolVal and BoolVal are different (true and false)
					break;
				case ATTVAL_REFERENCE:
					break;
				case ATTVAL_ENUM:
					COMTHROW(attr->get_StringValue(&prevStringVal));
					if(CString(prevStringVal).Compare(CString(StringVal))!=0)
						COMTHROW(attr->put_StringValue(StringVal));
					break;
				case ATTVAL_DYNAMIC:
					break;
			}
			return TRUE;
		}

	}MGACOLL_ITERATE_END

	return FALSE;
}