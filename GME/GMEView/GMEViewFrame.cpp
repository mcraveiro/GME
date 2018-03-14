// GMEViewFrame.cpp : implementation file
//

#include "stdafx.h"
#include "GMEViewFrame.h"
#include "GMEViewCtrl.h"
#include "GMEViewStd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CGMEViewFrame dialog

int			CGMEViewFrame::instanceCount = 0;
CString		CGMEViewFrame::strMyViewWndClass;


IMPLEMENT_DYNCREATE(CGMEViewFrame, CMiniFrameWnd)

CGMEViewFrame::CGMEViewFrame()
{
	instanceCount++;

	//{{AFX_DATA_INIT(CGMEViewFrame)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

}

CGMEViewFrame::~CGMEViewFrame()
{
}

void CGMEViewFrame::DoDataExchange(CDataExchange* pDX)
{
	CMiniFrameWnd::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGMEViewFrame)
//	DDX_Control(pDX, IDC_ASPECT_TAB, tab);
	//}}AFX_DATA_MAP
}

BOOL CGMEViewFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~WS_CAPTION;
	cs.style |= WS_CHILD;
	return CMiniFrameWnd::PreCreateWindow(cs);
}

void CGMEViewFrame::SetCurrentProject(CComPtr<IMgaProject> project)
{
	mgaProject = project;
	viewWnd.SetCurrentProject(project);
}

void CGMEViewFrame::SetMetaModel(CComPtr<IMgaMetaModel> meta)
{
	InitPropBarFromMeta(meta);
	viewWnd.SetMetaModel(meta);
}

void CGMEViewFrame::SetModel(CComPtr<IMgaModel> model)
{
	InitPropBarFromModel(model);
	viewWnd.SetModel(model);
}

void CGMEViewFrame::ChangeAspect(int aspect)
{
	SetAspectProperty(aspect);
	viewWnd.ChangeAspect(aspect + 1);
}

void CGMEViewFrame::Invalidate(void)
{
	// TODO?
	viewWnd.Invalidate();
}

void CGMEViewFrame::InitPropBarFromMeta(CComPtr<IMgaMetaModel> meta)
{
	InitAspectBox(meta);
	InitModelKindName(meta);
}

void CGMEViewFrame::InitPropBarFromModel(CComPtr<IMgaModel> mgaModel)
{
	if (mgaModel) {
		try	{
			long status;
			COMTHROW(mgaProject->get_ProjectStatus(&status));
			bool inTrans = (status & 0x08L) != 0;
			CComPtr<IMgaTerritory> terr;
			if (!inTrans) {
				COMTHROW(mgaProject->CreateTerritory(NULL, &terr));
				COMTHROW(mgaProject->BeginTransaction(terr, TRANSACTION_READ_ONLY));
			} else {
				COMTHROW(mgaProject->get_ActiveTerritory(&terr));
			}

			CComPtr<IMgaFCO> modelFco;
			COMTHROW(terr->OpenFCO(mgaModel, &modelFco));
			CComQIPtr<IMgaModel> model = modelFco;

			status = OBJECT_ZOMBIE;
			COMTHROW(model->get_Status(&status));
			if (status == OBJECT_EXISTS) {
				InitModelName(model);
				InitModelType(model);
			}

			if (!inTrans) {
				mgaProject->CommitTransaction();
			}
		} catch(...) {
			ASSERT(0);
		}
	}
}

void CGMEViewFrame::InitAspectBox(CComPtr<IMgaMetaModel> meta)
{
	CComboBox* box = (CComboBox*) propBar.GetDlgItem(IDC_ASPECT);
	if (meta) {
		CComPtr<IMgaMetaAspects> mmAspects;
		COMTHROW(meta->get_Aspects(&mmAspects));
		CComPtr<IMgaMetaAspect> mmAspect;
		MGACOLL_ITERATE(IMgaMetaAspect, mmAspects) {
			mmAspect = MGACOLL_ITER;

			CComBSTR bstr;
			COMTHROW(mmAspect->get_DisplayedName(&bstr));
			CString cString;
			CopyTo(bstr, cString);
			box->AddString(cString);
		}
		MGACOLL_ITERATE_END;
	}
}

