//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLException.cpp
//
//###############################################################################################################################################
#include "Solve4786.h"
#include "OCLException.h"

//###############################################################################################################################################
//
//	M E S S A G E   C O N S T A N T S
//
//###############################################################################################################################################

const char* ErrorMessages[] = {

	"Type [ ? ] does not exist.",
	"Type [ ? ] is ambiguous.",
	"BaseType of types [ ? , ? ] does not exist.",
	"Operator [ ? ] is ambiguous.",
	"Operator [ ? ] does not exist.",
	"Function [ ? ] is ambiguous.",
	"Function [ ? ] does not exist.",
	"Attribute [ ? ] is ambiguous.",
	"Attribute [ ? ] does not exist.",
	"Association [ ? ] is ambiguous. If you did not, then use qualifier.",
	"Association [ ? ] does not exist.",
	"Method [ ? ] is ambiguous.",
	"Method [ ? ] does not exist.",
	"Iterator [ ? ] is ambiguous.",
	"Iterator [ ? ] does not exist.",
	"TypeSequence [ ? ] is invalid.",
	"Type [ ? ] is not compound.",
	"Variable [ ? ] already exists.",
	"Root expression has to return type [ ocl::Boolean ].",
	"Then and Else braches of If statement returns different types [ ? , ? ].",
	"Invalid type cast. Object of type [ ? ] is not an instance of type [ ? ].",
	"Operator is not implemented.",
	"Member selection operator [ \"?\" ] is invalid.",
	"Iterator is not implemented.",
	"On literals of type [ ? ] member selection operator cannot be used.",
	"Method [ ? ] and Iterator [ ? ] are ambiguous. Use \"self\" or variable and member selection operator.",
	"Method is not implemented.",
	"Function is not implemented.",
	"Association is not implemented.",
	"Attribute is not implemented.",
	"Attribute [ ? ] and Association [ ? ] are ambiguous. Use \"self\", variable and member selection operator or qualifier.",
	"Variable [ ? ] does not exist.",
	"Literal of type [ ? ] with value [ ? ] cannot be created.",
	"Collection literal of compound type [ ? ] cannot be created.",
	"Attribute [ ? ] and Attribute [ ? ] are ambiguous. Use \"self\" or variable and member selection operator.",
	"Type [ ? ] is not compound and does not have Iterator.",
	"Type [ ? ] is compound and does not have Association.",
	"Iterators has exactly one argument.",
	"Constraint definition has to have a Type to return.",
	"Root expression has to return Type [ ? ]. Type [ ? ] is not appropriate.",
	"Constraint definition [ ? ] failed.",
	"Assignment is invalid. Type [ ? ] of right-value does not conform to Type [ ? ] of left-value.",
	"Type cast is invalid. Types [ ? , ? ] do not conform to each other.",
	"Implicit Iterator [ ? ] does not exist. ",
	"Neither Function, implicit Method nor implicit Iterator [ ? ] exists.",
	"Implicit Association [ ? ] does not exist.",
	"Neither Type, Variable, implicit Attribute nor implicit Association [ ? ] exists.",
	"Stop evaluation."
};

namespace OclCommon
{

//###############################################################################################################################################
//
//	C L A S S : Exception
//
//###############################################################################################################################################

	std::string Exception::GGetMessage()
	{
		std::string strMessage = m_strMessage;
		if ( m_iCode != - 1 ) {
			switch ( m_eType ) {
				case ET_SYNTACTIC 	:
				case ET_SEMANTIC 	:
				case ET_RUNTIME	 	:
				case ET_WARNING	 	: strMessage = ErrorMessages[ m_iCode - 100 ]; break;
			}
		}
		unsigned int iCnt = 0;
		size_t iPos = strMessage.find( '?' );
		while ( iPos != std::string::npos && iCnt < m_vecParameters.size() ) {
			strMessage = strMessage.substr( 0, iPos ) + m_vecParameters[ iCnt++ ] + strMessage.substr( iPos + 1, strMessage.length() - iPos - 1 );
			iPos = strMessage.find( '?' );
		}
		return strMessage;
	}

}; // namespace OclCommon