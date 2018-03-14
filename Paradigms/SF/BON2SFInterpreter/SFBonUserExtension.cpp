/*
	Copyright (c) Vanderbilt University, 2000-2004
	ALL RIGHTS RESERVED

	Vanderbilt University disclaims all warranties with regard to this
	software, including all implied warranties of merchantability
	and fitness.  In no event shall Vanderbilt University be liable for
	any special, indirect or consequential damages or any damages
	whatsoever resulting from loss of use, data or profits, whether
	in an action of contract, negligence or other tortious action,
	arising out of or in connection with the use or performance of
	this software.
*/

#include "stdafx.h"
#include "SFBonExtension.h"

namespace SF_BON
{

using namespace BON;

PrimitiveTable PrimitiveImpl::contextTable;
std::set<Param> ParamImpl::instances;

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

//###############################################################################################################################################
//
// 	C L A S S : BON::CompoundImpl
//
//###############################################################################################################################################


	void CompoundImpl::initialize()
	{
		ProcessingImpl::initialize();
	}

	void CompoundImpl::traverse(std::set<Primitive> &primitives)
	{
		std::set<Processing> children = getParts();
		for ( std::set<Processing>::iterator it = children.begin() ; it != children.end() ; it++ )
			Processing(*it)->traverse(primitives);
	}

//###############################################################################################################################################
//
// 	C L A S S : BON::PrimitiveImpl
//
//#########################################################p######################################################################################

	void PrimitiveImpl::initialize()
	{
		ProcessingImpl::initialize();
		if(!IsValidName(getScript().c_str()))
			AfxMessageBox(CString("Invalid Name! You Must Use C Syntax! (primitive script: ") + getName().c_str() + " )");

		PrimitiveTable::iterator it = contextTable.find( getScript() );
		if ( it == contextTable.end() )
			contextTable[ getScript() ] = Primitive( this );
	}

	void PrimitiveImpl::traverse(std::set<Primitive> &primitives)
	{
		static int idCounter = 0;
		primitives.insert(this);
		id = idCounter++;
		setActorName();

		{
			std::set<FCO> list = getChildFCOsAs("OutputSignals");
			outputPortNo = list.size();
			for ( std::set<FCO>::iterator it = list.begin() ; it != list.end() ; it++ ) {
				Signal signal( *it );
				signal->setLocation();
				std::multiset<ConnectionEnd> conns = signal->getDirectOutConnEnds("DataflowConn");
				if(conns.empty())
					AfxMessageBox("OutputSignal " + CString( signal->getName().c_str() ) + " not connected!");
				for ( std::multiset<ConnectionEnd>::iterator cit = conns.begin() ; cit != conns.end() ; cit++ ) {
					Signal dst( *cit ); 
					ASSERT(dst);
					if(Primitive( dst->getParent() )) {
						signal->addConnection(dst);
						dst->addConnection(signal);
					}
				}
			}
			setPorts(list);
		}
		{
			std::set<FCO> list = getChildFCOsAs("InputSignals");
			inputPortNo = list.size();
			for ( std::set<FCO>::iterator it = list.begin() ; it != list.end() ; it++ ) {
				Signal signal( *it );
				signal->setLocation();
			}
			setPorts(list);
		}
		{
			std::set<Param> list = getParameters();
			for ( std::set<Param>::iterator it = list.begin() ; it != list.end() ; it++ ) {
				(*it)->addToList();
				parameters.insert(*it);
			}
		}
			std::set<InputParam> list = getInputParameters();
			for ( std::set<InputParam>::iterator it = list.begin() ; it != list.end() ; it++ ) {
				inputParameters.insert(*it);
				std::multiset<ConnectionEnd> conns = (*it)->getDirectInConnEnds("ParameterConn");
				if(conns.empty())
					AfxMessageBox(CString("InputParameter ") + (*it)->getName().c_str() + " is not connected!");
				else {
				if(conns.size() > 1)
				AfxMessageBox(CString("InputParameter ") + (*it)->getName().c_str() + " is connected multiple times!");

				ConnectionEnd src(*conns.begin());
				if(Param(src)) {
					(*it)->source = (Param)src;
					(*it)->source->addToList();
				}
				else
					AfxMessageBox(CString("InputParameter ") + (*it)->getName().c_str() + " not connected to a Parameter!");
			}
		}
	}

	void PrimitiveImpl::setActorName()
	{
		char txt[32];
		sprintf(txt,"_actor_%d",id);
		actorName = getName() + txt;
	}

	void PrimitiveImpl::setPorts(const std::set<FCO> &ports)
	{
		for ( std::set<FCO>::const_iterator it = ports.begin() ; it != ports.end() ; it++ ) {
			Signal p1 (*it);
			std::set<FCO>::const_iterator itInner = it;
			itInner++;
			for ( ; itInner != ports.end() ; itInner++ ) {
				Signal p2 ( *itInner );
				((p1->yloc >= p2->yloc) ? p1 : p2)->incrementPort();
			}
		}
	}

