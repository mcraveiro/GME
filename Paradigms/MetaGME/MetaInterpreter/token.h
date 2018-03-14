#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "string"
#include "vector"


class Tokenizer
{
public:
	void split( const std::string& source, const std::string& delimiter, std::vector< std::string> & strVec);
	void trimLeft( std::string& s);
	void trimRight( std::string& s);
	void removeWSP( std::string& s);

};

#endif // TOKENIZER_H