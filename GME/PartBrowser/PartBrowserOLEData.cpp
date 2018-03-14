#include <stdafx.h>
#include <afxadv.h>
#include "afxcoll.h"
#include "afxtempl.h"
#include "..\Interfaces\meta.h"
#include "..\Interfaces\mga.h"
#include "..\Interfaces\parser.h"

#include "..\GME\GMEstd.h"

#include "PartBrowserOLEData.h"

#include "PartBrowser_i.c"

CLIPFORMAT CPartBrowserDataSource::cfGMEDesc =	(CLIPFORMAT)(RegisterClipboardFormat(_T("GME Descriptor")));
int CPartBrowserDataSource::myData = 0;

/* --------------------------- CPartBrowserDataDescriptor --------------------------- */
//static // called externally, to destruct certain lists
void CPartBrowserDataDescriptor::destructList(CTypedPtrList<CPtrList, CRect*>& pList)// it is a CRectList
{
	POSITION pos = pList.GetHeadPosition();
	while (pos)
		delete pList.GetNext(pos);
	pList.RemoveAll();
}

CPartBrowserDataDescriptor::CPartBrowserDataDescriptor(CTypedPtrList<CPtrList, CRect*> &list,
													   CTypedPtrList<CPtrList, CRect*> &annList,
													   CPoint dragPoint, CPoint offs)
{
	POSITION pos = list.GetHeadPosition();
	while (pos) {
		CRect *rect = new CRect(list.GetNext(pos));
		rect->OffsetRect(-dragPoint.x,-dragPoint.y);
		rects.AddTail(rect);
	}

	pos = annList.GetHeadPosition();
	while (pos) {
		CRect* rect = new CRect(annList.GetNext(pos));
		rect->OffsetRect(-dragPoint.x,-dragPoint.y);
		annRects.AddTail(rect);
	}

	offset = offs;
	pFile = 0;
}

void CPartBrowserDataDescriptor::Reset()
{
	POSITION pos = rects.GetHeadPosition();
	while (pos)
		delete rects.GetNext(pos);
	rects.RemoveAll();

	pos = annRects.GetHeadPosition();
	while (pos)
		delete annRects.GetNext(pos);
	annRects.RemoveAll();
}

void CPartBrowserDataDescriptor::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {
		ar << rects.GetCount();
		POSITION pos = rects.GetHeadPosition();
		while (pos)
			ar << *(rects.GetNext(pos));

		ar << annRects.GetCount();
		pos = annRects.GetHeadPosition();
		while (pos)
			ar << *(annRects.GetNext(pos));

		ar << offset;
	} else {
		Reset();
		int n;
		ar >> n;
		for(int i = 0; i < n; i++) {
			CRect* rect = new CRect();
			ar >> *rect;
			rects.AddTail(rect);
		}

		ar >> n;
		for(int j = 0; j < n; j++) {
			CRect* rect = new CRect();
			ar >> *rect;
			annRects.AddTail(rect);
		}
		ar >> offset;
	}
}

bool CPartBrowserDataDescriptor::Load(COleDataObject* pDataObject)
{
	ASSERT(pDataObject != NULL);

	if (pDataObject->IsDataAvailable(CPartBrowserDataSource::cfGMEDesc)) {
		ASSERT(pFile == 0);
		pFile = pDataObject->GetFileData(CPartBrowserDataSource::cfGMEDesc);
		ASSERT(pFile != NULL);
		CArchive ar(pFile, CArchive::load);
		Serialize(ar);
		return true;
	}
	return false;
}

int CPartBrowserDataDescriptor::GetCount()
{
	/* return (rects.GetCount() + annRects.GetCount()); */
	return rects.GetCount(); 
}

void CPartBrowserDataDescriptor::Clean()
{
	delete pFile;
	pFile = 0;
}

