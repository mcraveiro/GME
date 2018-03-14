#pragma once
#include "Rule.h"

class RuleEditorDlg;
// RuleListCtrl

class RuleListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(RuleListCtrl)

public: // typedefs
	typedef std::vector<Rule> RULE_VEC;
	typedef std::vector<Rule>::iterator RULE_VEC_ITERATOR;

public:
	RuleListCtrl();
	virtual ~RuleListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

public:
	void setParent( RuleEditorDlg* pDlg);
	void reset();

	CString genAll( bool pSeparateFiles);
	CString genSelected( bool pSeparateFiles);
	void add();

	void fileNew();
	void fileSave();
	void fileLoad();

	void showPopupMenu();

	bool erasable();
	void onAdd();
	void onEdit();
	void onDeleSel();
	void onDeleAll();
	void onClone();
	void onGenAll();
	void onGenSel();
	void onMoveUp();
	void onMoveDown();

protected: // impl
	void addNewItem( int pChoice);
	int gen( const std::string& pOutputFileName, const std::vector<int>& pSelectVec);
	int genIntoSeparate( const std::string& pOutputFileName, const std::vector<int>& pSelectVec);

	int rowID;
	int HitTestEx(CPoint &point, int *col) const;
	RULE_VEC m_rules;
	int  getFirstFocSel();
	void setFocSel( int index);

	int addRule( int chc, std::vector<std::string>& vec);
	void showPropertyPage( int idx);
	void getItem( CString& pStr, int idx, int subIdx = 0);
	void setItem( CString pStr, int idx, int subIdx = 0);
	int insertItem( const Rule& pRule);
	void createPopupMenu( CPoint point);

	RuleEditorDlg* m_parentDlg;

};


