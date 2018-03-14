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
#include "ComException.h"

extern "C"
{

JNIEXPORT jstring JNICALL Java_org_isis_jaut_ComException_formatMessage
  (JNIEnv *env, jclass, jint hr)
{
	char *msg = NULL;

	// FIXME: use FormatMessageW and env->NewString
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, NULL, (char*)&msg, 0, NULL);
	
	if( msg == NULL )
		return NULL;

	jstring ret = env->NewStringUTF(msg);

	LocalFree(msg);

	return ret;
}

}
