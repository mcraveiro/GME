#include "stdafx.h"

#include "Transcoder.h"
#include <xercesc/util/XMLUniDefs.hpp>


static const XMLCh  gXMLDecl1[] =
{
        chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l
    ,   chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i
    ,   chLatin_o, chLatin_n, chEqual, chDoubleQuote, chDigit_1, chPeriod
    ,   chDigit_0, chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c
    ,   chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual
    ,   chDoubleQuote, chNull
}; // = `<?xml version="1.0" encoding="` 

static const XMLCh  gXMLDecl2[] =
{
        chDoubleQuote, chQuestion, chCloseAngle
    ,   chLF, chNull
}; // = `"?>LF`




// ---------------------------------------------------------------------------
//  Transcoder: Constructors and Destructor
// ---------------------------------------------------------------------------
Transcoder::Transcoder()
	: m_pFormatter(0)
{
}

Transcoder::~Transcoder()
{
	if ( m_pFormatter)
	{
		delete m_pFormatter;
		m_pFormatter = 0;
	}

	if ( is_open()) close();
}

void Transcoder::init( const TCHAR * f, const TCHAR * const encodingName)
{
	ASSERT( !m_pFormatter);

	m_pFormatter = new XMLFormatter
    (
        encodingName
        , 0
        , this
        , XMLFormatter::NoEscapes
        , XMLFormatter::UnRep_CharRef
    );

	ASSERT( !is_open() );

	open(f, std::ios::out | std::ios::trunc | std::ios::binary);
	if( fail() || !is_open() )
		HR_THROW(E_INVALID_FILENAME);

	// write BOM
	if (_tcsicmp(_T("UTF-16"), encodingName) == 0)
	{
		*m_pFormatter << (XMLCh) 0xFEFF;
	}
    *m_pFormatter << gXMLDecl1 << m_pFormatter->getEncodingName() << gXMLDecl2; //will dump '<?xml version="1.0" encoding="UTF-8"?> 
}


void Transcoder::finalize()
{
	delete m_pFormatter;
	m_pFormatter = 0;

	close();
}


Transcoder& 
Transcoder::operator <<( Modes mode)
{
    if ( mode == NoEscape)
		*m_pFormatter << XMLFormatter::NoEscapes;
	else if ( mode == StdEscape)
		*m_pFormatter << XMLFormatter::StdEscapes;

	return *this;
}


Transcoder& 
Transcoder::operator <<( const XMLCh* const toWrite)
{
	*m_pFormatter << toWrite;
	return *this;
}

Transcoder& 
Transcoder::operator <<( const char * const toWrite)
{
    XMLCh * fUnicodeForm = XMLString::transcode( toWrite);
	
	operator<<( fUnicodeForm);
	
	XMLString::release(&fUnicodeForm);

	return *this;
}

Transcoder& 
Transcoder::operator <<( const char toWrite)
{
	wchar_t tmp[2] = { toWrite, 0 };

	operator<<( tmp);

	return *this;
}

Transcoder& 
Transcoder::operator <<( const std::tstring& toWrite)
{
	operator<<( toWrite.c_str());

	return *this;
}

// ---------------------------------------------------------------------------
//  Transcoder: Overrides of the output formatter target interface
// ---------------------------------------------------------------------------
void Transcoder::writeChars(const   XMLByte* const  toWrite, const XMLSize_t count, XMLFormatter* const   formatter)
{
	ASSERT( sizeof( XMLByte) == sizeof( char));
	write( (const char * const)toWrite, count);//zolmol: cast from const unsigned char * const
}
