#ifndef XML_TRANS_H
#define XML_TRANS_H

#include <fstream> // fstream.h
#include    <xercesc/sax/HandlerBase.hpp>
#include    <xercesc/framework/XMLFormatter.hpp>

XERCES_CPP_NAMESPACE_USE

class Transcoder : private XMLFormatTarget, private std::ofstream
{
public:
    // -----------------------------------------------------------------------
    //  Constructors
    // -----------------------------------------------------------------------
    Transcoder();
    ~Transcoder();

	void init( const TCHAR *, const TCHAR * const);
	void finalize();
	enum Modes
	{
		NoEscape, // normal transcoding 
		StdEscape // additionally escaping special characters like &<>'"
	};

    // -----------------------------------------------------------------------
    //  Implementations of the format target interface
    // -----------------------------------------------------------------------
    void writeChars(const   XMLByte* const  toWrite, const XMLSize_t count, XMLFormatter* const   formatter);
	
	Transcoder& operator << ( Modes mode);
	Transcoder& operator << ( const XMLCh* const toWrite);
	Transcoder& operator << ( const char * const toWrite);
	Transcoder& operator << ( const char toWrite);
	Transcoder& operator << ( const std::tstring& toWrite);

private :
    XMLFormatter* m_pFormatter;
	XMLPlatformUtilsTerminate_RAII xercesInit;
};


#endif // XML_TRANS_H