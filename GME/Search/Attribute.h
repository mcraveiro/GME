#pragma once

#include <regex>

/*
This class represents an attribute expression
*/
class Attribute
{
public:
    Attribute(void);
    Attribute(CString& str);
    ~Attribute(void);
    BOOL Compare(CString &rhs,int type);
    BOOL LogicalCompare(BOOL lhs,const Attribute & oper,BOOL rhs);
    std::tr1::wregex GetRegExp(CString& str,BOOL full);
    BOOL CheckInteger(int lhs,int rhs);
    BOOL CheckDouble(double lhs,double rhs);
    BOOL CheckBool(BOOL lhs,BOOL rhs);
    BOOL CheckString(BOOL lhs);
    BOOL IsOperator(){ return name=="&" || name=="|";    }
private:
    void Parse(const CString& inputString,const CString& str);
public:
    //name of the attribute
    CString name;

    //operator can be <=, >= etc
    CString operation;

    //represents value
    CString value;

    //the evaluation result defaulted to false
    BOOL eval;
};
