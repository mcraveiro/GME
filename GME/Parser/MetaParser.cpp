
#include "stdafx.h"
#include "Parser.h"
#include "MetaParser.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/SAXParser.hpp>

#include <stdio.h>
#include <fstream>//fstream.h
// --------------------------- CMgaMetaParser

CMgaMetaParser::CMgaMetaParser():currentPass(FIRST_PASS)
{
}

CMgaMetaParser::~CMgaMetaParser()
{
}

// ------- Methods

STDMETHODIMP CMgaMetaParser::Parse(BSTR filename, BSTR connection)
{
	try
	{
		ASSERT( metaproject == NULL );
		COMTHROW( metaproject.CoCreateInstance(L"Mga.MgaMetaProject") );

		COMTHROW( metaproject->Create(connection) );
		COMTHROW( metaproject->BeginTransaction() );

		CopyTo(filename, xmlfile);


		XMLPlatformUtilsTerminate_RAII term;
		try
		{
			foundconstraints = false;
			explicitguid = false;
			{
				SAXParser parser;
				//parser.setDoValidation(true);
				parser.setValidationScheme(SAXParser::Val_Always);
				parser.setDocumentHandler(this);
				parser.setErrorHandler(this);
				parser.setEntityResolver(this);

				
				//elementfuncs = elementfuncs_firstpass;
				currentPass = FIRST_PASS;
				parser.parse(xmlfile.c_str());
			}

			{
				SAXParser parser;
				parser.setValidationScheme(SAXParser::Val_Never);
				parser.setDocumentHandler(this);
				parser.setErrorHandler(this);
				parser.setEntityResolver(this);

				
				//elementfuncs = elementfuncs_secondpass;
				currentPass = SECOND_PASS;
				parser.parse(xmlfile.c_str());
			}
			if (!explicitguid) {
				// Old pradigm guid logic
//calculate checksum:
				int sum = 0, cyc = 0;
				std::ifstream inf(xmlfile.c_str());
				while(inf.good()) {
					int l = inf.get();
					if(inf.eof()) break;
					sum += l << cyc;
					cyc = (cyc + 1) % 20;
				}
				sum += 1;    // compensate for meta bug in registering MetaID-s (January 2002)
//
				OLECHAR buf[40];
				swprintf(buf, 40, L"{%08X-DEAD-BEEF-FEED-DAD00000000%c}",sum, foundconstraints?'1':'0');
				CComBstrObj bstr(buf);
				
				GUID guid;
				CopyTo(bstr, guid);
	
				CComVariant v;
				CopyTo(guid, v);
				
	
				COMTHROW( metaproject->put_GUID(v) );
			}
		}
	    catch(const XMLException &e)
		{
			XmlStr desc(e.getMessage());

			ThrowXmlError(L"%s", desc.c_str());
		}

		COMTHROW( metaproject->CommitTransaction() );

		HRESULT hr = CloseAll();
		if (FAILED(hr))
			return hr; // IErrorInfo already set by metaproject->Close()
	}
	catch(hresult_exception &e)
	{
		if( metaproject != NULL )
			metaproject->AbortTransaction();

		CloseAll();

		ASSERT( FAILED(e.hr) );
		if( e.hr == E_XMLPARSER )
			SetErrorInfo(errorinfo);
		else
			SetErrorInfo2(e.hr);

		return e.hr;
	}
	return S_OK;
}

HRESULT CMgaMetaParser::CloseAll()
{
	HRESULT hr;

	elements.clear();

	if (metaproject != NULL)
		hr = metaproject->Close();
	else
		hr = S_OK;

	metaproject = NULL;
	return hr;
};

// ------- Attributes

const std::tstring CMgaMetaParser::GetNextToken(std::tstring::const_iterator &i,
	std::tstring::const_iterator &e, std::tstring::const_iterator end)
{
	i = e;
	while( i != end && *i == ' ' )
		++i;

	e = i;
	while( e != end && *e != ' ' )
		++e;

	return std::tstring(i, e);
}


