#include "stdafx.h"
#include <stdio.h>
#include "BONComponent.h"

#define DEFAULT_PRIORITY 100

#define INT_TYPE			0	// must match order in edf
#define DOUBLE_TYPE			1
#define CHAR_TYPE			2
#define POINTER_TYPE		3

int idCounter = 0;
CParameterList CParameterBuilder::instances;
CPrimitiveTable CPrimitiveBuilder::contextTable;

bool IsValidName(const char *name)
{
	char c = *(name++);
	if(!__iscsymf(c))
		return false;
	while((c = *(name++)) != 0)
		if(!iscsym(c))
			return false;
	return true;
}

void CComponent::CheckScriptTypes (CPrimitiveList* primitives)
{
	ScriptCollection            keepAcuracy;
	ScriptDescriptor*           currentDescription;
 	const CBuilderAtomList*     inparams;
	const CBuilderAtomList*     params;
  
	POSITION listPos = primitives->GetHeadPosition();

	while (listPos)
	{
	  CPrimitiveBuilder *primitive = primitives->GetNext(listPos);

	  if (primitive->GetName())
	  {
  		inparams = primitive->GetAtoms("InputParameters");
		params   = primitive->GetAtoms("Parameters");
		int numberParams = inparams->GetCount() + params->GetCount();
		if (numberParams > 0) {
	     POSITION inparamPos = inparams->GetHeadPosition();
         currentDescription = new ScriptDescriptor(primitive->GetName(), primitive->GetScriptName(), numberParams);
      
		 while (inparamPos)
		 {
		   CInputParameterBuilder* inparam = (CInputParameterBuilder*)inparams->GetNext(inparamPos);
           currentDescription->AddParamLine(inparam->GetName(),inparam->GetPartName(), inparam->GetSize(), inparam->GetDataType());
 		 }

         POSITION paramPos = params->GetHeadPosition();

		 while (paramPos)
		 {
		   CParameterBuilder* param = (CParameterBuilder*)params->GetNext(paramPos);
	   	   currentDescription->AddParamLine(param->GetName(), param->GetPartName(), param->GetSize(), param->GetDataType());
		 }
	      int isOk = keepAcuracy.AddDescriptor(currentDescription);
		  if (isOk)
		  { AfxMessageBox("The primitive model " + primitive->GetName() + " with script: '" + primitive->GetScriptName() + "' has different parameters than " + keepAcuracy.GetModelName(isOk - 1) + " which has the same script name."); }
    	}
	
	  }
	}
   
}

void CComponent::InvokeEx(CBuilder &builder,CBuilderObject *focus, CBuilderObjectList &selected, long param)
{
	// ASSERT(!(selected.IsEmpty()));
	// CBuilderObject *sfBuilder = selected.GetHead();
	if (!focus && !selected.IsEmpty()) {
		focus = selected.GetHead();
	}
	if(!focus || !focus->IsKindOf(RUNTIME_CLASS(CCompoundBuilder))) {
		AfxMessageBox("Interpretation must start from a Compound model!");
		return;
	}
    
	CString filebase = focus->GetName();
	CString cfile = filebase + ".c";
	CString hfile = filebase + ".h";
    
												  

	CPrimitiveList primitives;
	((CProcessingBuilder *)focus)->Traverse(primitives);

    CheckScriptTypes(&primitives);

	FILE *fpt = fopen(hfile,"wt");
	if(!fpt) {
		AfxMessageBox("Error opening file " + hfile);
		return;
	}

	CPrimitiveBuilder::WriteScriptProto(fpt);
	CPrimitiveBuilder::WriteContextDefs(fpt);
	CParameterBuilder::WriteGlobals(fpt);
	fprintf(fpt,"\n");
    
	POSITION pos = primitives.GetHeadPosition();
	while(pos)
	  primitives.GetNext(pos)->WriteContextCreate(fpt); 
	
	
	
	fclose(fpt);

	fpt = fopen(cfile,"wt");
	if(!fpt) {
		AfxMessageBox("Error opening file " + cfile);
		return;
	}

	fprintf(fpt,"#include <string.h>\n");
	fprintf(fpt,"#include <mgk60.h>\n");
	fprintf(fpt,"#include \"%s\"\n\n",hfile);

	CParameterBuilder::WriteGlobals(fpt,false);
	fprintf(fpt,"\n");

	pos = primitives.GetHeadPosition();
	while(pos)
		primitives.GetNext(pos)->WriteContextCreate(fpt,false);
	fprintf(fpt,"\n");

	fprintf(fpt,"\nvoid register_scripts()\n{\n");
	CPrimitiveBuilder::WriteScriptRegs(fpt);
	fprintf(fpt,"}\n\n");

	fprintf(fpt,"\nvoid build()\n{\n");

	pos = primitives.GetHeadPosition();
	while(pos)
		primitives.GetNext(pos)->WriteActorCreate(fpt);
	fprintf(fpt,"\n");

	pos = primitives.GetHeadPosition();
	while(pos)
		primitives.GetNext(pos)->WriteSetContext(fpt);
	fprintf(fpt,"\n");

	pos = primitives.GetHeadPosition();
	while(pos)
		primitives.GetNext(pos)->WriteActorConnect(fpt);

	fprintf(fpt,"}\n");
	fclose(fpt);

	AfxMessageBox(cfile + " and " + hfile + " generated");

	idCounter = 0;
}

