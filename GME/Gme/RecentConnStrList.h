
#ifndef MGA_RECENTCONNSTRLIST_H
#define MGA_RECENTCONNSTRLIST_H

#include <vector>

/*
	This class is based on the CRecentFileList class, but unfortunately we
	could not reuse it as it was, it is too coupled with files.
	We have to store connection strings, mask the password from them,
	abbreviate them appropriately, etc. The usage should be similar to
	the original CRecentFileList.
*/

class CRecentConnStrList
{
// Constructors
public:
	CRecentConnStrList(UINT nStart, LPCTSTR lpszSection,
		LPCTSTR lpszEntryFormat, int nSize,
		int nMaxDispLen = 40);

// Attributes
	int GetSize() const { return m_arrNames.size(); }
	CString& operator[](int nIndex) 
	{ return m_arrNames[nIndex]; }

// Operations
	BOOL GetDisplayName(CString& strName, int nIndex) const;
	void UpdateMenu(CCmdUI* pCmdUI,bool enable);
	void ReadList();    // reads from registry or ini file
	void AddAndWriteList(LPCTSTR lpszConnName);

	// Implementation
	virtual ~CRecentConnStrList();

private:
	void Add(LPCTSTR lpszConnName);
	void Remove(int nIndex);
	void WriteList();   // writes to registry or ini file

	std::vector<CString> m_arrNames;
	CString m_strSectionName;   // for saving
	CString m_strEntryFormat;
	UINT m_nStart;              // for displaying
	int m_nMaxDisplayLength;
	CString m_strOriginal;      // original menu item contents
};

#endif//MGA_RECENTCONNSTRLIST_H
