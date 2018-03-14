//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLParserStatic.h
//
//###############################################################################################################################################

#include "OCLCommon.h"

#include "OCLTokens.h"
#include <set>

using namespace std;

typedef std::set< int > TokenSet;

extern TokenSet First_name;
extern TokenSet First_typeName;
extern TokenSet First_colon;
extern TokenSet First_inputend;
extern TokenSet First_expression;
extern TokenSet First_contextDeclarationHelper;

extern TokenSet First_inv;
extern TokenSet First_right_parenthesis;
extern TokenSet First_left_parenthesis;
extern TokenSet First_formalParameterList;
extern TokenSet First_prePost;
extern TokenSet First_formalParameter;
extern TokenSet First_letExpression;
extern TokenSet First_implicationExpression;
extern TokenSet First_in;
extern TokenSet First_equal;
extern TokenSet First_then;
extern TokenSet First_else;
extern TokenSet First_endif;
extern TokenSet First_orExpression;
extern TokenSet First_xorExpression;
extern TokenSet First_andExpression;
extern TokenSet First_relationalExpression;
extern TokenSet First_additiveExpression;
extern TokenSet First_multiplicativeExpression;
extern TokenSet First_unaryExpression;
extern TokenSet First_postfixExpression;
extern TokenSet First_featureCall;
extern TokenSet First_primaryExpression;
extern TokenSet First_featureCallParametersHelper;
extern TokenSet First_featureCallParameters;
extern TokenSet First_delimiter;
extern TokenSet First_actualParameterList;
extern TokenSet First_right_bracket;
extern TokenSet First_literal;
extern TokenSet First_left_brace;
extern TokenSet First_right_brace;
extern TokenSet First_expressionListOrRange;
extern TokenSet First_qualifiers;

extern TokenSet Last_contextDeclarationHelper;
extern TokenSet Last_literal;
extern TokenSet Last_primaryExpression;
extern TokenSet Last_featureCall;
extern TokenSet Last_featureCallParametersHelper;
extern TokenSet Last_expression;
extern TokenSet Last_implicationExpression;

#define CLASS_MULTIPLICATIVE 		1000
#define CLASS_ADDITIVE		  		1001
#define CLASS_RELATIONAL			1002
#define CLASS_LOGICAL				1003
#define CLASS_PREPOST				1004
#define CLASS_STEREOTYPES			1005
#define CLASS_UNARY				1006
#define CLASS_BOOLEAN				1007
#define CLASS_CALLKIND				1008
#define CLASS_LOGICAL_AND			1009
#define CLASS_LOGICAL_OR			1010
#define CLASS_LOGICAL_IMPLIES	1011

#define LID_NODE_START					0
#define LID_CALL_OPERATOR				1
#define LID_DECLARATOR_NAME 			2
#define LID_DECLARATOR_NAME2 		3
#define LID_DECLARATOR_TYPE 			4
#define LID_ACCUMULATOR_NAME 		5
#define LID_ACCUMULATOR_TYPE 		6
#define LID_FEATURE_NAME 				7
#define LID_CONSTRAINT_NAME 			8
#define LID_CONTEXT_TYPE 				9
#define LID_CONTEXT_STEREOTYPE 		10
#define LID_CONTEXT_RETURNTYPE 		11
#define LID_VARIABLE_NAME 			12
#define LID_VARIABLE_TYPE 				13
#define LID_ROLE_NAME 					14
#define LID_PARAMETER_NAME 			100
#define LID_PARAMETER_TYPE 			200

void 		InitializeTokenSets();
TokenSet 	Union( const TokenSet& setTokens1, const TokenSet& setTokens2 );
TokenSet 	Union( const TokenSet& setTokens, int iToken );
std::string 		PrintToken( int iToken, bool bQuote );
std::string 		PrintTokens( const TokenSet& setTokens );
bool 		Contains( const TokenSet& setTokens1, const TokenSet& setTokens2 );
bool 		Contains( const TokenSet& setTokens, int iToken );

