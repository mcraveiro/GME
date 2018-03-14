#pragma once
#include "Extractor.h"

class ScriptAbs
{
public:
	ScriptAbs();
	~ScriptAbs();
	virtual void gen( const Extractor::NAMES& pNames, const Extractor::ATTRS& pAttrs, const char * pfName) = 0;
};
