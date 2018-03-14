#include "stdafx.h"
#include "token.h"

void Tokenizer::split( const std::string& source, const std::string& delimiter, std::vector< std::string> & result)
{
	if ( delimiter.length() <= 0) return;
	std::string::size_type curr_token_start = 0;
	std::string::size_type curr_token_end = source.find( delimiter, curr_token_start);

	while ( curr_token_end != std::string::npos) // more delimiters found
	{
		if ( curr_token_end - curr_token_start > 0) // not inserting empty token
			result.push_back( source.substr( curr_token_start, curr_token_end - curr_token_start));
		curr_token_start = curr_token_end + delimiter.length();
		curr_token_end = source.find( delimiter, curr_token_start);
	}
	
	if ( curr_token_end == std::string::npos && source.length() - curr_token_start > 0 ) // no more delimiters found and we don't insert empty token
		result.push_back( source.substr( curr_token_start, source.length() - curr_token_start));

}


void Tokenizer::trimLeft( std::string& s)
{
	if ( s.empty()) return;
	int i = 0;
	while( i < (int) s.length() &&
		( s[i] == _T(' ') 
		|| s[i] == _T('\t')
		|| s[i] == _T('\n')))
		++i;
	//if ( i < (int) s.length())
	s.erase(0, i);
}


void Tokenizer::trimRight( std::string& s)
{
	if ( s.empty()) return;
	int i = (int) s.length() - 1;
	while( i >= 0 &&
		( s[i] == _T(' ') 
		|| s[i] == _T('\t')
		|| s[i] == _T('\n')))
		--i;
	//if ( i >= 0)
	s.erase(i + 1, s.length() - i - 1);
}


void Tokenizer::removeWSP( std::string& s)
{
	if ( s.empty()) return;
	std::string r;
	for( int i = 0; i < (int) s.length(); ++i)
		if ( s[i] != _T(' ') 
			&& s[i] != _T('\t')
			&& s[i] != _T('\n'))
			r += s[i];
	s = r;
}