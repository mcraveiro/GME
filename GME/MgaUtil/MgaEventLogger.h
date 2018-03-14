// MgaEventLogger.h: interface for the CMgaEventLogger class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MGA_MGAEVENTLOGGER_H
#define MGA_MGAEVENTLOGGER_H

#include <stdio.h>
#include <direct.h>

class ATL_NO_VTABLE CMgaEventLogger :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaEventLogger, &__uuidof(MgaEventLogger)>,
	public IDispatchImpl<IMgaEventLogger, &__uuidof(IMgaEventLogger), &__uuidof(__MGAUtilLib)>  
{
public:
	CMgaEventLogger();
	~CMgaEventLogger();

DECLARE_REGISTRY_RESOURCEID(IDR_MGAEVENTLOGGER)

DECLARE_CLASSFACTORY_SINGLETON(CMgaEventLogger)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaEventLogger)
	COM_INTERFACE_ENTRY(IMgaEventLogger)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

public:
	STDMETHOD(LogEvent)(BSTR eventMsg);
	STDMETHOD(StartLogging)();
	STDMETHOD(StopLogging)();
	STDMETHOD(EmergencyEvent)();

private:
	FILE *EventLog;
	bool initialized;
	bool newLine;

};

#endif // !defined(MGA_MGAEVENTLOGGER_H)
