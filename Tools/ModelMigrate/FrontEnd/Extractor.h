#pragma once
#include <string>
#include <vector>
#include <map>

class Extractor
{
public:
	typedef std::vector<std::string> NAMES;
	typedef std::map< std::string, std::string> ATTRS;

public:
	Extractor(void);
	~Extractor(void);
public:
	void doJob( std::string& pFName);
	void doPlainLoad( std::string& pFName);
	const NAMES& getKinds();
	const ATTRS& getAttrs();

protected:
	void init();
	NAMES m_names;
	ATTRS m_attrs;

public:
	static std::string get( const std::string& attr, const std::string& buff);
	static NAMES tokenize( const std::string& pList);
};
