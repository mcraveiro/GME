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
#include "Variant.h"
#include <atlsafe.h>

extern "C"
{

VARTYPE GetJavaVartype(VARTYPE vt)
{
	int a = vt & VT_TYPEMASK;
	int b = vt & ~VT_TYPEMASK;

	if( b != (vt & (VT_ARRAY|VT_BYREF)) )
		return VT_ILLEGAL;

	if( a == VT_EMPTY || a == VT_NULL )
	{
		if( b != 0 )
			return VT_ILLEGAL;
	}
	else if( a == VT_VARIANT )
	{
		if( (b & VT_BYREF) == 0 )
			return VT_ILLEGAL;
	}
	else if( a == VT_BOOL || a == VT_BSTR || a== VT_R4 
		|| a == VT_DISPATCH || a == VT_UNKNOWN || a == VT_CY )
		;
	else if( a == VT_UI1 || a == VT_I1 )
		a = VT_UI1;
	else if( a == VT_I2 || a == VT_UI2 )
		a = VT_I2;
	else if( a == VT_I4 || a == VT_UI4 || a == VT_ERROR || a == VT_INT || a == VT_UINT )
		a = VT_I4;
	else if( a == VT_R8 || a == VT_DATE )
		a = VT_R8;
	else
		return VT_ILLEGAL;

	return a | b;
}

JNIEXPORT jint JNICALL Java_org_isis_jaut_Variant_getVartype
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return NULL;
	}

	return V_VT(v);
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_changeType
  (JNIEnv *env, jobject obj, jobject dest, jint flgs, jint vt)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
		ThrowJAutException(env, ERR_VariantPointer);

	VARIANT *d = NULL;
	if( dest != NULL )
		d = (VARIANT*)env->GetIntField(dest, JAUT_Variant_Pointer);
	if( d == NULL )
		d = v;

	HRESULT hr = VariantChangeType(d, v, (unsigned short)flgs, (VARTYPE)vt);
	if( FAILED(hr) )
		ThrowComException(env, hr);
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_clear
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
		ThrowJAutException(env, ERR_VariantPointer);

	HRESULT hr = VariantClear(v);
	if( FAILED(hr) )
		ThrowComException(env, hr);
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_copy
  (JNIEnv *env, jobject obj, jobject dest)
{
	if( dest == NULL )
	{
		ThrowNullPointerException(env);
		return;
	}

	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	VARIANT *d = (VARIANT*)env->GetIntField(dest, JAUT_Variant_Pointer);
	if( v == NULL || d == NULL )
		ThrowJAutException(env, ERR_VariantPointer);

	HRESULT hr = VariantCopy(d, v);
	if( FAILED(hr) )
		ThrowComException(env, hr);
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_copyInd
  (JNIEnv *env, jobject obj, jobject dest)
{
	if( dest == NULL )
	{
		ThrowNullPointerException(env);
		return;
	}

	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	VARIANT *d = (VARIANT*)env->GetIntField(dest, JAUT_Variant_Pointer);
	if( v == NULL || d == NULL )
		ThrowJAutException(env, ERR_VariantPointer);

	HRESULT hr = VariantCopyInd(d, v);
	if( FAILED(hr) )
		ThrowComException(env, hr);
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_allocate
  (JNIEnv *env, jobject obj, jint ivt)
{
	VARTYPE vt = (VARTYPE)ivt;

	if( (GetJavaVartype(vt) & ~VT_TYPEMASK) != 0 )
	{
		ThrowJAutException(env, ERR_InvalidType);
		return;
	}
	
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		v = (VARIANT*)CoTaskMemAlloc(sizeof(VARIANT));
        //printf("variant allocated at: %x\n", v );
		if( v == NULL )
		{
			ThrowOutOfMemoryError(env);
			return;
		}

		VariantInit(v);

		env->SetIntField(obj, JAUT_Variant_Pointer, (jint)v);
	}
	else
	{
		HRESULT hr = VariantClear(v);
		if( FAILED(hr) )
		{
			ThrowComException(env, hr);
			return;
		}
	}

	V_VT(v) = vt;
//xxx
	if( vt == VT_UNKNOWN || vt == VT_DISPATCH || vt & VT_BSTR )
		V_UNKNOWN(v) = NULL;
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_release
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v != NULL )
	{
		HRESULT hr = VariantClear(v);
		if( FAILED(hr) )
		{
			ThrowComException(env, hr);
			return;
		}

		CoTaskMemFree(v);

		env->SetIntField(obj, JAUT_Variant_Pointer, 0);
	}
}

