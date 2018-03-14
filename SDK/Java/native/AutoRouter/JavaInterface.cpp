#include "stdafx.h"
#include "ArGraph.h"
#include "JavaInterface.h"


void ThrowOutOfMemoryError(JNIEnv* env)
{
    jclass    cls  = env->FindClass("java/lang/OutOfMemoryError");
    jmethodID con  = env->GetMethodID( cls, "<init>", "()V");
    
    jthrowable exc = (jthrowable)env->NewObject( cls, con );

	if( exc != NULL )
		env->Throw(exc);
}

void ThrowNullPointerError(JNIEnv* env)
{
    jclass    cls  = env->FindClass("java/lang/NullPointerException");
    jmethodID con  = env->GetMethodID( cls, "<init>", "()V");
    
    jthrowable exc = (jthrowable)env->NewObject( cls, con );

	if( exc != NULL )
		env->Throw(exc);
}

/*
 * Class: org_isis_gme_modeleditor_autorouter_Box
 */

JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_autorouter_Box_createNative
  (JNIEnv * env, jobject)
{
    CArBox * box = new CArBox();
    if( box == NULL ) ThrowOutOfMemoryError( env );
    return (jint)box;

}

JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Box_deleteNative
  (JNIEnv * env, jobject, jint address )
{
    CArBox * box = (CArBox*)address;
    if( box == NULL ) ThrowNullPointerError( env );
    delete box;
}

JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Box_setRect
  (JNIEnv * env, jobject, jint address, jint x1, jint y1, jint x2, jint y2 ) 
{
    CArBox * box = (CArBox*)address;
    if( box == NULL ) ThrowNullPointerError( env );
    box->SetRect( CRect( x1, y1, x2, y2 ) );
}

JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Box_addPort
  (JNIEnv * env, jobject, jint address, jint portAddress)
{
    CArBox * box = (CArBox*)address;
    if( box == NULL ) ThrowNullPointerError( env );
    CArPort * port = (CArPort*)portAddress;
    if( port == NULL ) ThrowNullPointerError( env );
    box->Add( port );
}

/*
 * Class: org_isis_gme_modeleditor_autorouter_Path
 */

JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_autorouter_Path_createNative
  (JNIEnv * env, jobject)
{
    CArPath * path = new CArPath();
    if( path == NULL ) ThrowOutOfMemoryError( env );
    return (jint)path;    
}

JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Path_deleteNative
  (JNIEnv * env, jobject, jint address )
{
    CArPath * path = (CArPath*)address;
    if( path == NULL ) ThrowNullPointerError( env );
    delete path;
}


JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_autorouter_Path_getCount
  (JNIEnv * env, jobject, jint address )
{
    CArPath * path = (CArPath*)address;
    if( path == NULL ) ThrowNullPointerError( env );
    return path->GetPointList().GetCount();
}

JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_autorouter_Path_getHead
  (JNIEnv * env, jobject, jint address )
{
    CArPath * path = (CArPath*)address;
    if( path == NULL ) ThrowNullPointerError( env );
    return (jint)path->GetPointList().GetHeadPosition();
}

JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_autorouter_Path_getNext
  (JNIEnv * env, jobject, jint address, jint iterator )
{
    CArPath * path = (CArPath*)address;
    if( path == NULL ) ThrowNullPointerError( env );
    POSITION pos = (POSITION)iterator;
    path->GetPointList().GetNext( pos );
    return (jint)pos;
}

JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_autorouter_Path_getX
  (JNIEnv * env, jobject, jint address, jint iterator)
{
    CArPath * path = (CArPath*)address;
    if( path == NULL ) ThrowNullPointerError( env );
    CPoint p = path->GetPointList().GetAt( (POSITION)iterator );
    return p.x;

}

JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_autorouter_Path_getY
  (JNIEnv * env, jobject, jint address, jint iterator)
{
    CArPath * path = (CArPath*)address;
    if( path == NULL ) ThrowNullPointerError( env );
    CPoint p = path->GetPointList().GetAt( (POSITION)iterator );
    return p.y;
}


JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Path_setEndDir
  (JNIEnv * env, jobject, jint address, jint dir)
{
    CArPath * path = (CArPath*)address;
    if( path == NULL ) ThrowNullPointerError( env );
    path->SetEndDir( dir );
}


JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Path_setStartDir
  (JNIEnv * env, jobject, jint address, jint dir)
{
    CArPath * path = (CArPath*)address;
    if( path == NULL ) ThrowNullPointerError( env );
    path->SetEndDir( dir );
}