	void PrimitiveImpl::writeActorCreate(FILE *fpt)
	{
		char scr[128];
		sprintf(scr,"\"%s\"",getScript().c_str());
		fprintf(fpt,"   mgk_nodep %-18s = mgk_create_node_indirect( %-18s,%3d,%3d,%4d, %s, 0 );\n",
			actorName.c_str(),scr,inputPortNo,outputPortNo,(int)getPriority(),(getFiring() == IFANY_Firing_Type) ? "AT_IFANY" : "AT_IFALL");
	}

	void PrimitiveImpl::writeActorConnect(FILE *fpt)
	{
		std::set<Signal> list = getOutputSignals();
		outputPortNo = list.size();
		for ( std::set<Signal>::iterator it = list.begin() ; it != list.end() ; it++ ) {
			Signal signal( *it );
			for ( std::set<Signal>::iterator sit = signal->connections.begin() ; sit != signal->connections.end() ; sit++ ) {
	 			Signal dst ( *sit );
	 			fprintf(fpt,"   mgk_connect_nodes( %-18s,%3d, %-18s,%3d );\n",
					actorName.c_str(),signal->port,
	 				((Primitive)(dst->getParent()))->getActorName().c_str(),dst->port);
	 		}
	 	}
	}

	void PrimitiveImpl::writeScriptRegs(FILE *fpt)
	{
		for ( PrimitiveTable::iterator it = contextTable.begin() ; it != contextTable.end() ; it++ ) {
			fprintf(fpt,"   mgk_register_script(%s,\"%s\");\n",it->second->getScript().c_str(),it->second->getScript().c_str());
		}
	}
	
	void PrimitiveImpl::writeScriptProto(FILE *fpt)
	{
		for ( PrimitiveTable::iterator it = contextTable.begin() ; it != contextTable.end() ; it++ ) {
			fprintf(fpt,"extern void %s(void);\n",it->second->getScript().c_str());
		}
		fprintf(fpt,"\n");
	}

	void PrimitiveImpl::writeContextDefs(FILE *fpt)
	{
		for ( PrimitiveTable::iterator it = contextTable.begin() ; it != contextTable.end() ; it++ )
			it->second->writeContextDef(fpt);
	}

	void PrimitiveImpl::writeContextDef(FILE *fpt)
	{
		if(parameters.empty() && inputParameters.empty())
			return;
		fprintf(fpt,"typedef struct {\n");

		for ( std::set<Param>::iterator it = parameters.begin() ; it != parameters.end() ; it++ ) {
			Param parameter(*it);
			parameter->writeContextDef(fpt,parameter->getName());
		}
		for ( std::set<InputParam>::iterator it2 = inputParameters.begin() ; it2 != inputParameters.end() ; it2++ ) {
			InputParam parameter(*it2);
			if(parameter->source)
				parameter->source->writeContextDef(fpt,parameter->getName());
			else
				fprintf(fpt,"/* ERROR: InputParameter %s not connected! */\n",parameter->getName().c_str());
		}
		fprintf(fpt,"} %s_context;\n\n",getScript().c_str());
	}

	void PrimitiveImpl::writeContextCreate(FILE *fpt,bool ext)
	{
		if(parameters.empty() && inputParameters.empty())
			return;
		fprintf(fpt,"%s%s_context %s_context;\n",ext ? "extern " : "",getScript().c_str(),actorName.c_str());
	}

	void PrimitiveImpl::writeSetContext(FILE *fpt)
	{
		if(parameters.empty() && inputParameters.empty())
			return;
		for ( std::set<Param>::iterator it = parameters.begin() ; it != parameters.end() ; it++ ) {
			Param parameter(*it);
			parameter->writeContextInit(fpt,actorName,parameter->getName());
		}
		for ( std::set<InputParam>::iterator it2 = inputParameters.begin() ; it2 != inputParameters.end() ; it2++ ) {
			InputParam parameter(*it2);
			if(parameter->source)
				parameter->source->writeContextInit(fpt,actorName,parameter->getName());
		}
		fprintf(fpt,"   mgk_set_node_context( %s, &%s_context,T_ARRAY(sizeof(%s_context)) | T_CHAR);\n",
					actorName.c_str(),
					actorName.c_str(),
					getScript().c_str());
	}


//###############################################################################################################################################
//
// 	C L A S S : BON::SignalImpl
//
//###############################################################################################################################################


