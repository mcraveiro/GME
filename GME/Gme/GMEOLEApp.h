#if !defined(AFX_GMEOLEAPP_H__78B0CD97_4EC0_450F_A3B3_C3AE29A254FD__INCLUDED_)
#define AFX_GMEOLEAPP_H__78B0CD97_4EC0_450F_A3B3_C3AE29A254FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GMEOLEApp.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CGMEOLEApp command target

class CGMEOLEApp : public CCmdTarget
{
	DECLARE_DYNCREATE(CGMEOLEApp)

	CGMEOLEApp();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual void OnFinalRelease();


// Implementation
protected:
	virtual ~CGMEOLEApp();

	// Generated message map functions
	//{{AFX_MSG(CGMEOLEApp)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CGMEOLEApp)

	// OLE dispatch map functions
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg BSTR GetVersion();
	afx_msg void SetVersion(LPCTSTR lpszNewValue);
	afx_msg short GetVersionMajor();
	afx_msg void SetVersionMajor(short nNewValue);
	afx_msg short GetVersionMinor();
	afx_msg void SetVersionMinor(short nNewValue);
	afx_msg short GetVersionPatchLevel();
	afx_msg void SetVersionPatchLevel(short nNewValue);
	afx_msg LPDISPATCH GetModels();
	afx_msg void SetModels(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetMgaProject();
	afx_msg void SetMgaProject(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetOleIt();
	afx_msg void SetOleIt(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetPanels();
	afx_msg void SetPanels(LPDISPATCH newValue);
	afx_msg BSTR GetConsoleContents();
	afx_msg void SetConsoleContents(LPCTSTR contents);
	afx_msg void CreateProject(LPCTSTR metaname, LPCTSTR connstr);
	afx_msg void OpenProject(LPCTSTR connstr);
	afx_msg void CreateProjectDialog();
	afx_msg void OpenProjectDialog();
	afx_msg void CloseProject(BOOL saveOnClose);
	afx_msg void SaveProject();
	afx_msg void SaveProjectAs(LPCTSTR connstr);
	afx_msg void SaveProjectAsDialog();
	afx_msg void ExportProject(LPCTSTR connstr);
	afx_msg void ImportProject(LPCTSTR connstr);
	afx_msg void ConstraintsDialog();
	afx_msg void CheckAllConstraints();
	afx_msg void RegisterParagimsDialog();
	afx_msg void RegisterComponentsDialog();
	afx_msg void RunComponent(LPCTSTR progID);
	afx_msg void RunComponentDialog();
	afx_msg void SettingsDialog();
	afx_msg void Undo();
	afx_msg void Redo();
	afx_msg void ClearUndoQueue();
	afx_msg void ProjectPropertiesDialog();
	afx_msg void ShowHelpContents();
	afx_msg void ShowAbout();
	afx_msg void ShowFCO(LPDISPATCH mgaFCO, BOOL inParent);
	afx_msg void ConsoleMessage(LPCTSTR msg, long type);
	afx_msg void ConsoleClear();
	afx_msg void ConsoleNavigateTo(LPCTSTR url);

	afx_msg void ChangeEditmode(long mode);
	afx_msg void GridShow(BOOL show);
	afx_msg void AttributepanelPage(long page);
	afx_msg void DisableComp(LPCTSTR pCompName, BOOL pDisable);
	afx_msg void DisableCompForKinds( LPCTSTR pCompName, LPCTSTR pKindNameSequence);
	afx_msg void SetCompFiltering( BOOL pOn);
	afx_msg BOOL GetCompFiltering();
	afx_msg void SetWorkingDirectory( LPCTSTR pPath);
	afx_msg void Exit();
	afx_msg long RefreshLib( LPCTSTR pLibName, BOOL pUngroup);

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_DUAL_INTERFACE_PART(Dual, IGMEOLEApp)
		STDMETHOD(put_Visible)(THIS_ VARIANT_BOOL isVisible);
		STDMETHOD(get_Visible)(THIS_ VARIANT_BOOL* isVisible);
		STDMETHOD(get_Version)(THIS_ BSTR* versionStr);
		STDMETHOD(get_VersionMajor)(THIS_ short* n);
		STDMETHOD(get_VersionMinor)(THIS_ short* n);
		STDMETHOD(get_VersionPatchLevel)(THIS_ short* n);
		STDMETHOD(get_Models)(THIS_ IGMEOLEColl** coll);
		STDMETHOD(get_MgaProject)(THIS_ IMgaProject** project);
		STDMETHOD(get_Panels)(THIS_ IGMEOLEColl** coll);
		STDMETHOD(put_ConsoleContents)(THIS_ BSTR contents);
		STDMETHOD(get_ConsoleContents)(THIS_ BSTR* contents);
		STDMETHOD(get_OleIt)( THIS_ IGMEOLEIt ** opened_mod);

		STDMETHOD(CreateProject)(THIS_ BSTR metaname, BSTR connstr);
		STDMETHOD(OpenProject)(THIS_ BSTR connstr);
		STDMETHOD(CreateProjectDialog)(THIS);
		STDMETHOD(OpenProjectDialog)(THIS);
		STDMETHOD(CloseProject)(THIS_ VARIANT_BOOL saveOnClose);
		STDMETHOD(SaveProject)(THIS);
		STDMETHOD(SaveProjectAs)(THIS_ BSTR connstr);
		STDMETHOD(SaveProjectAsDialog)(THIS);
		STDMETHOD(ExportProject)(THIS_ BSTR connstr);
		STDMETHOD(ImportProject)(THIS_ BSTR connstr);
		STDMETHOD(ConstraintsDialog)(THIS);
		STDMETHOD(CheckAllConstraints)(THIS);
		STDMETHOD(RegisterParagimsDialog)(THIS);
		STDMETHOD(RegisterComponentsDialog)(THIS);
		STDMETHOD(RunComponent)(THIS_ BSTR progID);
		STDMETHOD(RunComponentDialog)(THIS);
		STDMETHOD(SettingsDialog)(THIS);
		STDMETHOD(Undo)(THIS);
		STDMETHOD(Redo)(THIS);
		STDMETHOD(ClearUndoQueue)(THIS);
		STDMETHOD(ProjectPropertiesDialog)(THIS);
		STDMETHOD(ShowHelpContents)(THIS);
		STDMETHOD(ShowAbout)(THIS);
		STDMETHOD(ShowFCO)(THIS_ IMgaFCO* mgaFCO, VARIANT_BOOL inParent = FALSE);
		STDMETHOD(ConsoleMessage)(THIS_ BSTR msg, msgtype_enum type);
		STDMETHOD(ConsoleClear)(THIS);
		STDMETHOD(ConsoleNavigateTo)(THIS_ BSTR url);

		STDMETHOD(ChangeEditmode)(THIS_ editmode_enum mode);
		STDMETHOD(GridShow)(THIS_ VARIANT_BOOL show);
		STDMETHOD(AttributepanelPage)(THIS_ attribpanel_page page);

		STDMETHOD(DisableComp)(THIS_ BSTR pCompName, VARIANT_BOOL pDisable);
		STDMETHOD(DisableCompForKinds)(THIS_ BSTR pCompName, BSTR pKindSequence);
		STDMETHOD(SetCompFiltering)( THIS_ VARIANT_BOOL pOn);
		STDMETHOD(GetCompFiltering)( THIS_ VARIANT_BOOL* pOn);
		STDMETHOD(SetWorkingDirectory)(THIS_ BSTR pPath);
		STDMETHOD(Exit)(THIS);
		STDMETHOD(RefreshLib)(THIS_ BSTR pLibName, VARIANT_BOOL pUngroup, long* pNumOfErrors);

	END_DUAL_INTERFACE_PART(Dual)

	//     add declaration of ISupportErrorInfo implementation
	//     to indicate we support the OLE Automation error object
	DECLARE_DUAL_ERRORINFO()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEOLEAPP_H__78B0CD97_4EC0_450F_A3B3_C3AE29A254FD__INCLUDED_)
