#include "StdAfx.h"
#include ".\scriptattrname.h"

ScriptAttrName::ScriptAttrName(void)
{
}

ScriptAttrName::~ScriptAttrName(void)
{
}

CString ScriptAttrName::instantiat2( const std::vector< std::string >& pars, std::ostream& pstream)
{
	CString res = Script::instantiat2( pars, pstream);
	if( pars.size() >= 4 && pars[2] != "1") // not a global attribute
	{
		if( pars[3] == "") { AfxMessageBox( "No owner specified for local attribute!"); return ""; }

		CString toIns = CString( " and ../@kind='") + pars[3].c_str() + "'";
		res.Replace( "##|OPTIONALLOCALCONDITION|##", toIns);
	}
	else
		res.Replace( "##|OPTIONALLOCALCONDITION|##", "");

	return res;
}