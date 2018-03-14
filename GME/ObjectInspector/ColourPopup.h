#if !defined(AFX_COLOURPOPUP_H__D0B75902_9830_11D1_9C0F_00A0243D1382__INCLUDED_)
#define AFX_COLOURPOPUP_H__D0B75902_9830_11D1_9C0F_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ColourPopup.h : header file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.
// It's free - so you get what you pay for.

#include "resource.h"

#define DEFAULT_BOX_VALUE -3

// To hold the colours and their names
typedef struct {
    COLORREF crColour;
    TCHAR    *szName;
} ColourTableEntry;

/////////////////////////////////////////////////////////////////////////////
// CColourPopup window

class CColourPopup : public CDialog
{
	DECLARE_DYNAMIC(CColourPopup)

// Construction
public:
    CColourPopup(CWnd* pParent = NULL);
	virtual ~CColourPopup();

// Dialog Data
	//{{AFX_DATA(CColourPopup)
	enum { IDD = IDD_COLORPICKER_DIALOG };
	//}}AFX_DATA

    static ColourTableEntry m_crColours[];
	CRect           m_RectBound;
    int             m_nNumColours;
    int             m_nNumColumns, m_nNumRows;
    int             m_nBoxSize, m_nMargin;
    int             m_nCurrentSel;
    int             m_nChosenColourSel;
    CString         m_strDefaultText;
    CString         m_strCustomText;
    CRect           m_CustomTextRect, m_DefaultTextRect, m_WindowRect;
    CFont           m_Font;
    CPalette        m_Palette;
    COLORREF        m_crInitialColour, m_crColour;
    CToolTipCtrl    m_ToolTip;

	BOOL            m_bChildWindowVisible;
	bool			m_bRightSideClick;
	bool			m_bFirstMouseMsgReceived;

// Implementation
public:
	void SetWindowSize(void);
	void SetParameters(CRect rectBound, COLORREF crColour, bool rightSideClick,
					   LPCTSTR szDefaultText = NULL, LPCTSTR szCustomText = NULL);
	COLORREF GetSelectedColor(void) const { return m_crColour; };

protected:
    void Initialize(void);
    int  GetIndex(int row, int col) const;
    int  GetRow(int nIndex) const;
    int  GetColumn(int nIndex) const;
    void FindCellFromColour(COLORREF crColour);
	BOOL GetCellRect(int nIndex, const LPRECT& rect);
    void CreateToolTips(void);
    void ChangeSelection(int nIndex);
    void EndSelection(int nMessage);
    void DrawCell(CDC* pDC, int nIndex);

    COLORREF GetColour(int nIndex)              { return m_crColours[nIndex].crColour; }
    LPCTSTR GetColourName(int nIndex)           { return m_crColours[nIndex].szName; }

    // Generated message map functions
protected:
    //{{AFX_MSG(CColourPopup)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnQueryNewPalette();
    afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CColourPopup)
    public:
    virtual void EndDialog(int nResult);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOURPOPUP_H__D0B75902_9830_11D1_9C0F_00A0243D1382__INCLUDED_)