void CMgaMetaParser::fireStartFunction(const std::tstring & namestr, const attributes_type& attributes)
{
	if(currentPass == FIRST_PASS)
	{
		for(unsigned int index = 0; !elementfuncs_firstpass[index].name.empty(); index++)
		{
				if( namestr == elementfuncs_firstpass[index].name )
				{
					elementfuncs_firstpass[index].Start(this, attributes);
					break;
				}
		}
	}
	else
	{
		for(unsigned int index = 0; !elementfuncs_secondpass[index].name.empty(); index++)
		{
				if( namestr == elementfuncs_secondpass[index].name )
				{
					elementfuncs_secondpass[index].Start(this, attributes);
					break;
				}
		}

	}
}


void CMgaMetaParser::fireEndFunction(const std::tstring & namestr)
{
	if(currentPass == FIRST_PASS)
	{
		for(unsigned int index = 0; !elementfuncs_firstpass[index].name.empty(); index++)
		{
				if( namestr == elementfuncs_firstpass[index].name )
				{
					elementfuncs_firstpass[index].End(this);
					break;
				}
		}
	}
	else
	{
		for(unsigned int index = 0; !elementfuncs_secondpass[index].name.empty(); index++)
		{
				if( namestr == elementfuncs_secondpass[index].name )
				{
					elementfuncs_secondpass[index].End(this);
					break;
				}
		}

	}
}


// ------- ElementFuncs

CMgaMetaParser::elementfunc CMgaMetaParser::elementfuncs_firstpass[] = 
{
	elementfunc(_T("paradigm"), &StartParadigm, &EndNone),
	elementfunc(_T("comment"), &StartNone, &EndComment),
	elementfunc(_T("author"), &StartNone, &EndAuthor),
	elementfunc(_T("dispname"), &StartNone, &EndDispName),
	elementfunc(_T("folder"), &StartFolder, &EndNone),
	elementfunc(_T("atom"), &StartAtom, &EndNone),
	elementfunc(_T("model"), &StartModel, &EndNone),
	elementfunc(_T("connection"), &StartConnection, &EndNone),
	elementfunc(_T("reference"), &StartReference, &EndNone),
	elementfunc(_T("set"), &StartSet, &EndNone),
	elementfunc(_T("attrdef"), &StartAttrDef, &EndNone),
	elementfunc(_T("regnode"), &StartRegNode, &EndNone),
	elementfunc(_T("connjoint"), &StartConnJoint, &EndNone),
	elementfunc(_T("pointerspec"), &StartPointerSpec, &EndNone),
	elementfunc(_T("pointeritem"), &StartPointerItem, &EndNone),
	elementfunc(_T("enumitem"), &StartEnumItem, &EndNone),
	elementfunc(_T("constraint"), &StartConstraint, &EndConstraint),
	elementfunc(_T(""), NULL, NULL)
};

CMgaMetaParser::elementfunc CMgaMetaParser::elementfuncs_secondpass[] = 
{
	elementfunc(_T("folder"), &StartFolder2, &EndNone),
	elementfunc(_T("model"), &StartFCO2, &EndNone),
	elementfunc(_T("atom"), &StartFCO2, &EndNone),
	elementfunc(_T("connection"), &StartFCO2, &EndNone),
	elementfunc(_T("reference"), &StartFCO2, &EndNone),
	elementfunc(_T("set"), &StartFCO2, &EndNone),
	elementfunc(_T("role"), &StartRole2, &EndNone),
	elementfunc(_T("aspect"), &StartAspect2, &EndNone),
	elementfunc(_T("part"), &StartPart2, &EndNone),
	elementfunc(_T("regnode"), &StartRegNode, &EndNone),
	elementfunc(_T("dispname"), &StartNone, &EndDispName),
	elementfunc(_T(""), NULL, NULL)
};

// ------- Element Handlers

void CMgaMetaParser::StartParadigm(const attributes_type &attributes)
{
	GetCurrent().object = metaproject;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("name"), metaproject, &IMgaMetaProject::put_Name);
		Attr(i, _T("version"), metaproject, &IMgaMetaProject::put_Version);
		
		/*  New Paradigm GUID logic */	
		if( i->first == _T("guid") )
		{
			_bstr_t bstr = i->second.c_str();

			GUID guid;
			CopyTo(bstr, guid);

			CComVariant v;
			CopyTo(guid, v);

			COMTHROW( metaproject->put_GUID(v) );
			explicitguid = true;
		}

		Attr(i, _T("cdate"), metaproject, &IMgaMetaProject::put_CreatedAt);
		Attr(i, _T("mdate"), metaproject, &IMgaMetaProject::put_ModifiedAt);

		++i;
	}
}