/////////////////////////// Signal Flow /////////////////////////

IMPLEMENT_CUSTOMMODEL(CProcessingBuilder, CBuilderModel, "")

IMPLEMENT_CUSTOMMODEL(CCompoundBuilder, CProcessingBuilder, "Compound")

void CCompoundBuilder::Traverse(CPrimitiveList &primitives)
{
	if(!IsValidName((LPCTSTR)GetName()))
		DisplayError("Invalid Name! You Must Use C Syntax!");
	const CBuilderModelList *list = GetModels();
	POSITION pos = list->GetHeadPosition();
	while(pos)
		((CProcessingBuilder *)(list->GetNext(pos)))->Traverse(primitives);
}

void CCompoundBuilder::Initialize()
{
}

IMPLEMENT_CUSTOMMODEL(CPrimitiveBuilder,CProcessingBuilder, "Primitive")

void CPrimitiveBuilder::Initialize()
{
	firing = 0;

	if(!IsValidName((LPCTSTR)GetName()))
		DisplayError("Invalid Name! You Must Use C Syntax!");

	static bool init = false;
	if(!init) {
		contextTable.InitHashTable(29);
		init = true;
	}
}

CPrimitiveBuilder::~CPrimitiveBuilder()
{
	contextTable.RemoveAll();
}

void CPrimitiveBuilder::SetPriority()
{
	priority = DEFAULT_PRIORITY;
	CString number;
	if(!GetAttribute("Priority",priority))
		DisplayError("Error getting Priority attribute!");
}

void CPrimitiveBuilder::SetScript()
{
	if(!GetAttribute("Script",script))
		DisplayError("Error getting Script attribute!");
	else {
		CPrimitiveBuilder *cntxdef = 0;
		if(!contextTable.Lookup(script,cntxdef))
			contextTable[script] = this;
	}
}

void CPrimitiveBuilder::SetFiring()
{
//	if(!GetAttribute("FiringCondition",firing))
	if(!GetAttribute("Firing",firing))
		DisplayError("Error getting Firing Condition attribute!");
}

void CPrimitiveBuilder::SetActorName()
{
	char txt[32];
	sprintf(txt,"_actor_%d",id);
	actorName = GetName() + txt;
}

void CPrimitiveBuilder::SetPorts(const CBuilderAtomList *ports)
{
	POSITION outer = ports->GetHeadPosition();
	while(outer) {
		CSignalBuilder *p1 = (CSignalBuilder *)(ports->GetNext(outer));
		POSITION inner = outer;
		while(inner) {
			CSignalBuilder *p2 = (CSignalBuilder *)(ports->GetNext(inner));
			((p1->yloc >= p2->yloc) ? p1 : p2)->IncrementPort();
		}
	}
}

