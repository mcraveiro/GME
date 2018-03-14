// ObjectEventWrapper.cpp: implementation of the CMgaObjectEventWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ObjectEventWrapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMgaObjectEventWrapper::CMgaObjectEventWrapper(IMgaObject *pMgaObject, 
			unsigned long eventmask):m_ccpMgaObject(pMgaObject),m_eventmask(eventmask)
{

}

CMgaObjectEventWrapper::~CMgaObjectEventWrapper()
{

}
