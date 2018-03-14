// RegBrwNode.h: interface for the CRegBrwNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGBRWNODE_H__BB47535E_08D3_4306_8BCC_B453A54EE26D__INCLUDED_)
#define AFX_REGBRWNODE_H__BB47535E_08D3_4306_8BCC_B453A54EE26D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CRegBrwNode;

class CRegBrwNode  
{
public:
	long status;
	void* handle;
	CRegBrwNode* parent;
	CString value;
	CString name;
	CString path;
	CRegBrwNode();
	virtual ~CRegBrwNode();

};

#endif // !defined(AFX_REGBRWNODE_H__BB47535E_08D3_4306_8BCC_B453A54EE26D__INCLUDED_)
