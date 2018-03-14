#pragma once
#include "Script.h"

class ScriptMoveUp :
	public Script
{
public:
	ScriptMoveUp(void);
	~ScriptMoveUp(void);
	virtual CString instantiat2( const std::vector< std::string >& pars, std::ostream& pstream);

};
