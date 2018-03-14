//#include "Mga.h"
#include "StdAfx.h"
#include "MgaEventSink.h"

class MgaEventSinkAdapter// : public IMgaEventSink
{
public:
	MgaEventSinkAdapter( JNIEnv * env, jobject obj )
	{
		m_env = env;
		m_obj = obj;
	}

	ULONG STDMETHODCALLTYPE AddRef() 
	{ 
		return ++m_dwRef;
	}

	ULONG STDMETHODCALLTYPE Release()
	{ 
		if (--m_dwRef == 0)
		{
			delete this;
			return 0;
		}
		return m_dwRef;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID iid, void FAR* FAR* ppvObj )
	{
		if( iid == IID_IUnknown ) //|| iid == IID_IMgaEventSink )
		{
			*ppvObj = this;
			AddRef();
			return NOERROR;
		}
		return ResultFromScode(E_NOINTERFACE);
	}

	/*HRESULT STDMETHODCALLTYPE GlobalEvent( globalevent_enum event )
	{
		printf( "GlobalEvent::ObjectEvent called\n" );
		return S_OK;
	}
		
	HRESULT STDMETHODCALLTYPE ObjectEvent( IMgaObject * obj, unsigned long eventmask, VARIANT v )
	{
		printf( "MgaEventSinkAdapter::ObjectEvent called\n" );
		return S_OK;
	}*/

private:
	DWORD    m_dwRef;
	JNIEnv * m_env;
	jobject  m_obj;
};


extern "C"
{

JNIEXPORT void JNICALL Java_org_isis_gme_mga_MgaEventSink_createSink( JNIEnv * env, jobject obj )
{	
	/*MgaEventSinkAdapter * p = new MgaEventSinkAdapter( env, obj );

	jclass   mgaEventSinkClass = env->FindClass  ( "org/isis/gme/mga/MgaEventSink" );
	jfieldID sinkPointerID     = env->GetFieldID (mgaEventSinkClass, "sinkPointer", "I");

	env->SetIntField( obj, sinkPointerID, (jint)p );*/
}



}