void CGMEViewFrame::InitModelKindName(CComPtr<IMgaMetaModel> meta)
{
	if (meta) {
		CString kindName;
		CComBSTR bstr;
		COMTHROW(meta->get_DisplayedName(&bstr));
		CopyTo(bstr, kindName);
		SetKindNameProperty(kindName);
	}
}

void CGMEViewFrame::InitModelName(CComPtr<IMgaModel> mgaModel)
{
	CString name;
	CComBSTR bstr;
	COMTHROW(mgaModel->get_Name(&bstr));
	CopyTo(bstr, name);
	SetNameProperty(name);
}

void CGMEViewFrame::InitModelType(CComPtr<IMgaModel> mgaModel)
{
	CComPtr<IMgaFCO> baseType = NULL;
	VARIANT_BOOL b;
	COMTHROW(mgaModel->get_IsInstance(&b));
	bool isType = (b == VARIANT_FALSE);
	if (isType) {
		COMTHROW(mgaModel->get_BaseType(&baseType));
	} else {
		COMTHROW(mgaModel->get_Type(&baseType));
	}
	SetTypeProperty(isType);
	InitModelTypeName(baseType, mgaModel);
}

void CGMEViewFrame::InitModelTypeName(CComPtr<IMgaFCO> baseType, CComPtr<IMgaModel> mgaModel)
{
	CComPtr<IMgaFCO> fco;
	CComBSTR bstr;
	CString typeName = "N/A";
	if (baseType != 0) {
		COMTHROW(baseType->get_Name(&bstr));
		CopyTo(bstr, typeName);
	}
	SetTypeNameProperty(typeName);
}


BEGIN_MESSAGE_MAP(CGMEViewFrame, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CGMEViewFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_WINDOWPOSCHANGED()
	ON_CBN_SELCHANGE(IDC_ASPECT, OnSelchangeAspectBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMEViewFrame message handlers

BOOL CGMEViewFrame::OnInitDialog()
{
//	CMiniFrameWnd::OnInitDialog();

	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}

int CGMEViewFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add a menu item that will toggle the visibility of the
	// dialog bar named "ModelPropertiesBar":
	//   1. In ResourceView, open the menu resource that is used by
	//      the CChildFrame class
	//   2. Select the View submenu
	//   3. Double-click on the blank item at the bottom of the submenu
	//   4. Assign the new item an ID: ID_GMEVIEW_MODELPROPERTIESBAR
	//   5. Assign the item a Caption: ModelPropertiesBar

	// TODO: Change the value of ID_GMEVIEW_MODELPROPERTIESBAR to an appropriate value:
	//   1. Open the file resource.h
	// CG: The following block was inserted by the 'Dialog Bar' component
	{
		// Initialize dialog bar propBar
		if (propBar.Create(this, IDD_MODELPROPERTIESBAR,
			CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_HIDE_INPLACE,
			IDC_MODELPROPERTIESBAR) == FALSE)
		{
			TRACE0("Failed to create GME View dialog bar propBar\n");
			return -1;
			// fail to create
		}
		int zoomvals[] = { ZOOM_MIN, 10, 25, 50, 75, ZOOM_NO, 150, 200, 300, ZOOM_MAX, ZOOM_WIDTH, ZOOM_HEIGHT, ZOOM_ALL, 0 };
//		int zoomvals[] = { ZOOM_NO, 150, 200, 250, 300, 350, 400, 0 }; // for test
		propBar.SetZoomList(zoomvals);
	}

	{
		if (strMyViewWndClass.IsEmpty()) {
			try {
				strMyViewWndClass = AfxRegisterWndClass(
					CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW,
					::LoadCursor(NULL, IDC_ARROW),
					(HBRUSH) ::GetStockObject(WHITE_BRUSH),
					::LoadIcon(NULL, IDI_APPLICATION));
			}
			catch (CResourceException* pEx) {
				TRACE0("Couldn't register class! (Already registered?)");
				pEx->Delete();
			}
		}
		// TODO
		CRect rectClient;
		GetClientRect(&rectClient);
		if (!viewWnd.Create(strMyViewWndClass, "GME View Window", WS_VISIBLE | WS_CHILD, rectClient, this, 0)) {
			TRACE0("Failed to create GME View dialog\n");
			return -1;
		}
	}

	return 0;
}

void CGMEViewFrame::OnClose()
{
	CMiniFrameWnd::OnClose();

	// CMDIChildWnd::OnClose: when the last ChildWnd is closed
	// the document is considered closed and the title changes to Paradigm
	// that's why we call this:
//	theApp.UpdateMainTitle();
}

void CGMEViewFrame::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	CMiniFrameWnd::OnWindowPosChanged(lpwndpos);

	CRect propBarRect;
	propBar.GetWindowRect(&propBarRect);

	CRect newModelDialogBarRect;
	GetClientRect(&newModelDialogBarRect);
	newModelDialogBarRect.top += propBarRect.Height();
	viewWnd.MoveWindow(newModelDialogBarRect, TRUE);
}