void CMgaMetaParser::EndComment()
{
	if( GetPrevName() == _T("paradigm") )
	{
		COMTHROW( metaproject->put_Comment(PutInBstr(GetCurrData())) );
	}
	else
		HR_THROW(E_INVALID_DTD);
}

void CMgaMetaParser::EndAuthor()
{
	if( GetPrevName() == _T("paradigm") )
	{
		COMTHROW( metaproject->put_Author(PutInBstr(GetCurrData())) );
	}
	else
		HR_THROW(E_INVALID_DTD);
}

void CMgaMetaParser::EndDispName()
{
	if( GetPrevious().object == NULL )
		return;

	if( GetPrevName() == _T("paradigm") )
	{
		COMTHROW( metaproject->put_DisplayedName(PutInBstr(GetCurrData())) );
	}
	else if( GetPrevName() == _T("constraint") )
	{
		CComObjPtr<IMgaMetaConstraint> c;
		GetPrevObj(c);
		COMTHROW( c->put_DisplayedName(PutInBstr(GetCurrData())) );
	}
	else if( GetPrevious().object != NULL ) 
	{
		CComObjPtr<IMgaMetaBase> prev;
		GetPrevObj(prev);
		COMTHROW( prev->put_DisplayedName(PutInBstr(GetCurrData())) );
	}
}

void CMgaMetaParser::StartFolder(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaFolder> folder;

	if( GetPrevName() == _T("paradigm") )
	{
		COMTHROW( metaproject->get_RootFolder(PutOut(folder)) );
	}
	else
	{
		CComObjPtr<IMgaMetaFolder> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineFolder(PutOut(folder)) );
	}

	ASSERT( folder != NULL );
	GetCurrent().object = folder;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("name"), folder, &IMgaMetaFolder::put_Name);
		Attr(i, _T("metaref"), folder, &IMgaMetaFolder::put_MetaRef);

		++i;
	}
}

void CMgaMetaParser::StartFolder2(const attributes_type &attributes)
{
	ASSERT( GetCurrent().object == NULL );

	CComObjPtr<IMgaMetaFolder> folder;

	if( GetPrevName() == _T("paradigm") )
	{
		ASSERT( metaproject != NULL );
		COMTHROW( metaproject->get_RootFolder(PutOut(folder)) );
	}
	else
	{
		CComObjPtr<IMgaMetaFolder> prev;
		GetPrevObj(prev);

		HRESULT hr = prev->get_DefinedFolderByName(PutInBstr(GetByName(attributes, _T("name"))),
			VARIANT_FALSE, PutOut(folder));

		if( hr == E_NOTFOUND )
			ThrowXmlError(_T("Parent folder %s was not found"), GetByName(attributes, _T("name")).c_str());
		else
			COMTHROW(hr);
	}

	ASSERT( folder != NULL );
	GetCurrent().object = folder;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		if( i->first == _T("subfolders") )
		{
			std::tstring::const_iterator ip = i->second.begin();
			std::tstring::const_iterator ep = ip;
			std::tstring::const_iterator xp = i->second.end();
			for(;;)
			{
				std::tstring token = GetNextToken(ip, ep, xp);
				if( ip == xp )
					break;

				ASSERT( !token.empty() );

				CComObjPtr<IMgaMetaFolder> child;
				HRESULT hr = folder->get_DefinedFolderByName(PutInBstr(token),
					VARIANT_TRUE, PutOut(child));

				if( hr == E_NOTFOUND )
					ThrowXmlError(_T("Defined folder %s was not found"), token.c_str());
				else
					COMTHROW(hr);

				COMTHROW( folder->AddLegalChildFolder(child) );
			}
		}
		else if( i->first == _T("rootobjects") )
		{
			std::tstring::const_iterator ip = i->second.begin();
			std::tstring::const_iterator ep = ip;
			std::tstring::const_iterator xp = i->second.end();
			for(;;)
			{
				std::tstring token = GetNextToken(ip, ep, xp);
				if( ip == xp )
					break;

				ASSERT( !token.empty() );

				CComObjPtr<IMgaMetaFCO> fco;
				HRESULT hr = folder->get_DefinedFCOByName(PutInBstr(token),
					VARIANT_TRUE, PutOut(fco));

				if( hr == E_NOTFOUND )
                              ThrowXmlError(_T("No definition for FCO %s was not found"), token.c_str());
				else
					COMTHROW(hr);

				COMTHROW( folder->AddLegalRootObject(fco) );
			}
		}

		++i;
	}

}

