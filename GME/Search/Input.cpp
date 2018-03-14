// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "ComHelp.h"
//#include "GMECOM.h"
//#include "ComponentConfig.h"
//#include "RawComponent.h"
#include "Input.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{
    
}

CInput::~CInput()
{

}

//Get the search criterias
void CInput::GetInput(CString strNameFirst, CString strRoleFirst, CString strKindFirst, CString strAttributeFirst,CString strNameSecond,CString strRoleSecond,CString strKindSecond,CString strAttributeSecond,
                      CString attrval, BOOL mod, BOOL atom, BOOL ref, BOOL set,BOOL connection,BOOL splSearch, BOOL full, 
                      IMgaFCO*, int, //WAS: IMgaFCO *root, int searchscope,
                      BOOL bMatchCase, int scopedSearch,int logicalExpr)
{
    //doScopedSearch = bScopedSearch;

    m_bGetCaseIgnored = !bMatchCase;
    if( m_bGetCaseIgnored)
    {
        strNameFirst.MakeLower();
        strRoleFirst.MakeLower();
        strKindFirst.MakeLower();
        strAttributeFirst.MakeLower();

        //attrval seems not used
        attrval.MakeLower();
        //addded
        strNameSecond.MakeLower();
        strRoleSecond.MakeLower();
        strKindSecond.MakeLower();
        strAttributeSecond.MakeLower();
    }
    
    m_bFull=full;

    //get regular expression forms
    m_regNameFirst = GetRegExp(strNameFirst);
    m_regRoleFirst = GetRegExp(strRoleFirst);
    m_regKindFirst = GetRegExp(strKindFirst);

    m_regNameSecond=GetRegExp(strNameSecond);
    m_regKindSecond=GetRegExp(strKindSecond);
    m_regRoleSecond=GetRegExp(strRoleSecond);

    //get string forms

    //objVal = attrval;
    //objattribName = strAttributeFirst;
    m_strNameFirst=strNameFirst;
    m_strKindFirst=strKindFirst;
    m_strRoleFirst=strRoleFirst;
    m_strAttributeFirst=strAttributeFirst;

    m_strNameSecond=strNameSecond;
    m_strKindSecond=strKindSecond;
    m_strRoleSecond=strRoleSecond;
    m_strAttributeSecond=strAttributeSecond;
   

    //check if nothing is present in the second
    if(strNameSecond.Trim()==_T("") && strKindSecond.Trim()==_T("")&& strRoleSecond.Trim()==_T("") && strAttributeSecond.Trim()==_T(""))
    {
        m_bDoSecond=FALSE;
    }
    else m_bDoSecond=TRUE;
    //getSplSearch = spl;

    m_bGetModels = mod;
    m_bGetAtoms = atom;
    m_bGetReferences = ref;
    m_bGetSets = set;
    m_bGetConnections=connection;
    m_bGetSplSearch=splSearch;
    m_intScope=scopedSearch;
    m_intLogical=logicalExpr;
    ParseAttribute();
}

//Parses attribute expressions and keeps it in stack
//Two stacks are used for both lhs and rhs search criteria
void CInput::ParseAttribute()
{
    //prepare the stack containing expressions and operations for first attribute
    PrepareExpressionStack(m_strAttributeFirst,m_stackExpressionFirst);

    //prepare the stack containing expressions and operations for second attribute
    PrepareExpressionStack(m_strAttributeSecond,m_stackExpressionSecond);
}

void CInput::PrepareExpressionStack(const CString &atrAttributeExpression,std::vector<Attribute>& stack)
{
    //strOperation stores operators like &, |
    //strUnitExpression is the expression like age> 30, i.e. it
    //is a complete single expression without logical operator
    CString strOperation,strUnitExpression; 

    CString strAttributeExpression(atrAttributeExpression);

    //get the index of logical operator
    int index= strAttributeExpression.FindOneOf(_T("&|"));

    //if no logical operator was found push the whole expression
    if(index==-1 && strAttributeExpression!=_T(""))
    {
        stack.push_back(Attribute(strAttributeExpression.Trim()));
    }
   
    while(index!=-1)
    {
        //extract the operator
        strOperation=strAttributeExpression[index];

        //extract a single expression, the string on left of the index is a complete
        //expression
        AfxExtractSubString(strUnitExpression,strAttributeExpression,0,strAttributeExpression[index]);

        //put the expression on stack
        stack.push_back(Attribute(strUnitExpression.Trim()));

        //push the operator either &, |
        stack.push_back(Attribute(CString(strOperation)));

        //extract the remaining string on the right of index
        strAttributeExpression=strAttributeExpression.Right(strAttributeExpression.GetLength()-index-1);
        
        //again find the logical operator
        index= strAttributeExpression.FindOneOf(_T("&|"));

        //if no more found treat all remaining string as expression
        if(index==-1) stack.push_back(Attribute(strAttributeExpression.Trim()));
    }
}

//Obtain regular expression form of the string
//supplied
wregex CInput::GetRegExp(CString name)
{
    CString temp=_T("");
    if(name.IsEmpty())
    {
        return wregex(name);
    }
    if(m_bFull)
    {
        temp=_T("^");
        temp+=name;
        temp+=_T("$");
        return wregex(temp);
    }

    return wregex(name);
}
