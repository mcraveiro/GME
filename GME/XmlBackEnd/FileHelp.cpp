#include "stdafx.h"
#include "FileHelp.h"


// FileHelp
bool FileHelp::fileExist( const std::string& p_file)
{
	FILE * f = fopen( p_file.c_str(), "r");
	if( !f) 
		return false;

	fclose( f);
	return true;
}

bool FileHelp::isFile( const std::string& p_file)
{
	WIN32_FILE_ATTRIBUTE_DATA attr;

	if( GetFileAttributesEx( p_file.c_str(), GetFileExInfoStandard, &attr ) )
	{
		return FILE_ATTRIBUTE_DIRECTORY != ( attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
	}

	return false;
}

bool FileHelp::isDir( const std::string& p_path)
{
	WIN32_FILE_ATTRIBUTE_DATA attr;

	if( GetFileAttributesEx( p_path.c_str(), GetFileExInfoStandard, &attr ) )
	{
		return FILE_ATTRIBUTE_DIRECTORY == ( attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
	}

	return false;
}

bool FileHelp::isFileReadOnly( const std::string& p_file)
{
	WIN32_FILE_ATTRIBUTE_DATA attr;

	if( GetFileAttributesEx( p_file.c_str(), GetFileExInfoStandard, &attr ) )
	{
		return attr.dwFileAttributes & FILE_ATTRIBUTE_READONLY;
	}

	return false;
}

bool FileHelp::isFileReadOnly2( const std::string& p_file, bool* p_ptrFileExists)
{
	WIN32_FILE_ATTRIBUTE_DATA attr;
	ASSERT( p_ptrFileExists); // valid pointer

	if( GetFileAttributesEx( p_file.c_str(), GetFileExInfoStandard, &attr ) )
	{
		if( p_ptrFileExists) // pointer not zero
			*p_ptrFileExists = true;
		return attr.dwFileAttributes & FILE_ATTRIBUTE_READONLY;
	}

	return false;
}

bool FileHelp::isFileReadWrite( const std::string& p_file) // is a file and is read write
{
	WIN32_FILE_ATTRIBUTE_DATA attr;

	if( GetFileAttributesEx( p_file.c_str(), GetFileExInfoStandard, &attr ) )
	{
		return (attr.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != FILE_ATTRIBUTE_READONLY
			&&    (attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY;
	}

	return false;
}
