#ifndef GLOBALS_H
#define GLOBALS_H

#include "fstream"
#include "string"
#include "LogStream.h"

class Globals
{
public: // enum
	typedef enum {
		ALLTOGETHER  = 0,
		PERCLASS     = 1,
		PERNAMESPACE = 2,
		X_THE_NO_OF_METHODS= 3 // upper boundary
	} OUTPUTMETHOD_ENUM;
public:
	Globals() 
		: err_file_name("\\ParadigmBonExt.log")
		, header_backup_name("")
		, header_file_name("\\ParadigmBonExtension.h")
		, source_file_name("\\ParadigmBonExtension")
		, m_visitorHeaderFileName("\\ParadigmVisitor.h")
		, m_visitorSourceFileName("\\ParadigmVisitor")
		, output_directory_name()
		, m_namespace_name()
		, m_outputMethod( ALLTOGETHER)
		, silent_mode( false)
		{ }

	LogStream     err;
	std::ofstream dmp;
	std::ofstream dmp_s;
	std::ofstream dmp_h;

	std::string err_file_name;
	std::string header_backup_name;
	std::string header_file_name;
	std::string source_file_name;
	std::string m_visitorHeaderFileName;
	std::string m_visitorSourceFileName;
	std::string output_directory_name;
	std::string m_namespace_name;
	OUTPUTMETHOD_ENUM m_outputMethod;
	bool        silent_mode;
};

#endif // GLOBALS_H