void CPrimitiveBuilder::Traverse(CPrimitiveList &primitives)
{
	primitives.AddTail(this);
	id = idCounter++;
	SetScript();
	SetPriority();
	SetFiring();
	SetActorName();

	const CBuilderAtomList *list = GetAtoms("OutputSignals");
	ASSERT(list);
	outputPortNo = list->GetCount();
	POSITION pos = list->GetHeadPosition();
	int n = 0;
	while(pos) {
		CSignalBuilder *signal = (CSignalBuilder *)(list->GetNext(pos));
		signal->SetLocation();
		CBuilderAtomicObjectList conns;
		signal->GetDirectOutConnections("DataflowConn",conns);
		if(conns.IsEmpty())
			DisplayError("OutputSignal " + signal->GetName() + " not connected!");
		POSITION connPos = conns.GetHeadPosition();
		while(connPos) {
			CSignalBuilder *dst = (CSignalBuilder *)(conns.GetNext(connPos));
			ASSERT(dst);
			if(dst->GetParent()->IsKindOf(RUNTIME_CLASS(CPrimitiveBuilder))) {
				signal->AddConnection(dst);
				dst->AddConnection(signal);
			}
		}
		conns.RemoveAll();
	}
	SetPorts(list);

	list = GetAtoms("InputSignals");
	ASSERT(list);
	inputPortNo = list->GetCount();
	pos = list->GetHeadPosition();
	n = 0;
	while(pos) {
		CSignalBuilder *signal = (CSignalBuilder *)(list->GetNext(pos));
		signal->SetLocation();
	}
	SetPorts(list);

	list = GetAtoms("Parameters");
	ASSERT(list);
	pos = list->GetHeadPosition();
	while(pos) {
		CParameterBuilder *parameter = (CParameterBuilder *)(list->GetNext(pos));
		parameter->AddToList();
		parameters.AddTail(parameter);
	}
	list = GetAtoms("InputParameters");
	ASSERT(list);
	pos = list->GetHeadPosition();
	while(pos) {
		CInputParameterBuilder *inp = (CInputParameterBuilder *)(list->GetNext(pos));
		inputParameters.AddTail(inp);
		CBuilderAtomicObjectList conns;
		inp->GetDirectInConnections("ParameterConn",conns);
		if(conns.IsEmpty())
			DisplayError("InputParameter " + inp->GetName() + " not connected!");
		else {
			if(conns.GetCount() > 1)
				DisplayError("InputParameter " + inp->GetName() + " connected multiple times!");
			CBuilderAtom *src = (CBuilderAtom *)(conns.GetHead());
			ASSERT(src);
			if(src->IsKindOf(RUNTIME_CLASS(CParameterBuilder))) {
				inp->source = (CParameterBuilder *)src;
				inp->source->AddToList();
			}
			else
				DisplayError("InputParameter " + inp->GetName() + " not connected to a Parameter!");
		}
	}
}

void CPrimitiveBuilder::WriteScriptRegs(FILE *fpt)
{
	POSITION pos = contextTable.GetStartPosition();		// one-to-one relation between scripts and context-defs!
	while(pos) {
		CPrimitiveBuilder *prim = 0;
		CString nm;
		contextTable.GetNextAssoc(pos,nm,prim);
		fprintf(fpt,"   mgk_register_script(%s,\"%s\");\n",prim->script,prim->script);
	}
}

void CPrimitiveBuilder::WriteScriptProto(FILE *fpt)
{
	POSITION pos = contextTable.GetStartPosition();
	while(pos) {
		CPrimitiveBuilder *prim = 0;
		CString nm;
		contextTable.GetNextAssoc(pos,nm,prim);
		fprintf(fpt,"extern void %s(void);\n",prim->script);
	}
	fprintf(fpt,"\n");
}

void CPrimitiveBuilder::WriteContextDefs(FILE *fpt)
{
	POSITION pos = contextTable.GetStartPosition();
	while(pos) {
		CPrimitiveBuilder *prim = 0;
		CString nm;
		contextTable.GetNextAssoc(pos,nm,prim);
		prim->WriteContextDef(fpt);
	}
}

void CPrimitiveBuilder::WriteContextDef(FILE *fpt)
{
	if(parameters.IsEmpty() && inputParameters.IsEmpty())
		return;
	fprintf(fpt,"typedef struct {\n");
	POSITION pos = parameters.GetHeadPosition();
	while(pos) {
		CParameterBuilder *parameter = parameters.GetNext(pos);
		parameter->WriteContextDef(fpt,parameter->GetName());
	}
	pos = inputParameters.GetHeadPosition();
	while(pos) {
		CInputParameterBuilder *parameter = inputParameters.GetNext(pos);
		if(parameter->source)
			parameter->source->WriteContextDef(fpt,parameter->GetName());
		else
			fprintf(fpt,"/* ERROR: InputParameter %s not connected! */\n",parameter->GetName());
	}
	fprintf(fpt,"} %s_context;\n\n",script);
}

