#include "stdafx.h"
#include "SFBonExtension.h"


namespace BON
{

IMPLEMENT_ABSTRACT_BONEXTENSION( SF_BON::Processing );
IMPLEMENT_BONEXTENSION( SF_BON::Compound, "Compound" );
IMPLEMENT_BONEXTENSION( SF_BON::Primitive, "Primitive" );
IMPLEMENT_BONEXTENSION( SF_BON::ParameterBase, " OutputParam" );
IMPLEMENT_BONEXTENSION( SF_BON::InputParam, "InputParam" );
IMPLEMENT_BONEXTENSION( SF_BON::Param, "Param" );
IMPLEMENT_BONEXTENSION( SF_BON::Signal, " InputSignal OutputSignal" );


}; // end namespace BON

namespace SF_BON
{
//*******************************************************************
// getter for role "InputParameters" among "ParameterBase"s and its descendants
//*******************************************************************
std::set<InputParam> ProcessingImpl::getInputParameters()
{
	std::set<InputParam> res;
	std::set<BON::FCO> roles = getChildFCOsAs("InputParameters");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		InputParam elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//*******************************************************************
// getter for role "InputSignals" among "Signal"s and its descendants
//*******************************************************************
std::set<Signal> ProcessingImpl::getInputSignals()
{
	std::set<Signal> res;
	std::set<BON::FCO> roles = getChildFCOsAs("InputSignals");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		Signal elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//*******************************************************************
// getter for role "OutputParameters" among "ParameterBase"s and its descendants
//*******************************************************************
std::set<ParameterBase> ProcessingImpl::getOutputParameters()
{
	std::set<ParameterBase> res;
	std::set<BON::FCO> roles = getChildFCOsAs("OutputParameters");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		ParameterBase elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//*******************************************************************
// getter for role "OutputSignals" among "Signal"s and its descendants
//*******************************************************************
std::set<Signal> ProcessingImpl::getOutputSignals()
{
	std::set<Signal> res;
	std::set<BON::FCO> roles = getChildFCOsAs("OutputSignals");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		Signal elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//*******************************************************************
// getter for role "PC" among "ParameterConn"s
//*******************************************************************
std::set<BON::Connection> ProcessingImpl::getPC()
{
	std::set<BON::Connection> res;
	std::set<BON::FCO> roles = getChildFCOsAs("PC");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		BON::Connection elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//*******************************************************************
// getter for role "Parameters" among "ParameterBase"s and its descendants
//*******************************************************************
std::set<Param> ProcessingImpl::getParameters()
{
	std::set<Param> res;
	std::set<BON::FCO> roles = getChildFCOsAs("Parameters");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		Param elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//*******************************************************************
// aggregated getter for role "eters" among "ParameterBase"s and its descendants
//*******************************************************************
std::set<ParameterBase> ProcessingImpl::geteters()
{
	std::set<ParameterBase> res;
	const int len = 3;
	std::set<BON::FCO> roles_vec[ len];
	roles_vec[0] = getChildFCOsAs("InputParameters");
	roles_vec[1] = getChildFCOsAs("OutputParameters");
	roles_vec[2] = getChildFCOsAs("Parameters");
	for( int k = 0; k < len; ++k)
		for( std::set<BON::FCO>::iterator i = roles_vec[k].begin(); i != roles_vec[k].end(); ++i)
		{
			ParameterBase elem(*i);
			ASSERT(elem);
			res.insert(elem);
		}
	return res;
}


//*******************************************************************
// aggregated getter for role "s" among "Signal"s and its descendants
//*******************************************************************
std::set<Signal> ProcessingImpl::gets()
{
	std::set<Signal> res;
	const int len = 2;
	std::set<BON::FCO> roles_vec[ len];
	roles_vec[0] = getChildFCOsAs("InputSignals");
	roles_vec[1] = getChildFCOsAs("OutputSignals");
	for( int k = 0; k < len; ++k)
		for( std::set<BON::FCO>::iterator i = roles_vec[k].begin(); i != roles_vec[k].end(); ++i)
		{
			Signal elem(*i);
			ASSERT(elem);
			res.insert(elem);
		}
	return res;
}


//*******************************************************************
// getter for role "CompoundParts" among "Processing"s and its descendants
//*******************************************************************
std::set<Compound> CompoundImpl::getCompoundParts()
{
	std::set<Compound> res;
	std::set<BON::FCO> roles = getChildFCOsAs("CompoundParts");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		Compound elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//*******************************************************************
// getter for role "DFC" among "DataflowConn"s
//*******************************************************************
std::set<BON::Connection> CompoundImpl::getDFC()
{
	std::set<BON::Connection> res;
	std::set<BON::FCO> roles = getChildFCOsAs("DFC");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		BON::Connection elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//*******************************************************************
// aggregated getter for role "Parts" among "Processing"s and its descendants
//*******************************************************************
std::set<Processing> CompoundImpl::getParts()
{
	std::set<Processing> res;
	const int len = 2;
	std::set<BON::FCO> roles_vec[ len];
	roles_vec[0] = getChildFCOsAs("CompoundParts");
	roles_vec[1] = getChildFCOsAs("PrimitiveParts");
	for( int k = 0; k < len; ++k)
		for( std::set<BON::FCO>::iterator i = roles_vec[k].begin(); i != roles_vec[k].end(); ++i)
		{
			Processing elem(*i);
			ASSERT(elem);
			res.insert(elem);
		}
	return res;
}


//*******************************************************************
// getter for role "PrimitiveParts" among "Processing"s and its descendants
//*******************************************************************
std::set<Primitive> CompoundImpl::getPrimitiveParts()
{
	std::set<Primitive> res;
	std::set<BON::FCO> roles = getChildFCOsAs("PrimitiveParts");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		Primitive elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//*******************************************************************
// 
//*******************************************************************
PrimitiveImpl::Firing_Type PrimitiveImpl::getFiring()
{
	std::string val = FCOImpl::getAttribute("Firing")->getStringValue();

	if ( val == "IFALL") return IFALL_Firing_Type;
	else if ( val == "IFANY") return IFANY_Firing_Type;
	else throw("None of the possible items");
}


//*******************************************************************
// 
//*******************************************************************
long PrimitiveImpl::getPriority()
{
	return FCOImpl::getAttribute("Priority")->getIntegerValue();
}


//*******************************************************************
// 
//*******************************************************************
std::string PrimitiveImpl::getScript()
{
	return FCOImpl::getAttribute("Script")->getStringValue();
}


//*******************************************************************
// 
//*******************************************************************
ParameterBaseImpl::DataType_Type ParameterBaseImpl::getDataType()
{
	std::string val = FCOImpl::getAttribute("DataType")->getStringValue();

	if ( val == "Integer") return Integer_DataType_Type;
	else if ( val == "Double") return Double_DataType_Type;
	else if ( val == "Character") return Character_DataType_Type;
	else if ( val == "Pointer") return Pointer_DataType_Type;
	else throw("None of the possible items");
}


//*******************************************************************
// 
//*******************************************************************
long ParameterBaseImpl::getSize()
{
	return FCOImpl::getAttribute("Size")->getIntegerValue();
}


//*******************************************************************
// 
//*******************************************************************
std::string ParamImpl::getInitValue()
{
	return FCOImpl::getAttribute("InitValue")->getStringValue();
}


//*******************************************************************
// 
//*******************************************************************
bool ParamImpl::isGlobal()
{
	return FCOImpl::getAttribute("Global")->getBooleanValue();
}



}; // namespace SF_BON

