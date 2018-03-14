// AutoComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "AutoComboBox.h"
#include "ActiveBrowsePropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CAutoComboBox

CAutoComboBox::CAutoComboBox(CTreeCtrlEx* pTreeCtrl)
{
	m_pTreeCtrl=pTreeCtrl;
}

CAutoComboBox::~CAutoComboBox()
{
}


BEGIN_MESSAGE_MAP(CAutoComboBox, CComboBox)
	//{{AFX_MSG_MAP(CAutoComboBox)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditChange)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoComboBox message handlers


void CAutoComboBox::OnEditKeyDown(int nVKeyCode)
{
		
	switch(nVKeyCode)
	{
		case VK_RETURN: OnKeyEnter();break;
		case VK_DELETE:;break;
		case VK_BACK:;break;
		case VK_ESCAPE:;break;
		case VK_DOWN:
			this->ShowDropDown();
			break;
		case VK_TAB: 
			{
				if( ::GetKeyState( VK_CONTROL) < 0) {
					CActiveBrowserPropertySheet *p = (CActiveBrowserPropertySheet *)m_pTreeCtrl->GetParent()->GetParent();
					if( p) p->nextTab( ::GetKeyState( VK_SHIFT) >= 0);
				} else {
					m_pTreeCtrl->SetFocus();
				}
			}break;
	}

}

void CAutoComboBox::Fill(CString& strText, BOOL bReload)
{
	// Clear combo
	for (int i = GetCount() - 1; i >= 0; i--)
	{
		DeleteString(i);
	}

	if(strText=="")
	{
		m_strPrevTxt="";
		return;
	}
	if(bReload)
	{
		
		m_pTreeCtrl->FindTextInItems(strText,NULL,m_strResults,m_hResults);
	}

	// Fill combo
	for(int i=0;i<=m_strResults.GetUpperBound();i++)
	{
		AddString(m_strResults.GetAt(i));
	}


	if(m_strResults.GetSize()>0)
	{
		
		// Setting selection
		SetCurSel(0);
		
		// Saving current combo edit text			
		m_strPrevTxt=strText;

		//Getting Current text again after drop down
		CString strComboText;
		GetWindowText(strComboText);

		// Set the selection to be all characters after the current selection.
		SetEditSel(m_strPrevTxt.GetLength(), strComboText.GetLength());
		
	}

}



void CAutoComboBox::OnEditChange() 
{
	// Get edit text
	CString strComboText;
	GetWindowText(strComboText);
	
	if(strComboText==m_strPrevTxt) // Deleted selection
	{
		return;
	}

	// Filling the combo
	Fill(strComboText);


	// Dumping CStringListEx for debug purposes
	#ifdef _DEBUG
	TRACE("\nCStringArray:\n");
	for( int j=0;j<=m_strResults.GetUpperBound();j++)
	{
		TRACE1("%d :",j);
		TRACE(m_strResults.GetAt(j));
		TRACE("\n");
	}
	TRACE("___________________________________________________________\n");
	#endif
					
}



void CAutoComboBox::OnSelendOk() 
{
	int nCurSel=GetCurSel();

	if(nCurSel!=CB_ERR)
	{		
		m_strPrevTxt=m_strResults.GetAt(nCurSel);
		m_pTreeCtrl->SetFocus();
		m_pTreeCtrl->ClearSelection();
		m_pTreeCtrl->SelectItem(m_hResults.GetAt(nCurSel));

	}
}

void CAutoComboBox::SetSelection(HTREEITEM hItem)
{
	
	CString strItemText=m_pTreeCtrl->GetItemText(hItem);
	
	Fill(strItemText);
	
	if(strItemText=="")return;

	for(int i=0;i<=m_hResults.GetUpperBound();i++)
	{
		if(m_hResults.GetAt(i)==hItem)
		{
			SetCurSel(i);
			return;
		}
	}
	ASSERT(FALSE);

}

void CAutoComboBox::OnKeyEnter()
{
	int nCurSel=GetCurSel();
	if(nCurSel==CB_ERR)
	{
		// Get the current text
		CString strComboText;
		GetWindowText(strComboText);

		m_strPrevTxt=strComboText;

		for(int i=0;i<=m_strResults.GetUpperBound();i++)
		{
			if(m_strResults.GetAt(i)==strComboText)
			{
				SetCurSel(i);
				OnSelendOk();
				return;
			}
		}
	}
}


//____________________________________________________
// NOT CALLED: We do not use this function, but can be useful 
BOOL CAutoComboBox::Filter(CString &strFilter)
{

	for(int i=0;i<=m_strResults.GetUpperBound();i++)
	{
		if(m_strResults.GetAt(i).Find(strFilter)!=0)
		{
			m_strResults.RemoveAt(i);
			m_hResults.RemoveAt(i);
			i--;
		}
	}
	return m_strResults.GetSize();

}
//___________________________________________________
