//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	GMEConstraintTreeCtrl.h
//
//###############################################################################################################################################

#if !defined(AFX_CONSTRAINTTREECTRL_H__2D5FDBA1_844B_49BA_A3E1_E360F51E90A3__INCLUDED_)
#define AFX_CONSTRAINTTREECTRL_H__2D5FDBA1_844B_49BA_A3E1_E360F51E90A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TreeCtrlEx.h"

#define CTVN_CHECKCHANGED ( WM_USER + 131 )
#define CTVN_CHECKCHANGEDREC ( WM_USER + 132 )
#define CTVN_SHOWITEM ( WM_USER + 133 )

#define GETSTATE( hItem )	\
	GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12

#define SETSTATE( hItem, eState )	\
	SetItemState( hItem, eState << 12, TVIS_STATEIMAGEMASK )

class CConstraintTreeCtrl;

//##############################################################################################################################################
//
//	C L A S S : CConstraintTreeCtrl <<< + CTreeCtrlEx
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################


class CConstraintTreeCtrl
	: public CTreeCtrlEx
{
	DECLARE_DYNAMIC(CConstraintTreeCtrl)

	public :
		typedef enum Kind_tag { TK_CONSTRAINT = 0, TK_OBJECT = 1, TK_UNKNOWN = 2 } Kind;

	private :
		Kind	m_eKind;

	public :
						CConstraintTreeCtrl();
		virtual 			~CConstraintTreeCtrl();

				void 	SetKind( Kind eKind );
				Kind 	GetKind() const;
				void 	SelectAllItems( HTREEITEM root, bool select );
				void 	SortItemChildren( HTREEITEM hItem );

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConstraintTreeCtrl)
	//}}AFX_VIRTUAL

protected:
				void 	SendTreeNotification( UINT uiMessage, HTREEITEM hItem );

protected:

	//{{AFX_MSG(CConstraintTreeCtrl)
	afx_msg 	void 	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg 	void 	OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg 	void 	OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg 	void 	OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//################################################################################################################################

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONSTRAINTTREECTRL_H__2D5FDBA1_844B_49BA_A3E1_E360F51E90A3__INCLUDED_)
