#ifndef RAWCOMPONENT_H
#define RAWCOMPONENT_H

#include <jni.h>
#include <string>
#include "Mga.h"

// Declaration of the main RAW COM component interface class


#ifdef BUILDER_OBJECT_NETWORK
#error   This file should only be included in the RAW COM configurations
#endif

typedef jint (JNICALL *P_JNI_CreateJavaVM)        (JavaVM **pvm, void** penv, void *args);
typedef jint (JNICALL *P_JNI_GetCreatedJavaVMs)   (JavaVM **vmBuf,jsize bufLen, jsize *nVMs);

class RawComponent 
{
public:
	 RawComponent();
    ~RawComponent();

	STDMETHODIMP Initialize(struct IMgaProject *);
	STDMETHODIMP Invoke(IMgaProject* gme, IMgaFCOs *models, long param);
	STDMETHODIMP InvokeEx( IMgaProject *project,  IMgaFCO *currentobj,  IMgaFCOs *selectedobjs,  long param);
	STDMETHODIMP ObjectsInvokeEx( IMgaProject *project,  IMgaObject *currentobj,  IMgaObjects *selectedobjs,  long param);
	STDMETHODIMP get_ComponentParameter(BSTR name, VARIANT *pVal);
	STDMETHODIMP put_ComponentParameter(BSTR name, VARIANT newVal);

#ifdef GME_ADDON
	STDMETHODIMP GlobalEvent(globalevent_enum event);
	STDMETHODIMP ObjectEvent(IMgaObject * obj, unsigned long eventmask, VARIANT v);
#endif

protected:
    void getJvmDllPath();
    void loadJavaVM();
    void unloadJavaVM();


public:
#ifdef GME_ADDON
	CComPtr<IMgaProject> project;  // this is set before Initialize() is called
	CComPtr<IMgaAddOn>   addon;      // this is set before Initialize() is called
#endif
	bool                 interactive;

protected:
    HMODULE                   m_javaVMDll;
    P_JNI_CreateJavaVM        m_createJavaVM;
    P_JNI_GetCreatedJavaVMs   m_getCreatedJavaVMs;

    JNIEnv                  * m_env;
    JavaVM                  * m_jvm;
        
    std::string  m_javaClassPath;
    std::string  m_javaClass;

    friend class CJavaCompFactory;
};


#endif //RAWCOMPONENT_H