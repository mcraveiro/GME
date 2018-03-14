
#include "stdafx.h"

#include "RecentConnStrList.h"

/////////////////////////////////////////////////////////////////////////////
// CRecentConnStrList

CRecentConnStrList::CRecentConnStrList(UINT nStart, LPCTSTR lpszSection,
	LPCTSTR lpszEntryFormat, int nSize, int nMaxDispLen)
{
	ASSERT(nSize != 0);
	m_arrNames.resize(nSize);

	m_nStart = nStart;
	m_strSectionName = lpszSection;
	m_strEntryFormat = lpszEntryFormat;
	m_nMaxDisplayLength = nMaxDispLen;
}

CRecentConnStrList::~CRecentConnStrList()
{
}

// Operations
void CRecentConnStrList::Add(LPCTSTR lpszConnName)
{
	ASSERT(m_arrNames.size() > 0);
	ASSERT(lpszConnName != NULL);
	ASSERT(AfxIsValidString(lpszConnName));

	// update the MRU list, if an existing MRU string matches conn name
	int iMRU;
	for (iMRU = 0; iMRU < (int)m_arrNames.size() - 1; iMRU++)
	{
		if (m_arrNames[iMRU].Compare(lpszConnName) == 0)
			break;      // iMRU will point to matching entry
	}
	// move MRU strings before this one down
	for (; iMRU > 0; iMRU--)
	{
		ASSERT(iMRU > 0);
		ASSERT(iMRU < (int)m_arrNames.size());
		m_arrNames[iMRU] = m_arrNames[iMRU - 1];
	}
	// place this one at the beginning
	m_arrNames[0] = lpszConnName;
}

void CRecentConnStrList::Remove(int nIndex)
{
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < (int)m_arrNames.size());

	m_arrNames[nIndex].Empty();
	int iMRU;
	for (iMRU = nIndex; iMRU < (int)m_arrNames.size() - 1; iMRU++)
		m_arrNames[iMRU] = m_arrNames[iMRU + 1];

	ASSERT(iMRU < (int)m_arrNames.size());
	m_arrNames[iMRU].Empty();
}

BOOL CRecentConnStrList::GetDisplayName(CString& strName, int nIndex) const
{
	ASSERT(m_arrNames.size() > 0);
	ASSERT(nIndex < (int)m_arrNames.size());
	if (m_arrNames[nIndex].IsEmpty())
		return FALSE;

	strName = m_arrNames[nIndex];
	return TRUE;
}

void CRecentConnStrList::UpdateMenu(CCmdUI* pCmdUI, bool enable)
{
	ASSERT(m_arrNames.size() > 0);

	CMenu* pMenu = pCmdUI->m_pMenu;
	if (m_strOriginal.IsEmpty() && pMenu != NULL)
		pMenu->GetMenuString(pCmdUI->m_nID, m_strOriginal, MF_BYCOMMAND);

	if (m_arrNames[0].IsEmpty())
	{
		// no MRU files
		if (!m_strOriginal.IsEmpty())
			pCmdUI->SetText(m_strOriginal);
		pCmdUI->Enable(FALSE);
		return;
	}

	if (pCmdUI->m_pMenu == NULL)
		return;

	for (unsigned int iMRU = 0; iMRU < m_arrNames.size(); iMRU++)
		pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);

	if (!enable) {
		pCmdUI->Enable(FALSE);
		return;
	}

	CString strName;
	for (int iMRU = 0; iMRU < (int)m_arrNames.size(); iMRU++)
	{
		if (!GetDisplayName(strName, iMRU))
			break;

		// double up any '&' characters so they are not underlined
		strName.Replace(_T("&"), _T("&&"));

		// insert mnemonic + the file name
		CString buf;
		if (iMRU < 10)	// Do not overlap mnemonic (in case of more than 10 (actually 9) MRU item)
			buf.Format(_T("&%ld "), (iMRU + 1 + m_nStart) % 10);
		pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex,
			MF_STRING | MF_BYPOSITION, pCmdUI->m_nID,
			buf + strName);
		// This causes error message with new MFC when we try to disable the last recent item
		// So instead we don't add any disabled recent item at all (see exit above the iteration).
		// Disabled recent list would occur when there's an open project without any open model,
		// in case of this MFC automatically switches back from the IDR_GMETYPE menu to the IDR_MAINFRAME
//		if (!enable)
//			pCmdUI->Enable(FALSE);
		pCmdUI->m_nIndex++;
		pCmdUI->m_nID++;
	}

	// update end menu count
	pCmdUI->m_nIndex--; // point to last menu added
	pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();

	pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
}

void CRecentConnStrList::AddAndWriteList(LPCTSTR lpszConnName)
{
	ReadList();
	Add(lpszConnName);
	WriteList();
}

void CRecentConnStrList::WriteList()
{
	ASSERT(m_arrNames.size() > 0);
	ASSERT(!m_strSectionName.IsEmpty());
	ASSERT(!m_strEntryFormat.IsEmpty());
	CString strEntry;
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileString(m_strSectionName, NULL, NULL);
	for (int iMRU = 0; iMRU < (int)m_arrNames.size(); iMRU++)
	{
		strEntry.Format(m_strEntryFormat, iMRU + 1);
		if (!m_arrNames[iMRU].IsEmpty())
		{
			pApp->WriteProfileString(m_strSectionName, strEntry,
				m_arrNames[iMRU]);
		}
	}
}

void CRecentConnStrList::ReadList()
{
	ASSERT(m_arrNames.size() > 0);
	ASSERT(!m_strSectionName.IsEmpty());
	ASSERT(!m_strEntryFormat.IsEmpty());
	CString strEntry;
	CWinApp* pApp = AfxGetApp();
	for (int iMRU = 0; iMRU < (int)m_arrNames.size(); iMRU++)
	{
		strEntry.Format(m_strEntryFormat, iMRU + 1);
		m_arrNames[iMRU] = pApp->GetProfileString(
			m_strSectionName, strEntry, NULL);// '&afxChNil' previously
	}
}

