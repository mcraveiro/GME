#include "stdafx.h"
#include "MsgConsole.h"


// MsgConsole
MsgConsole::MsgConsole(bool p_create)
{
	if( p_create)
		m_gme.CoCreateInstance( L"GME.Application");
}

void MsgConsole::sendMsg( const char* p_msg, int p_mtype)
{
	if( m_gme)
	{
		m_gme->put_Visible( VARIANT_TRUE);
		m_gme->ConsoleMessage( _bstr_t(p_msg), (msgtype_enum) p_mtype);
	}
}

void MsgConsole::sendMsg( const std::string& p_msg, int p_mtype)
{
	sendMsg(p_msg.c_str(), p_mtype);
}

