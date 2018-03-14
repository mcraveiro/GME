#include "stdafx.h"
#include "Mga.h"
#include "Mga_i.c"
#include "Meta.h"
#include "Meta_i.c"
#include "Gme.h"
#include "Gme_i.c"
#include "JavaInterface.h"

#include <string>

#define COMCHECK( hr ) if( FAILED(hr) ) ThrowCOMException( env, hr )

void ThrowCOMException(JNIEnv* env, int hresult)
{
    jclass    cls  = env->FindClass("org/isis/jaut/ComException");
    jmethodID con  = env->GetMethodID( cls, "<init>", "(I)V");
    
    jthrowable exc = (jthrowable)env->NewObject( cls, con, hresult );

	if( exc != NULL )
		env->Throw(exc);
}

JNIEXPORT jint JNICALL Java_org_isis_gme_mgadecorator_MgaDecorator_createNative__
  (JNIEnv * env , jobject)
{
    CLSID clsid;
    HRESULT hr = CLSIDFromProgID(L"Mga.BoxDecorator", &clsid);
    COMCHECK( hr );

    IMgaDecorator * p;
    hr = CoCreateInstance( clsid, NULL, CLSCTX_ALL, IID_IMgaDecorator, (void**)&p );
    COMCHECK( hr );

    return (jint)p;
}


JNIEXPORT jint JNICALL Java_org_isis_gme_mgadecorator_MgaDecorator_createNative__Ljava_lang_String_2
  (JNIEnv * env, jobject, jstring progid )
{   
    const jchar * chars = env->GetStringChars( progid, false );

	std::wstring strUni;
	strUni.append((const wchar_t*)chars, env->GetStringLength(progid));

	CLSID clsid;
    HRESULT hr = CLSIDFromProgID(strUni.c_str(), &clsid);
    env->ReleaseStringChars( progid, chars );
    COMCHECK( hr );

    IMgaDecorator * p;
    hr = CoCreateInstance( clsid, NULL, CLSCTX_ALL, IID_IMgaDecorator, (void**)&p );
    COMCHECK( hr );
    return (jint)p;
}


JNIEXPORT void JNICALL Java_org_isis_gme_mgadecorator_MgaDecorator_initialize
  (JNIEnv * env, jobject, jint decorator, jint project, jint metaPart, jint fco )
{
    HRESULT hr;

    IMgaDecorator * decorator2;
    IMgaProject   * project2;
    IMgaMetaPart  * metaPart2;
    IMgaFCO       * fco2;

    hr = ((IUnknown*)decorator)->QueryInterface( IID_IMgaDecorator, (void**)&decorator2 );
    COMCHECK( hr );

    hr = ((IUnknown*)project)->QueryInterface( IID_IMgaProject, (void**)&project2 );
    COMCHECK( hr );

    hr = ((IUnknown*)metaPart)->QueryInterface( IID_IMgaMetaPart, (void**)&metaPart2 );
    COMCHECK( hr );

    hr = ((IUnknown*)fco)->QueryInterface( IID_IMgaFCO, (void**)&fco2 );
    COMCHECK( hr );
        
    hr = decorator2->Initialize( project2, metaPart2, fco2 );
    COMCHECK( hr );
}


JNIEXPORT void JNICALL Java_org_isis_gme_mgadecorator_MgaDecorator_destroy
  (JNIEnv *, jobject, jint)
{

}

JNIEXPORT void JNICALL Java_org_isis_gme_mgadecorator_MgaDecorator_draw
  (JNIEnv * env, jobject, jint decorator, jint hdc )
{
    HRESULT         hr;
    IMgaDecorator * decorator2;

    hr = ((IUnknown*)decorator)->QueryInterface( IID_IMgaDecorator, (void**)&decorator2 );
    COMCHECK( hr );

    hr = decorator2->Draw( (ULONG)hdc );

    /*CDC * dc = CDC::FromHandle( (HDC)hdc );
    dc->MoveTo(0,0);
    dc->LineTo(1000,1000);*/

    COMCHECK( hr );
}

JNIEXPORT void JNICALL Java_org_isis_gme_mgadecorator_MgaDecorator_setLocation
  (JNIEnv * env, jobject, jint decorator, jint sx, jint sy, jint ex, jint ey )
{
    HRESULT         hr;
    IMgaDecorator * decorator2;

    hr = ((IUnknown*)decorator)->QueryInterface( IID_IMgaDecorator, (void**)&decorator2 );
    COMCHECK( hr );

    hr = decorator2->SetLocation( sx, sy, ex, ey );
    COMCHECK( hr );
}


