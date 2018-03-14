// TableEditorXMLDumper.h: interface for the TableEditorXMLDumper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TABLEEDITORDUMPER_H__8E6DCC47_5A1A_46D0_9CD7_CA0ADE359A49__INCLUDED_)
#define AFX_TABLEEDITORDUMPER_H__8E6DCC47_5A1A_46D0_9CD7_CA0ADE359A49__INCLUDED_


#include <fstream>
using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class TableEditorDumper  
{
public:
	TableEditorDumper();
	virtual ~TableEditorDumper();

// ------- Attributes
	
public:
	ofstream ofs;

public:
	void InitDump(CString filename, int numCols, int numRows);
	void DoneDump();
	void DumpCell(CString cell_type, CString cell_entry, BOOL multi, BOOL boolean);

// ------- Low level stuff
	void StartElem(const char *name);
	void StartElemAttr(const char *name);
	void Attr(const char *name, const char *value, BOOL last);
	void EndElem(const char *name);

};

#endif // !defined(AFX_TABLEEDITORDUMPER_H__8E6DCC47_5A1A_46D0_9CD7_CA0ADE359A49__INCLUDED_)
