// GMEEventLogger.h: interface for the CGMEEventLogger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMEEVENTLOGGER_H__A65D0066_7479_4B9B_8B3D_37EAAAD5C67E__INCLUDED_)
#define AFX_GMEEVENTLOGGER_H__A65D0066_7479_4B9B_8B3D_37EAAAD5C67E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <direct.h>
#include "MgaUtil.h"

#define GMEEVENTLOG_GUIOBJS(guiobjlist) \
{ \
	CString gmelog = "    "; \
	POSITION pos = guiobjlist.GetHeadPosition(); \
	while(pos) { \
		CGuiObject *obj = guiobjlist.GetNext(pos); \
		gmelog = gmelog + obj->GetName()+" "+obj->GetID()+", "; \
	} \
	gmelog = gmelog+"\r\n"; \
	CGMEEventLogger::LogGMEEvent(gmelog); \
} \


#define GMEEVENTLOG_GUIANNOTATORS(guiannlist) \
{ \
	CString gmelog = "    "; \
	POSITION pos = guiannlist.GetHeadPosition(); \
	while(pos) { \
		CGuiAnnotator *ann = guiannlist.GetNext(pos); \
		gmelog = gmelog + ann->GetName()+" "; \
	} \
	gmelog = gmelog+"\r\n"; \
	CGMEEventLogger::LogGMEEvent(gmelog); \
} \


#define GMEEVENTLOG_GUIFCOS(guifcolist) \
{ \
	CString gmelog = "    "; \
	POSITION pos = guifcolist.GetHeadPosition(); \
	while(pos) { \
		CGuiFco *fco = guifcolist.GetNext(pos); \
		gmelog = gmelog + fco->GetName()+" "+fco->GetID()+", "; \
	} \
	gmelog = gmelog+"\r\n"; \
	CGMEEventLogger::LogGMEEvent(gmelog); \
} \

class CGMEEventLogger  
{
public:
	CGMEEventLogger();
	virtual ~CGMEEventLogger();
	 

	static void initialize(CString filePathAndName) //depracated
	{
		initialize();
	};

	static void initialize()
	{
		if( comLogger == NULL) // check introd by zolmol
		{
			COMTHROW( comLogger.CoCreateInstance(L"Mga.MgaEventLogger"));
			COMTHROW( comLogger->StartLogging() );
		}
	};

	static void StopLogging()
	{
		if(comLogger != NULL)
			COMTHROW( comLogger->StopLogging());
	};

	static void EmergencyEvent()
	{
		if(comLogger != NULL)
			COMTHROW( comLogger->EmergencyEvent());
	};

	static void LogGMEEvent(CString s)
	{
		if(comLogger != NULL)
		{
			CComBSTR b = s;
			COMTHROW( comLogger->LogEvent(b));
		}

	};

	// %Z for first fco %z for second ONLY WORKS IN TRANSACTION - this function does not make it's own transaction
	static void GMEEventPrintf(const TCHAR *format, IMgaObject *objOne, IMgaObject *objTwo, ...) //Only works in Transaction!!!!!!!!!
	{
		if(comLogger != NULL)
		{
			CString output(format);
			if(output.Find(_T("%Z"))) {
				if(objOne == NULL)
					output.Replace(CString(_T("%Z")),CString(_T("")));
				else
				{
					CComBSTR ID;
					CComPtr<IMgaObject> ONE = objOne;
					COMTHROW(ONE->get_ID(&ID));
					output.Replace(CString(_T("%Z")),CString(ID));
				}
			}

			if(output.Find(_T("%z"))) {
				if(objTwo == NULL)
					output.Replace(CString(_T("%z")),CString(_T("")));
				else
				{
					CComBSTR ID;
					CComPtr<IMgaObject> TWO = objTwo;
					COMTHROW(TWO->get_ID(&ID));
					output.Replace(CString(_T("%z")),CString(ID));
				}
			}
			TCHAR buffer[250];
			va_list v1;
			va_start(v1,objTwo);
			_vstprintf_s(buffer,output,v1);
			va_end(v1);

			CComBSTR b = buffer;
			COMTHROW( comLogger->LogEvent(b));

		}

	};


private:

	static FILE *EventLog;
	static BOOL initialized;
	static BOOL newLine;
	static CComPtr<IMgaEventLogger> comLogger; //new com object that lives in MgaUtil

	
	

};

#endif // !defined(AFX_GMEEVENTLOGGER_H__A65D0066_7479_4B9B_8B3D_37EAAAD5C67E__INCLUDED_)

