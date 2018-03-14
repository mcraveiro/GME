#pragma once
#include "Script.h"

class ScriptMoveDown :
	public Script
{
public:
	ScriptMoveDown(void);
	~ScriptMoveDown(void);
	virtual CString instantiat2( const std::vector< std::string >& pars, std::ostream& pstream);

};
