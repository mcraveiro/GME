
#ifndef MGA_MGAPROGRESSDLG_H
#define MGA_MGAPROGRESSDLG_H

// --------------------------- CMgaProgressDlg

class ATL_NO_VTABLE CMgaProgressDlg : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaProgressDlg, &__uuidof(MgaProgressDlg)>,
	public IMgaProgressDlg
{
public:
	CMgaProgressDlg();
	~CMgaProgressDlg();

DECLARE_REGISTRY_RESOURCEID(IDR_MGAPROGRESSDLG)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaProgressDlg)
	COM_INTERFACE_ENTRY(IMgaProgressDlg)
END_COM_MAP()

// ------- Properties

	STDMETHOD(SetTitle)(BSTR title);
	STDMETHOD(SetLine)(int line, BSTR msg);
	STDMETHOD(StartProgressDialog)(HWND hwndParent);
	STDMETHOD(HasUserCancelled)(VARIANT_BOOL *p);
	STDMETHOD(SetProgress)(long completed, long total);
	STDMETHOD(StopProgressDialog)();

// ------- Attributes

protected:
	CString title;
	CString lines[2];

	CDialog dlg;
};

#endif//MGA_MGAPROGRESSDLG_H
