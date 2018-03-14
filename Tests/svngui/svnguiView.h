
// svnguiView.h : interface of the CsvnguiView class
//

#pragma once


class CsvnguiView : public CEditView
{
protected: // create from serialization only
	CsvnguiView();
	DECLARE_DYNCREATE(CsvnguiView)

// Attributes
public:
	CsvnguiDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CsvnguiView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in svnguiView.cpp
inline CsvnguiDoc* CsvnguiView::GetDocument() const
   { return reinterpret_cast<CsvnguiDoc*>(m_pDocument); }
#endif