void CMgaMetaParser::StartAtom(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaAtom> atom;

	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaMetaFolder> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineAtom(PutOut(atom)) );
	}
	else
	{
		CComObjPtr<IMgaMetaModel> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineAtom(PutOut(atom)) );
	}

	ASSERT( atom != NULL );
	GetCurrent().object = atom;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("name"), atom, &IMgaMetaAtom::put_Name);
		Attr(i, _T("metaref"), atom, &IMgaMetaAtom::put_MetaRef);
		++i;
	}
	COMTHROW(atom->put_AliasingEnabled((GetByName(attributes, _T("aliasenabled")) == _T("yes") ) ? VARIANT_TRUE : VARIANT_FALSE));
}

void CMgaMetaParser::StartConnection(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaConnection> conn;

	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaMetaFolder> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineConnection(PutOut(conn)) );
	}
	else
	{
		CComObjPtr<IMgaMetaModel> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineConnection(PutOut(conn)) );
	}

	ASSERT( conn != NULL );
	GetCurrent().object = conn;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("name"), conn, &IMgaMetaConnection::put_Name);
		Attr(i, _T("metaref"), conn, &IMgaMetaConnection::put_MetaRef);
		++i;
	}
	COMTHROW(conn->put_AliasingEnabled((GetByName(attributes, _T("aliasenabled")) == _T("yes") ) ? VARIANT_TRUE : VARIANT_FALSE));
}

void CMgaMetaParser::StartConnJoint(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaConnJoint> joint;

	CComObjPtr<IMgaMetaConnection> prev;
	GetPrevObj(prev);
	COMTHROW( prev->CreateJoint(PutOut(joint)) );

	ASSERT( joint != NULL );
	GetCurrent().object = joint;
}

void CMgaMetaParser::StartPointerSpec(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaPointerSpec> spec;

	if( GetPrevName() == _T("connjoint") )
	{
		CComObjPtr<IMgaMetaConnJoint> prev;
		GetPrevObj(prev);
		COMTHROW( prev->CreatePointerSpec(PutOut(spec)) );
	}
	else if( GetPrevName() == _T("reference") )
	{
		CComObjPtr<IMgaMetaReference> prev;
		GetPrevObj(prev);
		COMTHROW( prev->get_RefSpec(PutOut(spec)) );
	}
	else
	{
		ASSERT( GetPrevName() == _T("set") );

		CComObjPtr<IMgaMetaSet> prev;
		GetPrevObj(prev);
		COMTHROW( prev->get_MemberSpec(PutOut(spec)) );
	}

	ASSERT( spec != NULL );
	GetCurrent().object = spec;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		if( i->first == _T("name") )
			COMTHROW( spec->put_Name(PutInBstr(i->second.c_str())) );

		++i;
	}
}

void CMgaMetaParser::StartPointerItem(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaPointerItem> item;

	CComObjPtr<IMgaMetaPointerSpec> prev;
	GetPrevObj(prev);
	COMTHROW( prev->CreateItem(PutOut(item)) );

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("desc"), item, &IMgaMetaPointerItem::put_Desc);

		++i;
	}
}

void CMgaMetaParser::StartReference(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaReference> reference;

	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaMetaFolder> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineReference(PutOut(reference)) );
	}
	else
	{
		CComObjPtr<IMgaMetaModel> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineReference(PutOut(reference)) );
	}

	ASSERT( reference != NULL );
	GetCurrent().object = reference;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("name"), reference, &IMgaMetaReference::put_Name);
		Attr(i, _T("metaref"), reference, &IMgaMetaReference::put_MetaRef);
		++i;
	}
	COMTHROW(reference->put_AliasingEnabled((GetByName(attributes, _T("aliasenabled")) == _T("yes") ) ? VARIANT_TRUE : VARIANT_FALSE));
}

