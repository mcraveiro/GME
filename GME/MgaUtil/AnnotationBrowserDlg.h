#if !defined(AFX_ANNOTATIONBROWSERDLG_H__64911022_2D71_4518_9D08_99D93BE9D2DD__INCLUDED_)
#define AFX_ANNOTATIONBROWSERDLG_H__64911022_2D71_4518_9D08_99D93BE9D2DD__INCLUDED_

#include "AnnotationList.h"
#include "AnnotationAspectList.h"
#include "AnnotationPanel.h"
#include "AnnotationNode.h"
#include "SplitterBar.h"
#include "colorbtn.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnnotationBrowserDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnnotationBrowserDlg dialog

class CAnnotationBrowserDlg : public CDialog
{
// Construction
public:
	CAnnotationBrowserDlg(const CComPtr<IMgaModel> &model, const CComPtr<IMgaRegNode> &focus, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAnnotationBrowserDlg();
	
	void UpdateAnnotations(void);

// Dialog Data
	//{{AFX_DATA(CAnnotationBrowserDlg)
	enum { IDD = IDD_ANNOTATION_BROWSER };
	CAnnotationAspectList	m_wndAnnotationAspectList;
	CAnnotationList			m_wndAnnotationList;
	CAnnotationPanel		m_wndAnnotation;
	CString					m_modelName;
	CString					m_modelRole;
	CString					m_modelKind;
	CString					m_anName;
	CString					m_anText;
	CString					m_anFont;
	BOOL					m_aaDefPos;
	long					m_aaXCoord;
	long					m_aaYCoord;
	BOOL					m_bGradientFill;
	int						m_iGradientDirection;
	BOOL					m_bCastShadow;
	int						m_iShadowDepth;
	int						m_iShadowDirection;
	BOOL					m_bRoundCornerRect;
	int						m_iRoundCornerRadius;
	//}}AFX_DATA
	LOGFONT					m_anLogFont;
	CColorBtn				m_colorbtn;
	CColorBtn				m_bgcolorbtn;
	CColorBtn				m_shadowcolorbtn;
	CColorBtn				m_gradientcolorbtn;
	CSplitterBar			m_wndSplitterBar;
	CComPtr<IMgaModel>		m_model;
	CComPtr<IMgaRegNode>	m_focus;

	CStringArray			m_aspectNames;
	CAnnotationNodeList		m_annotations;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnnotationBrowserDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillAspects(void);
	void FillAnnotations(void);
	void UpdateFontStr(void);
	void SavePanelToNode(CAnnotationNode *node);
	void LoadNodeToPanel(CAnnotationNode *node);
	void LoadAAToVisibilityPanel(CAnnotationAspect *aa);
	void SaveVisibilityPanelToAA(CAnnotationAspect *aa);

	// Generated message map functions
	//{{AFX_MSG(CAnnotationBrowserDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	afx_msg void OnItemchangedAnnotationList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonFont();
	afx_msg void OnItemchangedListAspect(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckDefpos();
	afx_msg void OnBnClickedShowhidebutton();
	afx_msg void OnBnClickedRederivebutton();
	afx_msg void OnBnClickedCheckGradientfill();
	afx_msg void OnBnClickedCheckCastshadow();
	afx_msg void OnBnClickedCheckRoundCorner();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CButton m_inheritable;
	CButton m_showHideBtn;
	CButton m_rederiveBtn;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANNOTATIONBROWSERDLG_H__64911022_2D71_4518_9D08_99D93BE9D2DD__INCLUDED_)
