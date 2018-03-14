#include "stdafx.h"
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

 
#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include "BONComponent.h"

#define PATLINELENGTH 500



class  FileGenerator {
private:
	FILE *fo;
	CList<CString *, CString *> outstrings;
	CMapStringToString vars;
	void outchannel_push(CString &buf) {
			outstrings.AddHead(&buf);
	}
	void outchannel_pop() {
			outstrings.RemoveHead();
	}

	void outc(CString str) {
		if(!outstrings.IsEmpty()) {
			*outstrings.GetHead() += str;
		}
		else if(fo) fputs(str, fo); 
		else  {
				str.TrimLeft();
				if(!str.IsEmpty()) throw CString("Nowhere to write generated text");
		}
	}
	void outc(char c) {
		if(!outstrings.IsEmpty()) {
			*outstrings.GetHead() += c;
		}
		else if(fo) fputc(c, fo); 
		else {   // it it OK to silently ignore whitespace
				if(!isspace(c)) throw CString("Nowhere to write generated text");
		}
	}
	int readarg(const char *pptr, CString &outstr, char term);
	void evaluate_line(const char * line, CBuilderObject* context, bool escape);
	int evaluate_cmd(const char *ptr, char *cmd, CBuilderObject* context);
public:
	void Define(CString name, CString value) {
				vars.SetAt(name, value);
	}
	FileGenerator() {
		fo = NULL;			// no output file
	}
	int GenFile(const char *patfilename, CBuilderObject* context);
};



void eatss(const char *&ptr, CString &outstr);
// on entry, ptr points to the opening '"'
// on exit, ptr points past the closing '"', but both '"' are omitted from the output.
void eatdq(const char *&ptr, CString &outstr) {
		char c = 0;
		if(*ptr != '"') {    // non-quoted argument
			while(*ptr != ',' && *ptr != ')') {
			  if(*ptr == '$' && c != '\\') eatss(ptr, outstr);
			  else {
				c = *ptr++;
				if(c == '\0') throw 0;
				outstr += c;
			  }
			}
			return;
		}
		else {				   //  quoted argument
			ptr++;
			while(*ptr != '"' || c == '\\') {
				if(*ptr == '$' && c != '\\') eatss(ptr, outstr);
				else {
					c = *ptr++;
					if(c == '\0') throw 0;
					outstr += c;
				}
			}
			if(*ptr++ != '"') throw 0;
		}
}

// on entry ptr points to the $
// on exit, ptr points one char behind the closing ')'     both characters are printed
void eatss(const char *&ptr, CString &outstr) {
	bool cmd = false;
	if(*ptr != '$') throw 0;
	outstr += *ptr++;
	if(*ptr == '(') {
		char c;
		do  {
			c = *ptr;
			if(c == '\0') throw 0;
			if(c == '"') {
				outstr += '"';
				eatdq(ptr, outstr);
				outstr += '"';
			}
			else if(c == '$') eatss(ptr, outstr);
			else outstr += *ptr++;
		} while(c != ')');
	}
	else {
		if(*ptr == '!') {
			cmd = true;
			outstr += *ptr++;
		}
		while(isalnum(*ptr) || *ptr == '_') {
			outstr += *ptr++;
		}
	}
	if(cmd && *ptr == '(') {
		char c;
		do  {
			c = *ptr;
			if(c == '\0') throw 0;
			if(c == '"') {
				outstr += '"';
				eatdq(ptr, outstr);
				outstr += '"';
			}
			else if(c == '$') eatss(ptr, outstr);
			else outstr += *ptr++;
		} while(c != ')');
	}
}

#define ANY_TERM ((char)0xFF)
// copy buffer until first unescaped ' " '
int FileGenerator::readarg(const char *pptr, CString &outstr, char term) {
	const char *ptr = pptr;
	try {
		while(isspace(*ptr)) ptr++;
		eatdq(ptr, outstr);
		while(isspace(*ptr)) ptr++;
		if(*ptr++ != term && term != ANY_TERM) throw 0;
	}
	catch(int) {
		throw CString("Error reading argument: ") + pptr;
	};
	return ptr - pptr;
}


