// GMEActiveBrowserDropTarget.cpp: implementation of the CGMEActiveBrowserDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "MgaMappedTreeCtrl.h"
#include "GMEActiveBrowserDropTarget.h"
#include "..\GME\GMEOLEData.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGMEActiveBrowserDropTarget::CGMEActiveBrowserDropTarget(CMgaMappedTreeCtrlBase* pParent)
{
	m_pParent=pParent;
	m_bCreatedDragImageList=FALSE;
	m_pDragImageList=NULL;

	try
	{
		COMTHROW( m_EventLogger.CoCreateInstance(L"Mga.MgaEventLogger"));
	}
	catch(...)
	{
		m_EventLogger = NULL;
	}
}

CGMEActiveBrowserDropTarget::~CGMEActiveBrowserDropTarget()
{

}

DROPEFFECT CGMEActiveBrowserDropTarget::OnDragEnter(CWnd *pWnd, COleDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
	ACTIVEBROWSERDROPTARGET_LOGEVENT("CGMEActiveBrowserDropTarget::OnDragEnter()\r\n");

	m_pDragImageList=NULL;
	m_bCreatedDragImageList=FALSE;

	if(m_pParent->m_CurrentDragImage.GetSafeHandle()!=NULL) // Drag initiated by the tree control
	{
		ACTIVEBROWSERDROPTARGET_LOGEVENT("    Drag initiated by the tree control\r\n");
		m_pDragImageList=&m_pParent->m_CurrentDragImage;
		m_ptHotSpot=m_pParent->m_ptHotSpot;

	}

	if(m_pDragImageList==NULL)
	{
		m_pDragImageList=ExtractGMEDataObject(pDataObject);					 
	}

	
	if(m_pDragImageList!=NULL)
	{
		m_pDragImageList->BeginDrag(0,m_ptHotSpot);		
		m_pDragImageList->DragShowNolock(TRUE);
	}

	return OnDragOver(pWnd,pDataObject,dwKeyState,point);
}




