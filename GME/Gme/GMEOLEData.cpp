#include <stdafx.h>
#include <afxadv.h>
#include "afxcoll.h"
#include "afxtempl.h"
#include "meta.h"
#include "mga.h"
#include "parser.h"

#include <algorithm>
#include "CommonMfc.h"

#include "GMEstd.h"

#include "GMEOLEData.h"

#ifdef min
#undef min
#endif

CLIPFORMAT CGMEDataSource::cfGMEDesc =	(CLIPFORMAT)(RegisterClipboardFormat(_T("GME Descriptor")));
int CGMEDataSource::myData = 0;

/* --------------------------- CGMEDataDescriptor --------------------------- */
//static // called externally, to destruct certain lists
void CGMEDataDescriptor::destructList( CTypedPtrList<CPtrList, CRect *>& pList)// it is a CRectList
{
	POSITION pos = pList.GetHeadPosition();
	while(pos)
		delete pList.GetNext(pos);
	pList.RemoveAll();
}

CGMEDataDescriptor::CGMEDataDescriptor(CTypedPtrList<CPtrList, CRect *> &list, CTypedPtrList<CPtrList, CRect *> &annList,CPoint dragPoint,CPoint offs)
{
	POSITION pos = list.GetHeadPosition();
	while(pos) {
		CRect *rect = new CRect(list.GetNext(pos));
		rect->OffsetRect(-dragPoint.x,-dragPoint.y);
		rects.AddTail(rect);
	}

	pos =  annList.GetHeadPosition();
	while (pos) {
		CRect *rect = new CRect(annList.GetNext(pos));
		rect->OffsetRect(-dragPoint.x,-dragPoint.y);
		annRects.AddTail(rect);
	}

	offset = offs;
	pFile = 0;
}

void CGMEDataDescriptor::Reset()
{
	POSITION pos = rects.GetHeadPosition();
	while(pos)
		delete rects.GetNext(pos);
	rects.RemoveAll();

	pos = annRects.GetHeadPosition();
	while(pos)
		delete annRects.GetNext(pos);
	annRects.RemoveAll();
}

void CGMEDataDescriptor::Serialize(CArchive& ar)
{
	if(ar.IsStoring()) {
		ar << (int)rects.GetCount();
		POSITION pos = rects.GetHeadPosition();
		while(pos)
			ar << *(rects.GetNext(pos));
		
		ar << (int)annRects.GetCount();
		pos = annRects.GetHeadPosition();
		while(pos)
			ar << *(annRects.GetNext(pos));
		
		ar << offset;
	}
	else {
		Reset();
		int n;
		ar >> n;
		for(int i = 0; i < n; i++) {
			CRect *rect = new CRect();
			ar >> *rect;
			rects.AddTail(rect);
		}

		ar >> n;
		for(int j = 0; j < n; j++) {
			CRect *rect = new CRect();
			ar >> *rect;
			annRects.AddTail(rect);
		}
		ar >> offset;
	}
}

bool CGMEDataDescriptor::Load(COleDataObject* pDataObject)
{
	ASSERT( pDataObject != NULL );

	if(pDataObject->IsDataAvailable(CGMEDataSource::cfGMEDesc))	{
		ASSERT(pFile == 0);
		pFile = pDataObject->GetFileData(CGMEDataSource::cfGMEDesc);
		ASSERT( pFile != NULL );
		CArchive ar(pFile,CArchive::load);
		Serialize(ar);
		return true;
	}
	return false;
}

int CGMEDataDescriptor::GetCount()
{
	/* return (rects.GetCount() + annRects.GetCount()); */
	return (int)rects.GetCount(); 
}

void CGMEDataDescriptor::Clean()
{
	delete pFile;
	pFile = 0;
}

