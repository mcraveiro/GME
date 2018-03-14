#include "stdafx.h"
#include "logger.h"

#include "string"
#include "algorithm"

#include "globals.h"
extern Globals global_vars;

bool existsFile( const char * src_name)
{
	std::ifstream input_file( src_name, std::ios_base::in);
	if ( !input_file.is_open())
		return false;

	input_file.close();
	return true;
}

int makeFileCopy( const char * src_name, const char * dst_name)
{
	char buff[4096];

	std::ifstream input_file( src_name, std::ios_base::in);
	std::ofstream output_file( dst_name, std::ios_base::out);
	if ( !input_file.is_open())
		return 2;
	if ( !output_file.is_open())
		return 3;

	while ( !input_file.eof())
	{
		input_file.read( buff, 4096);
		output_file.write( buff, input_file.gcount());
	}

	input_file.close();
	output_file.close();
	return 1;
}

int makeFileMove( const char * src_name, const char * dst_name)
{
	if( !fileExists( src_name)) return 1; // src nonexistent, done

	BOOL dres = TRUE;
	if( fileExists( dst_name)) // exists and not read only
	{
		dres = DeleteFile( dst_name); // must delete before move
	}
	
	if( !dres) return 0; // failure?, move won't succeed, return
		
	// either dst didn't exist or successful deletion
	dres = MoveFile( src_name, dst_name);
	return dres == TRUE? 1: 0;
}


bool fileExists( const char * file)
{
	bool res = false;
	CFileStatus stat;
	if( file                              // not-empty string
		&& CFile::GetStatus( file, stat)) // status fetched succesfully
	{
		if( (stat.m_attribute & CFile::directory) == 0     // it is NOT a directory
			&& (stat.m_attribute & CFile::readOnly)  == 0) // not read only
		{
			res = true;
		}
	}

	return res;
}

std::string shortFileName( const std::string& long_name)
{
	std::string::size_type pos = long_name.rfind( '\\');
	if ( pos != std::string::npos)
	{
		std::string short_name;
		if ( pos + 1 < long_name.length())
			short_name = long_name.substr( pos + 1);
		else
			short_name = "insert_your_specialized_header_filename_here.h";

		return short_name;
	}

	return long_name;
}

std::string cutExtension( const std::string& filename)
{
	std::string::size_type p = filename.rfind( ".");
	if ( p != std::string::npos)
		return filename.substr( 0, p);

	return filename;
}

std::string capitalizeString( const std::string& name)
{
	std::string capitalized_name = name;
	std::transform( capitalized_name.begin(), capitalized_name.end(), capitalized_name.begin(), toupper);
	return capitalized_name;
}

bool directoryExists( const char * dir)
{
	bool res = false;
	CFileStatus stat;
	if( dir                              // not-empty string
		&& CFile::GetStatus( dir, stat)) // status fetched succesfully
	{
		if( (stat.m_attribute & CFile::directory) == CFile::directory // it is a directory
			&& (stat.m_attribute & CFile::readOnly)  == 0) // not read only
		{
			res = true;
		}
	}

	return res;
}

bool directoryCreate( const char * dir)
{
	return CreateDirectory( dir, NULL) == TRUE;
}