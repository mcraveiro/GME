#if !defined(AFX_GMEVIEWFRAME_H__7F2998F4_8B14_42a5_933D_B27192C030AC__INCLUDED_)
#define AFX_GMEVIEWFRAME_H__7F2998F4_8B14_42a5_933D_B27192C030AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GMEViewFrame.h : header file
//

#include "Resource.h"
#include "ModelPropertiesDlgBar.h"
#include "GMEViewDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CGMEViewFrame dialog


class CGMEViewFrame : public CMiniFrameWnd
{
	DECLARE_DYNCREATE(CGMEViewFrame)

// Construction
public:
	CGMEViewFrame();   // standard constructor
	virtual ~CGMEViewFrame();

public:
// Dialog Data
	//{{AFX_DATA(CGMEViewFrame)
	enum { IDD = IDD_GMEVIEW_DIALOG };
	//}}AFX_DATA

	CModelPropertiesDlgBar	propBar;
	CGMEViewDlg				viewWnd;

	static int				instanceCount;
	static CString			strMyViewWndClass;
	CComPtr<IMgaProject>	mgaProject;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMEViewFrame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

public:
	void					SetCurrentProject(CComPtr<IMgaProject> project);
	void					SetMetaModel(CComPtr<IMgaMetaModel> meta);
	void					SetModel(CComPtr<IMgaModel> model);
	void					ChangeAspect(int aspect);
	void					Invalidate(void);
// Implementation
protected:
	void					InitPropBarFromMeta(CComPtr<IMgaMetaModel> meta);
	void					InitPropBarFromModel(CComPtr<IMgaModel> mgaModel);
	void					InitAspectBox(CComPtr<IMgaMetaModel> meta);
	void					InitModelKindName(CComPtr<IMgaMetaModel> meta);
	void					InitModelName(CComPtr<IMgaModel> mgaModel);
	void					InitModelType(CComPtr<IMgaModel> mgaModel);
	void					InitModelTypeName(CComPtr<IMgaFCO> baseType, CComPtr<IMgaModel> mgaModel);

	// Generated message map functions
	//{{AFX_MSG(CGMEViewFrame)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSelchangeAspectBox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int		GetAspectProperty(void) const;
	void	SetAspectProperty(int ind);
	void	SetKindNameProperty(CString& kindName);
	void	GetNameProperty(CString& txt) const;
	void	SetNameProperty(CString& name);
	void	SetTypeProperty(bool isType);
	void	SetTypeNameProperty(CString& typeName);

	void	SendAspectChange(long index);
	void	SendZoomChange(long index);
	void	SendWriteStatusZoom(long zoomVal);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEVIEWFRAME_H__7F2998F4_8B14_42a5_933D_B27192C030AC__INCLUDED_)
