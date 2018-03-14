
#include <comdef.h>

class CXslt
{
public:
	static void doNativeXslt(LPCTSTR stylesheet, LPCTSTR infile, LPCTSTR outfile, _bstr_t& error);
};
