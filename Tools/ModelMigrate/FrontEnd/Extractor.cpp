#include "StdAfx.h"
#include ".\extractor.h"

#include <fstream>
#include <algorithm>

Extractor::Extractor(void)
{
}

Extractor::~Extractor(void)
{
}

void Extractor::init()
{
	m_names.clear();
	m_attrs.clear();
}

void Extractor::doJob( std::string& pName)
{
	init();

	std::ofstream ofile( ( pName + ".parsed").c_str(), std::ios_base::out);
	std::ifstream file( pName.c_str());
	if( !file) return;
	char buff[1024];
	bool global_attr = true;
	std::string last_kind = "";
	while( file)
	{
		// 
		file.getline( buff, 1024);
		std::string bf( buff);
		unsigned int i = 0;
		for (; i < bf.length() && (bf[i] == ' ' || bf[i] == 9); ++i);
		const int kindsNo = 6;
		std::string kinds[ kindsNo ] =
			{ "<atom name"
			, "<set name"
			, "<reference name"
			, "<connection name"
			, "<model name"
			, "<folder name"
		};
		
		bool found = false;
		unsigned int j = 0;
		for( ; !found && j < kindsNo; ++j)
		{
			found = 0 == kinds[j].compare( bf.substr( i, kinds[j].length()));
		}

		if( found)
		{
			--j;

			std::string one_name = get( " name", bf.substr(i)); // whitespaces stripped off
			last_kind = one_name;
			if( std::find( m_names.begin(), m_names.end(), one_name) != m_names.end()) // already there
				ASSERT(0); // how come?
			else
				m_names.push_back( one_name);

			// my attributes are inserted there
			std::string myattrs = get( " attributes", bf.substr(i));
			if( !myattrs.empty())
				m_attrs[ one_name ] = myattrs;
		}
	}

	for( NAMES::iterator it = m_names.begin(); it != m_names.end(); ++it)
	{
		ofile << "k" << ' ' << *it << ' ' << std::endl;
		if( m_attrs.find( *it) != m_attrs.end()) // found, key/value really exists
		 ofile << "a" << ' ' << *it << ' ' << m_attrs[ *it ] << std::endl; // the attributes of *it
	}
	
	file.close();
	ofile.close();
}

void Extractor::doPlainLoad( std::string& pName)
{
	init();

	std::basic_ifstream<char> f( pName.c_str());
	while( f)
	{
		std::string type;
		std::string one_name;
		f >> type >> one_name;
		
		if( type == "k")
		{
			if( std::find( m_names.begin(), m_names.end(), one_name) != m_names.end()) // already there
				ASSERT(0); // how come?
			else
				m_names.push_back( one_name);
		}
		else if( type == "a")
		{
			char buffer[5024];
			f.getline( buffer, 5024);
			m_attrs[ one_name ] = buffer; // one_name in this case is the owner of the attributes
		}
	}
	f.close();
}

const Extractor::NAMES& Extractor::getKinds()
{
	return m_names;
}

const Extractor::ATTRS& Extractor::getAttrs()
{
	return m_attrs;
}

/*static*/ std::string Extractor::get( const std::string& attr, const std::string& buff)
{
	std::string res;
	size_t attr_start = buff.find( attr);
	if( attr_start == std::string::npos) return "";

	size_t quot_begin = buff.find( '"', attr_start + attr.length());
	size_t quot_end   = buff.find( '"', quot_begin + 1);
	if( quot_begin == std::string::npos || quot_end == std::string::npos) return "";

	return buff.substr( quot_begin + 1, quot_end - quot_begin - 1);
}

/*static*/ Extractor::NAMES Extractor::tokenize( const std::string& source)
{
	NAMES result;
	if( source.empty()) return result;

	const std::string whsp = " "; // whitespaces to ignore
	unsigned int last = 0;
	do
	{
		unsigned int curr_token_start = source.find_first_not_of( whsp, last);
		unsigned int curr_token_end = source.find( whsp, curr_token_start);
		if( curr_token_end > curr_token_start)
			result.push_back( source.substr( curr_token_start, curr_token_end - curr_token_start));
		last = curr_token_end;
	}
	while( last != std::string::npos );

	return result;
}
