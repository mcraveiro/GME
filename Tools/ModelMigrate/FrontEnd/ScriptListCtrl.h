#pragma once
#include <vector>
#include <string>

class FileTransDlg;
// ScriptListCtrl

class ScriptListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(ScriptListCtrl)

public:
	ScriptListCtrl();
	virtual ~ScriptListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void createPopupMenu( CPoint pnt);

public:
	FileTransDlg* m_parent;
	void addFile(const CString& file, const CString& path);
	int m_rowID;
	std::vector< std::string> getSelFiles();
	std::vector< std::string> getAllFiles();
	void removeSel();
	void removeAll();
	void mark( std::vector< std::string>& vecs);
	void showPopupMenu();
};


