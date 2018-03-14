// PendingObjectPosRequest.h: interface for the CPendingObjectPosRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PENDINGOBJECTPOSREQUEST_H__EBB6D4C1_99EC_4065_8DE0_A3A8917F7D53__INCLUDED_)
#define AFX_PENDINGOBJECTPOSREQUEST_H__EBB6D4C1_99EC_4065_8DE0_A3A8917F7D53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PendingRequest.h"

class CPendingObjectPosRequest : public CPendingRequest  
{
public:
	void Execute(CGMEView *view);
	CPendingObjectPosRequest(CGuiObject *obj, CRect loc, int asp) : object(obj), location(loc), aspect(asp){}
	virtual ~CPendingObjectPosRequest();
	
	CGuiObject * object;

protected:
	int aspect;
	CRect location;
};

#endif // !defined(AFX_PENDINGOBJECTPOSREQUEST_H__EBB6D4C1_99EC_4065_8DE0_A3A8917F7D53__INCLUDED_)
