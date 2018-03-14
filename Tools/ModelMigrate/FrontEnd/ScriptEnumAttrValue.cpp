#include "StdAfx.h"
#include ".\ScriptEnumAttrValue.h"

ScriptEnumAttrValue::ScriptEnumAttrValue(void)
{
}

ScriptEnumAttrValue::~ScriptEnumAttrValue(void)
{
}

CString ScriptEnumAttrValue::instantiat2( const std::vector< std::string >& pars, std::ostream& pstream)
{
	CString res = Script::instantiat2( pars, pstream);
	if( pars.size() >= 5 && pars[3] != "1") // not a global attribute
	{
		if( pars[4] == "") { AfxMessageBox( "No owner specified for local attribute!"); return ""; }

		CString toIns = CString( " and ../@kind='") + pars[4].c_str() + "'";
		res.Replace( "##|OPTIONALLOCALCONDITION|##", toIns);
	}
	else
		res.Replace( "##|OPTIONALLOCALCONDITION|##", "");

	return res;
}