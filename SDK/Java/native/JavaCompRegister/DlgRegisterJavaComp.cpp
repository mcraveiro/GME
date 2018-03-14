#include "stdafx.h"
#include "JavaCompRegister.h"
#include "DlgRegisterJavaComp.h"
#include "CommonError.h"
#include "CommonSmart.h"
#include "CommonMfc.h"
#include <MgaUtil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRegisterJavaComp dialog


CDlgRegisterJavaComp::CDlgRegisterJavaComp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRegisterJavaComp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRegisterJavaComp)
	m_strClass = _T("");
	m_strClassPath = _T("");
	m_strName = _T("");
	m_strGuid = _T("");
	m_strProgid = _T("");
	m_strDesc = _T("");
	m_strParadigm = _T("*");
	m_strMenu = _T("");
	m_boolSystemwide = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgRegisterJavaComp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRegisterJavaComp)
	DDX_Text(pDX, IDC_EDIT_CLASS, m_strClass);
	DDX_Text(pDX, IDC_EDIT_CLASSPATH, m_strClassPath);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_STATIC_GUID, m_strGuid);
	DDX_Text(pDX, IDC_STATIC_PROGID, m_strProgid);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_strDesc);
	DDX_Text(pDX, IDC_EDIT_PARADIGMS, m_strParadigm);
	DDX_Text(pDX, IDC_EDIT_MENU, m_strMenu);
	DDX_Check(pDX, IDC_CHECK_SYSTEMWIDE, m_boolSystemwide);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRegisterJavaComp, CDialog)
	//{{AFX_MSG_MAP(CDlgRegisterJavaComp)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeEditName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRegisterJavaComp message handlers

BOOL CDlgRegisterJavaComp::OnInitDialog() 
{
	CDialog::OnInitDialog();

    // set guid
    CoCreateGuid( &m_guid );
    OLECHAR * guidstr;
    StringFromCLSID( m_guid, &guidstr );
    m_strGuid = guidstr;
    CoTaskMemFree( guidstr );
    UpdateData(FALSE);

    OnChangeEditName();

	return TRUE;
}

void CDlgRegisterJavaComp::OnOK()
{
    char  buf[200];
    DWORD len = 200;

    try
	{
        UpdateData(TRUE);

        if(m_strName.IsEmpty())
        {
            AfxMessageBox("Name cannot be empty!" );
            return;
        }
        if(m_strClassPath.IsEmpty())
        {
            AfxMessageBox("ClassPath cannot be empty!" );
            return;
        }
        if(m_strClass.IsEmpty())
        {
            AfxMessageBox("Class cannot be empty!" );
            return;
        }

        // get JavaCompRunner.dll path
        char  dllPath[200];
        HKEY  dllPAthKey = 0;
        len = 200;
        RegOpenKeyEx(HKEY_CLASSES_ROOT, "CLSID\\{C97618C3-2574-4257-A6F9-6C3F497DCCED}\\InProcServer32", 0, KEY_READ, &dllPAthKey);
        if(RegQueryValueEx(dllPAthKey, "", NULL, NULL, (unsigned char*)dllPath, &len ) != ERROR_SUCCESS)
        {
            AfxMessageBox( "Error! Could not register java component. JavaCompRunner.dll is not registered properly." );
            return;
        }
        dllPath[len] = '\0';
      
        // register under CLSID
        sprintf(buf, "CLSID\\%s\\InprocServer32", m_strGuid);
        CRegKey classKey;
        classKey.Create(HKEY_CLASSES_ROOT, buf);
        classKey.SetKeyValue("", dllPath);

        // register progid
        sprintf(buf, "%s\\CLSID", m_strProgid);
        CRegKey progIDkey;
        progIDkey.Create(HKEY_CLASSES_ROOT, buf);
        progIDkey.SetKeyValue("", m_strGuid);

        // register gme component
        HKEY root = HKEY_CURRENT_USER;
        if(m_boolSystemwide)
            root = HKEY_LOCAL_MACHINE;
        HKEY gmeCompKey = 0;
        sprintf(buf, "SOFTWARE\\GME\\Components\\%s", m_strProgid);
        if(RegCreateKey(root, buf, &gmeCompKey)==ERROR_SUCCESS)
        {
            RegSetValueEx(gmeCompKey, "Description", 0, REG_SZ, (unsigned char*)(LPCSTR)m_strDesc, m_strDesc.GetLength());
            RegSetValueEx(gmeCompKey, "Icon", 0, REG_SZ, (unsigned char*)",IDI_COMPICON", 13);
            RegSetValueEx(gmeCompKey, "Paradigm", 0, REG_SZ, (unsigned char*)(LPCSTR)m_strParadigm, m_strParadigm.GetLength());
            RegSetValueEx(gmeCompKey, "Tooltip", 0, REG_SZ, (unsigned char*)(LPCSTR)m_strMenu, m_strProgid.GetLength());
            DWORD d = 1;
            RegSetValueEx(gmeCompKey, "Type", 0, REG_DWORD, (unsigned char*)&d, sizeof(DWORD));
            RegSetValueEx(gmeCompKey, "JavaClassPath", 0, REG_SZ, (unsigned char*)(LPCSTR)m_strClassPath, m_strClassPath.GetLength());
            RegSetValueEx(gmeCompKey, "JavaClass", 0, REG_SZ, (unsigned char*)(LPCSTR)m_strClass, m_strClass.GetLength());
            // register paradigms
            HKEY gmeCompAssocKey = 0;
            sprintf(buf, "SOFTWARE\\GME\\Components\\%s\\Associated", m_strProgid);
            RegCreateKey(root, buf, &gmeCompAssocKey);
            m_strParadigm += ' ';
			m_strParadigm.TrimLeft(" \n\t");
			while( !m_strParadigm.IsEmpty() )
			{
				int i = m_strParadigm.FindOneOf(" \n\t");
				ASSERT( i > 0 );
                RegSetValueEx(gmeCompAssocKey, m_strParadigm.Left(i), 0, REG_SZ, NULL, 0);
				m_strParadigm = m_strParadigm.Mid(i);
				m_strParadigm.TrimLeft(" \n\t");
			}

            CString msg = m_strName + " has been registered successfully";
            AfxMessageBox(msg);
        }
        else
        {
            AfxMessageBox("Cannot register component. Turn off 'Register systemwide' and try again!" );
            return;
        }
    }
    catch(...)
    {
        AfxMessageBox("Exception");
    }

            
    CDialog::OnOK();
}

void CDlgRegisterJavaComp::OnChangeEditName()
{
    UpdateData(TRUE);
    m_strProgid = "Mga.Interpreter." + m_strName;
    UpdateData(FALSE);
}