void CPartBrowserDataDescriptor::Draw(CDC *pDC,CPoint &pt)
{
	POSITION pos = rects.GetHeadPosition();
	while (pos) {
		CRect rect = *(rects.GetNext(pos));

		rect.OffsetRect(pt);
		CPoint align = rect.CenterPoint();

		// Emulate the grid.
		long gs = GME_GRID_SIZE;
		align.x = (align.x % gs);
		align.y = (align.y % gs);

		
		rect.OffsetRect(-align.x, -align.y);
		// rect.OffsetRect(-offset.x, -offset.y);
		pDC->DrawFocusRect(&rect);

	}

	pos = annRects.GetHeadPosition();
	while (pos) {
		CRect rect = *(annRects.GetNext(pos));

		rect.OffsetRect(pt);
		pDC->DrawFocusRect(&rect);
	}
}


// This function has been simplified for GMEActiveBrowser
// No annotations and grid alignment
void CPartBrowserDataDescriptor::SimpleDraw(CDC *pDC, CPoint &pt)
{
	POSITION pos = rects.GetHeadPosition();
	while (pos) {
		CRect rect = *(rects.GetNext(pos));

		rect.OffsetRect(pt);

		pDC->DrawFocusRect(&rect);

	}
}


void CPartBrowserDataDescriptor::GetBoundingRect(CRect &rBoundingRect)
{
	rBoundingRect = CRect(0, 0, 0, 0);

	CRect Rect;
	POSITION pos = rects.GetHeadPosition();
	
	while (pos) {
		Rect = *(rects.GetNext(pos));
		rBoundingRect.UnionRect(rBoundingRect, Rect);
	}
}


/* --------------------------- CPartBrowserDataSource ------------------------------- */

bool CPartBrowserDataSource::IsGmeNativeDataAvailable(COleDataObject *pDataObject, IMgaProject *project)
{
	ASSERT( project != NULL );

	CComPtr<IDataObject> p = pDataObject->GetIDataObject(FALSE);
	CComPtr<IPartBrowserDataSource> source;
	if (p!= NULL && p.QueryInterface(&source) == S_OK) {
		CComPtr<IUnknown> unknown;
		COMTHROW(source->get_Project(&unknown));
		ASSERT(unknown != NULL);

		CComPtr<IMgaProject> source_project;
		COMTHROW(unknown.QueryInterface(&source_project));
		ASSERT(source_project != NULL);

		return source_project.IsEqualObject(project);
	}
	return false;
}

bool CPartBrowserDataSource::IsXMLDataAvailable(COleDataObject *pDataObject)
{
	ASSERT(pDataObject != NULL);

	return pDataObject->IsDataAvailable(CF_TEXT) != FALSE;
}