	void SignalImpl::setLocation()
	{
		Point loc = getRegistry()->getLocation("SignalFlowAspect");
		if(!loc.second)
			AfxMessageBox("Error Getting Location!");
		yloc = loc.second;
	}


//###############################################################################################################################################
//
// 	C L A S S : BON::ParameterBaseImpl
//
//###############################################################################################################################################

	void ParameterBaseImpl::initialize()
	{
		if(!IsValidName(getName().c_str()))
			AfxMessageBox(CString("Invalid Name! You Must Use C Syntax! (parameter: ") + getName().c_str() + " )");

		size = getSize();
		if(size < 1) {
			size = 1;
			AfxMessageBox("Size attribute out of range. Resetting to 1");
		}
		else if(size > 1024) {
			size = 1024;
			AfxMessageBox("Size attribute out of range. Resetting to 1024");
		}
	}

//###############################################################################################################################################
//
// 	C L A S S : BON::ParameterImpl
//
//###############################################################################################################################################

	void ParamImpl::initialize()
	{
		ParameterBaseImpl::initialize();

		listed = false;
		longName = getName();
	}

	void ParamImpl::addToList()
	{
		if(!listed) {
			instances.insert(this);
			listed = true;
		}
	}

	void ParamImpl::writeGlobals(FILE *fpt,bool ext)
	{
		for ( std::set<Param>::iterator it = instances.begin() ; it != instances.end() ; it++ )
			((Param)(*it))->writeGlobal(fpt,ext);
	}

	void ParamImpl::writeGlobal(FILE *fpt,bool ext)
	{
		if(getDataType() == Pointer_DataType_Type)
			return;
		if(isGlobal() || (!ext && size > 1 && !getInitValue().empty())) {
			char *type = (getDataType() == Integer_DataType_Type) ?
							"int" :	(getDataType() == Double_DataType_Type) ?
							"double" :
							"char";
			if(size > 1) {
				fprintf(fpt,"%s%s %s[%d]",ext ? "extern " : "",type,longName.c_str(),size);
				if(getInitValue().empty() || ext)
					fprintf(fpt,";\n");
				else if(getDataType() == Character_DataType_Type)
					fprintf(fpt," = \"%s\";\n",getInitValue().c_str());
				else
					fprintf(fpt," = { %s };\n",getInitValue().c_str());
			}
			else {
				fprintf(fpt,"%s%s %s",ext ? "extern " : "",type,longName.c_str());
				if(!ext) {
					if(getDataType() == Character_DataType_Type)
						fprintf(fpt," = '%s';\n",getInitValue().empty() ? "\\0" : getInitValue().c_str());
					else
						fprintf(fpt," = %s;\n",getInitValue().empty() ? "0" : getInitValue().c_str());
				}
				else
					fprintf(fpt,";\n");
			}
		}
	}

	void ParamImpl::writeContextDef(FILE *fpt,const std::string &fieldName)
	{
		if(getDataType() == Pointer_DataType_Type) {
			fprintf(fpt,"   void *%s;\n",fieldName.c_str());
			return;
		}
		char *type = (getDataType() == Integer_DataType_Type) ?
							"int" :	(getDataType() == Double_DataType_Type) ?
							"double" :
							"char";
		char field[64];
		if(isGlobal() && size > 1)
			sprintf(field,"(*%s)",fieldName.c_str());
		else
			sprintf(field,"%s%s",isGlobal() ? "*" : " ",fieldName.c_str());
		char subs[64] = "";
		if(size > 1)
			sprintf(subs,"[%d]",size);
		fprintf(fpt,"   %s %s%s;\n",type,field,subs);
	}

	void ParamImpl::writeContextInit(FILE *fpt,std::string &actorName,const std::string &fieldName)
	{
		if(getDataType() == Pointer_DataType_Type)
			fprintf(fpt,"   %s_context.%s = 0;\n",actorName.c_str(),fieldName.c_str());
		else if(isGlobal())
			fprintf(fpt,"   %s_context.%s = &%s;\n",actorName.c_str(),fieldName.c_str(),longName.c_str());
		else if(!getInitValue().empty()) {
			if(size > 1)
				fprintf(fpt,"   memcpy(%s_context.%s,%s,sizeof(%s));\n",
							actorName.c_str(),fieldName.c_str(),longName.c_str(),longName.c_str());
			else {
				fprintf(fpt,"   %s_context.%s",actorName.c_str(),fieldName.c_str());
				if(getDataType() == Character_DataType_Type)
					fprintf(fpt," = '%s';\n",getInitValue().c_str());
				else
					fprintf(fpt," = %s;\n",getInitValue().c_str());
			}
		}
	}

//###############################################################################################################################################
//
// 	C L A S S : BON::InputParameterImpl
//
//###############################################################################################################################################

	void InputParamImpl::initialize()
	{
		ParameterBaseImpl::initialize();
	}

}; // namespace SF_BON
