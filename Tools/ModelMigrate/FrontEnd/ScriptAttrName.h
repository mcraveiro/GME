#pragma once
#include "script.h"

class ScriptAttrName :
	public Script
{
public:
	ScriptAttrName(void);
	~ScriptAttrName(void);
	virtual CString instantiat2( const std::vector< std::string >& pars, std::ostream& pstream);
};
