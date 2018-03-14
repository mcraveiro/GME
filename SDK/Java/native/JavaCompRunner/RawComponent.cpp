///////////////////////////////////////////////////////////////////////////
// RawComponent.cpp, the main RAW COM component implementation file
// This is the file (along with its header RawComponent.h)
// that the component implementor is expected to modify in the first place
//
///////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ComHelp.h"
#include "GMECOM.h"
#include "ComponentConfig.h"
#include "RawComponent.h"
#include "ComponentDll.h"


RawComponent::RawComponent()
{  
    m_javaVMDll    = NULL;
    m_createJavaVM = NULL;
    m_env          = NULL;
    m_jvm          = NULL;
}

RawComponent::~RawComponent()
{
    unloadJavaVM();
}

void RawComponent::loadJavaVM()
{
    unloadJavaVM();

    char          classPath[2000];
    char          memory[100];
    char          memory_buf[200];
    char          buf[2000];
    unsigned long bufSize;
    HKEY          regkey;
    DWORD         type;

    // query classpath from registry
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\GME", 0, KEY_EXECUTE, &regkey);
    type = REG_SZ;
    bufSize = 2000;
	if(RegQueryValueEx(regkey, "JavaClassPath", NULL, &type, (LPBYTE)classPath, &bufSize ) != ERROR_SUCCESS){
		AfxMessageBox("Cannot find JavaClassPath in registry under HKLM\\SOFTWARE\\GME");
        unloadJavaVM();
		//throw hresult_exception(E_FAIL);
		throw regkey;
	}
	classPath[bufSize] = '\0';

    RegCloseKey(regkey);

    // query java memory
    sprintf( memory, "" );
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\GME", 0, KEY_EXECUTE, &regkey);
    type = REG_SZ;
    bufSize = sizeof(memory) / sizeof(memory[0]);
	if (RegQueryValueEx(regkey, "JavaMemory", NULL, &type, (LPBYTE)memory, &bufSize) == ERROR_SUCCESS)
		memory[bufSize] = '\0';
    RegCloseKey(regkey);

    // query jvm.dll path from registry
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\JavaSoft\\Java Runtime Environment", 0, KEY_EXECUTE, &regkey);
    type = REG_SZ;        
	bufSize = 2000;
	if (RegQueryValueEx(regkey, "CurrentVersion", NULL, &type, (LPBYTE)buf, &bufSize) == ERROR_SUCCESS)
		buf[bufSize] = '\0';
    RegCloseKey(regkey);
    char javaVersionPath[2000];
    sprintf( javaVersionPath, "SOFTWARE\\JavaSoft\\Java Runtime Environment\\%s", buf );
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, javaVersionPath, 0, KEY_EXECUTE, &regkey);
    bufSize = 2000;
    type    = REG_SZ;
    buf[0] = 0;
	if (RegQueryValueEx(regkey, "RuntimeLib", NULL, &type, (LPBYTE)buf, &bufSize) == ERROR_SUCCESS)
		buf[bufSize] = '\0';
    RegCloseKey(regkey);
    if( strlen(buf)==0 )
    {        
        AfxMessageBox("Error loading java. Cannot query jvm.dll path from registry. (Is Java installed?)");
        unloadJavaVM();
		throw regkey;        
    }    

    // load jvm.dll
    m_javaVMDll = LoadLibrary(buf);
    if(m_javaVMDll == NULL)
    {
		LPTSTR errorText = NULL;

		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		   NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		   (LPTSTR)&errorText, 0, NULL);

		if (NULL != errorText) {
			std::string err = "Error loading java. Cannot load jvm.dll: ";
			err += errorText;
			AfxMessageBox(err.c_str());
			LocalFree(errorText);
		} else {
	        AfxMessageBox("Error loading java. Cannot find jvm.dll.");
		}
        unloadJavaVM();
        throw regkey;  
    }

    // Retrieve address of functions
    m_createJavaVM      = (P_JNI_CreateJavaVM)        GetProcAddress(m_javaVMDll, "JNI_CreateJavaVM");
    m_getCreatedJavaVMs = (P_JNI_GetCreatedJavaVMs)   GetProcAddress(m_javaVMDll, "JNI_GetCreatedJavaVMs");
    if(m_createJavaVM == NULL || m_getCreatedJavaVMs == NULL )
    {
        AfxMessageBox("Error loading java. Invalid jvm.dll.");
        unloadJavaVM();
        throw regkey;  
    }

    // find java vm or create it if not found
    jsize           machineNum;
    jsize           buflen = 1;
    JavaVMInitArgs  args;
    JavaVMOption    options[5];
	args.nOptions = 0;

	std::string libraryPathStr;
	std::string classPathStr = std::string("-Djava.class.path=") + classPath;
	{
		char gme_root[MAX_PATH];
		size_t gme_root_size;
		if (getenv_s(&gme_root_size, gme_root, "GME_ROOT") == 0) {
			classPathStr += std::string(";") + gme_root + "\\SDK\\java\\gme.jar";

			libraryPathStr = "-Djava.library.path";
			libraryPathStr += std::string("=") + gme_root + "\\bin";
			libraryPathStr += std::string(";") + gme_root + "\\SDK\\java\\native\\Jaut\\Release";
			libraryPathStr += std::string(";") + gme_root + "\\SDK\\java\\native\\Jaut\\Debug";
			options[args.nOptions].optionString = const_cast<char*>(libraryPathStr.c_str());
			args.nOptions++;
		}
	}
	options[args.nOptions].optionString = const_cast<char*>(classPathStr.c_str());
	args.nOptions++;

    args.version = JNI_VERSION_1_2;
    if( strlen(memory) > 0 )
    {
        sprintf(memory_buf, "-Xmx%s", memory);
        options[args.nOptions].optionString = memory_buf;
        args.nOptions++;
    }

	// Remote debugger support
	// TODO: read from reg key
	// options[args.nOptions].optionString = "-Xdebug";
	// args.nOptions++;
	// options[args.nOptions].optionString = "-Xrunjdwp:transport=dt_socket,server=y,suspend=y,address=8000";
	// args.nOptions++;

    args.options = options;
	args.ignoreUnrecognized = JNI_FALSE;

    int res = (*m_getCreatedJavaVMs)(&m_jvm,buflen,&machineNum);
    if( res == 0 && machineNum>0 )
        res = m_jvm->AttachCurrentThread((void **)&m_env,&args);
    else
        res = (*m_createJavaVM)(&m_jvm, (void**)&m_env, &args);
    if(res != 0)
    {
        char buf[200];
        sprintf(buf, "Error loading java. Cannot create java virtual machine. Error code: %d", res);
        AfxMessageBox(buf);
        unloadJavaVM();
        throw regkey;  
    }
}
    
