#pragma once

#include "Gme.h"

class MsgConsole
{
	CComPtr<IGMEOLEApp>   m_gme;
public:
	MsgConsole(bool gme);
	void sendMsg(const std::string&, int mtype);
	void sendMsg(const char* p_msg, int p_mtype);

};

