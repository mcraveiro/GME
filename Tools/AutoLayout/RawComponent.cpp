///////////////////////////////////////////////////////////////////////////
// RawComponent.cpp, the main RAW COM component implementation file
// This is the file (along with its header RawComponent.h)
// that the component implementor is expected to modify in the first place
//
///////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <Gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

//#include "ComHelp.h"
//#include "GMECOM.h"
#include "ComponentLib.h"
#include "ComponentConfig.h"
#include "RawComponent.h"
#include "CommonSmart.h"
#include "GMEGraph.h"
#include "DlgAutoLayout.h"

// this method is called after all the generic initialization is done
// this should be empty, unless application-specific initialization is needed
STDMETHODIMP RawComponent::Initialize(struct IMgaProject *) {
	return S_OK;
}

// this is the obsolete component interface
// this present implementation either tries to call InvokeEx, or returns an error;
STDMETHODIMP RawComponent::Invoke(IMgaProject* gme, IMgaFCOs *models, long param) {
#ifdef SUPPORT_OLD_INVOKE
	CComPtr<IMgaFCO> focus;
	CComVariant parval = param;
	return InvokeEx(gme, focus, selected, parvar);
#else
	if(interactive) {
		AfxMessageBox(_T("This component does not support the obsolete invoke mechanism"));
	}
	return E_MGA_NOT_SUPPORTED;
#endif
}


// This is the main component method for interpereters and plugins. 
// May als be used in case of invokeable addons
STDMETHODIMP RawComponent::InvokeEx( IMgaProject *project,  IMgaFCO *currentobj,  
									IMgaFCOs *selectedobjs,  long param) {
	COMTRY 
    {
		CComBSTR projname;
		CComObjPtr<IMgaTerritory> terr;
		COMTHROW(project->CreateTerritory(NULL, PutOut(terr)));
		COMTHROW(project->BeginTransaction(terr));
		try
        {
            if(currentobj==NULL)
                throw 0;

            objtype_enum objType;
            COMTHROW(currentobj->get_ObjType(&objType));
            if( objType != OBJTYPE_MODEL )
            {
				if (param != GME_SILENT_MODE)
					AfxMessageBox(_T("AutoLayout can only run on models."));
                throw 0;
            }

			VARIANT_BOOL isInstance;
			COMTHROW(currentobj->get_IsInstance(&isInstance));
			if (isInstance != VARIANT_FALSE)
			{
				if (param != GME_SILENT_MODE)
					AfxMessageBox(_T("AutoLayout cannot run on instances. Please AutoLayout run on the basetype instead."));
				throw 0;
			}
			
			CDlgAutoLayout dlg;
            dlg.initialize( project, (IMgaModel*)currentobj );
			if (param == GME_SILENT_MODE)
			{
				Gdiplus::GdiplusStartupInput  gdiplusStartupInput;
				Gdiplus::GdiplusStartupOutput  gdiplusStartupOutput;
				ULONG_PTR gdiplusToken;
				ULONG_PTR gdiplusHookToken;

				// Initializing GDI+
				// See "Special CWinApp Services" MSDN topic http://msdn.microsoft.com/en-us/library/001tckck.aspx
				gdiplusStartupInput.SuppressBackgroundThread = TRUE;
				VERIFY(Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput) == Gdiplus::Ok);
				gdiplusStartupOutput.NotificationHook(&gdiplusHookToken);

				dlg.OptimizeAllAspects();
				// n.b. need to unload decorators before shutting down GDI+
				CoFreeUnusedLibraries();
				// Closing GDI+
				gdiplusStartupOutput.NotificationUnhook(gdiplusHookToken);
				Gdiplus::GdiplusShutdown(gdiplusToken);
				COMTHROW( project->CommitTransaction() );
			} else {
				INT_PTR dlgResult = dlg.DoModal();
				if (dlgResult == IDOK ) {
					COMTHROW( project->CommitTransaction() );
				} else {
					COMTHROW( project->AbortTransaction() );
				}
			}
		}	
        catch(...)         
        { 
            project->AbortTransaction(); 
        }		
	} COMCATCH(;);
}

// GME currently does not use this function
// you only need to implement it if other invokation mechanisms are used
STDMETHODIMP RawComponent::ObjectsInvokeEx( IMgaProject *project,  IMgaObject *currentobj,  IMgaObjects *selectedobjs,  long param) {
	if(interactive) {
		AfxMessageBox(_T("Tho ObjectsInvoke method is not implemented"));
	}
	return E_MGA_NOT_SUPPORTED;
}


// implement application specific parameter-mechanism in these functions:
STDMETHODIMP RawComponent::get_ComponentParameter(BSTR name, VARIANT *pVal) {
	return S_OK;
}

STDMETHODIMP RawComponent::put_ComponentParameter(BSTR name, VARIANT newVal) {
	return S_OK;
}


#ifdef GME_ADDON

// these two functions are the main 
STDMETHODIMP RawComponent::GlobalEvent(globalevent_enum event) { 
	if(event == GLOBALEVENT_UNDO) {
		AfxMessageBox(_T("UNDO!!"));
	}
	return S_OK; 
}

STDMETHODIMP RawComponent::ObjectEvent(IMgaObject * obj, unsigned long eventmask, VARIANT v) {
	if(eventmask & OBJEVENT_CREATED) {
		CComBSTR objID;
		COMTHROW(obj->get_ID(&objID));
		AfxMessageBox( _T("Object created! ObjID: ") + CString(objID)); 
	}		
	return S_OK;
}

#endif
