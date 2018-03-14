#ifndef HELPER_H_259823495872394857
#define HELPER_H_259823495872394857

#include <list>
#include <string>

std::list<std::tstring> tokenizer( std::tstring m, TCHAR separator = '/', bool include_separator = true);

bool findExact( const std::tstring& m, const std::tstring& to_find);

std::tstring makeRel( std::tstring& nm, std::tstring& nm2);

std::tstring makeViewable( const std::tstring& m);

std::tstring makeNameViewable( const std::tstring& m);

CComBSTR makeLink( CComObjPtr<IMgaObject> obj, const std::tstring& nm_2 = _T(""), bool use_anyway_nm_2 = false);

CComBSTR makeLink( CComObjPtr<IMgaFCO> obj, const std::tstring& nm_2 = _T(""), bool use_anyway_nm_2 = false);
CComBSTR makeLink( CComObjPtr<IMgaReference> obj, const std::tstring& nm_2 = _T(""), bool use_anyway_nm_2 = false);
CComBSTR makeLink( CComObjPtr<IMgaSet> obj, const std::tstring& nm_2 = _T(""), bool use_anyway_nm_2 = false);


#endif // HELPER_H_259823495872394857