void CPrimitiveBuilder::WriteContextCreate(FILE *fpt,bool ext)
{
	if(parameters.IsEmpty() && inputParameters.IsEmpty())
		return;
	fprintf(fpt,"%s%s_context %s_context;\n",ext ? "extern " : "",script,actorName);
}

void CPrimitiveBuilder::WriteSetContext(FILE *fpt)
{
	if(parameters.IsEmpty() && inputParameters.IsEmpty())
		return;
	POSITION pos = parameters.GetHeadPosition();
	while(pos) {
		CParameterBuilder *parameter = parameters.GetNext(pos);
		parameter->WriteContextInit(fpt,actorName,parameter->GetName());
	}
	pos = inputParameters.GetHeadPosition();
	while(pos) {
		CInputParameterBuilder *parameter = inputParameters.GetNext(pos);
		if(parameter->source)
			parameter->source->WriteContextInit(fpt,actorName,parameter->GetName());
	}
	fprintf(fpt,"   mgk_set_node_context( %s, &%s_context,T_ARRAY(sizeof(%s_context)) | T_CHAR);\n",
				actorName,
				actorName,
				script);
}

void CPrimitiveBuilder::WriteActorCreate(FILE *fpt)
{
	char scr[128];
	sprintf(scr,"\"%s\"",script);
	fprintf(fpt,"   mgk_nodep %-18s = mgk_create_node_indirect( %-18s,%3d,%3d,%4d, %s, 0 );\n",
		actorName,scr,inputPortNo,outputPortNo,priority,firing ? "AT_IFANY" : "AT_IFALL");
}

void CPrimitiveBuilder::WriteActorConnect(FILE *fpt)
{
 	const CBuilderAtomList *list = GetAtoms("OutputSignals");
 	POSITION pos = list->GetHeadPosition();
 	while(pos) {
 		CSignalBuilder *signal = (CSignalBuilder *)(list->GetNext(pos));
 		POSITION dstPos = signal->connections.GetHeadPosition();
 		while(dstPos) {
 			CSignalBuilder *dst = signal->connections.GetNext(dstPos);
 			fprintf(fpt,"   mgk_connect_nodes( %-18s,%3d, %-18s,%3d );\n",
				actorName,signal->port,
 				((CPrimitiveBuilder *)dst->GetParent())->GetActorName(),dst->port);
 		}
 	}
}

IMPLEMENT_CUSTOMATOM(CSignalBuilder,CBuilderAtom,"InputSignal,OutputSignal")

void CSignalBuilder::SetLocation()
{
	CRect loc;
	if(!GetLocation(CString("SignalFlowAspect"),loc))
		DisplayError("Error Getting Location!");
	yloc = loc.top;
}

IMPLEMENT_CUSTOMATOM(CParameterBuilder,CBuilderAtom,"Param")

void CParameterBuilder::Initialize()
{
	listed = false;

	if(!IsValidName((LPCTSTR)GetName()))
		DisplayError("Invalid Name! You Must Use C Syntax!");

	GetNamePath(longName);

	if(!GetAttribute("Global",global))
		DisplayError("Error getting Global attribute!");

	CString tempDT;
	if(!GetAttribute("DataType",tempDT))
		DisplayError("Error getting Datatype attribute!");

	if (tempDT == "Integer") datatype = 0;
	else if (tempDT == "Double") datatype = 1;
	else if (tempDT == "Character") datatype = 2;
	else if (tempDT == "Pointer") datatype = 3;

	if(!GetAttribute("Size",size))
		DisplayError("Error getting Size attribute!");
	if(size < 1) {
		size = 1;
		DisplayWarning("Size attribute out of range. Resetting to 1");
	}
	else if(size > 1024) {
		size = 1024;
		DisplayWarning("Size attribute out of range. Resetting to 1024");
	}

	if(!GetAttribute("InitValue",initval))
		DisplayError("Error getting Initial Value attribute!");
}

