// MgaConnection.h : Declaration of the CMgaConnection

#ifndef __MGACONNECTION_H_
#define __MGACONNECTION_H_

#include "resource.h"       // main symbols
#include "MgaFCO.h"

/////////////////////////////////////////////////////////////////////////////
// CMgaConnection



class ATL_NO_VTABLE CMgaConnection : 
	public CComCoClass<CMgaConnection, &__uuidof(MgaSimpleConnection)>,
	public IMgaFCOImpl< 
		CComObjectRootEx<CComSingleThreadModel>,
		IDispatchImpl<IMgaSimpleConnection, &__uuidof(IMgaSimpleConnection), &__uuidof(__MGALib)> >,
	public ISupportErrorInfoImpl<&__uuidof(IMgaConnection)>
{
public:
	static HRESULT WINAPI simpletest(void* pv, REFIID riid, LPVOID* ppv, DWORD_PTR dw) {
				FCO *f = reinterpret_cast<CMgaConnection*>(pv)->innFCO;
				if(!f->simpleconn()) {
					*ppv = NULL;
					return E_NOINTERFACE; // this breaks COM rules
				}
				IUnknown* pUnk = (IUnknown*)((char*)pv+dw);
				pUnk->AddRef();
				*ppv = pUnk;
				return S_OK;
	}

	CMgaConnection() {	}

	
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaConnection)
	COM_INTERFACE_ENTRY(IMgaConnection)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IID(__uuidof(IMgaFCO),IMgaConnection)
	COM_INTERFACE_ENTRY_IID(__uuidof(IMgaObject),IMgaConnection)
	COM_INTERFACE_ENTRY_IID(IID_ISupportErrorInfo, IMyErrorInfoBase)
	COM_INTERFACE_ENTRY_FUNC(__uuidof(IMgaSimpleConnection), offsetofclass(IMgaSimpleConnection, _ComMapClass), simpletest)
END_COM_MAP()

// IMgaSimpleConnection
public:
	HRESULT findroleobj(BSTR rolename, ICoreObject **tt);

	STDMETHOD(get_NamedRole)(BSTR rolename, IMgaFCO **pVal) { 
											return inFCO->get_NamedRoleTarget(rolename,pVal); }
	STDMETHOD(get_NamedRoleReferences)(BSTR rolename, IMgaFCOs **pVal) {
											return inFCO->get_NamedRoleReferences( rolename,pVal); }

	STDMETHOD(get_Src)(IMgaFCO **pVal) { return inFCO->get_NamedRoleTarget(inFCO->SrcName, pVal); }
	STDMETHOD(get_Dst)(IMgaFCO **pVal) { return inFCO->get_NamedRoleTarget(inFCO->DstName, pVal); }
	STDMETHOD(get_SrcReferences)(IMgaFCOs **pVal) { return get_NamedRoleReferences(inFCO->SrcName, pVal); }
	STDMETHOD(get_DstReferences)(IMgaFCOs **pVal) { return get_NamedRoleReferences(inFCO->DstName, pVal); }

	STDMETHOD(SetSrc)(IMgaFCOs *refs, IMgaFCO * newVal)  { 
										return inFCO->put_NamedRole(inFCO->SrcName, refs, newVal); }
	STDMETHOD(SetDst)(IMgaFCOs *refs, IMgaFCO * newVal) { 
										return inFCO->put_NamedRole(inFCO->DstName, refs, newVal); }
	STDMETHOD(AddConnPoint)(BSTR rolename, long maxinrole, IMgaFCO *target, IMgaFCOs *refs, IMgaConnPoint **pVal) {
										return inFCO->AddConnPoint(rolename, maxinrole, target, refs, pVal); }
	STDMETHOD(get_ConnPoints)(IMgaConnPoints **pVal) {
										return inFCO->get_ConnPoints(pVal); }

	STDMETHOD(CompareToBase)(IMgaConnPoint *p, short *status)		{ return inFCO->ConnCompareToBase(p,status); }
    STDMETHOD(RevertToBase)(IMgaConnPoint *p)						{ return inFCO->ConnRevertToBase(p); }
};



/////////////////////////////////////////////////////////////////////////////
// CMgaConnPoint
class ATL_NO_VTABLE CMgaConnPoint : 
	public CComCoClass<CMgaConnPoint, &__uuidof(MgaConnPoint)>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IMgaConnPoint, &__uuidof(IMgaConnPoint), &__uuidof(__MGALib)>,
	public ISupportErrorInfoImpl<&__uuidof(IMgaConnPoint)>
{
public:
	DEFSIG;
	CMgaConnPoint()	{
		INITSIG('C');
	}

	CComPtr<FCO> fco;
	CoreObj cobj;
	static void GetConnPoint(FCO *f, CoreObj &c, IMgaConnPoint **cp);
	~CMgaConnPoint();

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaConnPoint)
	COM_INTERFACE_ENTRY(IMgaConnPoint)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IID(IID_ISupportErrorInfo, IMyErrorInfoBase)
END_COM_MAP()

// IMgaConnPoint
public:
	STDMETHOD(get_Owner)(IMgaConnection **pVal);
	STDMETHOD(get_ConnRole)(BSTR *pVal);
	STDMETHOD(put_ConnRole)(BSTR newVal);
	STDMETHOD(get_Target)(IMgaFCO **pVal);
	STDMETHOD(get_References)(IMgaFCOs **pVal);
	STDMETHOD(Remove)();
};

void MgaConnPointDelete(CoreObj& cobj);


class AddConnPTask : public DeriveTreeTask {
	CComBSTR rolename;
	long maxinrole;

	bool Do(CoreObj self, std::vector<CoreObj> *peers);

public:
	AddConnPTask(long mr, BSTR rn) : maxinrole(mr), rolename(rn) { endreserve = 1; }
	CoreObj retval;
};

#endif //__MGACONNECTION_H_
