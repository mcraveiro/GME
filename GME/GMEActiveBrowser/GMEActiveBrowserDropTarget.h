// GMEActiveBrowserDropTarget.h: interface for the CGMEActiveBrowserDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMEACTIVEBROWSERDROPTARGET_H__4A7D8C14_58DA_4785_932F_1BB960E09EF6__INCLUDED_)
#define AFX_GMEACTIVEBROWSERDROPTARGET_H__4A7D8C14_58DA_4785_932F_1BB960E09EF6__INCLUDED_

#include "GMEActiveBrowser.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ACTIVEBROWSERDROPTARGET_LOGEVENT(eventstring) \
{ \
	if(m_EventLogger != NULL) \
	{ \
		CComBSTR eventBSTR = eventstring; \
		m_EventLogger->LogEvent(eventBSTR); \
	} \
} \

class CMgaMappedTreeCtrlBase;

class CGMEActiveBrowserDropTarget : public COleDropTarget  
{
public:
	BOOL OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );
	eDragOperation m_doDragOperation;
	CPoint m_ptHotSpot;
	BOOL m_bCreatedDragImageList;
	CImageList* ExtractGMEDataObject(COleDataObject* pDataObject);
	CImageList* m_pDragImageList;
	void OnDragLeave( CWnd* pWnd );
	DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	CGMEActiveBrowserDropTarget(CMgaMappedTreeCtrlBase*);
	virtual ~CGMEActiveBrowserDropTarget();
	CMgaMappedTreeCtrlBase* m_pParent;
	CComPtr<IMgaEventLogger> m_EventLogger;

private:
	CGMEActiveBrowserDropTarget(const CGMEActiveBrowserDropTarget&);
};

#endif // !defined(AFX_GMEACTIVEBROWSERDROPTARGET_H__4A7D8C14_58DA_4785_932F_1BB960E09EF6__INCLUDED_)
