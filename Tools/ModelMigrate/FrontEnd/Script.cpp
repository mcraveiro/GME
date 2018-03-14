#include "StdAfx.h"
#include ".\script.h"
#include <fstream>

Script::Script(void)
{
}

Script::Script( const std::string& file)
	: m_file( file)
{
}

Script::~Script(void)
{
}


void Script::init( const std::string& file)
{
	m_file = file;
}

CString Script::instantiat2( const std::vector< std::string >& pars, std::ostream& pstream)
{
	std::string content = getXSLFromResource( m_file.c_str());
	CString cnt( content.c_str());
	for( unsigned int i = 0; i < pars.size(); ++i)
	{
		CString old_i;//##|pari|##
		old_i.Format("##|par%d|##", i + 1); 
		cnt.Replace( old_i, pars[i].c_str());
	}

	return cnt;
}


bool Script::instantiate( const std::vector< std::string >& pars, std::ostream& pstream)
{
	ASSERT(m_file.length() > 0);
	std::ifstream templ( m_file.c_str(), std::ios_base::in);
	ASSERT( templ.is_open());

	char c;
	while( templ.get( c))
	{
		if( c != '#')
			pstream.put( c);
		else
		{
			std::ifstream::pos_type fpos = templ.tellg();
			bool good = false;
			// par_i format : ##|pari|##
			char buff[3];
			if( templ.get( buff, 3) && buff[0] == '#' && buff[1] == '|')
			{
				std::string par_i;
				while( templ.get(c) && c != '|')
					par_i += c;
				if( par_i.length() > 3 && par_i.substr(0, 3) == "par")
				{
					int idx = atoi( par_i.substr(3).c_str());
					if( idx > 0 && idx <= (int) pars.size())
					{
						pstream.write( pars[ idx - 1].c_str(), (std::streamsize) pars[ idx - 1].length());
						good = true;
					}
				}
				if( good && ( !templ.get( buff, 3) || buff[0] != '#' || buff[1] != '#'))
					good = false;
			}
			
			if( !good)
			{
				pstream.put( '#'); // false alarm
				templ.seekg( fpos);
			}
		}
	}
	templ.close();

	
	return true;
}

std::string Script::getXSLFromResource( const char *idr)
{
	HMODULE hm = 0;//GetModuleHandle("PARSER.DLL");
	//if( !hm) return "";

	HRSRC res1 = FindResource(hm, idr, "XSL");
	if( !res1) return getXSLFromLocalFile( idr);
	
	HGLOBAL res2 = LoadResource(hm, res1);
	if( !res2) return getXSLFromLocalFile( idr);
	
	char *data = ( char* )LockResource( res2);
	if( !data) return getXSLFromLocalFile( idr);

	DWORD uLen = SizeofResource(hm, res1);
	return std::string( data, uLen);
}

std::string Script::getXSLFromLocalFile( const char * filename)
{
	std::fstream head;

	head.open( filename, std::ios_base::in);
	ASSERT( head.is_open());
	if( !head.is_open())
	{
		AfxMessageBox( CString("Could not open ") + filename + " template file!");
		return "";
	}

	//copy byte by byte
	std::string res;
	char ch;
	while( head.get( ch))
		res += ch;
	
	//copy byte by byte
	//char ch;
	//while( head.get( ch))
	//	f.put( ch);
	head.close();

	return res;
}
