/*
Copyright (C) 2002, Vanderbilt University
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions 
are met:

1.   Redistributions of source code must retain the above copyright 
     notice, this list of conditions and the following disclaimer.

2.   Redistributions in binary form must reproduce the above copyright 
     notice, this list of conditions and the following disclaimer in 
     the documentation and/or other materials provided with the 
     distribution.

3.   Neither the name of Vanderbilt University nor the names of its 
     contributors may be used to endorse or promote products derived 
     from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

author: Miklos Maroti
*/

#include "StdAfx.h"

extern "C"
{

const char *ERR_VariantPointer = "The underlying VARIANT is not created";
const char *ERR_InvalidType = "Invalid variant type";
const char *ERR_InvalidBounds = "Invalid array bounds";
const char *ERR_DispatchPointer = "The embedded interface pointer is NULL";

jclass JAUT_JAutException_Class = NULL;
jmethodID JAUT_JAutException_Constructor;

jclass JAUT_ComException_Class = NULL;
jmethodID JAUT_ComException_Constructor;
jmethodID JAUT_ComException_Constructor_IString;

jclass JAUT_InvokeException_Class = NULL;
jmethodID JAUT_InvokeException_Constructor;

jclass JAUT_Variant_Class = NULL;
jfieldID JAUT_Variant_Pointer;
jmethodID JAUT_Variant_Constructor;

jclass JAUT_Dispatch_Class = NULL;
jfieldID JAUT_Dispatch_Pointer;
jmethodID JAUT_Dispatch_Constructor;

jclass JAVA_OutOfMemoryError_Class = NULL;
jmethodID JAVA_OutOfMemoryError_Constructor;

jclass JAVA_NullPointerException_Class = NULL;
jmethodID JAVA_NullPointerException_Constructor;

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved)
{
	JNIEnv *env = NULL;
	vm->GetEnv((void**)&env, JNI_VERSION_1_2);
	if( env == NULL )
		return JNI_ERR;

	// JAutException
	JAUT_JAutException_Class = env->FindClass("org/isis/jaut/JAutException");
	if( JAUT_JAutException_Class != NULL )
		JAUT_JAutException_Class = (jclass)env->NewWeakGlobalRef(JAUT_JAutException_Class);

	if( JAUT_JAutException_Class != NULL )
		JAUT_JAutException_Constructor = env->GetMethodID(JAUT_JAutException_Class, 
			"<init>", "(Ljava/lang/String;)V");

	if( JAUT_JAutException_Class == NULL || JAUT_JAutException_Constructor == NULL )
		return JNI_ERR;

	// ComException
	JAUT_ComException_Class = env->FindClass("org/isis/jaut/ComException");
	if( JAUT_ComException_Class != NULL )
		JAUT_ComException_Class = (jclass)env->NewWeakGlobalRef(JAUT_ComException_Class);

	if( JAUT_ComException_Class != NULL )
		JAUT_ComException_Constructor = env->GetMethodID(JAUT_ComException_Class, 
			"<init>", "(I)V");

	if( JAUT_ComException_Class != NULL )
		JAUT_ComException_Constructor_IString = env->GetMethodID(JAUT_ComException_Class, 
			"<init>", "(ILjava/lang/String;)V");

	if( JAUT_ComException_Class == NULL || JAUT_ComException_Constructor == NULL )
		return JNI_ERR;

	// InvokeException
	JAUT_InvokeException_Class = env->FindClass("org/isis/jaut/InvokeException");
	if( JAUT_InvokeException_Class != NULL )
		JAUT_InvokeException_Class = (jclass)env->NewWeakGlobalRef(JAUT_InvokeException_Class);

	if( JAUT_InvokeException_Class != NULL )
		JAUT_InvokeException_Constructor = env->GetMethodID(JAUT_InvokeException_Class, 
			"<init>", "(ILjava/lang/String;Ljava/lang/String;)V");

	if( JAUT_ComException_Class == NULL || JAUT_ComException_Constructor == NULL )
		return JNI_ERR;

	// Variant
	JAUT_Variant_Class = env->FindClass("org/isis/jaut/Variant");
	if( JAUT_Variant_Class != NULL )
		JAUT_Variant_Class = (jclass)env->NewWeakGlobalRef(JAUT_Variant_Class);

	if( JAUT_Variant_Class != NULL )
		JAUT_Variant_Pointer = env->GetFieldID(JAUT_Variant_Class, "pVariant", "I");

	if( JAUT_Variant_Class != NULL )
		JAUT_Variant_Constructor = env->GetMethodID(JAUT_Variant_Class, 
			"<init>", "()V");

	if( JAUT_Variant_Class == NULL || JAUT_Variant_Pointer == NULL 
			|| JAUT_Variant_Constructor == NULL )
		return JNI_ERR;

	// Dispatch
	JAUT_Dispatch_Class = env->FindClass("org/isis/jaut/Dispatch");
	if( JAUT_Dispatch_Class != NULL )
		JAUT_Dispatch_Class = (jclass)env->NewWeakGlobalRef(JAUT_Dispatch_Class);

	if( JAUT_Dispatch_Class != NULL )
		JAUT_Dispatch_Pointer = env->GetFieldID(JAUT_Dispatch_Class, "pDispatch", "I");

	if( JAUT_Dispatch_Class != NULL )
		JAUT_Dispatch_Constructor = env->GetMethodID(JAUT_Dispatch_Class, 
			"<init>", "()V");

	if( JAUT_Dispatch_Class == NULL || JAUT_Dispatch_Pointer == NULL 
			|| JAUT_Dispatch_Constructor == NULL )
		return JNI_ERR;

	// OutOfMemoryError
	JAVA_OutOfMemoryError_Class = env->FindClass("java/lang/OutOfMemoryError");
	if( JAVA_OutOfMemoryError_Class != NULL )
		JAVA_OutOfMemoryError_Class = (jclass)env->NewGlobalRef(JAVA_OutOfMemoryError_Class);
	
	if( JAVA_OutOfMemoryError_Class != NULL )
		JAVA_OutOfMemoryError_Constructor = env->GetMethodID(JAVA_OutOfMemoryError_Class, 
			"<init>", "()V");
	
	if( JAVA_OutOfMemoryError_Class == NULL || JAVA_OutOfMemoryError_Constructor == NULL )
		return JNI_ERR;

	// NullPointerException
	JAVA_NullPointerException_Class = env->FindClass("java/lang/NullPointerException");
	if( JAVA_NullPointerException_Class != NULL )
		JAVA_NullPointerException_Class = (jclass)env->NewGlobalRef(JAVA_NullPointerException_Class);
	
	if( JAVA_NullPointerException_Class != NULL )
		JAVA_NullPointerException_Constructor = env->GetMethodID(JAVA_NullPointerException_Class, 
			"<init>", "()V");
	
	if( JAVA_NullPointerException_Class == NULL || JAVA_NullPointerException_Constructor == NULL )
		return JNI_ERR;

	return JNI_VERSION_1_2;
}

JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM *vm, void *reserved)
{
	JNIEnv *env = NULL;
	vm->GetEnv((void**)&env, JNI_VERSION_1_2);
	if( env == NULL )
		return;

	env->DeleteWeakGlobalRef(JAUT_JAutException_Class);
	env->DeleteWeakGlobalRef(JAUT_ComException_Class);
	env->DeleteWeakGlobalRef(JAUT_InvokeException_Class);
	env->DeleteWeakGlobalRef(JAUT_Variant_Class);
	env->DeleteWeakGlobalRef(JAUT_Dispatch_Class);
	env->DeleteGlobalRef(JAVA_OutOfMemoryError_Class);
	env->DeleteGlobalRef(JAVA_NullPointerException_Class);
}

void ThrowJAutException(JNIEnv *env, const char *desc)
{
	jstring str = env->NewStringUTF(desc);
	if( str == NULL )
		return;

	jthrowable exc = (jthrowable)env->NewObject(JAUT_JAutException_Class, 
		JAUT_JAutException_Constructor, str);

	if( exc != NULL )
		env->Throw(exc);
}

void ThrowComExceptionString(JNIEnv *env, HRESULT hr, jstring detail)
{
	if( hr == E_OUTOFMEMORY )
	{
		ThrowOutOfMemoryError(env);
		return;
	}

	jthrowable exc;
	if (detail != NULL && JAUT_ComException_Constructor_IString != NULL)
		exc = (jthrowable)env->NewObject(JAUT_ComException_Class, JAUT_ComException_Constructor_IString, (jint)hr, detail);
	else 
		exc = (jthrowable)env->NewObject(JAUT_ComException_Class, JAUT_ComException_Constructor, (jint)hr);

	if( exc != NULL )
		env->Throw(exc);
}

void ThrowComException(JNIEnv *env, HRESULT hr)
{
	ThrowComExceptionString(env, hr, NULL);
}

void ThrowInvokeException(JNIEnv* env, EXCEPINFO *info)
{
	if( info->pfnDeferredFillIn != NULL )
        (*(info->pfnDeferredFillIn))(info);

	jint code = info->wCode;
	if( code == 0 )
		code = info->scode;

	jstring source = env->NewString((const jchar*)(info->bstrSource), 
		SysStringLen(info->bstrSource));

	jstring descr = env->NewString((const jchar*)(info->bstrDescription), 
		SysStringLen(info->bstrDescription));

	jthrowable exc = (jthrowable)env->NewObject(JAUT_InvokeException_Class,
		JAUT_InvokeException_Constructor, code, source, descr);

	if( exc != NULL )
		env->Throw(exc);
}

void ThrowOutOfMemoryError(JNIEnv* env)
{
	jthrowable exc = (jthrowable)env->NewObject(JAVA_OutOfMemoryError_Class,
		JAVA_OutOfMemoryError_Constructor);

	if( exc != NULL )
		env->Throw(exc);
}

void ThrowNullPointerException(JNIEnv* env)
{
	jthrowable exc = (jthrowable)env->NewObject(JAVA_NullPointerException_Class,
		JAVA_NullPointerException_Constructor);

	if( exc != NULL )
		env->Throw(exc);
}

}