int FileGenerator::evaluate_cmd(const char *pptr, char *cmd, CBuilderObject* context) {
	const char *ptr = pptr;
	bool single;
	if((single = !strcmp(cmd, "EVAL_WITH")) ||
	   !strcmp(cmd, "EVAL_FORALL")) {
		CString arg1; ptr += readarg(ptr, arg1, ',');
		CString arg2; ptr += readarg(ptr, arg2, ')');
		int k = arg1.Find(':');		
		if(k++ <= 0) k = arg1.GetLength(); 
		CString mode = arg1.Left(k);
		arg1.Delete(0,k);
		CBuilderObjectList ll;
		if(mode == "R:") {
			if(!context->IsKindOf(RUNTIME_CLASS(CBuilderModel))) throw CString("Roles accessed ('R:') on non-model");
			CBuilderObjectList const*list = ((CBuilderModel *)context)->GetChildren();
			POSITION pos = list->GetHeadPosition();
			while(pos) {
				CBuilderObject *m = list->GetNext(pos);
				if(m->GetPartName() == arg1) ll.AddTail(m);
			}
		}
		else if(mode == "Src:") {
			context->GetInConnectedObjects(arg1,ll);
		}
		else if(mode == "Dst:") {
			context->GetOutConnectedObjects(arg1,ll);
		}
		else if(mode == "Ref") {
			const CBuilderObject *p;
			if(context->IsKindOf(RUNTIME_CLASS(CBuilderModelReference))) p = ((CBuilderModelReference *)context)->GetReferred();
			else if(context->IsKindOf(RUNTIME_CLASS(CBuilderAtomReference))) p = ((CBuilderAtomReference *)context)->GetReferred();
			else throw CString("Ref accessed ('Ref:') on non-reference");
			if(p != NULL) ll.AddTail(const_cast<CBuilderObject *>(p));
		}
		else if(mode == "Parent") {
			const CBuilderObject *p = context->GetParent();
			if(p != NULL) ll.AddTail(const_cast<CBuilderObject *>(p));
		}
		else if(mode == "Type") {
			const CBuilderObject *p = context->GetType();
			if(p != NULL) ll.AddTail(const_cast<CBuilderObject *>(p));
		}
		else throw CString("Unknown EVAL_xxx mode: ") + mode;
		if(single && ll.GetCount() != 1) throw CString("EVAL_WITH results not a single object") + arg1;
		POSITION pos = ll.GetHeadPosition();
		while(pos) {
			const CBuilderObject *m = ll.GetNext(pos);
			if(m->IsKindOf(RUNTIME_CLASS(CBuilderReferencePort))) m = ((CBuilderReferencePort *)m)->GetAtom();
			evaluate_line(arg2, const_cast<CBuilderObject *>(m), true);
		}
	}
	else if(!strcmp(cmd, "DEFINE")) {
		CString arg1; ptr += readarg(ptr, arg1, ',');
		CString arg2; ptr += readarg(ptr, arg2, ')');
		CString b;
		outchannel_push(b);
		evaluate_line(arg2, context, true);
		outchannel_pop();
		Define(arg1, b);
	}
	else if(!strcmp(cmd, "IFDEFAULT")) {
		CString arg1; ptr += readarg(ptr, arg1, ',');
		CString arg2; ptr += readarg(ptr, arg2, ')');
		CString b;
		context->GetAttribute(arg1, b);
		if(b.IsEmpty()) {
			outchannel_push(b);
			evaluate_line(arg2, context, true);
			outchannel_pop();
		}
		outc(b);
	}
	else if(!strcmp(cmd, "IFEMPTY")) {
		CString arg1; ptr += readarg(ptr, arg1, ',');
		CString arg2; ptr += readarg(ptr, arg2, ')');
		CString b;
		outchannel_push(b);
		evaluate_line(arg1, context, true);
		if(b.IsEmpty()) evaluate_line(arg2, context, true);
		outchannel_pop();
		outc(b);
	}
	else if(!strcmp(cmd, "TO_FILE")) {
		CString b;
		outchannel_push(b);
		CString arg; ptr += readarg(ptr, arg, ')');
		evaluate_line(arg, context, true);
		outchannel_pop();
		if(fo) fclose(fo);
		fo = fopen(b, "wt");
		if(fo == NULL) throw CString("Cannot open output file")+b;
	}
	else if(!strcmp(cmd, "POSTINCR")) {
		CString arg; ptr += readarg(ptr, arg, ')');
		CString attrval;
		if(!vars.Lookup(CString(arg), attrval)) throw CString("POSTINCR: undefined var: ") + arg;
		outc(attrval);
		int n = atoi(attrval);
		attrval.Format("%d",n+1);
		Define(arg, attrval);
	}
	else if(!strcmp(cmd, "SEQ")) {

		do {
				CString arg1; ptr += readarg(ptr, arg1, ANY_TERM);
				evaluate_line(arg1, context, true);
		} while(ptr[-1] == ',');
		if(ptr[-1] != ')') throw CString("Unbalanced '(' ')' for SEQ");
//		CString b;
//		outchannel_push(b);
//		evaluate_line(arg2, context, true);
//		outchannel_pop();
//		outc(b);
	}
	else if(!strcmp(cmd, "GETPRINTFIELD")) {
		CString arg; ptr += readarg(ptr, arg, ')');
		CString b;
		outchannel_push(b);
		evaluate_line(arg, context, true);
		outchannel_pop();
		CString code;
		if(!strcmp(b,"int")) code = "%d";
		else if(!strcmp(b,"long")) code = "%ld";
		else if(!strcmp(b,"float")) code = "%f";
		else if(!strcmp(b,"double")) code = "%f";
		else throw CString("Cannot generate printf field for ")+b;
		outc(code);
	}
	else if(!strcmp(cmd, "COMMENT")) {
		do {
				CString arg1; ptr += readarg(ptr, arg1, ANY_TERM);
		} while(ptr[-1] == ',');
	}
	return ptr - pptr;
}