void CMgaMetaParser::StartSet(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaSet> set;

	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaMetaFolder> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineSet(PutOut(set)) );
	}
	else
	{
		CComObjPtr<IMgaMetaModel> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineSet(PutOut(set)) );
	}

	ASSERT( set != NULL );
	GetCurrent().object = set;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("name"), set, &IMgaMetaSet::put_Name);
		Attr(i, _T("metaref"), set, &IMgaMetaSet::put_MetaRef);

		++i;
	}
	COMTHROW(set->put_AliasingEnabled((GetByName(attributes, _T("aliasenabled")) == _T("yes") ) ? VARIANT_TRUE : VARIANT_FALSE));
}

void CMgaMetaParser::StartModel(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaModel> model;

	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaMetaFolder> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineModel(PutOut(model)) );
	}
	else
	{
		CComObjPtr<IMgaMetaModel> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineModel(PutOut(model)) );
	}

	ASSERT( model != NULL );
	GetCurrent().object = model;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("name"), model, &IMgaMetaModel::put_Name);
		Attr(i, _T("metaref"), model, &IMgaMetaModel::put_MetaRef);

		++i;
	}
	COMTHROW(model->put_AliasingEnabled((GetByName(attributes, _T("aliasenabled")) == _T("yes") ) ? VARIANT_TRUE : VARIANT_FALSE));
}

void CMgaMetaParser::StartFCO2(const attributes_type &attributes)
{
	ASSERT( GetCurrent().object == NULL );

	CComObjPtr<IMgaMetaFCO> fco;
	HRESULT hr;

	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaMetaFolder> prev;
		GetPrevObj(prev);
		hr = prev->get_DefinedFCOByName(PutInBstr(GetByName(attributes, _T("name"))),
			VARIANT_FALSE, PutOut(fco));
	}
	else
	{
		CComObjPtr<IMgaMetaModel> prev;
		GetPrevObj(prev);
		hr = prev->get_DefinedFCOByName(PutInBstr(GetByName(attributes, _T("name"))),
			VARIANT_FALSE, PutOut(fco));
	}

	if( hr == E_NOTFOUND )
		ThrowXmlError(_T("Parent FCO %s was not found"), GetByName(attributes, _T("name")).c_str());
	else
		COMTHROW(hr);

	ASSERT( fco != NULL );
	GetCurrent().object = fco;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		if( i->first == _T("attributes") )
		{
			std::tstring::const_iterator ip = i->second.begin();
			std::tstring::const_iterator ep = ip;
			std::tstring::const_iterator xp = i->second.end();
			for(;;)
			{
				std::tstring token = GetNextToken(ip, ep, xp);
				if( ip == xp )
					break;

				ASSERT( !token.empty() );

				CComObjPtr<IMgaMetaAttribute> attr;
				HRESULT hr = fco->get_DefinedAttributeByName(PutInBstr(token),
					VARIANT_TRUE, PutOut(attr));

				if( hr == E_NOTFOUND )
					ThrowXmlError(_T("Defined attribute %s was not found"), token.c_str());
				else
					COMTHROW(hr);

				COMTHROW( fco->AddAttribute(attr) );
			}
		}

		++i;
	}
}

