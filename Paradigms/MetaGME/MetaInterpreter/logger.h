#ifndef LOGGER_H
#define LOGGER_H

#include "string"
#include "fstream"
#include "globals.h"

extern int ind;

extern Globals global_vars;

#define DMP_DEBUG_INFO 0

inline void TOF(const char * msg)
{
#if(DMP_DEBUG_INFO)
	std::string fname = global_vars.xmp_file_name + ".debug.log";//"c:\\atemp\\meta.log";
	std::ofstream of;
	of.open( fname.c_str(), std::ios_base::out | std::ios_base::app);
	of << msg << "\n";
	of.close();
#endif
}

inline void DMP(const char * msg)
{
	global_vars.dmp << msg;
}

inline void ERR_OUT(const char * msg)
{
	const char * fname = global_vars.err_file_name.c_str();
	std::ofstream of;
	of.open( fname, std::ios_base::out | std::ios_base::app);
	of << msg << "\n";
	of.close();
}

inline void TO(const char * msg)
{ 
	global_vars.err << MSG_INFO << msg << "\n";
#if(0)
	if ( global_vars.silent_mode)
		ERR_OUT( msg);
	else
		AfxMessageBox(msg);
	TOF(msg); 
#endif
}

inline void DMP(const std::string& msg) { DMP( msg.c_str() ); }

inline void TO( const CString& msg)			{ TO( (LPCTSTR) msg); }
inline void TO( const std::string& msg)		{ TO( msg.c_str()); }

inline void TOF( const CString& msg)		{ TOF( (LPCTSTR) msg); }
inline void TOF( const std::string& msg)	{ TOF( msg.c_str() ); }

inline void ERR_OUT( const CString& msg)		{ ERR_OUT( (LPCTSTR) msg); }
inline void ERR_OUT( const std::string& msg)	{ ERR_OUT( msg.c_str() ); }

inline std::string indStr()
{
	return std::string( ind, '\t');
}

#endif // LOGGER_H