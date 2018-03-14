// GMEDoc.h : interface of the CGMEDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMEDOC_H__BD235B53_BD3F_11D3_91E2_00104B98EAD9__INCLUDED_)
#define AFX_GMEDOC_H__BD235B53_BD3F_11D3_91E2_00104B98EAD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GMEStd.h"
#include "GuiObject.h"
#include "GMEOLEData.h"
#include "MgaUtil.h"
#include <list>
#include <string>

class CGMEChildFrame;

class CGMEDoc : public CDocument
{
friend class CGMEView;

protected: // create from serialization only
	CGMEDoc();
	DECLARE_DYNCREATE(CGMEDoc)

// Attributes
public:
	static CGMEDoc *theInstance;
protected:
	bool     m_isClosing;
	GMEModeCode editMode;

	CComPtr<IMgaModel> nextToView;
	CComPtr<IMgaFCO> initialCenterObj;
	CString nextAspect;
	CComPtr<IMgaResolver> resolver;

// Operations
public:
	GMEModeCode GetEditMode() const;
	void SetEditMode(GMEModeCode mode, const CString& statusMsg);
	int  HowManyViewsAreOpen();
	void InvalidateAllViews(bool thorough = false, bool fullAutoRoute = false);
	void ResetAllViews();
	void ViewModeChange();
#if !defined (ACTIVEXGMEVIEW)
	CGMEView *FindView(CComPtr<IMgaModel> model);
#else
	CGMEChildFrame *FindView(CComPtr<IMgaModel> model);
#endif
	void DoOnViewRefresh(void);

	class Historian
	{
	public:

		class HistoryElem 
		{
		public:
			HistoryElem()
			{
			}

			HistoryElem( const std::wstring& p_id
			           , const std::wstring& p_aspect)
				: m_id( p_id)
				, m_an( p_aspect)
			{
			}

			HistoryElem( const HistoryElem& peer)
				: m_id( peer.m_id)
				, m_an( peer.m_an)
			{
			}

			const HistoryElem& operator=( const HistoryElem& peer)
			{
				if( this == &peer) return *this;

				m_id = peer.m_id;
				m_an = peer.m_an;

				return *this;
			}
			
			const std::wstring& id() const { return m_id; }
			const std::wstring& aspect() const { return m_an; }

			bool operator==( const HistoryElem& peer) const
			{
				return id() == peer.id() && aspect() == peer.aspect();
			}

			bool operator!=( const HistoryElem& peer) const
			{
				return !( *this == peer);
			}

		private:
			std::wstring m_id;
			std::wstring m_an; // aspect name
		};

	public:
		bool isEmptyB() const;
		bool isEnabledBack() const;
		bool isEnabledFrwd() const;
		bool isEnabledHome() const;

		void totalBrainWash();
		void frwdBrainWash();

		void pushB( const HistoryElem& e);
		void pushF( const HistoryElem& e);

		void popB( HistoryElem& e, HistoryElem& c);
		void popF( HistoryElem& e);
		void frontB( HistoryElem& e);

		void eraseOccurences( const std::wstring& p_id);
		void erasePrevOccurencesB( const HistoryElem& e);

	private:
		typedef std::list< HistoryElem >   HISTLIST;
		typedef HISTLIST::iterator         HISTLIST_ITER;
		HISTLIST                           m_backStack;
		HISTLIST                           m_frwdStack;

	} m_historian;

	void SetNextToView(CComPtr<IMgaModel> &model,CString aspName, CComPtr<IMgaFCO> &centerObj)			{ nextToView = model; nextAspect = aspName; initialCenterObj = centerObj; }
	void ShowObject(CComPtr<IUnknown> alienObject, BOOL inParent = FALSE);
	void ShowObject(LPCTSTR objectID);
	void ResetNextToView()													{ nextToView = 0; }
	void ChangeAspects(int index, CString aspName);
	void ConvertPathToCustom(CComPtr<IUnknown>& pMgaObject);

	static bool CreateFcoList(CGuiObjectList* list,CComPtr<IMgaFCOs> &fcos,CGMEView *view);
	static bool CreateFcoList2(CGuiFcoList* list,CComPtr<IMgaFCOs> &fcos,CGMEView *view, long *howmany, bool* array[8]);
	static bool CreateAnnotationList(CGuiAnnotatorList* list,CComPtr<IMgaRegNodes> &anns,CGMEView *view);
	static void CopyClosureToClipboard(CGuiObjectList* list,CGuiAnnotatorList *annList,CGMEDataDescriptor* desc,CGMEView *view);
	static void CopySmartToClipboard(CGuiFcoList* list,CGuiAnnotatorList *annList,CGMEDataDescriptor* desc,CGMEView *view);
	static void CopyToClipboard(CGuiObjectList* list,CGuiAnnotatorList *annList,CGMEDataDescriptor* desc,CGMEView *view);
	static DROPEFFECT DoDragDrop(CGuiObjectList* list, CGuiAnnotatorList *annList, CGMEDataDescriptor* desc,
									DWORD dwEffects, LPCRECT lpRectStartDrag,CGMEView *view);
	static DROPEFFECT DoDragDrop(CGuiObject *guiObj, CGMEDataDescriptor* desc,
									DWORD dwEffects, LPCRECT lpRectStartDrag);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMEDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument(bool suppressErrors = false);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGMEDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
void SetMode(long mode);

	// history related
	void back();
	void forw();
	void home();
	void clearTotalHistory();
	void clearForwHistory();
	void tellHistorian( CComBSTR&           modid, CString asp);
	void tellHistorian( CComPtr<IMgaModel>& model, CString asp);
	void presentModel( LPCTSTR objectID, LPCTSTR aspectNM);
	void eraseFromHistory( LPCTSTR p_id);

	void closeActiveWnd();
	void closeAllWnd();
	void closeAllButActiveWnd();
	void cycleAspect();
	void cycleAllAspects();
	void cycleViews();

// Generated message map functions
protected:
	//{{AFX_MSG(CGMEDoc)
	afx_msg void OnModeAutoconnect();
	afx_msg void OnUpdateModeAutoconnect(CCmdUI* pCmdUI);
	afx_msg void OnModeDisconnect();
	afx_msg void OnUpdateModeDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnModeEdit();
	afx_msg void OnUpdateModeEdit(CCmdUI* pCmdUI);
	afx_msg void OnModeSet();
	afx_msg void OnUpdateModeSet(CCmdUI* pCmdUI);
	afx_msg void OnModeVisualize();
	afx_msg void OnUpdateModeVisualize(CCmdUI* pCmdUI);
	afx_msg void OnModeZoom();
	afx_msg void OnUpdateModeZoom(CCmdUI* pCmdUI);
	afx_msg void OnModeShortConnect();
	afx_msg void OnUpdateModeShortConnect(CCmdUI* pCmdUI);
	afx_msg void OnModeShortDisconnect();
	afx_msg void OnUpdateModeShortDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);
	afx_msg void OnViewRefresh();
	afx_msg void OnBtnBack();
	afx_msg void OnBtnFrwd();
	afx_msg void OnBtnHome();
	afx_msg void OnUpdateBtnFrwd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnBack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnHome(CCmdUI* pCmdUI);
	afx_msg void OnBtnCloseActive();
	afx_msg void OnBtnCloseAll();
	afx_msg void OnBtnCloseAllButOne();
	afx_msg void OnBtnCycleAspect();
	afx_msg	void OnBtnCycleAllAspects();
	afx_msg void OnBtnCycleViews();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEDOC_H__BD235B53_BD3F_11D3_91E2_00104B98EAD9__INCLUDED_)
