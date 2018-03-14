
#ifndef MGA_COMMONMGATRUKK_H
#define MGA_COMMONMGATRUKK_H

#include <memory>

#define MGACOLL_ITERATE(iftype, collifptr) \
{ \
	ASSERT( collifptr != NULL ); \
	long iter_count = 0; \
	COMTHROW( collifptr->get_Count(&iter_count) ); \
	ASSERT( iter_count >= 0 ); \
	std::unique_ptr<CComPtr<iftype>[]> array(new CComPtr<iftype>[iter_count]); \
	CComPtr<iftype> *arrptr, *arrend; \
	if(iter_count > 0) \
		COMTHROW( collifptr->GetAll(iter_count, &(*array.get())) ); \
	arrend = array.get()+iter_count; \
	for(arrptr = array.get(); arrptr != arrend; arrptr++)

#define MGACOLL_ITER (*arrptr)

#define MGACOLL_AT_END (arrptr == arrend)

#define MGACOLL_ITERATE_END }

#endif//MGA_COMMONMGATRUKK_H

