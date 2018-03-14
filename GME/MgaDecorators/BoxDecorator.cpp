//################################################################################################
//
// Box decorator class
//	BoxDecorator.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "BoxDecorator.h"
#include "BoxCompositePart.h"
#include <afxole.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _DEBUG
#define TRYDRAGNDROP
#endif

//################################################################################################
//
// CLASS : BoxDecorator
//
//################################################################################################

BoxDecorator::BoxDecorator(CComPtr<IMgaCommonDecoratorEvents>& eventSink):
	m_part(NULL),
	m_eventSink(eventSink)
{
}

BoxDecorator::~BoxDecorator()
{
}

void BoxDecorator::Initialize(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO)
{
	m_part->Initialize(pProject, pPart, pFCO);
}

void BoxDecorator::Destroy()
{
	if (m_part) {
		m_part->Destroy();
		delete m_part;
		m_part = NULL;
	}
}

CString BoxDecorator::GetMnemonic(void) const
{
	return m_part->GetMnemonic();
}

feature_code BoxDecorator::GetFeatures(void) const
{
	return m_part->GetFeatures();
}

void BoxDecorator::SetParam(const CString& strName, VARIANT vValue)
{
	m_part->SetParam(strName, vValue);
}

void BoxDecorator::GetParam(const CString& strName, VARIANT* pvValue)
{
	m_part->GetParam(strName, pvValue);
}

void BoxDecorator::SetActive(bool bIsActive)
{
	m_part->SetActive(bIsActive);
}

CSize BoxDecorator::GetPreferredSize(void) const
{
	return m_part->GetPreferredSize();
}


void BoxDecorator::SetLocation(const CRect& location)
{
	m_part->SetLocation(location);
}

CRect BoxDecorator::GetLocation(void) const
{
	return m_part->GetLocation();
}

CRect BoxDecorator::GetLabelLocation(void) const
{
	return m_part->GetLabelLocation();
}

CRect BoxDecorator::GetPortLocation(CComPtr<IMgaFCO>& fco) const
{
	return m_part->GetPortLocation(fco);
}

void BoxDecorator::GetPorts(CComPtr<IMgaFCOs>& portFCOs) const
{
	m_part->GetPorts(portFCOs);
}

void BoxDecorator::Draw(CDC* pDC, Gdiplus::Graphics* gdip)
{
	m_part->Draw(pDC, gdip);
}

void BoxDecorator::SaveState()
{
	m_part->SaveState();
}

// New functions
void BoxDecorator::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
								HWND parentWnd)
{
	DecoratorSDK::BoxCompositePart* boxComposite = new DecoratorSDK::BoxCompositePart(NULL, m_eventSink);
	m_part = boxComposite;

	boxComposite->InitializeEx(pProject, pPart, pFCO, parentWnd);
}

void BoxDecorator::SetSelected(bool bIsSelected)
{
	m_part->SetSelected(bIsSelected);
}

bool BoxDecorator::MouseMoved(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMoved(nFlags, point, transformHDC);
}

bool BoxDecorator::MouseLeftButtonDown(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonDown(nFlags, point, transformHDC);
}

bool BoxDecorator::MouseLeftButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonUp(nFlags, point, transformHDC);
}

bool BoxDecorator::MouseLeftButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonDoubleClick(nFlags, point, transformHDC);
}

bool BoxDecorator::MouseRightButtonDown(HMENU hCtxMenu, UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonDown(hCtxMenu, nFlags, point, transformHDC);
}

bool BoxDecorator::MouseRightButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonUp(nFlags, point, transformHDC);
}

bool BoxDecorator::MouseRightButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonDoubleClick(nFlags, point, transformHDC);
}

bool BoxDecorator::MouseMiddleButtonDown(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonDown(nFlags, point, transformHDC);
}

bool BoxDecorator::MouseMiddleButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonUp(nFlags, point, transformHDC);
}

bool BoxDecorator::MouseMiddleButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonDoubleClick(nFlags, point, transformHDC);
}

