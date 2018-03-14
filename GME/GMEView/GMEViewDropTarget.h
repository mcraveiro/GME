// GMEViewDropTarget.h: interface for the CGMEViewDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMEACTIVEBROWSERDROPTARGET_H__ACA6C0DC_0719_4C87_AD25_7B3CD4D15B66__INCLUDED_)
#define AFX_GMEACTIVEBROWSERDROPTARGET_H__ACA6C0DC_0719_4C87_AD25_7B3CD4D15B66__INCLUDED_

#include "GMEView.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define GMEVIEWDROPTARGET_LOGEVENT(eventstring) \
{ \
	if(m_EventLogger != NULL) \
	{ \
		CComBSTR eventBSTR = eventstring; \
		m_EventLogger->LogEvent(eventBSTR); \
	} \
} \

class CGMEViewDlg;

class CGMEViewDropTarget : public COleDropTarget  
{
public:
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

	eDragOperation				m_doDragOperation;
	CPoint						m_ptHotSpot;
	BOOL						m_bCreatedDragImageList;
	CImageList* ExtractGMEDataObject(COleDataObject* pDataObject);
	CImageList*					m_pDragImageList;

	void		OnDragLeave( CWnd* pWnd );
	DROPEFFECT	OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT	OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	CGMEViewDropTarget(CGMEViewDlg* pParent);
	virtual ~CGMEViewDropTarget();

	CGMEViewDlg*				m_pParent;
	CComPtr<IMgaEventLogger>	m_EventLogger;
};

#endif // !defined(AFX_GMEACTIVEBROWSERDROPTARGET_H__ACA6C0DC_0719_4C87_AD25_7B3CD4D15B66__INCLUDED_)