void CGMEDataDescriptor::Draw(CDC *pDC,CPoint &pt)
{
	POSITION pos = rects.GetHeadPosition();
	while(pos) {
		CRect rect = *(rects.GetNext(pos));

		rect.OffsetRect(pt);
		CPoint align = rect.CenterPoint();

		// Emulate the grid.
		long gs = GME_GRID_SIZE;
		rect.MoveToXY(rect.left / gs * gs, rect.top / gs * gs);

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
void CGMEDataDescriptor::SimpleDraw(CDC *pDC, CPoint &pt)
{
	POSITION pos = rects.GetHeadPosition();
	while(pos) {
		CRect rect = *(rects.GetNext(pos));

		rect.OffsetRect(pt);

		pDC->DrawFocusRect(&rect);

	}
	
}


void CGMEDataDescriptor::GetBoundingRect(CRect &rBoundingRect)
{
	rBoundingRect=CRect(0,0,0,0);

	CRect Rect;
	POSITION pos=rects.GetHeadPosition();
	
	while(pos)
	{
		Rect=*(rects.GetNext(pos));
		rBoundingRect.UnionRect(rBoundingRect,Rect);
	}
	

}


/* --------------------------- CGMEDataSource ------------------------------- */

bool CGMEDataSource::IsGmeNativeDataAvailable(COleDataObject *pDataObject, IMgaProject *project)
{
	ASSERT( project != NULL );

	CComPtr<IDataObject> p = pDataObject->GetIDataObject(FALSE);
	CComPtr<IMgaDataSource> source;
	if( p!= NULL && p.QueryInterface(&source) == S_OK )
	{
		CComPtr<IUnknown> unknown;
		HRESULT hr = source->get_Project(&unknown);
		if (hr == HRESULT_FROM_WIN32(RPC_S_CALL_FAILED)) // crashrpt 17f8cc45-c369-464b-8162-5dc7bf3bf04c
			return false;
		COMTHROW(hr);
		ASSERT( unknown != NULL );

		if (unknown == NULL)
		{
			// Likely a race, crashrpt e27fb137-199a-4ff8-842e-301655495738
			return false;
		}

		CComPtr<IMgaProject> source_project;
		// KMS: fixing crashrpt 8895373f-396e-490f-b882-036ba9d42961: this QI may fail
		hr = unknown.QueryInterface(&source_project);
		if ( source_project == NULL ) {
			ASSERT(false);
			return false;
		}

		return source_project.IsEqualObject(project);
	}
	return false;
}

bool CGMEDataSource::IsXMLDataAvailable(COleDataObject *pDataObject)
{
	ASSERT( pDataObject != NULL );

	return pDataObject->IsDataAvailable(CF_UNICODETEXT) != FALSE;
}

bool CGMEDataSource::ParseXMLData(COleDataObject *pDataObject, IMgaObject *target, bool merge = false)
{
	ASSERT( pDataObject != NULL );
	ASSERT( target != NULL );

	// create a temporary filename
      TCHAR *fname = _ttempnam(_T("c:\\temp"), _T("GMEtmp"));
	CString filename = fname;
	free(fname);

	try
	{
		// get the memory file
		CFile *memfile = pDataObject->GetFileData(CF_UNICODETEXT);
		if( memfile == NULL )
			return false;
		// FIXME: memfile->GetLength() isn't the same as pFile->GetLength() in OnRenderFileData

		// copy
		CFile file;
		if( file.Open(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary) == 0 )
			return false;

		wchar_t buff[10240];
		UINT c;
		do
		{
			c = memfile->Read(buff, sizeof(buff));

			c = wcsnlen(buff, c/sizeof(wchar_t)) * sizeof(wchar_t);

			file.Write(buff, c);
		} while( c == sizeof(buff) );
		file.Close();

		// clear the memory file
		delete memfile;

		// parse
		CComPtr<IMgaParser> parser;
		COMTHROW( parser.CoCreateInstance(L"Mga.MgaParser") );
		ASSERT( parser != NULL );

		CComBstrObj acckind, version; 
		VARIANT_BOOL is_acc_target;
		COMTHROW( parser->GetClipXMLInfo( PutInBstr(filename), target, &is_acc_target, PutOut( acckind), PutOut( version)) );
		CString ver = _T("0"); // defval
		if( version) // clipboard main token found, otherwise use defval
			CopyTo( version, ver);


		CComObjPtr<IMgaProject> t_project;
		COMTHROW(target->get_Project( PutOut(t_project)));
		CComPtr<IGMEOLEApp> t_GME = CGMEDataSource::get_GME( t_project);
		
		CComBSTR msg, done;
		COMTHROW(done.Append(_T("Done.")));
		if( ver == _T("0"))
		{
			COMTHROW(msg.Append(_T("Inserting XML data...")));
			if( t_GME) COMTHROW( t_GME->ConsoleMessage( msg, MSG_INFO));
			COMTHROW( parser->ParseFCOs(target, PutInBstr(filename)) );
			if( t_GME) COMTHROW( t_GME->ConsoleMessage( done, MSG_INFO));
		}
		else if( ver == _T("4"))
		{
			COMTHROW(msg.Append(_T("Inserting XML SmartCopied data...")));
			if( t_GME) COMTHROW( t_GME->ConsoleMessage( msg, MSG_INFO));
			COMTHROW( parser->ParseClos4( target, PutInBstr(filename), merge?MERGE:ADDITION ));
			if( t_GME) COMTHROW( t_GME->ConsoleMessage( done, MSG_INFO));
		}
		else if ( ver == _T("1") || ver == _T(""))
		{
			COMTHROW(msg.Append(_T("Inserting XML CopyClosured data...")));
			if( t_GME) COMTHROW( t_GME->ConsoleMessage( msg, MSG_INFO));
			COMTHROW( parser->ParseClos1(target, PutInBstr(filename)) );
			if( t_GME) COMTHROW( t_GME->ConsoleMessage( done, MSG_INFO));
		}
		else
		{
			COMTHROW(msg.Append(_T("Error: Unknown clipboard closure format")));
			if( t_GME) COMTHROW( t_GME->ConsoleMessage( msg, MSG_INFO));

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
			COMTHROW( GetErrorInfo(0, PutOut(errinfo)) );
			ASSERT( errinfo != NULL );

			CString desc;
			COMTHROW( errinfo->GetDescription(PutOut(desc)) );

			AfxMessageBox(CString(_T("Error while parsing XML file: ")) + desc);
		}
		catch(hresult_exception &)
		{
			AfxMessageBox(_T("Fatal error while parsing XML file!"));
		}

		throw e;
	}

	return false;
}

void CGMEDataSource::CacheDescriptor(CGMEDataDescriptor* desc)
{
	CacheGlobalData(cfGMEDesc, CreateDescriptor(desc));
	DelayXMLDump();
}

HGLOBAL CGMEDataSource::CreateDescriptor(CGMEDataDescriptor* desc)
{
	ASSERT(desc);

	CSharedFile file;
	CArchive ar(&file, CArchive::store);
	desc->Serialize(ar);
	ar.Close();
	return file.Detach();
}

void CGMEDataSource::DelayXMLDump()
{
	FORMATETC fe = {
    CF_UNICODETEXT, NULL, DVASPECT_CONTENT, -1, TYMED_ISTREAM|TYMED_HGLOBAL
	};
          
	DelayRenderFileData(CF_UNICODETEXT, &fe);
}

BOOL CGMEDataSource::OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile)
{
	if( lpFormatEtc->cfFormat == CF_UNICODETEXT )
	{
		ASSERT( pFile != NULL );

		// TODO: we have to dump it directly to the shared memory file

		try
		{
			TCHAR *fname = _ttempnam(_T("c:\\temp"), _T("tmp"));
			CString filename = fname;
			free(fname);
		
			CComPtr<IMgaDumper> dumper;
			COMTHROW( dumper.CoCreateInstance(L"Mga.MgaDumper") );

			CComObjPtr<IMgaFCOs> fcos;
			if ( data) // check whether it is set
				COMTHROW( ::QueryInterface( data, fcos) );

			CComObjPtr<IMgaFolders> folds;
			if ( folders) // check whether it is set
				COMTHROW( ::QueryInterface( folders, folds) );

			CComObjPtr<IMgaRegNodes> regd;
			if ( regdata) // check whether it is set
				COMTHROW( ::QueryInterface( regdata, regd) );

			COMTHROW( dumper->DumpFCOs( project, fcos, folds, regd, PutInBstr(filename)) );

			CFile file;
			if( file.Open(filename, CFile::modeRead | CFile::typeBinary) == 0 )
				return FALSE;
			
			const int buffsize = 10240;
			unsigned char buff[buffsize];
			file.Read(buff, 2); // skip BOM
			UINT c;
			do
			{
				c = file.Read(buff, buffsize);
				pFile->Write(buff, c);
			} while( c == buffsize );
			pFile->Write(L"", sizeof(wchar_t));

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
// *********************************   CGMEClosureDataSource  *****************************************
// ****************************************************************************************************
BOOL CGMEClosureDataSource::OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile)
{
	if( lpFormatEtc->cfFormat == CF_UNICODETEXT )
	{
		ASSERT( pFile != NULL );

		// TODO: we have to dump it directly to the shared memory file

		try
		{
			TCHAR *fname = _ttempnam(_T("c:\\temp"), _T("tmp"));
			CString filename = fname;
			free(fname);
		
			CComPtr<IMgaDumper> dumper;
			COMTHROW( dumper.CoCreateInstance(L"Mga.MgaDumper") );

			CComObjPtr<IMgaFCOs> fcos;
			if ( data) // is it set?
				COMTHROW( ::QueryInterface(data, fcos) );

			CComObjPtr<IMgaFolders> fols;
			if ( folders) // is it set?
				COMTHROW( ::QueryInterface( folders, fols) );

			bool is_top_set = false;
			CComObjPtr<IMgaFCOs> top_fcos;
			CComObjPtr<IMgaFolders> top_folds;

			if ( m_topFcos)
			{
				COMTHROW( ::QueryInterface( m_topFcos, top_fcos) );
				is_top_set = true;
			}

			if ( m_topFolders)
			{
				COMTHROW( ::QueryInterface( m_topFolders, top_folds) );
				is_top_set = true;
			}

			if ( is_top_set)
				COMTHROW( dumper->DumpClosR( fcos, fols, PutInBstr( filename), top_fcos, top_folds, m_options, m_absPathPart, m_acceptingKinds) );
			else // if top objects are not set dump starting from RootFolder
				COMTHROW( dumper->DumpClos( fcos, fols, PutInBstr( filename), m_options) );

			CFile file;
			if( file.Open(filename, CFile::modeRead | CFile::typeBinary) == 0 )
				return FALSE;

			const int buffsize = 10240;
			unsigned char buff[buffsize];
			UINT c;
			do
			{
				c = file.Read(buff, buffsize);
				pFile->Write(buff, c);
			} while( c == buffsize );

			pFile->Write(L"", sizeof(wchar_t));

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

BEGIN_MESSAGE_MAP(CGMEDataSource, COleDataSource)
	//{{AFX_MSG_MAP(CGMEDataSource)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Interface Maps

BEGIN_DISPATCH_MAP(CGMEDataSource, COleDataSource)
	//{{AFX_DISPATCH_MAP(CGMEDataSource)
	DISP_PROPERTY_EX(CGMEDataSource, "Data", DispGetData, DispSetData, VT_DISPATCH)
	DISP_PROPERTY_EX(CGMEDataSource, "Folders", DispGetFolders, DispSetFolders, VT_DISPATCH)
	DISP_PROPERTY_EX(CGMEDataSource, "RegistryData", DispGetRegistryData, DispSetRegistryData, VT_DISPATCH)
	DISP_PROPERTY_EX(CGMEDataSource, "Project", DispGetProject, DispSetProject, VT_DISPATCH)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


BEGIN_INTERFACE_MAP(CGMEDataSource, COleDataSource)
	INTERFACE_PART(CGMEDataSource, IID_IMgaDataSource, MgaDataSource)
	DUAL_ERRORINFO_PART(CGMEDataSource)
END_INTERFACE_MAP()

DELEGATE_DUAL_INTERFACE(CGMEDataSource, MgaDataSource)

// Implement ISupportErrorInfo to indicate we support the
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CGMEDataSource, IID_IMgaDataSource)


STDMETHODIMP CGMEDataSource::XMgaDataSource::get_Data(IUnknown **p)
{
	METHOD_PROLOGUE(CGMEDataSource, MgaDataSource)
	CHECK_OUT(p);

	if( pThis->data != NULL )
		return pThis->data.QueryInterface(p);

	return S_OK;
}

STDMETHODIMP CGMEDataSource::XMgaDataSource::get_Folders(IUnknown **p)
{
	METHOD_PROLOGUE(CGMEDataSource, MgaDataSource)
	CHECK_OUT(p);

	if( pThis->folders != NULL )
		return pThis->folders.QueryInterface(p);

	return S_OK;
}

STDMETHODIMP CGMEDataSource::XMgaDataSource::get_RegistryData(IUnknown **p)
{
	METHOD_PROLOGUE(CGMEDataSource, MgaDataSource)
	CHECK_OUT(p);

	if( pThis->regdata != NULL )
		return pThis->regdata.QueryInterface(p);

	return S_OK;
}

STDMETHODIMP CGMEDataSource::XMgaDataSource::get_Project(IUnknown **p)
{
	METHOD_PROLOGUE(CGMEDataSource, MgaDataSource)
	CHECK_OUT(p);

	return pThis->project.QueryInterface(p);
}




/*static*/ CComPtr<IGMEOLEApp> CGMEDataSource::get_GME(CComObjPtr<IMgaProject> project)
{
	CComPtr<IGMEOLEApp> gme;
	if ( (project != NULL)) {		
		CComBSTR bstrName(L"GME.Application");
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

LPDISPATCH CGMEDataSource::DispGetData() 
{
	CComPtr<IDispatch> p;
	if( data != NULL ) {
		data.QueryInterface(&p);
	}

	return p.Detach();
}

void CGMEDataSource::DispSetData(LPDISPATCH) 
{
	SetNotSupported();
}

LPDISPATCH CGMEDataSource::DispGetFolders() 
{
	CComPtr<IDispatch> p;
	if( folders != NULL ) {
		folders.QueryInterface(&p);
	}

	return p.Detach();
}

void CGMEDataSource::DispSetFolders(LPDISPATCH) 
{
	SetNotSupported();
}

LPDISPATCH CGMEDataSource::DispGetRegistryData() 
{
	CComPtr<IDispatch> p;
	if(  regdata != NULL ) {
		regdata.QueryInterface(&p);
	}

	return p.Detach();
}

void CGMEDataSource::DispSetRegistryData(LPDISPATCH) 
{
	SetNotSupported();
}

LPDISPATCH CGMEDataSource::DispGetProject() 
{
	CComPtr<IDispatch> p;
	if( project != NULL ) {
		COMTHROW(project.QueryInterface(&p));
	}

	return p.Detach();
}

void CGMEDataSource::DispSetProject(LPDISPATCH) 
{
	SetNotSupported();
}