void RawComponent::unloadJavaVM()
{
    if(m_javaVMDll != NULL)
    {
        FreeLibrary(m_javaVMDll);
        m_javaVMDll = NULL;
    }
    m_createJavaVM      = NULL;
    m_getCreatedJavaVMs = NULL;
}



// this method is called after all the generic initialization is done
// this should be empty, unless application-specific initialization is needed
STDMETHODIMP RawComponent::Initialize(struct IMgaProject *) 
{
	return S_OK;
}

// this is the obsolete component interface
// this present implementation either tries to call InvokeEx, or returns an error;
STDMETHODIMP RawComponent::Invoke(IMgaProject* gme, IMgaFCOs *models, long param)
{
#ifdef SUPPORT_OLD_INVOKE
	CComPtr<IMgaFCO> focus;
	CComVariant parval = param;
	return InvokeEx(gme, focus, selected, parvar);
#else
	if(interactive) {
		AfxMessageBox("This component does not support the obsolete invoke mechanism");
	}
	return E_MGA_NOT_SUPPORTED;
#endif
}


// This is the main component method for interpereters and plugins.
// May als be used in case of invokeable addons
STDMETHODIMP RawComponent::InvokeEx( IMgaProject *project,  IMgaFCO *currentobj,
									 IMgaFCOs *selectedobjs,  long param)
{
	COMTRY 
    {
		CComPtr<IMgaTerritory> terr;
		long projectStatus;
		COMTHROW(project->get_ProjectStatus(&projectStatus));
		bool invokeExStartedATransaction = false;
		if (!(projectStatus & 8))
		{
			COMTHROW(project->BeginTransactionInNewTerr(TRANSACTION_GENERAL, &terr));
			invokeExStartedATransaction = true;
		}
		try 
        {
            // load java VM
            loadJavaVM();

            CComponentApp * app = (CComponentApp*)AfxGetApp();

			jthrowable exc;

            // call the java interpreter
            jclass    entryClass  = m_env->FindClass("org/isis/gme/bon/ComponentInvoker");
			exc = m_env->ExceptionOccurred();
			if (exc) {
				throw exc;
			}


            jmethodID entryMethod = m_env->GetStaticMethodID(entryClass, "invokeEx", "(Ljava/lang/String;Ljava/lang/String;IIII)V");
			exc = m_env->ExceptionOccurred();
			if (exc) {
				throw exc;
			}

            m_env->CallStaticVoidMethod(entryClass, entryMethod, 
                m_env->NewStringUTF(this->m_javaClassPath.c_str()), 
                m_env->NewStringUTF(this->m_javaClass.c_str()), project, currentobj, selectedobjs, param);
			exc = m_env->ExceptionOccurred();
			if (exc) {
				throw exc;
			}
            /*jclass    entryClass  = app->m_env->FindClass("org/isis/gme/bon/ComponentInvoker");
            jmethodID entryMethod = app->m_env->GetStaticMethodID(entryClass, "invokeEx", "(Ljava/lang/String;Ljava/lang/String;IIII)V");
            app->m_env->CallStaticVoidMethod(entryClass, entryMethod, 
                app->m_env->NewStringUTF(this->m_javaClassPath.c_str()), 
                app->m_env->NewStringUTF(this->m_javaClass.c_str()), project, currentobj, selectedobjs, param);*/

            //app->m_env->CallStaticVoidMethod(entryClass, entryMethod, NULL, NULL, project, currentobj, 
              //          selectedobjs, param);

			if (invokeExStartedATransaction)
				COMTHROW(project->CommitTransaction());
		}	
		catch(jthrowable jexc){
			char buf[200];
			m_env->ExceptionClear();
			try{
				jclass    throwableClass  = m_env->FindClass("java/lang/Throwable");
				if (throwableClass == NULL) {
					throw; /* class not found */
				}
                        //jmethodID msgMethod = m_env->GetMethodID(throwableClass,"getMessage","()Ljava/lang/String;");
                        jmethodID msgMethod = m_env->GetMethodID(throwableClass,"toString","()Ljava/lang/String;");
				if (msgMethod == NULL) {
					throw; /* method not found */
				}

				jthrowable exc;
				jstring msg = (jstring)m_env->CallObjectMethod(jexc,msgMethod);	
				exc = m_env->ExceptionOccurred();
				if (exc) {
					throw exc;
				}
				
				const char *str = m_env->GetStringUTFChars(msg, 0);
				sprintf(buf, "Java exception occurred at component invokation: %s", str);;
				m_env->ReleaseStringUTFChars(msg, str);
				
				
				
				
				AfxMessageBox(buf);
			}catch(...){
				m_env->ExceptionClear();
				AfxMessageBox("Java exception occurred at component invokation, the cause is unrecoverable.");
			}

			if (invokeExStartedATransaction)
				project->AbortTransaction();

		}
		catch(HKEY){
			if (invokeExStartedATransaction)
				project->AbortTransaction(); 
		}
        catch(...) 
        {
            AfxMessageBox("Internal error while executing java interpreter.");
			if (invokeExStartedATransaction)
				project->AbortTransaction(); 
            throw;
        }
	}
    COMCATCH(;);
}

