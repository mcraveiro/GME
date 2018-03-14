#include "stdafx.h"
#include "helper.h"

//
// bool include_separator:
//	true : includes the separator in the token
//	false: does not include the separator character in the token
std::list<std::tstring> tokenizer( std::tstring m, TCHAR separator /*= '/' */, bool include_separator /* = true */)
{
	int offs = include_separator?0:1;
	std::list<std::tstring> r;
	std::tstring last;
	while( !m.empty())
	{
		int i = m.find(separator, 1);
		if( i == -1)
		{
			r.push_back( m);
			m = _T("");
		}
		else if( i != 0)
		{
			r.push_back( m.substr( 0, i));
			m = m.substr( i + offs);
		}
	}

	return r;
}

//
// does not include the separator character in the token
bool findExact( const std::tstring& m, const std::tstring& to_find)
{
	std::list< std::tstring> tokens = tokenizer( m, _T(' '), false);
	std::list< std::tstring>::iterator i = tokens.begin();
	std::list< std::tstring>::iterator e = tokens.end();
	while( i != e && (*i).compare( to_find) != 0)
		++i;

	return ( i != e); // false if not found
}

// it calculates nm1's name relatively to nm2
std::tstring makeRel( std::tstring& nm1, std::tstring& nm2)
{
	std::list<std::tstring> r1 = tokenizer( nm1);
	std::list<std::tstring> r2 = tokenizer( nm2);

	std::list<std::tstring>::iterator i1 = r1.begin();
	std::list<std::tstring>::iterator i2 = r2.begin();
	while( i1 != r1.end() && i2 != r2.end() && *i1 == *i2)
	{
		++i1;
		++i2;
	}

	std::tstring relpath;
	int count = 0; // how many levels in nm2
	while( i2 != r2.end()) ++i2, ++count, relpath +=_T("/@..");
	while( i1 != r1.end()) relpath +=*i1, ++i1;

	if( relpath == _T("")) // same name nm1 and nm2
	{
		ASSERT( nm1 == nm2);
		//#1 solution: to give back the path as it goes back to the topmost level
		// and then goes down again
		/*for( i2 = r2.begin(); i2 != r2.end(); ++i2) 
			relpath +="/@..";
		relpath += nm1;*/

		//#2 solution: to use a shorter form (only one step up): /@../name
		relpath = _T("/@..") + *r2.rbegin();
	}
	return relpath;
}

std::tstring makeViewable( const std::tstring& m)
{
	std::tstring res;
	std::list<std::tstring> r1 = tokenizer( m);
	std::list<std::tstring>::iterator i1 = r1.begin();
	while( i1 != r1.end())
	{
		std::tstring p = *i1;
		if( p.substr(0, 2) == _T("/@"))
			p = p.substr(2); // _T("/@")
		int pos = p.find(_T("|kind="));
		if( pos != -1) p = p.substr( 0, pos);

		res += _T('/') + p;
		++i1;
	}
	
	return res;
}


std::tstring makeNameViewable( const std::tstring& m)
{
	std::tstring res = m;
	if( res.substr(0, 2) == _T("/@"))
		res = res.substr(2);

	ASSERT( res.find( _T("/@")) == -1); //no more than one token

	int pos = res.find(_T("|kind="));
	if( pos != -1)
		res = res.substr( 0, pos);

	return res;
}

CComBSTR makeLink( CComObjPtr<IMgaObject> p_obj, const std::tstring& nm2, bool use_anyway_nm_2)
{
	CComBSTR bstr, id, nm;
	COMTHROW( p_obj->get_ID( &id));
	COMTHROW( p_obj->get_Name( &nm));
	if( nm == 0 || nm == _T("") || use_anyway_nm_2) // if called from the Start... (or just derived) then the name its not yet parsed
		nm = nm2.c_str();
	COMTHROW(bstr.Append(L"<A HREF=\"mga:"));
	COMTHROW(bstr.AppendBSTR( id));
	COMTHROW(bstr.Append(L"\">"));
	if( nm.Length() != 0)
		COMTHROW(bstr.AppendBSTR( nm));
	else
		COMTHROW(bstr.Append(L"emptyname"));
	COMTHROW(bstr.Append(L"</A>"));

	return bstr;
}

CComBSTR makeLink( CComObjPtr<IMgaFCO> p_fco, const std::tstring& nm_2, bool use_anyway_nm_2)
{
	return makeLink( CComObjPtr<IMgaObject>( p_fco), nm_2, use_anyway_nm_2);
}

CComBSTR makeLink( CComObjPtr<IMgaSet> p_fco, const std::tstring& nm_2, bool use_anyway_nm_2)
{
	return makeLink( CComObjPtr<IMgaObject>( p_fco), nm_2, use_anyway_nm_2);
}

CComBSTR makeLink( CComObjPtr<IMgaReference> p_fco, const std::tstring& nm_2, bool use_anyway_nm_2)
{
	return makeLink( CComObjPtr<IMgaObject>( p_fco), nm_2, use_anyway_nm_2);
}