bool CPartBrowserDataSource::ParseXMLData(COleDataObject *pDataObject, IMgaObject *target, bool merge = false)
{
	ASSERT(pDataObject != NULL);
	ASSERT(target != NULL);

	// create a temporary filename
	char *fname = _tempnam("c:\\temp", "tmp");
	CString filename = fname;
	free(fname);

	try
	{
		// get the memory file
		CFile *memfile = pDataObject->GetFileData(CF_TEXT);
		if (memfile == NULL)
			return false;
		memfile->SeekToBegin();

		// copy
		CFile file;
		if(file.Open(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary) == 0)
			return false;

		const int buffsize = 10240;
		unsigned char buff[buffsize];
		UINT c;
		do
		{
			c = memfile->Read(buff, buffsize);

			UINT new_c = 0; // addition by ZolMol
			while (buff[new_c] != 0 && new_c < c) ++new_c;
			c = new_c;		// end

			file.Write(buff, c);
		} while(c == buffsize);
		file.Close();

		// clear the memory file
		delete memfile;

		// parse
		CComPtr<IMgaParser> parser;
		COMTHROW(parser.CoCreateInstance(L"Mga.MgaParser"));
		ASSERT(parser != NULL);

		CComBstrObj acckind, version; 
		VARIANT_BOOL is_acc_target;
		COMTHROW(parser->GetClipXMLInfo(PutInBstr(filename), target, &is_acc_target, PutOut(acckind), PutOut( version)));
		CString ver = "0"; // defval
		if (version) // clipboard main token found, otherwise use defval
			CopyTo(version, ver);


		CComObjPtr<IMgaProject> t_project;
		COMTHROW(target->get_Project(PutOut(t_project)));
		CComPtr<IGMEOLEApp> t_GME = CPartBrowserDataSource::get_GME(t_project);
		
		CComBSTR msg, done;
		done.Append("Done.");
		if (ver == "0") {
			msg.Append("Inserting XML data...");
			if (t_GME) COMTHROW(t_GME->ConsoleMessage(msg, MSG_INFO));
			COMTHROW(parser->ParseFCOs(target, PutInBstr(filename)));
			if (t_GME) COMTHROW(t_GME->ConsoleMessage(done, MSG_INFO));
		} else if (ver == "4") {
			msg.Append("Inserting XML SmartCopied data...");
			if (t_GME) COMTHROW(t_GME->ConsoleMessage(msg, MSG_INFO));
			COMTHROW(parser->ParseClos4(target, PutInBstr(filename), merge ? MERGE:ADDITION));
			if (t_GME) COMTHROW(t_GME->ConsoleMessage(done, MSG_INFO));
		} else if (ver == "1" || ver == "") {
			msg.Append("Inserting XML CopyClosured data...");
			if (t_GME) COMTHROW(t_GME->ConsoleMessage(msg, MSG_INFO));
			COMTHROW(parser->ParseClos1(target, PutInBstr(filename)));
			if (t_GME) COMTHROW(t_GME->ConsoleMessage(done, MSG_INFO));
		} else {
			msg.Append("Error: Unknown clipboard closure format");
			if(t_GME) COMTHROW(t_GME->ConsoleMessage(msg, MSG_INFO));

			ASSERT(0);
		}


		CFile::Remove(filename);

		return true;
	}
	catch(hresult_exception &e)
	{
		CFile::Remove(filename);

		try
		{
			CComObjPtr<IErrorInfo> errinfo;
			COMTHROW(GetErrorInfo(0, PutOut(errinfo)));
			ASSERT(errinfo != NULL);

			CString desc;
			COMTHROW(errinfo->GetDescription(PutOut(desc)));

			AfxMessageBox(CString("Error while parsing XML file: ") + desc);
		}
		catch(hresult_exception &)
		{
			AfxMessageBox("Fatal error while parsing XML file!");
		}

		throw e;
	}

	return false;
}

void CPartBrowserDataSource::CacheDescriptor(CPartBrowserDataDescriptor* desc)
{
	CacheGlobalData(cfGMEDesc, CreateDescriptor(desc));
	DelayXMLDump();
}

HGLOBAL CPartBrowserDataSource::CreateDescriptor(CPartBrowserDataDescriptor* desc)
{
	ASSERT(desc);

	CSharedFile file;
	CArchive ar(&file, CArchive::store);
	desc->Serialize(ar);
	ar.Close();
	return file.Detach();
}

void CPartBrowserDataSource::DelayXMLDump()
{
	FORMATETC fe = {
	CF_TEXT, NULL, DVASPECT_CONTENT, -1, TYMED_ISTREAM|TYMED_HGLOBAL
	};

	DelayRenderFileData(CF_TEXT, &fe);
}

