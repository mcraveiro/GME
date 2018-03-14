#pragma once
#include "ScriptAbs.h"
#include "Extractor.h"

class ScriptCmplx
	: public ScriptAbs
{
public:
	static const char * header;
	static const char * tail;
	static const char * kind_removed_log_cmd;
	static const char * attr_removed_log_cmd;
	static const char * output_log_file;
	ScriptCmplx();
	~ScriptCmplx();
	void gen( const Extractor::NAMES& pNames, const Extractor::ATTRS& pAttrs, const char * pfName);
protected:
	void genPredKinds( std::ostream& ostr, const Extractor::NAMES& pNames);
	void genPredAttrs( std::ostream& ostr, const Extractor::ATTRS::const_iterator& iter);
};
