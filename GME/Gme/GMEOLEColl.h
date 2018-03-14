// CGMEOLEColl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGMEOLEColl command target

class CGMEOLEColl : public CCmdTarget
{
	DECLARE_DYNCREATE(CGMEOLEColl)
public:
	CGMEOLEColl();

// Attributes
public:
	CPtrArray m_ptrArray;

// Operations
public:
	void CheckIndex(long nIndex);

// Overrides
public:
	virtual void OnFinalRelease();


	// OLE dispatch map functions
	afx_msg long GetCount();
	afx_msg void Add(LPDISPATCH newValue);
	afx_msg long Find(LPDISPATCH findValue);
	afx_msg void Remove(const VARIANT FAR& removeValue);
	afx_msg void RemoveAll();
	afx_msg LPDISPATCH GetItem(long nIndex);
	afx_msg void SetItem(long nIndex, LPDISPATCH newValue);
	afx_msg LPUNKNOWN GetNewEnum();
	

// Implementation
protected:
	virtual ~CGMEOLEColl();
	
	// Generated message map functions
	//{{AFX_MSG(CGMEOLEColl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	DECLARE_DISPATCH_MAP()

	DECLARE_INTERFACE_MAP()

	BEGIN_DUAL_INTERFACE_PART(Dual, IGMEOLEColl)
		STDMETHOD(get_Count)(THIS_ long *cnt);

		STDMETHOD(Add)(THIS_ IDispatch* newValue);
		STDMETHOD(Find)(THIS_ IDispatch* findValue, long *cnt);
		STDMETHOD(Remove)(THIS_ VARIANT removeValue);
		STDMETHOD(RemoveAll)(THIS);

		STDMETHOD(get_Item)(THIS_ long nIndex, IDispatch** val);
		STDMETHOD(put_Item)(THIS_ long nIndex, IDispatch* newValue);
		STDMETHOD(get__NewEnum)(THIS_ IUnknown** e);
	END_DUAL_INTERFACE_PART(Dual)

	//     add declaration of ISupportErrorInfo implementation
	//     to indicate we support the OLE Automation error object
	DECLARE_DUAL_ERRORINFO()
};

/////////////////////////////////////////////////////////////////////////////