JNIEXPORT jboolean JNICALL Java_org_isis_jaut_Variant_getBoolean
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return NULL;
	}

	VARIANT_BOOL b;

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_BOOL )
		b = V_BOOL(v);
	else if( vt == (VT_BOOL|VT_BYREF) )
		b =  *V_BOOLREF(v);
	else
	{
		ThrowJAutException(env, ERR_InvalidType);
		return NULL;
	}

	return (b == VARIANT_FALSE) ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_setBoolean
  (JNIEnv *env, jobject obj, jboolean val)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return;
	}

	VARIANT_BOOL b = (val == JNI_FALSE) ? VARIANT_FALSE : VARIANT_TRUE;

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_BOOL )
		V_BOOL(v) = b;
	else if( vt == (VT_BOOL|VT_BYREF) )
		*V_BOOLREF(v) = b;
	else
		ThrowJAutException(env, ERR_InvalidType);
}


JNIEXPORT jbyte JNICALL Java_org_isis_jaut_Variant_getByte
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return NULL;
	}

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_UI1 )
		return V_UI1(v);
	else if( vt == (VT_UI1|VT_BYREF) )
		return *V_UI1REF(v);
	else
	{
		ThrowJAutException(env, ERR_InvalidType);
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_setByte
  (JNIEnv *env, jobject obj, jbyte val)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return;
	}

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_UI1 )
		V_UI1(v) = val;
	else if( vt == (VT_UI1|VT_BYREF) )
		*V_UI1REF(v) = val;
	else
		ThrowJAutException(env, ERR_InvalidType);
}


JNIEXPORT jshort JNICALL Java_org_isis_jaut_Variant_getShort
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return NULL;
	}

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_I2 )
		return V_I2(v);
	else if( vt == (VT_I2|VT_BYREF) )
		return *V_I2REF(v);
	else
	{
		ThrowJAutException(env, ERR_InvalidType);
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_setShort
  (JNIEnv *env, jobject obj, jshort val)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return;
	}

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_I2 )
		V_I2(v) = val;
	else if( vt == (VT_I2|VT_BYREF) )
		*V_I2REF(v) = val;
	else
		ThrowJAutException(env, ERR_InvalidType);
}

JNIEXPORT jint JNICALL Java_org_isis_jaut_Variant_getInt
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return NULL;
	}

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_I4 )
		return V_I4(v);
	else if( vt == (VT_I4|VT_BYREF) )
		return *V_I4REF(v);
	else
	{
		ThrowJAutException(env, ERR_InvalidType);
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_setInt
  (JNIEnv *env, jobject obj, jint val)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return;
	}

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_I4 )
		V_I4(v) = val;
	else if( vt == (VT_I4|VT_BYREF) )
		*V_I4REF(v) = val;
	else
		ThrowJAutException(env, ERR_InvalidType);
}

#define TOJLONG(XXX) (*(jlong*)&(XXX))

JNIEXPORT jlong JNICALL Java_org_isis_jaut_Variant_getLong
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return NULL;
	}

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_CY )
		return TOJLONG(V_CY(v));
	else if( vt == (VT_CY|VT_BYREF) )
		return TOJLONG(V_CYREF(v));
	else
	{
		ThrowJAutException(env, ERR_InvalidType);
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_setLong
  (JNIEnv *env, jobject obj, jlong val)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return;
	}

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_CY )
		TOJLONG(V_CY(v)) = val;
	else if( vt == (VT_CY|VT_BYREF) )
		TOJLONG(V_CYREF(v)) = val;
	else
		ThrowJAutException(env, ERR_InvalidType);
}

JNIEXPORT jfloat JNICALL Java_org_isis_jaut_Variant_getFloat
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return NULL;
	}

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_R4 )
		return V_R4(v);
	else if( vt == (VT_R4|VT_BYREF) )
		return *V_R4REF(v);
	else
	{
		ThrowJAutException(env, ERR_InvalidType);
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_setFloat
  (JNIEnv *env, jobject obj, jfloat val)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return;
	}

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_R4 )
		V_R4(v) = val;
	else if( vt = (VT_R4|VT_BYREF) )
		*V_R4REF(v) = val;
	else
		ThrowJAutException(env, ERR_InvalidType);
}