void CMgaMetaParser::StartAttrDef(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaAttribute> attr;

	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaMetaFolder> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineAttribute(PutOut(attr)) );
	}
	else
	{
		CComObjPtr<IMgaMetaFCO> prev;
		GetPrevObj(prev);
		COMTHROW( prev->DefineAttribute(PutOut(attr)) );
	}

	ASSERT( attr != NULL );
	GetCurrent().object = attr;
	
	attr->put_Viewable( VARIANT_TRUE); // default behaviour if no 'viewable' attribute is found

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("name"), attr, &IMgaMetaAttribute::put_Name);
		Attr(i, _T("defvalue"), attr, &IMgaMetaAttribute::put_DefaultValue);
		Attr(i, _T("metaref"), attr, &IMgaMetaAttribute::put_MetaRef);

		if( i->first == _T("valuetype") )
		{
			attval_enum attval;

			if( i->second == _T("integer") )
				attval = ATTVAL_INTEGER;
			else if( i->second == _T("double") )
				attval = ATTVAL_DOUBLE;
			else if( i->second == _T("boolean") )
				attval = ATTVAL_BOOLEAN;
			else if( i->second == _T("string") )
				attval = ATTVAL_STRING;
			else if( i->second == _T("enum") )
				attval = ATTVAL_ENUM;
			else if( i->second == _T("dynamic") )
				attval = ATTVAL_DYNAMIC;
			else
				HR_THROW(E_XMLPARSER);

			COMTHROW( attr->put_ValueType(attval) );
		}

		if( i->first == _T("viewable") && i->second == _T("no"))
			attr->put_Viewable( VARIANT_FALSE);

		++i;
	}
}

void CMgaMetaParser::StartEnumItem(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaEnumItem> item;

	CComObjPtr<IMgaMetaAttribute> prev;
	GetPrevObj(prev);
	COMTHROW( prev->CreateEnumItem(PutOut(item)) );

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("dispname"), item, &IMgaMetaEnumItem::put_DisplayedName);
		Attr(i, _T("value"), item, &IMgaMetaEnumItem::put_Value);

		++i;
	}
}

void CMgaMetaParser::StartRole2(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaModel> prev;
	GetPrevObj(prev);

	CComObjPtr<IMgaMetaFCO> fco;
	HRESULT hr = prev->get_DefinedFCOByName(PutInBstr(GetByName(attributes, _T("kind"))),
		VARIANT_TRUE, PutOut(fco));

	if( hr == E_NOTFOUND )
		ThrowXmlError(_T("Kind FCO %s was not found"), GetByName(attributes, _T("kind")).c_str());
	else
		COMTHROW(hr);

	ASSERT( fco != NULL );

	CComObjPtr<IMgaMetaRole> role;
	COMTHROW( prev->CreateRole(fco, PutOut(role)) );

	ASSERT( role != NULL );
	GetCurrent().object = role;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("name"), role, &IMgaMetaRole::put_Name);
		Attr(i, _T("metaref"), role, &IMgaMetaRole::put_MetaRef);

		++i;
	}
}

void CMgaMetaParser::StartAspect2(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaAspect> aspect;

	CComObjPtr<IMgaMetaModel> prev;
	GetPrevObj(prev);
	COMTHROW( prev->CreateAspect(PutOut(aspect)) );

	CComObjPtr<IMgaMetaModel> model;
	COMTHROW( prev.QueryInterface(model) );

	ASSERT( aspect != NULL );
	GetCurrent().object = aspect;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("name"), aspect, &IMgaMetaAspect::put_Name);
		Attr(i, _T("metaref"), aspect, &IMgaMetaAspect::put_MetaRef);

		if( i->first == _T("attributes") )
		{
			std::tstring::const_iterator ip = i->second.begin();
			std::tstring::const_iterator ep = ip;
			std::tstring::const_iterator xp = i->second.end();
			for(;;)
			{
				std::tstring token = GetNextToken(ip, ep, xp);
				if( ip == xp )
					break;

				ASSERT( !token.empty() );

				CComObjPtr<IMgaMetaAttribute> attr;
				HRESULT hr = model->get_AttributeByName(PutInBstr(token), PutOut(attr));

				if( hr == E_NOTFOUND )
					ThrowXmlError(_T("Attribute %s was not found"), token.c_str());
				else
					COMTHROW(hr);

				COMTHROW( aspect->AddAttribute(attr) );
			}
		}
	
		++i;
	}
}

