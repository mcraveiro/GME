#ifndef METHOD_H
#define METHOD_H

#include "string"

class Any;
class Method;

class MethodLexicographicSort
{
public:
  bool operator()( const Method& op1, const Method& op2) const;
};


class Method
{
public:
	bool m_virtual;
	bool m_static;
	bool m_template;
	std::string m_returnValue;
	std::string m_signature;
	std::string m_implementation;
	std::string m_comment;
	Any * m_container;

	std::string getHeader();
	std::string getSource();
	std::string getExposed( const std::string& repl_containter);
	std::string getHidden();

	Method();
	operator bool();
};



#endif