#if !defined(AFX_DLGAUTOLAYOUT_H__FF60C817_C49F_4110_96C7_BD85490C6F17__INCLUDED_)
#define AFX_DLGAUTOLAYOUT_H__FF60C817_C49F_4110_96C7_BD85490C6F17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAutoLayout.h : header file
//

#include "resource.h"
#include "ComponentLib.h"
#include "GMEGraph.h"
#include "LayoutOptimization.h"
#include "GAOptimizer.h"
#include "CommonSmart.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoLayout dialog

class CDlgAutoLayout : public CDialog, public LayoutOptimizerListener
{
// Construction
public:
	CDlgAutoLayout(CWnd* pParent = NULL);   // standard constructor

    virtual ~CDlgAutoLayout();

    void initialize( IMgaProject * project, IMgaModel* model );

	virtual LayoutOptimizerListener::ContinueAbortOrCurrent update( int percentage, LayoutSolution * sol, double score );

// Dialog Data
	//{{AFX_DATA(CDlgAutoLayout)
	enum { IDD = IDD_DIALOG_AUTOLAYOUT };
	CListBox	m_listAspects;
	CProgressCtrl	m_progressOptimization;
	CProgressCtrl	m_progressAspect;
	CButton	m_graph;
	BOOL	m_startFromScratch;
	CButton	m_startButton;
	CButton	m_abortButton;
	CButton m_currentResultsButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAutoLayout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void OptimizeAllAspects();
	void Optimize(CComObjPtr<IMgaMetaAspect>& aspect);
// Implementation
protected:    
    void drawSolution( CDC * dc, LayoutSolution * sol );


    CRect                       m_graphRect;
    CBrush                      m_backBrush;
    CDC                         m_graphDC;
    CBitmap                     m_graphBmp;
    CBitmap *                   m_graphOldBmp;
    
    LayoutSolution *            m_currentSolution;  // only for display purpose
    double                      m_score;

    CComObjPtr<IMgaProject>     m_project;
    CComObjPtr<IMgaModel>       m_model;
    CComObjPtr<IMgaMetaModel>   m_metaModel;
    CComObjPtr<IMgaMetaAspects> m_metaAspects;    

    int                         m_updateTime;
	bool						m_bAbortionRequested;
	bool m_bCurrentResults;

	// Generated message map functions
	//{{AFX_MSG(CDlgAutoLayout)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonStart();
	afx_msg void OnButtonAbort();
	afx_msg void OnButtonCurrentResults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAUTOLAYOUT_H__FF60C817_C49F_4110_96C7_BD85490C6F17__INCLUDED_)
