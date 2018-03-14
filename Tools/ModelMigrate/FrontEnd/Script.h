#pragma once
#include <string>
#include <vector>
#include <ostream>

class Script
{
public:
	Script(void);
	Script( const std::string& file);
	virtual ~Script(void);

	void init( const std::string& file);

	bool instantiate( const std::vector< std::string >& pars, std::ostream& pstream);
	virtual CString instantiat2( const std::vector< std::string >& pars, std::ostream& pstream);

protected:
	std::string m_file;
	std::string m_content;

public:
	static std::string getXSLFromResource( const char *idr);
	static std::string getXSLFromLocalFile( const char * filename);

};
