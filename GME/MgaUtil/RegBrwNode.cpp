// RegBrwNode.cpp: implementation of the CRegBrwNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mgautil.h"
#include "RegBrwNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegBrwNode::CRegBrwNode()
{
	handle = NULL;
	parent = NULL;
	status = ATTSTATUS_INVALID;
}

CRegBrwNode::~CRegBrwNode()
{

}
