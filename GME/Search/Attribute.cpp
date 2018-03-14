#include "stdafx.h"
#include "Attribute.h"


Attribute::Attribute(void):eval(FALSE)
{
}
Attribute::Attribute(CString &strInput):eval(FALSE)
{
    if(strInput==_T("&") ||strInput==_T("|"))
    {
        name =strInput;
        return;
    }
    name=strInput;

    //parse the attribute expressions for operators and operands
    //keep single lettered operators first
    Parse(strInput,_T("<"));
    Parse(strInput,_T(">"));
    Parse(strInput,_T("="));
    Parse(strInput,_T(">="));
    Parse(strInput,_T("<="));
    Parse(strInput,_T("!="));
 
}

void Attribute::Parse(const CString& strInput,const CString& operation)
{
    CString strTemp(strInput);
    int index=strTemp.Find(operation,0);
    //CString token=temp.Tokenize(_T("<="),index);
    if(index!=-1)
    {
        name=strTemp.Left(index).Trim();
        this->operation=operation;
        value=strTemp.Right(strTemp.GetLength()-index-operation.GetLength()).Trim();
        return;
    }
    
}

Attribute::~Attribute(void)
{
}

BOOL Attribute::Compare(CString& rhs,int type)
{
    switch(type)
	{
		case 0: 
            return _tcscmp(value,rhs);
            
		case 1: 
            return CheckInteger(_ttoi(value),_ttoi(rhs));
            
		case 2: 
            return CheckDouble(_ttof(value),_ttof(rhs));
            break;
		case 3: 
            return value==rhs;
            break;
		default:	
            break;
	}
    return TRUE;
}

//compare the integer values based on operator it has
BOOL Attribute::CheckInteger(int lhs,int rhs)
{
    if(operation==_T(">="))
    {
        return lhs>=rhs;
    }
    if(operation==_T("<="))
    {
        return lhs<=rhs;
    }
    if(operation==_T("!="))
    {
        return lhs!=rhs;
    }
    if(operation==_T("="))
    {
        return lhs==rhs;
    }
    if(operation==_T(">"))
    {
        return lhs>rhs;
    }
    if(operation==_T("<"))
    {
        return lhs<rhs;
    }
    return TRUE;
}

//Compare the double values best on operator it has
BOOL Attribute::CheckDouble(double lhs,double rhs)
{
    if(operation==_T(">="))
    {
        return lhs>=rhs;
    }
    if(operation==_T("<="))
    {
        return lhs<=rhs;
    }
    if(operation==_T("!="))
    {
        return lhs!=rhs;
    }
    if(operation==_T("="))
    {
        return lhs==rhs;
    }
    if(operation==_T(">"))
    {
        return lhs>rhs;
    }
    if(operation==_T("<"))
    {
        return lhs<rhs;
    }
    return TRUE;
}

BOOL Attribute::CheckBool(BOOL lhs,BOOL rhs)
{
    if(operation==_T("="))
    {
        return lhs==rhs;
    }
    else if(operation==_T("!="))
    {
        return lhs!=rhs;
    }
    return FALSE;
}

BOOL Attribute::CheckString(BOOL lhs)
{
    if(operation==_T("="))
        return lhs;
    else if(operation==_T("!="))
        return !lhs;
    return FALSE;
}

BOOL Attribute::LogicalCompare(BOOL lhs,const Attribute & operation,BOOL rhs)
{
    if(operation.name==_T("&"))
    {
        return lhs&&rhs;
    }
    if(operation.name==_T("|"))
    {
        return lhs||rhs;
    }
    return FALSE;
}

std::tr1::wregex Attribute::GetRegExp(CString& str,BOOL full)
{
    CString strTemp;
    if(full)
    {
        strTemp.Append(_T("^"));
        strTemp.Append(str);
        strTemp.Append(_T("$"));

    }
    else
        strTemp=str;

    return std::tr1::wregex(strTemp);
}
