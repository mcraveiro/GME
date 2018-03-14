// ScriptEdit.h: interface for the CScriptEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTEDIT_H__5EF35F5A_41C7_4F59_A025_215986107399__INCLUDED_)
#define AFX_SCRIPTEDIT_H__5EF35F5A_41C7_4F59_A025_215986107399__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <comdef.h>
#include "Console.h"
#include "ScriptHost.h"
#include <vector>


_COM_SMARTPTR_TYPEDEF(IScriptHost, __uuidof(IScriptHost));

class CConsoleCtrl;
class CScriptEdit : public CEdit  
{
public:
	CScriptEdit();
	virtual ~CScriptEdit();
	bool Init(CConsoleCtrl *console);
	void SetGMEApp(IDispatch *disp);
	void SetGMEProj(IDispatch *dsp);
	void ExecuteScript( CString& p_str);
	inline void SetLoadedScript( const CString& p_script)  { m_loadedScript   = p_script; }
	inline void SetScriptFileName( const CString& p_fname) { m_loadedFileName = p_fname;  }
	inline CString& GetLoadedScript()                      { return m_loadedScript;       }
	inline CString& GetLoadedScriptFileName()              { return m_loadedFileName;     }
	void showPrev();
	void showNext();
	void returnHit();
protected:
	static const TCHAR* defPrompt;

	CConsoleCtrl *m_console;
	IScriptHostPtr m_host;
	typedef std::vector<CString> Strings;
	Strings m_inputlist;
	CString m_loadedScript;
	CString m_loadedFileName;

	DECLARE_MESSAGE_MAP()

	afx_msg void OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );

public:
	afx_msg void OnEnKillfocus();
	afx_msg void OnEnSetfocus();
};

#endif // !defined(AFX_SCRIPTEDIT_H__5EF35F5A_41C7_4F59_A025_215986107399__INCLUDED_)
