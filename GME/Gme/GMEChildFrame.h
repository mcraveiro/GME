// GMEChildFrame.h: interface for the CGMEChildFrame class.
//
//////////////////////////////////////////////////////////////////////
//{{AFX_INCLUDES()
#include "GMEChildFrameWrapper.h"
//}}AFX_INCLUDES

#if !defined(AFX_GMECHILDFRAME_H__A4303466_69AA_4DE9_BEDC_F67BBACF83FD__INCLUDED_)
#define AFX_GMECHILDFRAME_H__A4303466_69AA_4DE9_BEDC_F67BBACF83FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "GuiMeta.h"

/////////////////////////////////////////////////////////////////////////////
// CGMEChildFrame window

class CGMEChildFrame : public CView
{
protected:
	DECLARE_DYNCREATE(CGMEChildFrame)
// Construction
public:
	CGMEChildFrame();
	virtual ~CGMEChildFrame() { theInstance = 0; };

	IDispatch * GetInterface() {
		LPUNKNOWN pUnk = m_ChildFrame.GetControlUnknown();

		// From there get the IDispatch interface of control.
		LPDISPATCH pDisp = NULL;
		pUnk->QueryInterface(IID_IDispatch, (LPVOID*)&pDisp);
		return pDisp;
	}

	// Dialog Data
	//{{AFX_DATA(CGMEChildFrame)
	enum { IDD = IDD_CHILDFRAME_DIALOG };
	CGMEChildFrameWrapper	m_ChildFrame;
	//}}AFX_DATA

	static CGMEChildFrame *theInstance;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMEChildFrame)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CGMEChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAspectChangedGmeViewCtrl(LONG index);
	afx_msg void OnWriteStatusZoomGmeViewCtrl(LONG zoomVal);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
public:
	void SetProject(CComPtr<IMgaProject> mgaProject);
	void SetMetaModel(CGuiMetaModel* meta);
	void SetModel(CComPtr<IMgaModel> mgaModel);
	void ChangeAspect(int aspect);
	void CycleAspect(void);
	void Invalidate(void);

	CComPtr<IMgaModel>&	GetMgaModel(void) { return mgaModel; };

protected:
	CGuiMetaModel*		guiMetaModel;
	CComPtr<IMgaModel>	mgaModel;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMECHILDFRAME_H__A4303466_69AA_4DE9_BEDC_F67BBACF83FD__INCLUDED_)
