// InheritanceTreeCtrl.h: interface for the CInheritanceTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INHERITANCETREECTRL_H__BB1A14C1_4C16_4C3D_B83D_786F8AB9438E__INCLUDED_)
#define AFX_INHERITANCETREECTRL_H__BB1A14C1_4C16_4C3D_B83D_786F8AB9438E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MgaMappedTreeCtrl.h"

class CInheritanceTreeCtrl : public CMgaMappedTreeCtrl<CMgaObjectProxy>
{
	friend class CInheritancePropertyPage;
public:
	CInheritanceTreeCtrl();
	virtual ~CInheritanceTreeCtrl();
	HTREEITEM InsertItem(HTREEITEM hParent, CString strObjectName, LPUNKNOWN pUnknown, objtype_enum otObjectType);

	void SetItemProperties(HTREEITEM hItem);
	CMapStringToString m_StateBuffer;
	BOOL m_bIsStateStored;
	void RestoreState();
	void StoreState();

};

#endif // !defined(AFX_INHERITANCETREECTRL_H__BB1A14C1_4C16_4C3D_B83D_786F8AB9438E__INCLUDED_)
