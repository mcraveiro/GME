// MgaContext.h: interface for the CMgaContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MGACONTEXT_H__48B8CD86_FF78_4183_8360_D33CE1792CD7__INCLUDED_)
#define AFX_MGACONTEXT_H__48B8CD86_FF78_4183_8360_D33CE1792CD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CMgaContext  
{
	friend class CCurrentProject;

	int m_nPendingTransactions;
	void SetTransactionState(int nValue);
public:
	void SetEventTransactionMode(bool bOnOff);

	CComPtr<IMgaProject> m_ccpProject;
	CComPtr<IMgaTerritory> m_ccpTerritory;


	void CreateContext(IMgaEventSink& rMgaEventSink,LPUNKNOWN pMgaProject);
	void CloseContext();
	CComPtr<IMgaComponentEx> FindConstraintManager();
	
	bool AbortTransaction();
	bool CommitTransaction();
	bool BeginTransaction(bool bIsReadOnly = true);
	bool BeginTransaction(transactiontype_enum type);
	bool IsInTransaction();
	CMgaContext();
	virtual ~CMgaContext();

private:
	bool m_bEventTransactionMode;
};

class Utils
{
public:
	static CComPtr<IGMEOLEApp> get_GME( CComPtr<IMgaProject> p_mgaproject)
	{
		CComPtr<IGMEOLEApp> gme;
		if( p_mgaproject) {		
			CComBSTR bstrName("GME.Application");
			CComPtr<IMgaClient> pClient;
			HRESULT hr = p_mgaproject->GetClientByName(bstrName, &pClient);
			if (SUCCEEDED(hr) && pClient) {
				CComPtr<IDispatch> pDispatch;
				hr = pClient->get_OLEServer(&pDispatch);
				if (SUCCEEDED(hr) && pDispatch) {
					hr = pDispatch.QueryInterface(&gme);
					if (FAILED(hr)) {
						gme = NULL;
					}
				}
			}
		}
		return gme;
	} 

	static void put2Console( CComPtr<IGMEOLEApp> gme, CComBSTR msg, msgtype_enum typ)
	{
		if(gme)
			gme->ConsoleMessage( msg, typ);
	}
};

#endif // !defined(AFX_MGACONTEXT_H__48B8CD86_FF78_4183_8360_D33CE1792CD7__INCLUDED_)