void CGMEViewFrame::OnSelchangeAspectBox(void)
{
	int ind = GetAspectProperty();
	if (ind >= 0) {
		viewWnd.ChangeAspect(ind + 1);
		SendAspectChange(ind);
	}
}

int CGMEViewFrame::GetAspectProperty(void) const
{
	CComboBox* box;
	box = (CComboBox*) propBar.GetDlgItem(IDC_ASPECT);
	ASSERT(box);
	return box->GetCurSel();
}

void CGMEViewFrame::SetAspectProperty(int ind)
{
	CComboBox* box;
	box = (CComboBox*) propBar.GetDlgItem(IDC_ASPECT);
	ASSERT(box);
	box->SetCurSel(ind);
}

void CGMEViewFrame::SetKindNameProperty(CString& kindName)
{
	CWnd* ctrl;
	ctrl = propBar.GetDlgItem(IDC_KINDNAME);
	ASSERT(ctrl);
	ctrl->SetWindowText(kindName);
}

void CGMEViewFrame::GetNameProperty(CString& txt) const
{
	CWnd* ctrl;
	ctrl = propBar.GetDlgItem(IDC_NAME);
	ASSERT(ctrl);
	ctrl->GetWindowText(txt);
}

void CGMEViewFrame::SetNameProperty(CString& name)
{
	CWnd* ctrl;
	ctrl = propBar.GetDlgItem(IDC_NAME);
	ASSERT(ctrl);
	ctrl->SetWindowText(name);
	// TODO?
//	RetrievePath();
//	frame->title = name + " - " + path;
//	frame->OnUpdateFrameTitle(true);
}

void CGMEViewFrame::SetTypeProperty(bool isType)
{
	if (isType)
		propBar.ShowType();
	else
		propBar.ShowInstance();
}

void CGMEViewFrame::SetTypeNameProperty(CString& typeName)
{
	CWnd* ctrl;
	ctrl = propBar.GetDlgItem(IDC_TYPENAME);
	ASSERT(ctrl);
	ctrl->SetWindowText(typeName);
//	RetrievePath();
//	frame->title = name + " - " + path;
//	frame->OnUpdateFrameTitle(true);
}

void CGMEViewFrame::SendAspectChange(long index)
{
	CWnd* wnd = GetParent();
	if (wnd->IsKindOf(RUNTIME_CLASS(CGMEViewCtrl))) {
		CGMEViewCtrl* ctrl = STATIC_DOWNCAST(CGMEViewCtrl, wnd);
		ctrl->SendAspectChanged(index);
	}
}

void CGMEViewFrame::SendZoomChange(long index)
{
	CWnd* wnd = GetParent();
	if (wnd->IsKindOf(RUNTIME_CLASS(CGMEViewCtrl))) {
		CGMEViewCtrl* ctrl = STATIC_DOWNCAST(CGMEViewCtrl, wnd);
		ctrl->SendZoomChanged(index);
	}
}

void CGMEViewFrame::SendWriteStatusZoom(long zoomVal)
{
	CWnd* wnd = GetParent();
	if (wnd->IsKindOf(RUNTIME_CLASS(CGMEViewCtrl))) {
		CGMEViewCtrl* ctrl = STATIC_DOWNCAST(CGMEViewCtrl, wnd);
		ctrl->SendWriteStatusZoom(zoomVal);
	}
}
