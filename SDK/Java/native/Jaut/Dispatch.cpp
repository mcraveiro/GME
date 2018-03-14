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
#include "Dispatch.h"

#include <string>

extern "C"
{

JNIEXPORT void JNICALL Java_org_isis_jaut_Dispatch_attach__Lorg_isis_jaut_Variant_2
  (JNIEnv *env, jobject obj, jobject target)
{
	if( target == NULL )
	{
		ThrowNullPointerException(env);
		return;
	}

	VARIANT *v = (VARIANT*)env->GetIntField(target, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return;
	}

	IDispatch *p;

	VARTYPE vt = V_VT(v);
	if( vt == VT_DISPATCH )
	{
		p = V_DISPATCH(v);
		if( p != NULL )
			p->AddRef();
	}
	else if( vt == (VT_DISPATCH|VT_BYREF) )
	{
		p = *V_DISPATCHREF(v);
		if( p != NULL )
			p->AddRef();
	}
	else
	{
		IUnknown *q;

		if( vt == VT_UNKNOWN )
			q = V_UNKNOWN(v);
		else if( vt == (VT_UNKNOWN|VT_BYREF) )
			q = *V_UNKNOWNREF(v);
		else
		{
			ThrowJAutException(env, ERR_InvalidType);
			return;
		}

		p = NULL;
		if( q != NULL )
		{
			HRESULT hr = q->QueryInterface(IID_IDispatch, (void**)&p);
			if( FAILED(hr) )
			{
				ThrowComException(env, hr);
				return;
			}
		}
	}

	IDispatch *q = (IDispatch*)env->GetIntField(obj, JAUT_Dispatch_Pointer);
	if( q != NULL )
		q->Release();

	env->SetIntField(obj, JAUT_Dispatch_Pointer, (jint)p);
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Dispatch_attach__Lorg_isis_jaut_Dispatch_2
  (JNIEnv *env, jobject obj, jobject target)
{
	if( target == NULL )
	{
		ThrowNullPointerException(env);
		return;
	}

	IDispatch *q = (IDispatch*)env->GetIntField(target, JAUT_Dispatch_Pointer);
	IDispatch *p = (IDispatch*)env->GetIntField(obj, JAUT_Dispatch_Pointer);

	if( p != q )
	{
		if( q != NULL )
			q->AddRef();

		if( p != NULL )
			p->Release();

		env->SetIntField(obj, JAUT_Dispatch_Pointer, (jint)q);
	}
}


JNIEXPORT void JNICALL Java_org_isis_jaut_Dispatch_attach__I
  (JNIEnv *env, jobject obj, jint target)
{
    if( target == 0 )
	{
		ThrowNullPointerException(env);
		return;
	}

	IDispatch *q = (IDispatch*)target;
	IDispatch *p = (IDispatch*)env->GetIntField(obj, JAUT_Dispatch_Pointer);

	if( p != q )
	{
        if( q != NULL )
			q->AddRef();

		if( p != NULL )
			p->Release();

		env->SetIntField(obj, JAUT_Dispatch_Pointer, (jint)q);
	}
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Dispatch_release
  (JNIEnv *env, jobject obj)
{
	IDispatch *p = (IDispatch*)env->GetIntField(obj, JAUT_Dispatch_Pointer);
	if( p != NULL )
	{
		p->Release();
		env->SetIntField(obj, JAUT_Dispatch_Pointer, (jint)NULL);
	}
}

JNIEXPORT jint JNICALL Java_org_isis_jaut_Dispatch_hashCode
  (JNIEnv *env, jobject obj)
{
	IDispatch *p = (IDispatch*)env->GetIntField(obj, JAUT_Dispatch_Pointer);
	if( p == NULL )
		return 0;

	IUnknown *q = NULL;
	p->QueryInterface(IID_IUnknown, (void**)&q);

	jint ret = (jint)q;
	if( q != NULL )
		q->Release();

	return ret;
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Dispatch_changeInterface
  (JNIEnv *env, jobject obj, jstring itf)
{
	if( itf == NULL )
	{
		ThrowNullPointerException(env);
		return;
	}

	IDispatch *p = (IDispatch*)env->GetIntField(obj, JAUT_Dispatch_Pointer);
	if( p == NULL )
	{
		ThrowJAutException(env, ERR_DispatchPointer);
		return;
	}

	const jchar *uni = env->GetStringChars(itf, NULL);
	if( uni == NULL )
	{
		// TODO: Should we throw an/this Exception here?
		ThrowOutOfMemoryError(env);
		return;
	}
	std::wstring strUni;
	strUni.append((const wchar_t*)uni, env->GetStringLength(itf));

	IID iid;
	HRESULT hr = IIDFromString(strUni.c_str(), &iid);
	env->ReleaseStringChars(itf, uni);
	
	if( FAILED(hr) )
	{
		ThrowComException(env, hr);
		return;
	}

	IDispatch *q = NULL;
	hr = p->QueryInterface(iid, (void**)&q);
	if( FAILED(hr) )
	{
		ThrowComException(env, hr);
		return;
	}

	p->Release();
	env->SetIntField(obj, JAUT_Dispatch_Pointer, (jint)q);
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Dispatch_attachNewInstance
  (JNIEnv *env, jobject obj, jstring progid, jint clsctx)
{
	if( progid == NULL )
	{
		ThrowNullPointerException(env);
		return;
	}

	const jchar *uni = env->GetStringChars(progid, NULL);
	if( uni == NULL )
	{
		// TODO: Should we throw an/this Exception here?
		ThrowOutOfMemoryError(env);
		return;
	}
	std::wstring strUni;
	strUni.append((const wchar_t*)uni, env->GetStringLength(progid));
	
	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(strUni.c_str(), &clsid);
	env->ReleaseStringChars(progid, uni);

	if( FAILED(hr) )
	{
		ThrowComException(env, hr);
		return;
	}

	IDispatch *q = NULL;
	hr = CoCreateInstance(clsid, NULL, clsctx, IID_IDispatch, (void**)&q);
	if( FAILED(hr) )
	{
		ThrowComException(env, hr);
		return;
	}

	IDispatch *p = (IDispatch*)env->GetIntField(obj, JAUT_Dispatch_Pointer);
	if( p != NULL )
		p->Release();

	env->SetIntField(obj, JAUT_Dispatch_Pointer, (jint)q);
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Dispatch_attachActiveObject
  (JNIEnv *env, jobject obj, jstring progid)
{
	if( progid == NULL )
	{
		ThrowNullPointerException(env);
		return;
	}

	const jchar *uni = env->GetStringChars(progid, NULL);
	if( uni == NULL )
	{
		// TODO: Should we throw an/this Exception here?
		ThrowOutOfMemoryError(env);
		return;
	}
	std::wstring strUni;
	strUni.append((const wchar_t*)uni, env->GetStringLength(progid));
	
	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(strUni.c_str(), &clsid);
	env->ReleaseStringChars(progid, uni);

	if( FAILED(hr) )
	{
		ThrowComException(env, hr);
		return;
	}

	IUnknown *u = NULL;
	hr = GetActiveObject(clsid, NULL, &u);
	if( FAILED(hr) )
	{
		ThrowComException(env, hr);
		return;
	}

	IDispatch *q = NULL;
	if( u != NULL )
	{
		hr = u->QueryInterface(IID_IDispatch, (void**)&q);
		u->Release();

		if( FAILED(hr) )
		{
			ThrowComException(env, hr);
			return;
		}
	}

	IDispatch *p = (IDispatch*)env->GetIntField(obj, JAUT_Dispatch_Pointer);
	if( p != NULL )
		p->Release();

	env->SetIntField(obj, JAUT_Dispatch_Pointer, (jint)q);
}

JNIEXPORT jintArray JNICALL Java_org_isis_jaut_Dispatch_getIDsOfNames
  (JNIEnv *env, jobject obj, jobjectArray names)
{
	if( names == NULL )
	{
		ThrowNullPointerException(env);
		return NULL;
	}

	IDispatch *p = (IDispatch*)env->GetIntField(obj, JAUT_Dispatch_Pointer);
	if( p == NULL )
	{
		ThrowJAutException(env, ERR_DispatchPointer);
		return NULL;
	}

	jsize size = env->GetArrayLength(names);

	jintArray dispids = env->NewIntArray(size);

	OLECHAR **ns = NULL;
	if( dispids != NULL )
		ns = (OLECHAR**)CoTaskMemAlloc(sizeof(OLECHAR*) * size);

	int index = 0;
	if( ns != NULL )
	{
		for(index = 0; index < size; ++index)
		{
			jstring name = (jstring)env->GetObjectArrayElement(names, index);
			if( name == NULL )
				break;
			jsize len = env->GetStringLength(name);
			ns[index] = new wchar_t[len+1];
			const jchar* str = env->GetStringChars(name, NULL);
			wcsncpy(ns[index], (const wchar_t*)str, len);
			ns[index][len] = L'\0';
			env->ReleaseStringChars(name, str);
			env->DeleteLocalRef(name);

			if( ns[index] == NULL )
				break;
		}
	}
	else
		ThrowOutOfMemoryError(env);

	jint *ds = NULL;
	if( ns != NULL && index == size )
		ds = env->GetIntArrayElements(dispids, NULL);

	HRESULT hr = S_OK;
	if( ds != NULL )
		hr = p->GetIDsOfNames(IID_NULL, ns, size, NULL, ds);

	if( ds != NULL )
		env->ReleaseIntArrayElements(dispids, ds, 0);

	while( --index >= 0 )
	{
		jstring name = (jstring)env->GetObjectArrayElement(names, index);
		delete[] ns[index];
		// FIXME: don't we do this above?
		env->DeleteLocalRef(name);
	}

	if( ns != NULL )
		CoTaskMemFree(ns);

	if( FAILED(hr) )
	{
		ThrowComException(env, hr);
		return NULL;
	}

	return dispids;
}

JNIEXPORT jint JNICALL Java_org_isis_jaut_Dispatch_getIDOfName
  (JNIEnv *env, jobject obj, jstring name)
{
	if( name == NULL )
	{
		ThrowNullPointerException(env);
		return NULL;
	}

	IDispatch *p = (IDispatch*)env->GetIntField(obj, JAUT_Dispatch_Pointer);
	if( p == NULL )
	{
		ThrowJAutException(env, ERR_DispatchPointer);
		return NULL;
	}

	const jchar *n = env->GetStringChars(name, NULL);
	if( n == NULL )
		return NULL;

	std::wstring strUni;
	strUni.append((const wchar_t*)n, env->GetStringLength(name));
	const wchar_t* names = strUni.c_str();
	long id = 0;
	HRESULT hr = p->GetIDsOfNames(IID_NULL, const_cast<wchar_t**>(&names), 1, NULL, &id);

	env->ReleaseStringChars(name, (const jchar*)n);
	
	if (hr == DISP_E_UNKNOWNNAME)
		ThrowComExceptionString(env, hr, name);
	else if( FAILED(hr) )
		ThrowComException(env, hr);

	return id;
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Dispatch_invoke
  (JNIEnv *env, jobject obj, jint dispid, jint flags, 
	jobjectArray args, jintArray nameids, jobject retval)
{
	IDispatch *p = (IDispatch*)env->GetIntField(obj, JAUT_Dispatch_Pointer);
	if( p == NULL )
	{
		ThrowJAutException(env, ERR_DispatchPointer);
		return;
	}

	DISPPARAMS params;

	if( args != NULL )
		params.cArgs = env->GetArrayLength(args);
	else
		params.cArgs = 0;

	if( params.cArgs > 0 )
	{
		params.rgvarg = (VARIANT*)CoTaskMemAlloc(params.cArgs * sizeof(VARIANT));
		if( params.rgvarg == NULL )
		{
			ThrowOutOfMemoryError(env);
			return;
		}

		for(unsigned int i = 0; i < params.cArgs; ++i)
		{
			jobject arg = env->GetObjectArrayElement(args, params.cArgs-1-i);

			VARIANT *v = NULL;
			if( arg != NULL )
				v = (VARIANT*)env->GetIntField(arg, JAUT_Variant_Pointer);

			if (v != NULL && v->vt == (VT_BSTR | VT_BYREF)) // FIXME: remove VT_BSTR and test
			{
				params.rgvarg[i] = *v;
				params.rgvarg[i].vt = VT_BSTR | VT_BYREF;
				//params.rgvarg
			} else if (v != NULL )
			{
				params.rgvarg[i] = *v;
				V_VT(v) = VT_EMPTY; //KMS: why this?
			}
			else
			{
				V_VT(&params.rgvarg[i]) = VT_ERROR;
				V_ERROR(&params.rgvarg[i]) = DISP_E_PARAMNOTFOUND;
			}

			env->DeleteLocalRef(arg);
		}
	}
	else
		params.rgvarg = NULL;

	if( nameids != NULL )
	{
		params.cNamedArgs = env->GetArrayLength(nameids);
		params.rgdispidNamedArgs = env->GetIntArrayElements(nameids, NULL);
	}
	else
	{
		params.cNamedArgs = 0;
		params.rgdispidNamedArgs = NULL;
	}

	VARIANT *varresult = NULL;
	if( retval != NULL )
		varresult = (VARIANT*)env->GetIntField(retval, JAUT_Variant_Pointer);

	unsigned int argerr = 0;

	EXCEPINFO excepinfo;
	HRESULT hr = p->Invoke(dispid, IID_NULL, NULL,
		(unsigned short)flags, &params, varresult, &excepinfo, &argerr);

	if( params.rgdispidNamedArgs != NULL )
		env->ReleaseIntArrayElements(nameids, params.rgdispidNamedArgs, JNI_ABORT);

	if( params.rgvarg != NULL && varresult == NULL || varresult->vt == VT_EMPTY )
	{
		for(unsigned int i = 0; i < params.cArgs; ++i)
		{
			jobject arg = env->GetObjectArrayElement(args, params.cArgs-1-i);

			VARIANT *v = NULL;
			if( arg != NULL )
				v = (VARIANT*)env->GetIntField(arg, JAUT_Variant_Pointer);

			if (v != NULL)
			{
				*v = params.rgvarg[i];
				params.rgvarg[i].vt = VT_EMPTY;
			}
			else
				VariantClear(&params.rgvarg[i]);

			env->DeleteLocalRef(arg);
		}

		CoTaskMemFree(params.rgvarg);
	}

	if( hr == DISP_E_EXCEPTION )
		ThrowInvokeException(env, &excepinfo);
	else if( FAILED(hr) )
		ThrowComException(env, hr);
}

}
