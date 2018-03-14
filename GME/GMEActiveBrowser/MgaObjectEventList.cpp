// MgaObjectEventList.cpp: implementation of the CMgaObjectEventList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MgaObjectEventList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMgaObjectEventList::CMgaObjectEventList()
{

}

CMgaObjectEventList::~CMgaObjectEventList()
{

}

void CMgaObjectEventList::Add(IMgaObject *pMgaObject, unsigned long eventmask)
{
 
	AddTail(new CMgaObjectEventWrapper(pMgaObject,eventmask));
}

void CMgaObjectEventList::RemoveAll()
{
 POSITION pos=GetHeadPosition();

 while(pos)
 {
	 CMgaObjectEventWrapper* ptr=(CMgaObjectEventWrapper*)GetNext(pos);
	 delete ptr;
 }

 CPtrList::RemoveAll();
}

void CMgaObjectEventList::RemoveAt(POSITION position)
{
	CMgaObjectEventWrapper* ptr=(CMgaObjectEventWrapper*)GetAt(position);
	CPtrList::RemoveAt(position);

	delete ptr;
}


