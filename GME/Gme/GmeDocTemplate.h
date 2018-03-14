
#ifndef MGA_GMEDOCTEMPLATE_H
#define MGA_GMEDOCTEMPLATE_H

class CGmeDocTemplate : public CMultiDocTemplate
{

public:
	CGmeDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

	CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible);

};

#endif//MGA_GMEDOCTEMPLATE_H
