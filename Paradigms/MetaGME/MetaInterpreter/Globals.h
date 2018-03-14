#ifndef GLOBALS_H
#define GLOBALS_H

#include "LogStream.h"
#include "fstream"
#include "string"

class Globals
{
public:
	Globals()
		: xmp_file_name("\\Paradigm.xmp")
		, err_file_name("\\Paradigm.xmp.log")
		, silent_mode  ( false)
		, genConstr()
		, skip_paradigm_register(false)
	{ }

	LogStream     err;
	std::ofstream dmp;

	std::string xmp_file_name;
	std::string err_file_name;

	bool		silent_mode;
	bool skip_paradigm_register;

	class GeneratedConstraints
	{
	public:
		GeneratedConstraints( )
			: reg_cont_mask( 0)
			, fol_cont_mask( 0)
			, connect_mask ( 0)
			, priority     ( 1)
		{ }

		int			reg_cont_mask;
		int			fol_cont_mask;
		int			connect_mask;
		int			priority;

	} genConstr;
};

#endif // GLOBALS_H