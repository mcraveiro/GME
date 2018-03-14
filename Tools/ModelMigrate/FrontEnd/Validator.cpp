#include "StdAfx.h"
#include ".\validator.h"
#include "Extractor.h"
#include "ScriptAbs.h"
#include "ScriptCmplx.h"

Validator::Validator(void)
{
}

Validator::~Validator(void)
{
}

void Validator::job( LPCTSTR lpszPathNameIn, LPCTSTR lpszPathNameOut)
{
	Extractor extr;

	std::string fName( lpszPathNameIn);
	size_t dpos = fName.rfind('.');
	if( dpos != std::string::npos && fName.substr( dpos).compare( ".parsed") == 0) // '.' found, and ends with '.parsed'
		extr.doPlainLoad( fName);
	else
		extr.doJob( fName);
	
	ScriptAbs* scr = new ScriptCmplx();

	scr->gen( extr.getKinds(), extr.getAttrs(), lpszPathNameOut);
	delete scr;
}