/*
 * Class: org_isis_gme_modeleditor_autorouter_Port
 */


JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_autorouter_Port_createNative
  (JNIEnv * env, jobject)
{
    CArPort * port = new CArPort();
    if( port == NULL ) ThrowOutOfMemoryError( env );
    return (jint)port;
}

JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Port_deleteNative
  (JNIEnv * env, jobject, jint address )
{
    CArPort * port = (CArPort*)address;
    if( port== NULL ) ThrowNullPointerError( env );
    delete port;
}

JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Port_setRect
  (JNIEnv * env, jobject, jint address, jint x1, jint y1, jint x2, jint y2 )
{
    CArPort * port = (CArPort*)address;
    if( port== NULL ) ThrowNullPointerError( env );
    port->SetRect( CRect( x1, y1, x2, y2 ) );
}

JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Port_setAttributes
  (JNIEnv * env, jobject, jint address, jint attr)
{
    CArPort * port = (CArPort*)address;
    if( port== NULL ) ThrowNullPointerError( env );
    port->SetAttributes( attr );
}


JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_autorouter_Port_getAttributes
  (JNIEnv * env, jobject, jint address )
{
    CArPort * port = (CArPort*)address;
    if( port== NULL ) ThrowNullPointerError( env );
    return port->GetAttributes();
}

/*
 * Class: org_isis_gme_modeleditor_autorouter_Graph
 */

JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_autorouter_Graph_createNative
  (JNIEnv * env, jobject)
{
    CArGraph * graph = new CArGraph();
    if( graph == NULL ) ThrowOutOfMemoryError( env );
    return (jint)graph;
}

JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Graph_deleteNative
  (JNIEnv * env, jobject, jint address )
{
    CArGraph * graph = (CArGraph*)address;
    if( graph== NULL ) ThrowNullPointerError( env );
    delete graph;
}

JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Graph_addBox
  (JNIEnv * env, jobject, jint address, jint boxAddress )
{
    CArGraph * graph = (CArGraph*)address;
    if( graph== NULL ) ThrowNullPointerError( env );
    CArBox * box = (CArBox*)boxAddress;
    if( box == NULL ) ThrowNullPointerError( env );
    graph->Add( box );
}

JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_autorouter_Graph_addPath
  (JNIEnv * env, jobject, jint address, jint srcPortAddr, jint destPortAddr)
{
    CArGraph * graph = (CArGraph*)address;
    if( graph== NULL ) ThrowNullPointerError( env );
    CArPort * srcPort = (CArPort*)srcPortAddr;
    if( srcPort == NULL ) ThrowNullPointerError( env );
    CArPort * destPort = (CArPort*)destPortAddr;
    if( destPort == NULL ) ThrowNullPointerError( env );

    CArPath * path = graph->AddPath( srcPort, destPort );
    return (jint)path;
}

JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Graph_removeBox
  (JNIEnv * env, jobject, jint address, jint boxAddress )
{
    CArGraph * graph = (CArGraph*)address;
    if( graph== NULL ) ThrowNullPointerError( env );
    CArBox * box = (CArBox*)boxAddress;
    if( box == NULL ) ThrowNullPointerError( env );
    graph->Remove( box );
}


JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Graph_removePath
  (JNIEnv * env, jobject, jint address, jint pathAddress )
{
    CArGraph * graph = (CArGraph*)address;
    if( graph== NULL ) ThrowNullPointerError( env );
    CArPath * path = (CArPath*)pathAddress;
    if( path == NULL ) ThrowNullPointerError( env );
    graph->Remove( path );
}

JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_autorouter_Graph_autoRoute
  (JNIEnv * env, jobject, jint address )
{
    CArGraph * graph = (CArGraph*)address;
    if( graph== NULL ) ThrowNullPointerError( env );
    return graph->AutoRoute();
}

JNIEXPORT void JNICALL Java_org_isis_gme_modeleditor_autorouter_Native_deleteNative
  (JNIEnv * env, jobject, jint address)
{
    delete (CArGraph *)address;
}


JNIEXPORT jint JNICALL Java_org_isis_gme_modeleditor_NativeDrawerTest_draw
  (JNIEnv *, jclass, jint handle)
{
    HDC hdc = (HDC)handle;

    MoveToEx( hdc, 0, 0, NULL );
    LineTo( hdc, 100, 10 );
    return 0;
}