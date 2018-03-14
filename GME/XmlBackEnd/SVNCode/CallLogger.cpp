#include "../stdafx.h"
#include ".\CallLogger.h"
#include <fstream>

CallLogger::CallLogger(void)
	: m_logEnabled( false)
	, m_logFile( "c:\\svnlog.txt")
{
}

CallLogger::~CallLogger(void)
{
}

void CallLogger::setLog( bool p_val, const std::string& p_logFile)
{
	m_logEnabled = p_val;
	if( !p_logFile.empty())
		m_logFile = p_logFile;
}

void CallLogger::log( const std::string& p_command, const std::string& p_par)
{
	if( !m_logEnabled) return;

	std::ofstream fs;
	fs.open( m_logFile.c_str(), std::ios_base::ate | std::ios_base::app); // append
	if( fs)
	{
		fs << p_command << " " << p_par << std::endl;
		fs.close();
	}
}

