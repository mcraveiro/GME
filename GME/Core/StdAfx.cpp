// stdafx.cpp : source file that includes just the standard includes
//  stdafx.pch will be the pre-compiled header
//  stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#endif


#ifdef CORE_USE_EASTL
// EASTL impl
void* operator new[](size_t size, const char* pName, int flags,
    unsigned debugFlags, const char* file, int line)
{
    return malloc(size);
}
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset,
    const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    // MSDN: The storage space pointed to by the return value is guaranteed to be suitably aligned for storage of any type of object. 
    return malloc(size);
}

int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
{
    #ifdef _MSC_VER
        return _vsnprintf(pDestination, n, pFormat, arguments);
    #else
        return vsnprintf(pDestination, n, pFormat, arguments);
    #endif
}

#include "allocator.cpp"
#include "assert.cpp"
#include "fixed_pool.cpp"
#include "hashtable.cpp"
#include "string.cpp"
#endif