// GME currently does not use this function
// you only need to implement it if other invokation mechanisms are used
STDMETHODIMP RawComponent::ObjectsInvokeEx( IMgaProject *project,  IMgaObject *currentobj,  IMgaObjects *selectedobjs,  long param) 
{
	if(interactive) {
		AfxMessageBox("The ObjectsInvoke method is not implemented");
	}
	return E_MGA_NOT_SUPPORTED;
}


// implement application specific parameter-mechanism in these functions:
STDMETHODIMP RawComponent::get_ComponentParameter(BSTR name, VARIANT *pVal)
{
	return S_OK;
}

STDMETHODIMP RawComponent::put_ComponentParameter(BSTR name, VARIANT newVal) 
{
	return S_OK;
}


#ifdef GME_ADDON

// these two functions are the main 
STDMETHODIMP RawComponent::GlobalEvent(globalevent_enum event) 
{ 
	if(event == GLOBALEVENT_UNDO) {
		AfxMessageBox("UNDO!!");
	}
	return S_OK; 
}

STDMETHODIMP RawComponent::ObjectEvent(IMgaObject * obj, unsigned long eventmask, VARIANT v) 
{
	if(eventmask & OBJEVENT_CREATED) {
		CComBSTR objID;
		COMTHROW(obj->get_ID(&objID));
		AfxMessageBox( "Object created! ObjID: " + CString(objID)); 
	}		
	return S_OK;
}

#endif
