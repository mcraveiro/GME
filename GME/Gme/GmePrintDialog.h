#if !defined(AFX_GMEPRINTDIALOG_H__2E29F7AF_AE75_4ED0_84DA_727658342D1E__INCLUDED_)
#define AFX_GMEPRINTDIALOG_H__2E29F7AF_AE75_4ED0_84DA_727658342D1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GmePrintDialog.h : header file
//

#include "GuiMeta.h"
#include "GmeView.h"
#include "PreviewWnd.h"
#include <set>

using namespace std;


/////////////////////////////////////////////////////////////////////////////
// CGmePrintDialog dialog

class CGmePrintDialog : public CPrintDialog
{
//	DECLARE_DYNAMIC(CGmePrintDialog)

public:
	CGmePrintDialog(CGMEView *view, CGuiMetaModel *guiMeta, BOOL bPrintSetupOnly,
		// TRUE for Print Setup, FALSE for Print Dialog
		DWORD dwFlags = 0, CWnd* pParentWnd = NULL);

	//{{AFX_DATA(CGmePrintDialog)
	enum { IDD = IDD_PRINT_DIALOG  };
	CPreviewBtn	m_demo;
	CStatic	m_scaletxt;
	CButton	m_aspSelBtn;
	CButton	m_aspDeselBtn;
	CButton m_landBtn;
	CButton m_portBtn;
	CButton m_curraspBtn;
	CButton m_allaspBtn;
	CButton m_selaspBtn;
	CButton m_noheaderBtn;
	CSliderCtrl	m_scaleslider;
	CListBox	m_asplist;
	BOOL m_noheader;
	BOOL m_1page;
	BOOL m_autorotate;
	int		m_scale;
	//}}AFX_DATA

	bool IsPortrait() {return (m_autorotate)? m_autoPortrait: m_portrait;}
	bool IsAllAspect() {return m_selasp == ALL_ASP;}
	bool IsCurAspect() {return m_selasp == CUR_ASP;}
	bool IsSelAspect() {return m_selasp == SEL_ASP;}
	bool IsSelectedAspect(CString aspect);
	int NumOfSelAspects() 
	{ // if no selected, answers all of them selected
		if (m_selasp == CUR_ASP)
			return 1;
		else
			return (int)m_selAspects.size();
	}
	bool HasHeader() {return m_noheader == FALSE;}
	bool IsAutorotate() {return m_autorotate != FALSE;}
	bool IsFitOnePage() {return m_1page != FALSE;}
	int NumOfPages() {return m_scale;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGmePrintDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	BOOL OnInitDialog();

private:
	bool m_portrait;
	bool m_autoPortrait;
	enum {CUR_ASP, ALL_ASP, SEL_ASP} m_selasp;
	CGuiMetaModel *m_guiMeta;
	CGMEView *m_gmeview;
	CRect m_previewrect;

	typedef set<CString> AspectList;
	AspectList m_selAspects;

	CDC *CreatePreviewBitmap();

protected:
	//{{AFX_MSG(CGmePrintDialog)
	afx_msg void OnBtndeselect();
	afx_msg void OnBtnselect();
	afx_msg void OnXlandscape();
	afx_msg void OnXportrait();
	afx_msg void OnChknoheader();
	afx_msg void OnFitto1page();
	afx_msg void OnAutorotate();
	afx_msg void OnSelasp1057();
	afx_msg void OnAllasp1058();
	afx_msg void OnCurasp1056();
	afx_msg void OnReverse();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEPRINTDIALOG_H__2E29F7AF_AE75_4ED0_84DA_727658342D1E__INCLUDED_)
