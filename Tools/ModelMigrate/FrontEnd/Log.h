#pragma once


// Log

class Log : public CEdit
{
	DECLARE_DYNAMIC(Log)

public:
	Log();
	virtual ~Log();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	void doClear();
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};