void CMgaMetaParser::StartPart2(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaPart> part;

	CComObjPtr<IMgaMetaAspect> prev;
	GetPrevObj(prev);

	ASSERT( elements.size() >= 3 );
	ASSERT( elements[elements.size()-3].name == _T("model") );

	CComObjPtr<IMgaMetaModel> model;
	COMTHROW( elements[elements.size()-3].object.QueryInterface(model) );
	ASSERT( model != NULL );

	CComObjPtr<IMgaMetaRole> role;
	HRESULT hr = model->get_RoleByName(PutInBstr(GetByName(attributes, _T("role"))), PutOut(role));

	if( hr == E_NOTFOUND )
		ThrowXmlError(_T("Role %s was not found"), GetByName(attributes, _T("role")).c_str());
	else
		COMTHROW(hr);

	ASSERT( role != NULL );

	COMTHROW( prev->CreatePart(role, PutOut(part)) );
	ASSERT( part != NULL );

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("kindaspect"), part, &IMgaMetaPart::put_KindAspect);
		Attr(i, _T("metaref"), part, &IMgaMetaPart::put_MetaRef);

		if( i->first == _T("primary") )
		{
			COMTHROW( part->put_IsPrimary(i->second == _T("yes") ? VARIANT_TRUE : VARIANT_FALSE) );
		}
		else if( i->first == _T("linked") )
		{
			COMTHROW( part->put_IsLinked(i->second == _T("yes") ? VARIANT_TRUE : VARIANT_FALSE) );
		}

		++i;
	}

}

void CMgaMetaParser::StartRegNode(const attributes_type &attributes)
{
	if( GetPrevious().object == NULL )
		return;

	CComObjPtr<IMgaMetaRegNode> regnode;

	const std::tstring &name = GetByName(attributes, _T("name"));

	if( GetPrevName() == _T("part") )
	{
		CComObjPtr<IMgaMetaPart> prev;
		GetPrevObj(prev);
		COMTHROW( prev->get_RegistryNode(PutInBstr(name), PutOut(regnode)) );
	}
	else if( GetPrevName() == _T("regnode") )
	{
		CComObjPtr<IMgaMetaRegNode> prev;
		GetPrevObj(prev);
		COMTHROW( prev->get_RegistryNode(PutInBstr(name), PutOut(regnode)) );
	}
	else
	{
		CComObjPtr<IMgaMetaBase> prev;
		GetPrevObj(prev);
		COMTHROW( prev->get_RegistryNode(PutInBstr(name), PutOut(regnode)) );
	}

	ASSERT( regnode != NULL );
	GetCurrent().object = regnode;

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("value"), regnode, &IMgaMetaRegNode::put_Value);

		++i;
	}
}

void CMgaMetaParser::StartConstraint(const attributes_type &attributes)
{
	CComObjPtr<IMgaMetaConstraint> c;

	CComObjPtr<IMgaMetaBase> prev;
	GetPrevObj(prev);
	COMTHROW( prev->CreateConstraint(PutOut(c)) );

	ASSERT( c != NULL );
	GetCurrent().object = c;

	foundconstraints = true;


	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		Attr(i, _T("name"), c, &IMgaMetaConstraint::put_Name);
		Attr(i, _T("eventmask"), c, &IMgaMetaConstraint::put_EventMask);
		Attr(i, _T("priority"), c, &IMgaMetaConstraint::put_Priority);
		Attr(i, _T("defdfornamesp"), c, &IMgaMetaConstraint::SetDefinedForNamespace);

		if( i->first == _T("depth") )
		{
			constraint_depth_enum depth;
			if( i->second == _T("0") )
				depth = CONSTRAINT_DEPTH_ZERO;
			else if( i->second == _T("any") )
				depth = CONSTRAINT_DEPTH_ANY;
			else
				depth = CONSTRAINT_DEPTH_ONE;
			COMTHROW( c->put_Depth(depth) );
		}
		else if( i->first == _T("type") )
		{
			constraint_type_enum type;
			if( i->second == _T("ondemand") )
				type = CONSTRAINT_TYPE_ONDEMAND;
			else if( i->second == _T("function") )
				type = CONSTRAINT_TYPE_FUNCTION;
			else
				type = CONSTRAINT_TYPE_EVENTBASED;
			COMTHROW( c->put_Type(type) );
		}

		++i;
	}
}

void CMgaMetaParser::EndConstraint()
{
	CComObjPtr<IMgaMetaConstraint> c;

	ASSERT( GetCurrent().object != NULL );
	COMTHROW( GetCurrent().object.QueryInterface(c) );
	ASSERT( c != NULL );

	COMTHROW( c->put_Expression(PutInBstr(GetCurrData())) );
}
