#pragma once


// GMEViewOverlay

class GMEViewOverlay : public CWnd
{
	DECLARE_DYNAMIC(GMEViewOverlay)
	int m_opacity;
	UINT_PTR m_timerID;
	HBITMAP m_bitmap;
public:
	CDC* m_memcdc;
public:
	GMEViewOverlay();
	virtual ~GMEViewOverlay();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL DestroyWindow();

public:
	afx_msg void PostNcDestroy()
	{
		// need to remove timer events from queue
		// delete this;
	}
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDraw(CDC* pDC);
	void Init1();
	void Init2();
	const static DWORD exstyle = WS_EX_LAYERED
            | WS_EX_TRANSPARENT 
			| WS_EX_TOPMOST
			| WS_EX_TOOLWINDOW 
			| WS_EX_NOACTIVATE;
			;
	const static DWORD style = WS_POPUP | WS_VISIBLE;
};
