// PendingRequest.h: interface for the CPendingRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PENDINGREQUEST_H__38524718_C794_45AF_ADE8_117CD7B0F1C2__INCLUDED_)
#define AFX_PENDINGREQUEST_H__38524718_C794_45AF_ADE8_117CD7B0F1C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPendingRequest  
{
public:
	CPendingRequest();
	virtual ~CPendingRequest();
	virtual void Execute(CGMEView *view) = 0;		// May be called only in READ_WRITE transactions !

};

#endif // !defined(AFX_PENDINGREQUEST_H__38524718_C794_45AF_ADE8_117CD7B0F1C2__INCLUDED_)
