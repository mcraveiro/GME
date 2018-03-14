
#include "stdafx.h"
#include "GmeDocTemplate.h"

CGmeDocTemplate::CGmeDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
	CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
	: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

CDocument* CGmeDocTemplate::OpenDocumentFile(LPCTSTR /*lpszPathName*/, BOOL /*bMakeVisible*/)
{
	CDocument* pDocument = CreateNewDocument();
	if (pDocument == NULL)
	{
		TRACE0("CDocTemplate::CreateNewDocument returned NULL.\n");
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return NULL;
	}
	ASSERT_VALID(pDocument);

	// keep the document alive without views
	pDocument->m_bAutoDelete = FALSE;

	SetDefaultTitle(pDocument);

	return pDocument;
}

