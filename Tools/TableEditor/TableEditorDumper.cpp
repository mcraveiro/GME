// TableEditorXMLDumper.cpp: implementation of the TableEditorXMLDumper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TableEditorDumper.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TableEditorDumper::TableEditorDumper()
{
	
}

TableEditorDumper::~TableEditorDumper()
{
	
}

// --------------------------- TableEditorXMLDumper

void TableEditorDumper::InitDump(CString filename, int numCols, int numRows) 
{
	ASSERT( !ofs.is_open() );
	ofs.open(filename, ios::out | ios::trunc);

// Manually write out header necessary for Excel to open spreadsheet properly
	ofs << "<?xml version=\"1.0\"?>\n";
	ofs << "<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n";
	ofs << " xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n";
	ofs << " xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n";
	ofs << " xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\">\n";
	StartElem("Styles");
	ofs << "<Style ss:ID=\"Default\" ss:Name=\"Normal\">\n";
	EndElem("Style");
	ofs << "<Style ss:ID=\"s21\">\n";
	ofs << "<Alignment ss:Vertical=\"Bottom\" ss:WrapText=\"1\"/>\n";
	EndElem("Style");
	EndElem("Styles");	
	StartElemAttr("Worksheet");
	Attr("ss:Name", "temp", TRUE);	
	ofs << "\n";
	StartElemAttr("Table");

	char numBuf[256];
	_itoa(numCols,numBuf,10);
	Attr("ss:ExpandedColumnCount",numBuf, FALSE);
	_itoa(numRows,numBuf,10);
	Attr("ss:ExpandedRowCount",numBuf, FALSE);
	Attr("x:FullColumns","1",FALSE);
	Attr("x:FullRows","1",TRUE);
}

void TableEditorDumper::DoneDump()
{

	EndElem("Table"); // </Table>
	
	EndElem("Worksheet"); // </Worksheet>
	
	EndElem("Workbook"); // </Workbook>
	
	ofs.close();
}


void TableEditorDumper::DumpCell(CString cell_type, CString cell_entry, BOOL multi, BOOL boolean)
{

	//xml special chars
	cell_entry.Replace("&", "&amp;");
	cell_entry.Replace("<","&lt;");
	cell_entry.Replace(">","&gt;");
	
	
	// First checks if cell entry is multiline. w/o specifying styleID, little boxes show up 
	if( multi ) 
	{
		StartElemAttr("Cell");
		Attr("ss:StyleID", "s21", TRUE);
		cell_entry.Replace("\n", "&#10;"); // to preserve multiline
	}
	else {
		StartElem("Cell");
	}

// Checks if cell entry is boolean. w/o converting to 1 / 0, gets Table strict parse error
	if( boolean )
	{
		cell_entry.Replace("true", "1");
		cell_entry.Replace("false", "0");
	}
	
	StartElemAttr("Data");
	Attr("ss:Type", cell_type, TRUE);
	ofs << cell_entry;
	EndElem("Data"); // </Data>
	EndElem("Cell"); // </Cell>
}


// ------- Low level stuff

inline void TableEditorDumper::StartElem(const char *name)
{
	ASSERT( name != NULL );
	ofs << '<' << name << '>';
}

inline void TableEditorDumper::StartElemAttr(const char *name)
{
	ASSERT( name != NULL );
	ofs << '<' << name;
}

inline void TableEditorDumper::Attr(const char *name, const char *value, BOOL last)
{
	ASSERT( name != NULL );
	ASSERT( value != NULL );

  	ofs << ' ' << name << "=\"" << value << "\"";
	if(last) ofs << ">";
}

inline void TableEditorDumper::EndElem(const char *name)
{
	ofs << "</" << name << ">\n";
}