bool BoxDecorator::MouseWheelTurned(UINT nFlags, short distance, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseWheelTurned(nFlags, distance, point, transformHDC);
}

bool BoxDecorator::DragEnter(DROPEFFECT* dropEffect, COleDataObject* pDataObject, DWORD dwKeyState, const CPoint& point, HDC transformHDC)
{
#ifdef TRYDRAGNDROP
//	pDataObject->BeginEnumFormats();
//	FORMATETC fi;
//	TRACE0("DragEnter:\n");
//	while (pDataObject->GetNextFormat(&fi) == TRUE) {
//		TRACE("\tcfFormat %d dwAspect %lu lindex %ld tymed %lu\n", fi.cfFormat, fi.dwAspect, fi.lindex, fi.tymed);
//	}
//	return DROPEFFECT_NONE;
	if (pDataObject->IsDataAvailable(CF_HDROP)) {
		*dropEffect = DROPEFFECT_COPY;
		return true;
	}
#endif
	return m_part->DragEnter(dropEffect, pDataObject, dwKeyState, point, transformHDC);
}

bool BoxDecorator::DragOver(DROPEFFECT* dropEffect, COleDataObject* pDataObject, DWORD dwKeyState, const CPoint& point, HDC transformHDC)
{
#ifdef TRYDRAGNDROP
	if (pDataObject->IsDataAvailable(CF_HDROP)) {
		*dropEffect = DROPEFFECT_COPY;
		return true;
	}
#endif
	return m_part->DragOver(dropEffect, pDataObject, dwKeyState, point, transformHDC);
}

bool BoxDecorator::Drop(COleDataObject* pDataObject, DROPEFFECT dropEffect, const CPoint& point, HDC transformHDC)
{
#ifdef TRYDRAGNDROP
	if (pDataObject->IsDataAvailable(CF_HDROP) && dropEffect == DROPEFFECT_COPY) {
		STGMEDIUM medium;
		medium.tymed = TYMED_HGLOBAL;
		pDataObject->GetData(CF_HDROP, &medium);
		return DropFile((HDROP)medium.hGlobal, point, transformHDC);
	}
#endif
	return m_part->Drop(pDataObject, dropEffect, point, transformHDC);
}

bool BoxDecorator::DropFile(HDROP p_hDropInfo, const CPoint& point, HDC transformHDC)
{
#ifdef TRYDRAGNDROP
	UINT nFiles = DragQueryFile(p_hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (nFiles < 1)
		return false;

	for (UINT iF = 0; iF < nFiles; ++iF) {
		TCHAR szFileName[_MAX_PATH];
		UINT res = DragQueryFile(p_hDropInfo, iF, szFileName, _MAX_PATH);
		if (res > 0) {
			bool is_dir = false; 
			struct _stat fstatus;
			if (0 == _tstat(szFileName, &fstatus))
				is_dir = (fstatus.st_mode & _S_IFDIR) == _S_IFDIR;

			CString conn(szFileName);
			if (!is_dir && conn.Right(4).CompareNoCase(_T(".txt")) == 0) {
				CFile txtFile(conn, CFile::modeRead);
				char pbufRead[100];
				UINT readLen = txtFile.Read(pbufRead, sizeof(pbufRead) - 1);
				pbufRead[readLen] = 0;
				AfxMessageBox("Decorator drop: '" + conn + "' first 100 bytes: " + pbufRead + ".");
				return true;
			} else {
				AfxMessageBox("Decorator drop: '.txt' files may be dropped only. Can't open file: " + conn + "!");
			}
		} else {
			AfxMessageBox(_T("Decorator drop: Can't inquire file information!"));
		}
	}
#endif
	return m_part->DropFile(p_hDropInfo, point, transformHDC);
}

bool BoxDecorator::MenuItemSelected(UINT menuItemId, UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MenuItemSelected(menuItemId, nFlags, point, transformHDC);
}

bool BoxDecorator::OperationCanceledByGME(void)
{
	return m_part->OperationCanceledByGME();
}