BOOL CPartBrowserDataSource::OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile)
{
	if (lpFormatEtc->cfFormat == CF_TEXT) {
		ASSERT(pFile != NULL);

		// TODO: we have to dump it directly to the shared memory file

		try
		{
			char *fname = _tempnam("c:\\temp", "tmp");
			CString filename = fname;
			free(fname);
		
			CComPtr<IMgaDumper> dumper;
			COMTHROW(dumper.CoCreateInstance(L"Mga.MgaDumper"));

			CComObjPtr<IMgaFCOs> fcos;
			if (data) // check whether it is set
				COMTHROW(::QueryInterface(data, fcos));

			CComObjPtr<IMgaFolders> folds;
			if (folders) // check whether it is set
				COMTHROW(::QueryInterface(folders, folds));

			CComObjPtr<IMgaRegNodes> regd;
			if (regdata) // check whether it is set
				COMTHROW(::QueryInterface(regdata, regd));

			COMTHROW(dumper->DumpFCOs(project, fcos, folds, regd, PutInBstr(filename)));

			CFile file;
			if(file.Open(filename, CFile::modeRead | CFile::typeBinary) == 0)
				return FALSE;
			
			const int buffsize = 10240;
			unsigned char buff[buffsize];
			UINT c;
			do
			{
				c = file.Read(buff, buffsize);
				pFile->Write(buff, c);
			} while(c == buffsize);

			buff[0] = 0;
			pFile->Write(buff, 1);

			file.Close();
			CFile::Remove(filename);
		}
		catch(hresult_exception &)
		{
			return FALSE;
		}
		catch(CFileException *)
		{
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

// ****************************************************************************************************
// *********************************   CPartBrowserClosureDataSource  *****************************************
// ****************************************************************************************************
BOOL CPartBrowserClosureDataSource::OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile)
{
	if (lpFormatEtc->cfFormat == CF_TEXT) {
		ASSERT(pFile != NULL);

		// TODO: we have to dump it directly to the shared memory file

		try
		{
			char *fname = _tempnam("c:\\temp", "tmp");
			CString filename = fname;
			free(fname);
		
			CComPtr<IMgaDumper> dumper;
			COMTHROW(dumper.CoCreateInstance(L"Mga.MgaDumper"));

			CComObjPtr<IMgaFCOs> fcos;
			if (data) // is it set?
				COMTHROW(::QueryInterface(data, fcos));

			CComObjPtr<IMgaFolders> fols;
			if (folders) // is it set?
				COMTHROW(::QueryInterface(folders, fols));

			bool is_top_set = false;
			CComObjPtr<IMgaFCOs> top_fcos;
			CComObjPtr<IMgaFolders> top_folds;

			if (m_topFcos) {
				COMTHROW(::QueryInterface( m_topFcos, top_fcos));
				is_top_set = true;
			}

			if (m_topFolders) {
				COMTHROW(::QueryInterface( m_topFolders, top_folds));
				is_top_set = true;
			}

			if (is_top_set)
				COMTHROW(dumper->DumpClosR(fcos, fols, PutInBstr(filename), top_fcos, top_folds, m_options, m_absPathPart, m_acceptingKinds));
			else // if top objects are not set dump starting from RootFolder
				COMTHROW(dumper->DumpClos(fcos, fols, PutInBstr(filename), m_options));

			CFile file;
			if (file.Open(filename, CFile::modeRead | CFile::typeBinary) == 0)
				return FALSE;

			const int buffsize = 10240;
			unsigned char buff[buffsize];
			UINT c;
			do
			{
				c = file.Read(buff, buffsize);
				pFile->Write(buff, c);
			} while (c == buffsize);

			buff[0] = 0;
			pFile->Write(buff, 1);

			file.Close();
			CFile::Remove(filename);
		}
		catch(hresult_exception &)
		{
			return FALSE;
		}
		catch(CFileException *)
		{
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CPartBrowserDataSource, COleDataSource)
	//{{AFX_MSG_MAP(CPartBrowserDataSource)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Interface Maps

BEGIN_DISPATCH_MAP(CPartBrowserDataSource, COleDataSource)
	//{{AFX_DISPATCH_MAP(CPartBrowserDataSource)
	DISP_PROPERTY_EX(CPartBrowserDataSource, "Data", DispGetData, DispSetData, VT_DISPATCH)
	DISP_PROPERTY_EX(CPartBrowserDataSource, "Folders", DispGetFolders, DispSetFolders, VT_DISPATCH)
	DISP_PROPERTY_EX(CPartBrowserDataSource, "RegistryData", DispGetRegistryData, DispSetRegistryData, VT_DISPATCH)
	DISP_PROPERTY_EX(CPartBrowserDataSource, "Project", DispGetProject, DispSetProject, VT_DISPATCH)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


BEGIN_INTERFACE_MAP(CPartBrowserDataSource, COleDataSource)
	INTERFACE_PART(CPartBrowserDataSource, IID_IPartBrowserDataSource, PartBrowserDataSource)
	DUAL_ERRORINFO_PART(CPartBrowserDataSource)
END_INTERFACE_MAP()

DELEGATE_DUAL_INTERFACE(CPartBrowserDataSource, PartBrowserDataSource)

// Implement ISupportErrorInfo to indicate we support the
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CPartBrowserDataSource, IID_IPartBrowserDataSource)


STDMETHODIMP CPartBrowserDataSource::XPartBrowserDataSource::get_Data(IUnknown **p)
{
	METHOD_PROLOGUE(CPartBrowserDataSource, PartBrowserDataSource)
	CHECK_OUT(p);

	if (pThis->data != NULL)
		return pThis->data.QueryInterface(p);

	return S_OK;
}

STDMETHODIMP CPartBrowserDataSource::XPartBrowserDataSource::get_Folders(IUnknown **p)
{
	METHOD_PROLOGUE(CPartBrowserDataSource, PartBrowserDataSource)
	CHECK_OUT(p);

	if (pThis->folders != NULL)
		return pThis->folders.QueryInterface(p);

	return S_OK;
}

STDMETHODIMP CPartBrowserDataSource::XPartBrowserDataSource::get_RegistryData(IUnknown **p)
{
	METHOD_PROLOGUE(CPartBrowserDataSource, PartBrowserDataSource)
	CHECK_OUT(p);

	if (pThis->regdata != NULL)
		return pThis->regdata.QueryInterface(p);

	return S_OK;
}

STDMETHODIMP CPartBrowserDataSource::XPartBrowserDataSource::get_Project(IUnknown **p)
{
	METHOD_PROLOGUE(CPartBrowserDataSource, PartBrowserDataSource)
	CHECK_OUT(p);

	return pThis->project.QueryInterface(p);
}




/*static*/ CComPtr<IGMEOLEApp> CPartBrowserDataSource::get_GME(CComObjPtr<IMgaProject> project)
{
	CComPtr<IGMEOLEApp> gme;
	if ( (project != NULL)) {
		CComBSTR bstrName("GME.Application");
		CComPtr<IMgaClient> pClient;
		HRESULT hr = project->GetClientByName(bstrName, &pClient);
		if (SUCCEEDED(hr) && pClient) {
			CComPtr<IDispatch> pDispatch;
			hr = pClient->get_OLEServer(&pDispatch);
			if (SUCCEEDED(hr) && pDispatch) {
				hr = pDispatch.QueryInterface(&gme);
				if (FAILED(hr)) {
					gme = NULL;
				}
			}
		}
	}
	return gme;
}

LPDISPATCH CPartBrowserDataSource::DispGetData() 
{
	CComPtr<IDispatch> p;
	if (data != NULL) {
		data.QueryInterface(&p);
	}

	return p.Detach();
}

void CPartBrowserDataSource::DispSetData(LPDISPATCH)
{
	SetNotSupported();
}

LPDISPATCH CPartBrowserDataSource::DispGetFolders()
{
	CComPtr<IDispatch> p;
	if (folders != NULL) {
		folders.QueryInterface(&p);
	}

	return p.Detach();
}

void CPartBrowserDataSource::DispSetFolders(LPDISPATCH)
{
	SetNotSupported();
}

LPDISPATCH CPartBrowserDataSource::DispGetRegistryData()
{
	CComPtr<IDispatch> p;
	if (regdata != NULL) {
		regdata.QueryInterface(&p);
	}

	return p.Detach();
}

void CPartBrowserDataSource::DispSetRegistryData(LPDISPATCH)
{
	SetNotSupported();
}

LPDISPATCH CPartBrowserDataSource::DispGetProject()
{
	CComPtr<IDispatch> p;
	if (project != NULL) {
		project.QueryInterface(&p);
	}

	return p.Detach();
}

void CPartBrowserDataSource::DispSetProject(LPDISPATCH)
{
	SetNotSupported();
}

