//#include "stdafx.h"

#define __msxml_h__
struct IXMLElement;
#import <msxml6.dll> no_namespace

#include <Windows.h>
#include "CMgaXslt.h"             

#include "..\IDLComp\GMEIDLs_h.h" 

#include <stdio.h>
#include <tchar.h>
#include <comdef.h>
#include <new>


_bstr_t GetErrorInfo(HRESULT hr)
{
	LPWSTR errorText = NULL;
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER |FORMAT_MESSAGE_IGNORE_INSERTS,  
		NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errorText, 0, NULL);
	if (errorText != NULL) {
		_bstr_t ret = errorText;
		LocalFree(errorText);
		return ret;
	}
	return _bstr_t(L"Unknown error");
}

struct XslException {
	HRESULT hr;
	_bstr_t message;
	explicit XslException(HRESULT hr, _bstr_t message)
		: hr(hr), message(message)
	{}
};

#define CHK_HR(stmt) do { HRESULT hr = (stmt); if (FAILED(hr)) throw XslException(hr, GetErrorInfo(hr)); } while(0)
#define CHK_ALLOC(p) do { if (!(p)) { throw XslException(E_OUTOFMEMORY, L"Out of memory"); } } while(0)

HRESULT VariantFromString(PCWSTR wszValue, VARIANT &Variant)
{
    HRESULT hr = S_OK;
    BSTR bstrString = SysAllocString(wszValue);
    CHK_ALLOC(bstrString);

    V_VT(&Variant)   = VT_BSTR;
    V_BSTR(&Variant) = bstrString;

    return hr;
}

void CreateAndInitDOM(IXMLDOMDocument **ppDoc)
{
    HRESULT hr = CoCreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ppDoc));
	if (FAILED(hr))
	{
		throw XslException(hr, GetErrorInfo(hr));
	}
    (*ppDoc)->async = VARIANT_FALSE;
    (*ppDoc)->validateOnParse = VARIANT_FALSE;
    (*ppDoc)->resolveExternals = VARIANT_FALSE;
	IXMLDOMDocument2Ptr dom2 = (*ppDoc);
	dom2->setProperty(L"ProhibitDTD", VARIANT_FALSE);
}

HRESULT LoadXMLFile(IXMLDOMDocument *pXMLDom, LPCWSTR lpszXMLFile)
{
    HRESULT hr = S_OK;
    VARIANT_BOOL varStatus;
    _variant_t varFileName;
    IXMLDOMParseErrorPtr pXMLErr;
    
    CHK_HR(VariantFromString(lpszXMLFile, varFileName));
    varStatus = pXMLDom->load(varFileName);

    if(varStatus != VARIANT_TRUE)
    {
		_bstr_t bstrErr = pXMLDom->parseError->reason;
		wchar_t error[512];
        swprintf_s(error, L"Failed to load %s:\n%s\n", lpszXMLFile, static_cast<const wchar_t*>(bstrErr));
		throw XslException(E_FAIL, error);
    }

    return hr;
}

HRESULT TransformDOM2Obj(IXMLDOMDocument *pXMLDom, IXMLDOMDocument *pXSLDoc, wchar_t* outputFilename)
{
    HRESULT hr = S_OK;
    _bstr_t bstrXML;
    IXMLDOMDocumentPtr pXMLOut;
    IDispatchPtr pDisp;
    _variant_t varFileName;

    CreateAndInitDOM(&pXMLOut);
    CHK_HR(pXMLOut->QueryInterface(IID_IDispatch, (void**)&pDisp));

	_variant_t varXMLOut((IDispatch*)pDisp);

    pXMLDom->transformNodeToObject(pXSLDoc, varXMLOut);
	// pXMLOut->get_xml(bstrXML.GetAddress());

    CHK_HR(VariantFromString(outputFilename, varFileName));
    pXMLOut->save(varFileName);

    return hr;
}

void CXslt::doNativeXslt(LPCTSTR xsltFilename, LPCTSTR inputXmlFilename, LPCTSTR outputXmlFilename, _bstr_t& resError)
{
	try
	{
		IXMLDOMDocumentPtr pXMLDom;
		IXMLDOMDocumentPtr pXSLDoc;

		CreateAndInitDOM(&pXMLDom);
		CHK_HR(LoadXMLFile(pXMLDom, _bstr_t(inputXmlFilename)));
		CreateAndInitDOM(&pXSLDoc);
		CHK_HR(LoadXMLFile(pXSLDoc, _bstr_t(xsltFilename)));

		CHK_HR(TransformDOM2Obj(pXMLDom, pXSLDoc, _bstr_t(outputXmlFilename)));
	}
	catch (_com_error& err)
	{
		if (err.Description().length())
			resError = err.Description();
		else
			resError = L"Unknown error";
	}
	catch (XslException& err)
	{
		if (err.message.length())
			resError = err.message;
		else
			resError = L"Unknown error";
	}
}