void FileGenerator::evaluate_line(const char * line, CBuilderObject* context, bool escape) {
			const char *ptr = line;
			bool in_esc = false;
			while( *ptr != '\0') {
				char c = *ptr++;
				if(escape && in_esc) {
						in_esc = false;
						if(c == 'n') outc('\n');
						else		 outc(c);
				}
				else if(escape && c == '\\') in_esc = true;
				else if(c != '$') {
					outc(c);
				}
				else if(!escape && *ptr == '$') outc(*ptr++);
				else {								// it is a $macro
					bool endparen = false;
					bool command = false;
					if(*ptr == '!') {
						command = true;
						ptr++;
					}
					if(*ptr == '(') {
						endparen = true;
						ptr++;
					}
					char cmd[56];
					int add;
					if(sscanf(ptr, "%53[A-Za-z_0-9]%n", cmd, &add) != 1 ||
						add > 50 ||
						(endparen && ptr[add++] != ')') ||
						(command && ptr[add++] != '(')
												) throw CString("Invalid $macro") ;
					ptr += add;
					if(command) {					// it is a $!macro
						ptr += evaluate_cmd(ptr, cmd, context);
					}
					else {							// it is a simple macro
						CString attrval;
						if(vars.Lookup(CString(cmd), attrval));
						else if(!strcmp(cmd, "Name")) attrval = context->GetName();
						else if(!strcmp(cmd, "Kind")) attrval = context->GetKindName();
						else if(!context->GetAttribute(cmd, attrval)) throw(CString("Unknown attribute: ")+cmd);
						if(attrval == "") throw CString("Attribute is empty: ")+cmd;
						outc(attrval);
					}
				}
			}
}			

int FileGenerator::GenFile(const char *patfilename, CBuilderObject* context) {
	int line = 0;
	FILE *fi = fopen(patfilename, "rt");
	try {
		if(fi == 0) throw CString("File not found: ");
		int rounds;
		for(line = 1;  ; line += rounds) {
			char buf[PATLINELENGTH+3];
			int pos = 0;
			rounds = 0;
			while(1) {
				fgets(buf+pos, sizeof(buf)-pos, fi);
				if(feof(fi)) break;
				rounds++;
				pos = strlen(buf);
				if(pos > PATLINELENGTH + 1) throw(CString("Too long line"));
				if(pos >= 2 && buf[pos-2] == '\\') pos -= 2;
				else break;
			} 
			if(feof(fi)) break;
			evaluate_line(buf, context, false);
		}
		if(!fo) throw CString("No output is generated");
		fclose(fi);
		fclose(fo);
	}
	catch(CString e) {
		if(fi) fclose(fi);
		if(fo) fclose(fo);
		CString linestr;
		linestr.Format("%d", line);
		AfxMessageBox(CString("Error: ") + e + "\nfile: " + CString(patfilename) + " line: " + linestr); 
	}
	return 0;
}




void CComponent::InvokeEx(CBuilder &builder,CBuilderObject *focus, CBuilderObjectList &selected, long param) 
{
	if (focus == NULL) {
		AfxMessageBox("Please select an element in an opened model!", MB_OK | MB_ICONSTOP);
		return;
	}

	CString script(builder.GetParameter("script"));

	if(script.IsEmpty()) {
		static char filter[] = "Pattern Files (*.pat)|*.pat|" "All Files (*.*)|*.*||";

		CFileDialog cfd(true, "pat", NULL, OFN_EXPLORER | OFN_FILEMUSTEXIST, filter);
		if( cfd.DoModal() != IDOK )
			return;
		script = cfd.GetPathName();
	}

	FileGenerator gen;
	gen.GenFile(script, focus);
}


