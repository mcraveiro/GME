// MetaTreeCtrl.h: interface for the CMetaTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_METATREECTRL_H__A323A05D_8098_488C_BB47_04A38D4F77A0__INCLUDED_)
#define AFX_METATREECTRL_H__A323A05D_8098_488C_BB47_04A38D4F77A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MgaMappedTreeCtrl.h"

class CMetaTreeCtrl : public CMgaMappedTreeCtrl<CMgaObjectProxy>
{
	friend class CMetaPropertyPage;
public:
	HTREEITEM InsertItem(HTREEITEM hParent, CString strObjectName, LPUNKNOWN pUnknown, objtype_enum otObjectType);
	CMetaTreeCtrl();
	virtual ~CMetaTreeCtrl();


};

#endif // !defined(AFX_METATREECTRL_H__A323A05D_8098_488C_BB47_04A38D4F77A0__INCLUDED_)
