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

#ifndef JAUT_LIBRARY_H
#define JAUT_LIBRARY_H

extern "C"
{

extern const char *ERR_VariantPointer;
extern const char *ERR_InvalidType;
extern const char *ERR_InvalidBounds;
extern const char *ERR_DispatchPointer;

extern jclass JAUT_JAutException_Class;
extern jmethodID JAUT_JAutException_Constructor;

extern jclass JAUT_ComException_Class;
extern jmethodID JAUT_ComException_Constructor;

extern jclass JAUT_InvokeException_Class;
extern jmethodID JAUT_InvokeException_Constructor;

extern jclass JAUT_Variant_Class;
extern jfieldID JAUT_Variant_Pointer;
extern jmethodID JAUT_Variant_Constructor;

extern jclass JAUT_Dispatch_Class;
extern jfieldID JAUT_Dispatch_Pointer;
extern jmethodID JAUT_Dispatch_Constructor;

extern jclass JAVA_OutOfMemoryError_Class;
extern jmethodID JAVA_OutOfMemoryError_Constructor;

extern jclass JAVA_NullPointerException_Class;
extern jmethodID JAVA_NullPointerException_Constructor;

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved);

JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM *vm, void *reserved);

void ThrowJAutException(JNIEnv *env, const char *desc);
void ThrowComException(JNIEnv *env, HRESULT hr);
void ThrowComExceptionString(JNIEnv *env, HRESULT hr, jstring detail);
void ThrowInvokeException(JNIEnv* env, EXCEPINFO *info);
void ThrowOutOfMemoryError(JNIEnv* env);
void ThrowNullPointerException(JNIEnv* env);

}

#endif