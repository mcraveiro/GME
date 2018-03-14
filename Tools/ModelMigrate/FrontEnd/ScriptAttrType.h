#pragma once
#include "script.h"

class ScriptAttrType :
	public Script
{
public:
	ScriptAttrType(void);
	~ScriptAttrType(void);
	virtual CString instantiat2( const std::vector< std::string >& pars, std::ostream& pstream);
};