DROPEFFECT CGMEActiveBrowserDropTarget::OnDragOver(CWnd *pWnd, COleDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{

	if(point.y>=0 && point.y<=2) // Upward scrolling
	{
		if(m_pDragImageList!=NULL)
		{
			m_pDragImageList->DragShowNolock(FALSE);		
			m_pParent->ScrollUp();
			m_pDragImageList->DragShowNolock(TRUE);
		}
		else
		{
			m_pParent->ScrollUp();
		}

	}

	
	if(m_pDragImageList!=NULL)
	{
		CRect rectWnd;
		m_pParent->GetWindowRect(rectWnd);
		m_pDragImageList->DragMove(point+rectWnd.TopLeft());						
	}
	
		
	if(!m_pParent->IsRelevantDropTarget(point,m_pDragImageList))
	{
		ACTIVEBROWSERDROPTARGET_LOGEVENT("CGMEActiveBrowserDropTarget::OnDragOver DROPEFFECT_NONE\r\n");
		return DROPEFFECT_NONE;
	}
	
	// PETER: Only copy is supported on XML based clipboard data
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CComPtr<IMgaProject> ccpMgaProject=pApp->m_CurrentProject.m_MgaContext.m_ccpProject;
	if(!CGMEDataSource::IsGmeNativeDataAvailable(pDataObject, ccpMgaProject)) {
		if (CGMEDataSource::IsXMLDataAvailable(pDataObject)) {
			m_doDragOperation=DRAGOP_COPY;
			ACTIVEBROWSERDROPTARGET_LOGEVENT("CGMEActiveBrowserDropTarget::OnDragOver DROPEFFECT_COPY\r\n");
			return DROPEFFECT_COPY;
		}
		ACTIVEBROWSERDROPTARGET_LOGEVENT("CGMEActiveBrowserDropTarget::OnDragOver DROPEFFECT_NONE\r\n");
		return DROPEFFECT_NONE;
	}
	// PETER: end

	// No modifier: Move operation 
	// Ctrl: Copy (signified by "plus" icon over mouse cursor) 
	// Ctrl+Shift: Create reference (signified by link icon over mouse cursor) 
	// Alt: Create Instance (signified by link icon over mouse cursor) 
	// Alt+Shift: Create Sub Type (signified by link icon over mouse cursor) 

	// Getting the modifiers
	BOOL bControl=dwKeyState & MK_CONTROL;		
	BOOL bShift=(dwKeyState & MK_SHIFT);		
	BOOL bAlt(dwKeyState & MK_ALT);				

	
	DROPEFFECT deRetVal;
	if(bControl&&bShift)		// Reference
	{
		deRetVal=DROPEFFECT_LINK;
		ACTIVEBROWSERDROPTARGET_LOGEVENT("CGMEActiveBrowserDropTarget::OnDragOver DROPEFFECT_LINK\r\n");
		m_doDragOperation=DRAGOP_REFERENCE;
	}
	else if(bAlt&&bShift)		// Create SubType
	{
		deRetVal=DROPEFFECT_COPY | DROPEFFECT_LINK; 
		ACTIVEBROWSERDROPTARGET_LOGEVENT("CGMEActiveBrowserDropTarget::OnDragOver DROPEFFECT_COPY | DROPEFFECT_LINK\r\n");
		m_doDragOperation=DRAGOP_SUBTYPE;
	}
	else if(bControl)			// Copy
	{
		deRetVal=DROPEFFECT_COPY;
		ACTIVEBROWSERDROPTARGET_LOGEVENT("CGMEActiveBrowserDropTarget::OnDragOver DROPEFFECT_COPY\r\n");
		m_doDragOperation=DRAGOP_COPY;
	}
	else if(bAlt)				// Instance
	{
		deRetVal=DROPEFFECT_COPY | DROPEFFECT_LINK;
		ACTIVEBROWSERDROPTARGET_LOGEVENT("CGMEActiveBrowserDropTarget::OnDragOver DROPEFFECT_COPY | DROPEFFECT_LINK\r\n");
		m_doDragOperation=DRAGOP_INSTANCE;
	}
	else						// Move
	{
		deRetVal=DROPEFFECT_MOVE;
		ACTIVEBROWSERDROPTARGET_LOGEVENT("CGMEActiveBrowserDropTarget::OnDragOver DROPEFFECT_MOVE\r\n");
		m_doDragOperation=DRAGOP_MOVE;
	}

	return deRetVal;
}

void CGMEActiveBrowserDropTarget::OnDragLeave(CWnd *pWnd)
{
	ACTIVEBROWSERDROPTARGET_LOGEVENT("CGMEActiveBrowserDropTarget::OnDragLeave()\r\n");
	if(m_pDragImageList!=NULL) 
	{
		m_pDragImageList->DragLeave(pWnd);
		m_pDragImageList->EndDrag();
		m_pParent->Invalidate();

		if(m_bCreatedDragImageList)  // The DragImageList was created here, so delete it!
		{
			m_pDragImageList->DeleteImageList();
			delete m_pDragImageList;
			m_pDragImageList=NULL;
			m_bCreatedDragImageList=FALSE;
		}
	}

}


BOOL CGMEActiveBrowserDropTarget::OnDrop(CWnd *pWnd, COleDataObject *pDataObject, DROPEFFECT, CPoint point)
{
	ACTIVEBROWSERDROPTARGET_LOGEVENT("CGMEActiveBrowserDropTarget::OnDrop()\r\n");
	OnDragLeave(pWnd);

	return m_pParent->DoDrop(m_doDragOperation,pDataObject,point);
}


CImageList* CGMEActiveBrowserDropTarget::ExtractGMEDataObject(COleDataObject *pDataObject)
{

	CGMEDataDescriptor GMEDataDescriptor;
	if(GMEDataDescriptor.Load(pDataObject))
	{		
		CRect rectBoundingRect;
		GMEDataDescriptor.GetBoundingRect(rectBoundingRect);

		 CClientDC dcClient(m_pParent);
		 CDC dcMem;
		 CBitmap Bitmap;

		 if (!dcMem.CreateCompatibleDC(&dcClient))
		  return NULL;

		 if (!Bitmap.CreateCompatibleBitmap(&dcClient, 
			 rectBoundingRect.Width(), 
			 rectBoundingRect.Height()))
		  return NULL;

		 CBitmap *pOldMemDCBitmap = dcMem.SelectObject(&Bitmap);

		 COLORREF cMaskColor=RGB(0,255,0);
		 dcMem.FillSolidRect(0, 0, 
							 rectBoundingRect.Width(), 
							 rectBoundingRect.Height(), 
							 cMaskColor);
		 
		 GMEDataDescriptor.SimpleDraw(&dcMem,-rectBoundingRect.TopLeft());
		 
		
				   
		 //BitBlt(dcClient.GetSafeHdc(),0,0,rectBoundingRect.Width(),rectBoundingRect.Height(),dcMem.GetSafeHdc(),0,0,SRCCOPY);				   
		
		 dcMem.SelectObject(pOldMemDCBitmap);

		 CImageList* pImageList=new CImageList();
	
		 pImageList->Create(rectBoundingRect.Width(), 
									rectBoundingRect.Height(), 
									ILC_COLOR | ILC_MASK, 
									0, 1);

		 // Green is used as mask color
		 pImageList->Add(&Bitmap, cMaskColor); 

    	 //pImageList->Draw(&dcClient,0,CPoint(0,0), ILD_TRANSPARENT);	

		 Bitmap.DeleteObject();
		 m_bCreatedDragImageList=TRUE;
		 m_ptHotSpot=-rectBoundingRect.TopLeft();
		 return pImageList;
	}
	return NULL;
}
