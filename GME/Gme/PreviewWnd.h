#if !defined(AFX_PREVIEWWND_H__C6E81D92_54EA_4673_B02F_6507CA7F76DD__INCLUDED_)
#define AFX_PREVIEWWND_H__C6E81D92_54EA_4673_B02F_6507CA7F76DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreviewWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreviewBtn window

class CPreviewBtn : public CButton
{
// Construction
public:
	CPreviewBtn();

// Attributes
public:
	void SetDC(CDC *bitmapDC) 
	{
		if (m_bitmapDC) 
		{
			CBitmap *bmp = m_bitmapDC->GetCurrentBitmap();
			delete m_bitmapDC; 
			if (bmp)
				delete bmp;
		}
		m_bitmapDC = bitmapDC;
	}
	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );

private:
	CDC *	m_bitmapDC;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewBtn)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPreviewBtn();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreviewBtn)
//	afx_msg void OnPaint();
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEWWND_H__C6E81D92_54EA_4673_B02F_6507CA7F76DD__INCLUDED_)
