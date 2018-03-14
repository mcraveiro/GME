// GMEPartBrowser.cpp: implementation of the CGMEPartBrowser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GMEApp.h"
#include "GMEstd.h"
#include "mainfrm.h"
#include "GMEDoc.h"
#include "GMEView.h"
#include "GMEPartBrowser.h"
#include "GuiMeta.h"


/////////////////////////////////////////////////////////////////////////////
// CGMEPartBrowser

CGMEPartBrowser* CGMEPartBrowser::theInstance = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGMEPartBrowser::CGMEPartBrowser():
	guiMetaModel (NULL)
{
	//{{AFX_DATA_INIT(CGMEPartBrowser)
	//}}AFX_DATA_INIT

	VERIFY(theInstance == 0);
	theInstance = this;
}


BEGIN_MESSAGE_MAP(CGMEPartBrowser, CDockablePane)
	//{{AFX_MSG_MAP(CGMEPartBrowser)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CGMEPartBrowser, CDockablePane)
	//{{AFX_EVENTSINK_MAP(CGMEPartBrowser)
	ON_EVENT(CGMEPartBrowser, IDC_PARTBROWSERCTRL1, 1 /* AspectChanged */, OnAspectChangedGmePartBrowserCtrl, VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMEPartBrowser message handlers

int CGMEPartBrowser::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_PartBrowserWrapper.Create(_T("PartBrowser"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 150), this, IDC_PARTBROWSERCTRL1))
		return -1;

	// older versions of Windows* (NT 3.51 for instance) fail with DEFAULT_GUI_FONT
	if (!m_font.CreateStockObject(DEFAULT_GUI_FONT))
		if (!m_font.CreatePointFont(80, _T("MS Sans Serif")))
			return -1;
	m_PartBrowserWrapper.SetFont(&m_font);

	return 0;
}


void CGMEPartBrowser::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CDockablePane::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);

	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}


void CGMEPartBrowser::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(rc);

	m_PartBrowserWrapper.MoveWindow(rc);
}


void CGMEPartBrowser::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CGMEPartBrowser)
	DDX_Control(pDX, IDC_PARTBROWSERCTRL1, m_PartBrowserWrapper);
	//}}AFX_DATA_MAP
}


/* 
ActiveX controls do not have their message pump, it is owned by their containers.
The container in case of GME is a kind of control bar, which is treated as a dialog.
Dialog box messages are filtered by ::IsDialogMessage, which does not allow the 
default dialog kestroke messages (ESC - close dialog, ENTER - push default button
TAB - next item in the tab order etc...) to be propagated to the controls placed on 
the dialog.

Here we avoid calling the default PreTranslateMessage which filtered by 
::IsDialogMessage, dispatch it directly to the controls.

Tihamer

*/

BOOL CGMEPartBrowser::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
			case VK_RETURN:
			case VK_ESCAPE:
			case VK_DELETE:
			// Modification by Volgyesi (undo problems)
			case VK_CONTROL:
			case 'z':
			case 'Z':
			// Modification End
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
		}
	}

	return CDockablePane::PreTranslateMessage(pMsg);
}


void CGMEPartBrowser::SetProject(CComPtr<IMgaProject>& mgaProject)
{
	m_PartBrowserWrapper.SetCurrentProject(mgaProject);
}


void CGMEPartBrowser::SetMetaModel(CGuiMetaModel* meta)
{
	CComPtr<IUnknown> pMeta = NULL;
	guiMetaModel = meta;
	if (meta != NULL) {
		CComQIPtr<IMgaMetaModel> iMeta;
		iMeta = meta->mgaMeta;
		if (iMeta)
			pMeta = iMeta;
	}
	m_PartBrowserWrapper.SetMetaModel(pMeta);
}


void CGMEPartBrowser::SetBgColor(COLORREF bgColor)
{
	m_PartBrowserWrapper.SetBgColor((OLE_COLOR)bgColor);
}


void CGMEPartBrowser::ChangeAspect(int ind)
{
	m_PartBrowserWrapper.ChangeAspect(ind);
}


void CGMEPartBrowser::CycleAspect()
{
	m_PartBrowserWrapper.CycleAspect();
}


void CGMEPartBrowser::RePaint(void)
{
	m_PartBrowserWrapper.RePaint();
	CDockablePane::PostMessage(WM_PAINT); // @@@ Tihamer: SHOULD NOT BE INVALIDATE HERE?
}


void CGMEPartBrowser::OnAspectChangedGmePartBrowserCtrl(LONG index)
{
	if (CGMEDoc::theInstance && guiMetaModel) {
		CGuiMetaAspect* am = guiMetaModel->FindAspect(index);
		VERIFY(am);
		CString aspName = am->name;
		CGMEDoc::theInstance->ChangeAspects(index, aspName);
	}
}
