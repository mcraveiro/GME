#pragma once
#include <string>

class CallLogger
{
protected:
	bool                m_logEnabled;
	std::string         m_logFile;

	void                log( const std::string& p_command, const std::string& p_parameter);

public:
	CallLogger(void);
	~CallLogger(void);

	void setLog( bool p_val, const std::string& p_logFile);
};
