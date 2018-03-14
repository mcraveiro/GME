#if !defined(AFX_MODELPROPERTIESDLGBAR_H__08501133_C2F9_11D3_91E6_00104B98EAD9__INCLUDED_)
#define AFX_MODELPROPERTIESDLGBAR_H__08501133_C2F9_11D3_91E6_00104B98EAD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelPropertiesDlgBar.h : header file
//

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CModelPropertiesDlgBar dialog

#define MAX_ZOOM 100

class CModelPropertiesDlgBar : public CDialogBar
{
// Construction
public:
	CModelPropertiesDlgBar();   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CModelPropertiesDlgBar)
	enum { IDD = IDD_MODELPROPERTIESBAR };
	//}}AFX_DATA

	void ShowInstance();
	void ShowType();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelPropertiesDlgBar)
	//}}AFX_VIRTUAL
	void SetZoomList(int *list);
	void SetZoomVal(int kk);
	void NextZoomVal(int &kk);
	void PrevZoomVal(int &kk);

private:
	int m_zoomlist[MAX_ZOOM];

	void WriteNumToEdit(CEdit *edit, int kk);

// Implementation
protected:

	//{{AFX_MSG(CModelPropertiesDlgBar)
	virtual void OnOK();
	afx_msg void OnSelEnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_MODELPROPERTIESDLGBAR_H__08501133_C2F9_11D3_91E6_00104B98EAD9__INCLUDED_)
