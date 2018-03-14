#pragma once
#include "script.h"

class ScriptEnumAttrValue :
	public Script
{
public:
	ScriptEnumAttrValue(void);
	~ScriptEnumAttrValue(void);
	virtual CString instantiat2( const std::vector< std::string >& pars, std::ostream& pstream);
};
