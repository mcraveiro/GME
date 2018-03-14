#include "stdafx.h"
#include ".\DomErrorPrinter.h"
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/util/XMLString.hpp>
#include "MsgConsole.h"
#include "xml_smart_ptr.h"
//#include "Transcoder.h"

using namespace XERCES_CPP_NAMESPACE;

bool DOMErrorPrinter::handleError(const DOMError &domError)
{
    msgtype_enum mt;
	bool fatal = false;

	if( domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
        mt = MSG_WARNING;
    else if( domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
        mt = MSG_ERROR;
    else
        fatal = true, mt = MSG_ERROR;

    // general message
	smart_Ch e_msg = XMLString::transcode(domError.getMessage());
	std::string msg = std::string( fatal? "[Fatal Error] ": "") + e_msg;

	// send msg
	if( m_consolePtr) m_consolePtr->sendMsg( msg, mt);
	

	// location info available?
	DOMLocator* loc = domError.getLocation();
	if( loc)
	{
		char line_nmbs[256];
		memset( line_nmbs, 0, sizeof(line_nmbs));

		smart_Ch f_str = XMLString::transcode(loc->getURI());
		sprintf( line_nmbs, "Line: %u Column: %u of File: ", (unsigned)loc->getLineNumber(), (unsigned)loc->getColumnNumber());
		std::string linfo = std::string( line_nmbs) + f_str;

		// send msg
		if( m_consolePtr) m_consolePtr->sendMsg( linfo, mt);
	}

    // Instructs the serializer to continue serialization if possible.
    return true;
}