JNIEXPORT jdouble JNICALL Java_org_isis_jaut_Variant_getDouble
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return NULL;
	}

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_R8 )
		return V_R8(v);
	else if( vt == (VT_R8|VT_BYREF) )
		return *V_R8REF(v);
	else
	{
		ThrowJAutException(env, ERR_InvalidType);
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_setDouble
  (JNIEnv * env, jobject obj, jdouble val)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return;
	}

	VARTYPE vt = GetJavaVartype(V_VT(v));
	if( vt == VT_R8 )
		V_R8(v) = val;
	else if( vt == (VT_R8|VT_BYREF) )
		*V_R8REF(v) = val;
	else
		ThrowJAutException(env, ERR_InvalidType);
}

JNIEXPORT jstring JNICALL Java_org_isis_jaut_Variant_getString
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return NULL;
	}

	BSTR b;
	if( V_VT(v) == VT_BSTR )
	    b = V_BSTR(v);
	else if( V_VT(v) == (VT_BSTR|VT_BYREF) )
		b = *V_BSTRREF(v);
	else
	{
		ThrowJAutException(env, ERR_InvalidType);
		return NULL;
	}

    return env->NewString((const jchar*)b, SysStringLen(b));
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_setString
  (JNIEnv *env, jobject obj, jstring val)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return;
	}

	BSTR *b;
	if( V_VT(v) == VT_BSTR )
		b = &V_BSTR(v);
	else if( V_VT(v) == (VT_BSTR|VT_BYREF) )
		b = V_BSTRREF(v);
	else
	{
		ThrowJAutException(env, ERR_InvalidType);
		return;
	}

	if( *b != NULL )
	{
		SysFreeString(*b);
		*b = NULL;
	}

	jsize l = env->GetStringLength(val);
	if( l <= 0 )
		return;

	const jchar *s = NULL;
	if( val != NULL )
		s = env->GetStringChars(val, NULL);
	if( s != NULL )
	{
		*b = SysAllocStringLen((const OLECHAR*)s, l);
		env->ReleaseStringChars(val, s);
	}
}

JNIEXPORT jarray JNICALL Java_org_isis_jaut_Variant_getStringArray
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return NULL;
	}

	SAFEARRAY* parray = NULL;
	if( V_VT(v) == (VT_BSTR | VT_ARRAY))
		parray = v->parray;
	//else if( V_VT(v) == (VT_BSTR|VT_BYREF) )
	//	b = *V_BSTRREF(v);

	ATL::CComSafeArray<BSTR> array;
	if (parray == NULL || parray->cDims != 1 || FAILED(array.Attach(parray)))
	{
		ThrowJAutException(env, ERR_InvalidType);
		return NULL;
	}
	jclass stringClass = env->FindClass("java/lang/String");
	long size = array.GetUpperBound(0) - array.GetLowerBound(0) + 1;
	jobjectArray jarray = env->NewObjectArray(size, stringClass, 0);
    for (jsize i = 0; i + array.GetLowerBound(0) <= array.GetUpperBound(); ++i) {
		BSTR element = array.GetAt(i + array.GetLowerBound(0));
		env->SetObjectArrayElement(jarray, i, env->NewString((jchar*)element, SysStringLen(element)));
    }
	array.Detach();

    return jarray;
}


JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_setDispatch
  (JNIEnv *env, jobject obj, jobject val)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return;
	}

	IDispatch **q;

	VARTYPE vt = V_VT(v);
	if( vt == VT_DISPATCH || vt == VT_UNKNOWN )
		q = &V_DISPATCH(v);
	else if( vt == (VT_DISPATCH|VT_BYREF) || vt == (VT_UNKNOWN|VT_BYREF) )
		q = V_DISPATCHREF(v);
	else
	{
		ThrowJAutException(env, ERR_InvalidType);
		return;
	}

	IDispatch *p = NULL;
	if( val != NULL )
	{
		p = (IDispatch*)env->GetIntField(val, JAUT_Dispatch_Pointer);
		if( p != NULL )
			p->AddRef();
	}

	if( (*q) != NULL )
		(*q)->Release();

	*q = p;
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_allocateArray
  (JNIEnv *env, jobject obj, jint ivt, jintArray lbounds, jintArray elements)
{
	if( lbounds == NULL || elements == NULL )
	{
		ThrowNullPointerException(env);
		return;
	}

	VARTYPE vt = (VARTYPE)ivt;

	if( (GetJavaVartype(vt) & ~VT_TYPEMASK) != 0 || vt == VT_EMPTY || vt == VT_NULL )
	{
		ThrowJAutException(env, ERR_InvalidType);
		return;
	}
	
	jsize dim = env->GetArrayLength(lbounds);
	if( dim < 0 || env->GetArrayLength(elements) != dim )
	{
		ThrowJAutException(env, ERR_InvalidBounds);
		return;
	}

	jint *lb = env->GetIntArrayElements(lbounds, NULL);
	jint *el = env->GetIntArrayElements(elements, NULL);
	SAFEARRAYBOUND *bounds = (SAFEARRAYBOUND*)CoTaskMemAlloc(sizeof(SAFEARRAYBOUND) * dim);
	if( lb == NULL || el == NULL || bounds == NULL )
	{
		if( lb != NULL )
			env->ReleaseIntArrayElements(lbounds, lb, JNI_ABORT);

		if( el != NULL )
			env->ReleaseIntArrayElements(elements, el, JNI_ABORT);

		if( bounds != NULL )
			CoTaskMemFree(bounds);

		ThrowOutOfMemoryError(env);
		return;
	}

	bool wrong = false;
	for(int i = 0; i < dim; ++i)
	{
		wrong |= (el[i] < 0);

		bounds[i].lLbound = lb[i];
		bounds[i].cElements = el[i];
	}

	env->ReleaseIntArrayElements(lbounds, lb, JNI_ABORT);
	env->ReleaseIntArrayElements(elements, el, JNI_ABORT);

	if( wrong )
	{
		CoTaskMemFree(bounds);

		ThrowJAutException(env, ERR_InvalidBounds);
		return;
	}

	SAFEARRAY *sa = SafeArrayCreate(vt, dim, bounds);

	CoTaskMemFree(bounds);

	if( sa == NULL )
	{
		ThrowOutOfMemoryError(env);
		return;
	}

	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		v = (VARIANT*)CoTaskMemAlloc(sizeof(VARIANT));
		if( v == NULL )
		{
			// disregard possible errors
			SafeArrayDestroy(sa);

			ThrowOutOfMemoryError(env);
			return;
		}

		VariantInit(v);

		env->SetIntField(obj, JAUT_Variant_Pointer, (jint)v);
	}
	else
	{
		HRESULT hr = VariantClear(v);
		if( FAILED(hr) )
		{
			// disregard possible errors
			SafeArrayDestroy(sa);

			ThrowComException(env, hr);
			return;
		}
	}

	V_VT(v) = vt|VT_ARRAY;
	V_ARRAY(v) = sa;
}