CParameterBuilder::~CParameterBuilder()
{
	instances.RemoveAll();
}

void CParameterBuilder::AddToList()
{
	if(!listed) {
		instances.AddTail(this);
		listed = true;
	}
}

void CParameterBuilder::WriteGlobals(FILE *fpt,bool ext)
{
	POSITION pos = instances.GetHeadPosition();
	while(pos)
		instances.GetNext(pos)->WriteGlobal(fpt,ext);
}

void CParameterBuilder::WriteGlobal(FILE *fpt,bool ext)
{
	if(datatype == POINTER_TYPE)
		return;
	if(global || (!ext && size > 1 && !initval.IsEmpty())) {
		char *type = (datatype == INT_TYPE) ?
							"int" :	(datatype == DOUBLE_TYPE) ?
							"double" :
							"char";
		if(size > 1) {
			fprintf(fpt,"%s%s %s[%d]",ext ? "extern " : "",type,longName,size);
			if(initval.IsEmpty() || ext)
				fprintf(fpt,";\n");
			else if(datatype == CHAR_TYPE)
				fprintf(fpt," = \"%s\";\n",initval);
			else
				fprintf(fpt," = { %s };\n",initval);
		}
		else {
			fprintf(fpt,"%s%s %s",ext ? "extern " : "",type,longName);
			if(!ext) {
				if(datatype == CHAR_TYPE)
					fprintf(fpt," = '%s';\n",initval.IsEmpty() ? "\\0" : initval);
				else
					fprintf(fpt," = %s;\n",initval.IsEmpty() ? "0" : initval);
			}
			else
				fprintf(fpt,";\n");
		}
	}
}

void CParameterBuilder::WriteContextDef(FILE *fpt,const CString &fieldName)
{
	if(datatype == POINTER_TYPE) {
		fprintf(fpt,"   void *%s;\n",fieldName);
		return;
	}
	char *type = (datatype == INT_TYPE) ?
						"int" :	(datatype == DOUBLE_TYPE) ?
						"double" :
						"char";
	char field[64];
	if(global && size > 1)
		sprintf(field,"(*%s)",fieldName);
	else
		sprintf(field,"%s%s",global ? "*" : " ",fieldName);
	char subs[64] = "";
	if(size > 1)
		sprintf(subs,"[%d]",size);
	fprintf(fpt,"   %s %s%s;\n",type,field,subs);
}

void CParameterBuilder::WriteContextInit(FILE *fpt,CString &actorName,const CString &fieldName)
{
	if(datatype == POINTER_TYPE)
		fprintf(fpt,"   %s_context.%s = 0;\n",actorName,fieldName);
	else if(global)
		fprintf(fpt,"   %s_context.%s = &%s;\n",actorName,fieldName,longName);
	else if(!initval.IsEmpty()) {
		if(size > 1)
			fprintf(fpt,"   memcpy(%s_context.%s,%s,sizeof(%s));\n",actorName,fieldName,longName,longName);
		else {
			fprintf(fpt,"   %s_context.%s",actorName,fieldName);
			if(datatype == CHAR_TYPE)
				fprintf(fpt," = '%s';\n",initval);
			else
				fprintf(fpt," = %s;\n",initval);
		}
	}
}

IMPLEMENT_CUSTOMATOM(CInputParameterBuilder,CBuilderAtom,"InputParam")

void CInputParameterBuilder::Initialize()
{
	source = 0;

	if(!IsValidName((LPCTSTR)GetName()))
		DisplayError("Invalid Name! You Must Use C Syntax!");

	CString tempDT;
	if(!GetAttribute("DataType",tempDT))
		DisplayError("Error getting Datatype attribute!");

	if (tempDT == "Integer") datatype = 0;
	else if (tempDT == "Double") datatype = 1;
	else if (tempDT == "Character") datatype = 2;
	else if (tempDT == "Pointer") datatype = 3;

	CString number;
	if(!GetAttribute("Size",size))
		DisplayError("Error getting Size attribute!");
	if(size < 1) {
		size = 1;
		DisplayWarning("Size attribute out of range. Resetting to 1");
	}
	else if(size > 1024) {
		size = 1024;
		DisplayWarning("Size attribute out of range. Resetting to 1024");
	}

}