JNIEXPORT void JNICALL Java_org_isis_gme_mgadecorator_MgaDecorator_getLocation
  (JNIEnv * env, jobject, jint decorator, jintArray loc)
{
    HRESULT         hr;
    IMgaDecorator * decorator2;
    jsize           len = env->GetArrayLength(loc);

    if( len<4 ) ThrowCOMException(env, 0);   // TODO: thrwo something else

    hr = ((IUnknown*)decorator)->QueryInterface( IID_IMgaDecorator, (void**)&decorator2 );
    if( FAILED(hr) ) ThrowCOMException( env, hr );


    long sx, sy, ex, ey;
    hr = decorator2->GetLocation( &sx, &sy, &ex, &ey );
    COMCHECK( hr );

    jint * locData = env->GetIntArrayElements(loc, false);

    locData[0] = sx;
    locData[1] = sy;
    locData[2] = ex;
    locData[3] = ey;

    env->ReleaseIntArrayElements(loc, locData, 0); 
}

JNIEXPORT void JNICALL Java_org_isis_gme_mgadecorator_MgaDecorator_getPreferredSize
  (JNIEnv * env, jobject, jint decorator, jintArray size)
{
    HRESULT         hr;
    IMgaDecorator * decorator2;
    jsize           len = env->GetArrayLength(size);

    if( len<2 ) ThrowCOMException(env, 0);   // TODO: thrwo something else

    hr = ((IUnknown*)decorator)->QueryInterface( IID_IMgaDecorator, (void**)&decorator2 );
    COMCHECK( hr );

    long sizex, sizey;
    hr = decorator2->GetPreferredSize( &sizex, &sizey );
    COMCHECK( hr );

    jint * sizeData = env->GetIntArrayElements(size, false);

    sizeData[0] = sizex;
    sizeData[1] = sizey;

    env->ReleaseIntArrayElements(size, sizeData, 0); 
}


JNIEXPORT void JNICALL Java_org_isis_gme_mgadecorator_MgaDecorator_setActvie
  (JNIEnv * env, jobject, jint decorator, jboolean active )
{
    HRESULT         hr;
    IMgaDecorator * decorator2;

    hr = ((IUnknown*)decorator)->QueryInterface( IID_IMgaDecorator, (void**)&decorator2 );
    COMCHECK( hr );

    hr = decorator2->SetActive( active );
    COMCHECK( hr );
}

JNIEXPORT jint JNICALL Java_org_isis_gme_mgadecorator_MgaDecorator_getPorts
  (JNIEnv * env, jobject, jint decorator )
{
    HRESULT         hr;
    IMgaDecorator * decorator2;
    IMgaFCOs      * fcos;

    hr = ((IUnknown*)decorator)->QueryInterface( IID_IMgaDecorator, (void**)&decorator2 );
    COMCHECK( hr );

    hr = decorator2->GetPorts( &fcos );
    COMCHECK( hr );

    return (jint)fcos;
}

JNIEXPORT void JNICALL Java_org_isis_gme_mgadecorator_MgaDecorator_getPortLocation
  (JNIEnv * env, jobject, jint decorator, jint fco, jintArray loc )
{
    HRESULT         hr;
    IMgaDecorator * decorator2;
    IMgaFCO       * fco2;
    jsize           len = env->GetArrayLength(loc);

    if( len<4 ) ThrowCOMException(env, 0);   // TODO: thrwo something else

    hr = ((IUnknown*)decorator)->QueryInterface( IID_IMgaDecorator, (void**)&decorator2 );
    COMCHECK( hr );

    hr = ((IUnknown*)fco)->QueryInterface( IID_IMgaFCO, (void**)&fco2 );
    COMCHECK( hr );
        
    long sx, sy, ex, ey;
    hr = decorator2->GetPortLocation( fco2, &sx, &sy, &ex, &ey );
    COMCHECK( hr );

    jint * locData = env->GetIntArrayElements(loc, false);

    locData[0] = sx;
    locData[1] = sy;
    locData[2] = ex;
    locData[3] = ey;

    env->ReleaseIntArrayElements(loc, locData, 0); 
}

