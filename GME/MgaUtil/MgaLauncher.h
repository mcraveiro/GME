
#ifndef MGA_MGALAUNCHER_H
#define MGA_MGALAUNCHER_H

// --------------------------- CMgaLauncher

class ATL_NO_VTABLE CMgaLauncher : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaLauncher, &__uuidof(MgaLauncher)>,
	public IDispatchImpl<IMgaLauncher, &__uuidof(IMgaLauncher), &__uuidof(__MGAUtilLib)>
{
public:
	CMgaLauncher() { }

DECLARE_REGISTRY_RESOURCEID(IDR_MGALAUNCHER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaLauncher)
	COM_INTERFACE_ENTRY(IMgaLauncher)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ------- Properties

public:
	CString paradigmname;
	componenttype_enum comptype;
	CComVariant parameter;

	CString connstr;
	CString progid;
	CComObjPtr<IMgaMetaProject> paradigm;

// ------- Methods

public:
	STDMETHOD(put_ParadigmName)(BSTR p);
	STDMETHOD(put_Parameter)(VARIANT newVal);
	STDMETHOD(put_ComponentType)(componenttype_enum type);

	STDMETHOD(get_ParadigmName)(BSTR *p);
	STDMETHOD(get_ConnStr)(BSTR *p);
	STDMETHOD(get_ProgID)(BSTR *p);
	STDMETHOD(get_Paradigm)(IMgaMetaProject **p);

	STDMETHOD(MetaDlg)(metadlg_enum flags);
	STDMETHOD(GmeDlg)();
	STDMETHOD(ComponentDlg)(componentdlg_enum flags);
	STDMETHOD(PropDlg)(IMgaObject * obj);
	STDMETHOD(AttrDlg)(IMgaObject * obj);
	STDMETHOD(ShowHelp)(IMgaObject * obj);
	STDMETHOD(RegistryBrowser)(IMgaObject * obj);
	STDMETHOD(AnnotationBrowser)(IMgaObject * obj, IMgaRegNode * focus);

	
	STDMETHOD(RunComponent)(BSTR progid, IMgaProject *project, IMgaFCO *focusobj, IMgaFCOs *selectedobjs, long param);
// ------- Helper functions

	void ThrowCOMError(HRESULT hr, const wchar_t* err)
	{
		ICreateErrorInfoPtr errCreate;
		if (SUCCEEDED(CreateErrorInfo(&errCreate))
			&& SUCCEEDED(errCreate->SetDescription(const_cast<wchar_t*>(err)))
			&& SUCCEEDED(errCreate->SetGUID(__uuidof(IMgaLauncher)))
			&& SUCCEEDED(errCreate->SetSource(L"Mga.MgaLauncher"))
			)
		{
			IErrorInfoPtr errorInfo = errCreate;
			if (errorInfo)
			{
				throw _com_error(hr, errorInfo.Detach());
			}
		}
		throw _com_error(hr);
	}

public:
	static CString PruneConnectionString(const CString &conn);
};

#endif//MGA_MGALAUNCHER_H
