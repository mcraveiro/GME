#pragma once

#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMError.hpp>
class MsgConsole;

class DOMErrorPrinter : public XERCES_CPP_NAMESPACE::DOMErrorHandler
{
public:

	DOMErrorPrinter( MsgConsole* p_consolePtr)
	: m_consolePtr( p_consolePtr)
	{};

	virtual ~DOMErrorPrinter() 
	{};

	void resetErrors()
	{};

    virtual bool handleError(const XERCES_CPP_NAMESPACE::DOMError& domError); // implemented callback method

protected:
	MsgConsole* m_consolePtr;

private :
    DOMErrorPrinter(const DOMErrorHandler&);
    void operator=(const DOMErrorHandler&);
};