JNIEXPORT jobject JNICALL Java_org_isis_jaut_Variant_accessArray
  (JNIEnv *env, jobject obj)
{
	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return NULL;
	}

	SAFEARRAY *p;

	VARTYPE vt = V_VT(v) & (VT_ARRAY|VT_BYREF);
	if( vt = VT_ARRAY )
		p = V_ARRAY(v);
	else if( vt == (VT_ARRAY|VT_BYREF) )
	{
		p = *V_ARRAYREF(v);
	}
	else
	{
		ThrowJAutException(env, ERR_InvalidType);
		return NULL;
	}

	return NULL;
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_allocateReference__ILorg_isis_jaut_Variant_2
  (JNIEnv *env, jobject obj, jint itype, jobject target)
{
    //printf("Jaut.dll: Java_org_isis_jaut_Variant_allocateReference__ILorg_isis_jaut_Variant_2 called\n" );

	if( target == NULL )
	{
		ThrowNullPointerException(env);
		return;
	}

	VARTYPE type = (VARTYPE)itype;

	VARIANT *w = (VARIANT*)env->GetIntField(target, JAUT_Variant_Pointer);
	if( w == NULL )
	{
		ThrowJAutException(env, ERR_VariantPointer);
		return;
	}

	VARIANT *v = (VARIANT*)env->GetIntField(obj, JAUT_Variant_Pointer);
	if( v == NULL )
	{
		v = (VARIANT*)CoTaskMemAlloc(sizeof(VARIANT));
		if( v == NULL )
		{
			ThrowOutOfMemoryError(env);
			return;
		}

		VariantInit(v);

		env->SetIntField(obj, JAUT_Variant_Pointer, (jint)v);
	}
	else
	{
		HRESULT hr = VariantClear(v);
		if( FAILED(hr) )
		{
			ThrowComException(env, hr);
			return;
		}
	}

	VARTYPE wt = GetJavaVartype(V_VT(w));
	VARTYPE vt = GetJavaVartype(type);

	if( vt == (VT_BOOL|VT_BYREF) && wt == VT_BOOL )
		V_BOOLREF(v) = &V_BOOL(w);
	else if( vt == (VT_BOOL|VT_BYREF) && wt == (VT_BOOL|VT_BYREF) )
		V_BOOLREF(v) = V_BOOLREF(w);
	else if( vt == (VT_UI1|VT_BYREF) && wt == VT_UI1 )
		V_UI1REF(v) = &V_UI1(w);
	else if( vt == (VT_UI1|VT_BYREF) && wt == (VT_UI1|VT_BYREF) )
		V_UI1REF(v) = V_UI1REF(w);
	else if( vt == (VT_I2|VT_BYREF) && wt == VT_I2 )
		V_I2REF(v) = &V_I2(w);
	else if( vt == (VT_I2|VT_BYREF) && wt == (VT_I2|VT_BYREF) )
		V_I2REF(v) = V_I2REF(w);
	else if( vt == (VT_I4|VT_BYREF) && wt == VT_I4 )
		V_I4REF(v) = &V_I4(w);
	else if( vt == (VT_I4|VT_BYREF) && wt == (VT_I4|VT_BYREF) )
		V_I4REF(v) = V_I4REF(w);
	else if( vt == (VT_CY|VT_BYREF) && wt == VT_CY )
		V_CYREF(v) = &V_CY(w);
	else if( vt == (VT_CY|VT_BYREF) && wt == (VT_CY|VT_BYREF) )
		V_CYREF(v) = V_CYREF(w);
	else if( vt == (VT_R4|VT_BYREF) && wt == VT_R4 )
		V_R4REF(v) = &V_R4(w);
	else if( vt == (VT_R4|VT_BYREF) && wt == (VT_R4|VT_BYREF) )
		V_R4REF(v) = V_R4REF(w);
	else if( vt == (VT_R8|VT_BYREF) && wt == VT_R8 )
		V_R8REF(v) = &V_R8(w);
	else if( vt == (VT_R8|VT_BYREF) && wt == (VT_R8|VT_BYREF) )
		V_R8REF(v) = V_R8REF(w);
	else if( vt == (VT_BSTR|VT_BYREF) && wt == VT_BSTR )
		V_BSTRREF(v) = &V_BSTR(w);
	else if( vt == (VT_BSTR|VT_BYREF) && wt == (VT_BSTR|VT_BYREF) )
		V_BSTRREF(v) = V_BSTRREF(w);
	else if( (vt == (VT_DISPATCH|VT_BYREF) && wt == VT_DISPATCH) 
			|| (vt == (VT_UNKNOWN|VT_BYREF) && wt == VT_DISPATCH) 
			|| (vt == (VT_UNKNOWN|VT_BYREF) && wt == VT_UNKNOWN) )
		V_UNKNOWNREF(v) = &V_UNKNOWN(w);
	else if( (vt == (VT_DISPATCH|VT_BYREF) && wt == (VT_DISPATCH|VT_BYREF)) 
			|| (vt == (VT_UNKNOWN|VT_BYREF) && wt == (VT_DISPATCH|VT_BYREF)) 
			|| (vt == (VT_UNKNOWN|VT_BYREF) && wt == (VT_UNKNOWN|VT_BYREF)) )
		V_UNKNOWNREF(v) = V_UNKNOWNREF(w);
	else if( vt == (VT_VARIANT|VT_BYREF) && wt != (VT_VARIANT|VT_BYREF) )
		V_VARIANTREF(v) = w;
	else if( vt == (VT_VARIANT|VT_BYREF) && wt == (VT_VARIANT|VT_BYREF) )
		V_VARIANTREF(v) = V_VARIANTREF(w);
	else
	{
		ThrowJAutException(env, ERR_InvalidType);
		return;
	}
	
	V_VT(v) = type;
}

JNIEXPORT void JNICALL Java_org_isis_jaut_Variant_allocateReference__ILorg_isis_jaut_SafeArray_2I
  (JNIEnv *env, jobject obj, jint type, jobject array, jint index)
{
}

}

