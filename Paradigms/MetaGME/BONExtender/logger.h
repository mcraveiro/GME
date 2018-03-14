#ifndef LOGGER_H
#define LOGGER_H

#define LONG_NAMES 0

#include "string"
#include "algorithm"
#include "globals.h"

extern Globals global_vars;

inline void DMP_H(const char * msg)
{
	global_vars.dmp_h << msg;
}

inline void DMP_S(const char * msg)
{
	global_vars.dmp_s << msg;
}


inline void TO(const char * msg) 
{ 
	//AfxMessageBox(msg); 
	global_vars.err << MSG_ERROR << msg << "\n" << MSG_NORMAL;
}

inline void TO(const CString msg) {		TO( (LPCTSTR) msg); }
inline void TO(const std::string msg) { TO( msg.c_str()); }
inline void DMP(const std::string& msg) { DMP( msg.c_str() ); }
inline void DMP_H(const std::string& msg) { DMP_H( msg.c_str() ); }
inline void DMP_S(const std::string& msg) { DMP_S( msg.c_str() ); }


bool existsFile( const char * src_name);
bool fileExists( const char * file_name);
int makeFileCopy( const char * src_name, const char * dst_name);
int makeFileMove( const char * src_name, const char * dst_name);
std::string shortFileName( const std::string& long_name);
std::string cutExtension( const std::string& filename);
std::string capitalizeString( const std::string& name);
bool directoryExists( const char * dir);
bool directoryCreate( const char * dir);

#endif // LOGGER_H