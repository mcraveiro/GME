#pragma once

class FileHelp
{
public:
	static bool isFileReadOnly   ( const std::string& p_file);
	static bool isFileReadOnly2  ( const std::string& p_file, bool * p_ptrExists);
	static bool isFileReadWrite  ( const std::string& p_file);
	static bool isFile           ( const std::string& p_dirOrFile);
	static bool isDir            ( const std::string& p_dir);
	static bool fileExist        ( const std::string& p_file);
};

