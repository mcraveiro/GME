#include "stdafx.h"
#include "LogStream.h"
#include "BON.h"
#include "BONImpl.h"

#include "globals.h"
extern Globals global_vars;

LogStream::LogStream()
	: m_proj( BON::Project())
	, m_msgtype( MSG_NORMAL)
	, m_buff()
{ }

LogStream::~LogStream() 
{
	m_proj = BON::Project();
	m_buff = "";
}

void LogStream::flushit()
{
	*this << "\n"; // flush
}

//
// friend put-to operators

LogStream& operator<<( LogStream& stream, msgtype_enum msgtype)
{
	if( !stream.m_buff.empty()) stream << "\n"; // like a flush

	stream.m_msgtype = msgtype;
	/*switch( stream.m_msgtype) {
		case MSG_ERROR:		stream.write( "Error: ", 7); break;
		case MSG_WARNING:	stream.write( "Warning: ", 9); break;
		case MSG_INFO:		stream.write( "Info: ", 5); break;
		case MSG_NORMAL:	;
	}*/

	return stream;

}

LogStream& operator<<( LogStream& stream, const BON::FCO& fco)
{
	std::string tok = "<b>Null_object</b>";
	if( fco)
	{
		try {
			std::string id, nm;
			id = fco->getID();
			nm = fco->getName();
			tok = "<A HREF=\"mga:" + id +"\">" + nm + "</A>";
		} catch( ...) {
		}
	}

	return stream << tok;
}

LogStream& operator<<( LogStream& stream, const char * r)
{
	return stream << std::string( r);
}

LogStream& operator<<( LogStream& stream, const std::string& r)
{
	stream.write( r.c_str(), r.length());
	
	stream.m_buff += r;
	if( r.find('\n') != std::string::npos)
	{
		if( !global_vars.silent_mode) {
			try { stream.m_proj->consoleMsg( stream.m_buff, stream.m_msgtype); }
			catch( ... ) { } // will throw if invoked without the Gui up&running
		}

		stream.m_buff = "";
	}
	
	return stream;
}

LogStream& operator<<( LogStream& stream, const int i)
{
	char t[32];
	sprintf( t, "%i", i);
	return stream << std::string( t);
}

