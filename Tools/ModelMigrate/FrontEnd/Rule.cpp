#include "StdAfx.h"
#include ".\rule.h"
#include "Script.h"
#include "ScriptMoveUp.h"
#include "ScriptMoveDown.h"
#include "ScriptAttrName.h"
#include "ScriptAttrType.h"
#include "ScriptEnumAttrValue.h"

//IMPLEMENT_SERIAL(Rule, CObject, 1)
Rule::Rule(void) // needed for the SERIAL
{
}

Rule::Rule( rule_type rt, std::vector<std::string>& vec)
	: m_type( rt)
	, m_params( vec)
	, m_name( "")
{
}

Rule::~Rule(void)
{
}

Rule::Rule( const Rule& pRule)
	: m_type( pRule.m_type)
	, m_params( pRule.m_params)
	, m_name( pRule.m_name)
{
}

const Rule& Rule::operator=( const Rule& pRule)
{
	if( this == &pRule)
		return *this;

	m_type = pRule.m_type;
	m_params = pRule.m_params;
	m_name = pRule.m_name;

	return *this;
}

void Rule::Serialize( CArchive& ar)
{
	// base is called
	//CObject::Serialize( ar);

	if( ar.IsStoring())
	{
		int type = m_type;
		unsigned int size = (unsigned int) m_params.size();
		ar << type;
		ar << CString( m_name.c_str());
		ar << size;
		for( unsigned int i = 0; i < m_params.size(); ++i)
		{
			ar << CString(m_params[i].c_str());
		}
	}
	else
	{
		ASSERT( ar.IsLoading());
		int type;
		CString name;
		unsigned int size;

		ar >> type;
		m_type = (Rule::rule_type) type;

		ar >> name;
		m_name = (LPCTSTR) name;

		ar >> size;
		m_params.resize( size);
		for( unsigned int i = 0; i < size; ++i)
		{
			CString el;
			ar >> el;
			m_params[i] = (LPCTSTR) el;
		}
	}
}
//static   const int  Rule::rule_nmb = 14;       // modify this in the Rule.h
/*static*/ const char * Rule::rule_templates[] = // these strings are used as prompts for the
	{ "TKindNameChange.xsl"                      // new rule dialog
	, "TAttrNameChange.xsl"
	, "TAttrTypeChange.xsl"
	, "TEnumAttrValueChange.xsl"
	, "TMoveUpElem.xsl"
	, "TMoveDownElem.xsl"
	, "TRemoveKind.xsl"
	, "TRemoveGlobalAttr.xsl"
	, "TRemoveLocalAttr.xsl"
	, "TAtom2Model.xsl"
	, "TModel2Atom.xsl"
	, "TParadigmGUIDChange.xsl"
	, "TParadigmNameChange.xsl"
	, "TParadigmVersionChange.xsl"
	};

/*static*/ CString Rule::ruleTypeStr( int i)
{
	CString fN( Rule::rule_templates[i]);
	fN.Replace( ".xsl", "");
	return fN.Right( fN.GetLength() - 1);
}

void Rule::gen( std::ostream& pFstream)
{
	// Script is a simple class, which is able for Search&Replace operations
	// In case of more complex rules like 'MoveDown' more sophisticated replace
	// operations are needed, so a special class handles them
	Script * scr = 0;
	if( m_type == Rule::MoveDownElem)
		scr = new ScriptMoveDown();
	else if( m_type == Rule::MoveUpElem)
		scr = new ScriptMoveUp();
	else if( m_type == Rule::AttrNameChange)
		scr = new ScriptAttrName();
	else if( m_type == Rule::AttrTypeChange)
		scr = new ScriptAttrType();
	else if( m_type == Rule::EnumAttrValueChange)
		scr = new ScriptEnumAttrValue();
	else
		scr = new Script();

	scr->init( rule_templates[ (int) m_type]);

	CString txt = scr->instantiat2( this->m_params, pFstream);
	pFstream << txt;

	delete scr;
}

/*static*/ CString Rule::ruleSummary( Rule::rule_type pTyp, const std::vector<std::string>& pParams)
{
	std::string sum;

	if( pTyp == Rule::KindNameChange)
	{
		sum = pParams[0] + " -> " + pParams[1];
	}
	else if( pTyp == Rule::AttrNameChange)
	{
		sum = pParams[0] + " -> " + pParams[1];
	}
	else if( pTyp == Rule::AttrTypeChange)
	{
		sum = pParams[0] + ": typecheck";
	}
	else if( pTyp == Rule::EnumAttrValueChange)
	{
		sum = pParams[0] + ":" + pParams[1] + " -> " + pParams[2];	
	}
	else if( pTyp == Rule::MoveUpElem)
	{
		if( pParams.empty())
			sum = "(empty)";
		else 
			sum = pParams.back() + " move up";
	}
	else if( pTyp == Rule::MoveDownElem)
	{
		if( pParams.empty())
			sum = "(empty)";
		else 
			sum = pParams.front() + " move down";
	}
	else if( pTyp == Rule::RemoveKind
		||   pTyp == Rule::RemoveGlobalAttr)
	{
		sum = pParams[0] + " remove";
	}
	else if( pTyp == Rule::RemoveLocalAttr)
	{
		sum = pParams[0] + " (in " + pParams[1] + ") remove";
	}
	else if( pTyp == Rule::Atom2Model)
	{
		sum = "Atom2Model cast of " + pParams[0];
	}
	else if( pTyp == Rule::Model2Atom)
	{
		sum = "Model2Atom cast of " + pParams[0];
	}
	else if( pTyp == Rule::ParadigmGUIDChange)
	{
		sum = "Paradigm GUID change";
	}
	else if( pTyp == Rule::ParadigmNameChange)
	{
		sum = "Paradigm name change: '" + pParams[0] + "' -> '" + pParams[1] + "'";
	}
	else if( pTyp == Rule::ParadigmVersionChange)
	{
		sum = "Paradigm version change: '" + pParams[0] + "' -> '" + pParams[1] + "'";
	}

	return CString( sum.c_str());
}

/*static*/ CString Rule::ruleSummary( const Rule& pRule)
{
	return ruleSummary( pRule.m_type, pRule.m_params);
}
