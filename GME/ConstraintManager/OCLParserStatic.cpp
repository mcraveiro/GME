//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLParserStatic.cpp
//
//###############################################################################################################################################
#include "Solve4786.h"
#include "OCLParserStatic.h"

bool bTokenSetInitialized = false;

TokenSet First_name = TokenSet();
TokenSet First_typeName = TokenSet();
TokenSet First_colon = TokenSet();
TokenSet First_inputend = TokenSet();
TokenSet First_inv = TokenSet();
TokenSet First_right_parenthesis = TokenSet();
TokenSet First_left_parenthesis = TokenSet();
TokenSet First_prePost = TokenSet();
TokenSet First_in = TokenSet();
TokenSet First_equal = TokenSet();
TokenSet First_then = TokenSet();
TokenSet First_else = TokenSet();
TokenSet First_endif = TokenSet();
TokenSet First_delimiter = TokenSet();
TokenSet First_right_bracket = TokenSet();
TokenSet First_left_brace = TokenSet();
TokenSet First_right_brace = TokenSet();
TokenSet First_literal = TokenSet();
TokenSet First_expressionListOrRange = TokenSet();
TokenSet First_actualParameterList = TokenSet();
TokenSet First_primaryExpression = TokenSet();
TokenSet First_contextDeclarationHelper = TokenSet();
TokenSet First_formalParameterList = TokenSet();
TokenSet First_formalParameter = TokenSet();
TokenSet First_letExpression = TokenSet();
TokenSet First_featureCallParametersHelper = TokenSet();
TokenSet First_featureCallParameters = TokenSet();
TokenSet First_featureCall = TokenSet();
TokenSet First_unaryExpression = TokenSet();
TokenSet First_postfixExpression = TokenSet();
TokenSet First_implicationExpression = TokenSet();
TokenSet First_orExpression = TokenSet();
TokenSet First_xorExpression = TokenSet();
TokenSet First_andExpression = TokenSet();
TokenSet First_relationalExpression = TokenSet();
TokenSet First_additiveExpression = TokenSet();
TokenSet First_multiplicativeExpression = TokenSet();
TokenSet First_expression = TokenSet();
TokenSet First_qualifiers = TokenSet();

TokenSet Last_primaryExpression = TokenSet();
TokenSet Last_featureCall = TokenSet();
TokenSet Last_featureCallParametersHelper = TokenSet();
TokenSet Last_literal = TokenSet();
TokenSet Last_expression = TokenSet();
TokenSet Last_implicationExpression = TokenSet();
TokenSet Last_contextDeclarationHelper = TokenSet();

void InitializeTokenSets()
{
	if ( ! bTokenSetInitialized ) {
		First_name.insert( IDENTIFIER );

		First_typeName.insert( IDENTIFIER );

		First_colon.insert( COLON );

		First_inputend.insert( INPUTEND );

		First_inv.insert( INV );

		First_right_parenthesis.insert( RIGHT_PARENTHESIS );

		First_left_parenthesis.insert( LEFT_PARENTHESIS );

		First_prePost.insert( CLASS_PREPOST );

		First_in.insert( INN );

		First_equal.insert( EQUAL );

		First_then.insert( THEN );

		First_else.insert( ELSE );

		First_endif.insert( ENDIF );

		First_delimiter.insert( DELIMITER );

		First_right_bracket.insert( RIGHT_BRACKET );

		First_left_brace.insert( LEFT_BRACE );

		First_right_brace.insert( RIGHT_BRACE );

		First_literal.insert( STRING );
		First_literal.insert( REAL );
		First_literal.insert( INTEGER );
		First_literal.insert( POUND );
		First_literal.insert( CLASS_BOOLEAN );

		First_primaryExpression = First_literal;
		First_primaryExpression .insert( IDENTIFIER );
		First_primaryExpression .insert( LEFT_PARENTHESIS );
		First_primaryExpression .insert( IF );

		First_contextDeclarationHelper.insert( IDENTIFIER );

		First_formalParameter.insert( IDENTIFIER );

		First_formalParameterList = First_formalParameter;

		First_featureCallParameters.insert( LEFT_PARENTHESIS );

		First_featureCall.insert( IDENTIFIER );

		First_postfixExpression = Union( First_primaryExpression, First_featureCall );

		First_unaryExpression = First_postfixExpression;
		First_unaryExpression.insert( CLASS_UNARY );

		First_multiplicativeExpression = First_unaryExpression;

		First_additiveExpression = First_multiplicativeExpression;

		First_relationalExpression = First_additiveExpression;

		First_andExpression = First_relationalExpression;

		First_xorExpression = First_andExpression;

		First_orExpression = First_xorExpression;

		First_implicationExpression = First_orExpression;

		First_letExpression.insert( LET );

		First_expression = Union( First_letExpression, First_implicationExpression );

		First_featureCallParametersHelper = First_expression;
		First_featureCallParametersHelper.insert( IDENTIFIER );
		First_featureCallParametersHelper.insert( RIGHT_PARENTHESIS );

		First_actualParameterList = First_expression;

		First_expressionListOrRange = First_expression;

		First_qualifiers.insert( LEFT_BRACKET );

		bTokenSetInitialized = true;

		Last_contextDeclarationHelper.insert( CLASS_STEREOTYPES );

		Last_literal.insert( STRING );
		Last_literal.insert( REAL );
		Last_literal.insert( INTEGER );
		Last_literal.insert( IDENTIFIER );
		Last_literal.insert( CLASS_BOOLEAN );

		Last_primaryExpression = Last_literal;
		Last_primaryExpression.insert( RIGHT_BRACE );
		Last_primaryExpression.insert( RIGHT_PARENTHESIS );
		Last_primaryExpression.insert( ENDIF );

		Last_featureCall.insert( IDENTIFIER );
		Last_featureCall.insert( RIGHT_PARENTHESIS );

		Last_implicationExpression = Union( Last_primaryExpression, Last_featureCall );

		Last_expression = Last_implicationExpression;

		Last_featureCallParametersHelper = Last_expression;
	}
}

TokenSet Union( const TokenSet& setTokens1, const TokenSet& setTokens2 )
{
	TokenSet setResult( setTokens1 );
	for ( TokenSet::const_iterator i = setTokens2.begin() ; i != setTokens2.end() ; ++i )
		setResult.insert( *i );
	return setResult;
}

TokenSet Union( const TokenSet& setTokens, int iToken )
{
	TokenSet setResult( setTokens );
	setResult.insert( iToken );
	return setResult;
}

std::string PrintToken( int iToken, bool bQuote )
{
	std::string strToken;
	switch ( iToken ) {
		case ARROWW					: strToken = "->"; break;
		case DOUBLECOLON 			: strToken = "::"; break;
		case COLON 					: strToken = ":"; break;
		case SEMICOLON 				: strToken = ";"; break;
		case LEFT_BRACE 				: strToken = "{"; break;
		case RIGHT_BRACE 			: strToken = "}"; break;
		case LEFT_PARENTHESIS 		: strToken = "("; break;
		case RIGHT_PARENTHESIS	: strToken = ")"; break;
		case LEFT_BRACKET 			: strToken = "["; break;
		case RIGHT_BRACKET 			: strToken = "]"; break;
		case POUND 					: strToken = "#"; break;
		case COMMA 					: strToken = ","; break;
		case DELIMITER 				: strToken = "|"; break;
		case ET 						: strToken = "@"; break;
		case MULTIPLE 				: strToken = "*"; break;
		case DIVIDE 					: strToken = "/"; break;
		case DIVIDEINT				: strToken = "div"; break;
		case MODULO					: strToken = "mod"; break;
		case PLUS 						: strToken = "+"; break;
		case MINUS 					: strToken = "-"; break;
		case DOT 						: strToken = "."; break;
		case PERCENT					: strToken = "%"; break;
		case EXCLAMATION			: strToken = "!"; break;
		case EQUAL 					: strToken = "="; break;
		case NONEQUAL				: strToken = "!="; break;
		case DOUBLEEQUAL			: strToken = "=="; break;
		case LESS						: strToken = "<"; break;
		case LESSEQUAL				: strToken = "<="; break;
		case LESSGREATER			: strToken = "<>"; break;
		case GREATEREQUAL			: strToken = ">="; break;
		case GREATER					: strToken = ">"; break;
		case SC_AND					: strToken = "&&"; break;
		case SC_OR					: strToken = "||"; break;
		case SC_IMPLIES				: strToken = "=>"; break;

		case CONTEXTT				: strToken = "context"; break;
		case INV 						: strToken = "inv"; break;
		case PRE 						: strToken = "pre"; break;
		case POST 						: strToken = "post"; break;
		case DEFATTRIBUTE			: strToken = "defattribute"; break;
		case DEFMETHOD				: strToken = "defattribute"; break;
		case IF 						: strToken = "if"; break;
		case THEN 						: strToken = "then"; break;
		case ELSE 						: strToken = "else"; break;
		case ENDIF 					: strToken = "endif"; break;
		case LET 						: strToken = "let"; break;
		case INN 						: strToken = "in"; break;
		case AND 						: strToken = "and"; break;
		case OR						: strToken = "or"; break;
		case XOR 						: strToken = "xor"; break;
		case NOT 						: strToken = "not"; break;
		case IMPLIES 					: strToken = "implies"; break;
		case TRUEE 					: strToken = "true"; break;
		case FALSEE 					: strToken = "false"; break;
		case UNDEFINED 				: strToken = "undefined"; break;
		case NULLL 					: strToken = "null"; break;

		case STRING					: strToken = "String"; break;
		case IDENTIFIER 				: strToken = "Identifier"; break;
		case INTEGER					: strToken = "Integer"; break;
		case REAL						: strToken = "Real"; break;

		case INPUTEND				: strToken = "InputEnd"; break;

		case CLASS_MULTIPLICATIVE		: strToken = "MultiplicativeOperator"; break;
		case CLASS_ADDITIVE 			: strToken = "AdditiveOperator"; break;
		case CLASS_RELATIONAL 			: strToken = "RelationalOperator"; break;
		case CLASS_LOGICAL				: strToken = "LogicalOperator"; break;
		case CLASS_UNARY 				: strToken = "UnaryOperator"; break;
		case CLASS_PREPOST 				: strToken = "pre post def"; break;
		case CLASS_STEREOTYPES		: strToken = "inv pre post def"; break;
		case CLASS_BOOLEAN 				: strToken = "Boolean"; break;
		case CLASS_CALLKIND 			: strToken = ". ->"; break;
		case CLASS_LOGICAL_AND		: strToken = "and &&"; break;
		case CLASS_LOGICAL_OR			: strToken = "or ||"; break;
		case CLASS_LOGICAL_IMPLIES	: strToken = "implies =>"; break;

		default 							: strToken = "UNKNOWN";
	}

	if ( bQuote ) {
		switch ( iToken ) {
			case INPUTEND					:
			case IDENTIFIER					:
			case INTEGER						:
			case REAL							:
			case STRING						:
			case CLASS_MULTIPLICATIVE		:
			case CLASS_ADDITIVE 			:
			case CLASS_RELATIONAL 			:
			case CLASS_UNARY 				:
			case CLASS_LOGICAL				:
			case CLASS_BOOLEAN 				: break;
			case CLASS_PREPOST 				: strToken = "\"pre\" \"post\" \"defattribute\" \"defmethod\""; break;
			case CLASS_STEREOTYPES		: strToken = "\"inv\" \"pre\" \"post\" \"defattribute\" \"defmethod\""; break;
			case CLASS_CALLKIND				: strToken = "\".\" \"->\""; break;
			case CLASS_LOGICAL_AND		: strToken = "\"and\" \"&&\""; break;
			case CLASS_LOGICAL_OR			: strToken = "\"or\" \"||\""; break;
			case CLASS_LOGICAL_IMPLIES	: strToken = "\"implies\" \"=>\""; break;

			default 							: strToken = "\"" + strToken; strToken += "\""; break;
		}
	}
	return strToken;
}

std::string PrintTokens( const TokenSet& setTokens )
{
	std::string strTokens( " " );
	for ( TokenSet::const_iterator i = setTokens.begin() ; i != setTokens.end() ; ++i )
		strTokens += PrintToken( *i, true ) + " ";
	return strTokens;
}

bool Contains( const TokenSet& setTokens, int iToken )
{
	if ( setTokens.empty() )
		return false;
	switch ( iToken ) {
		case MULTIPLE :	case DIVIDE :	case DIVIDEINT : case PERCENT : case MODULO :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_MULTIPLICATIVE );
				if ( i != setTokens.end() )
					return true;
				break;
			}
		case NOT :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_UNARY );
				if ( i != setTokens.end() )
					return true;
				break;
			}
		case MINUS :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_UNARY );
				if ( i != setTokens.end() )
					return true;
				i = setTokens.find( CLASS_ADDITIVE );
				if ( i != setTokens.end() )
					return true;
				break;
			}
		case PLUS :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_ADDITIVE );
				if ( i != setTokens.end() )
					return true;
				break;
			}
		case LESS : case LESSEQUAL : case LESSGREATER : case EQUAL : case DOUBLEEQUAL : case NONEQUAL : case GREATER : case GREATEREQUAL :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_RELATIONAL );
				if ( i != setTokens.end() )
					return true;
				break;
			}
		case XOR :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_LOGICAL );
				if ( i != setTokens.end() )
					return true;
				break;
			}
		case AND : case SC_AND :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_LOGICAL_AND );
				if ( i != setTokens.end() )
					return true;
				i = setTokens.find( CLASS_LOGICAL );
				if ( i != setTokens.end() )
					return true;
				break;
			}
		case OR : case SC_OR :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_LOGICAL_OR );
				if ( i != setTokens.end() )
					return true;
				i = setTokens.find( CLASS_LOGICAL );
				if ( i != setTokens.end() )
					return true;
				break;
			}
		case IMPLIES : case SC_IMPLIES :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_LOGICAL_IMPLIES );
				if ( i != setTokens.end() )
					return true;
				i = setTokens.find( CLASS_LOGICAL );
				if ( i != setTokens.end() )
					return true;
				break;
			}
		case TRUEE: case FALSEE :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_BOOLEAN );
				if ( i != setTokens.end() )
					return true;
				break;
			}
		case DOT : case ARROWW :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_CALLKIND );
				if ( i != setTokens.end() )
					return true;
				break;
			}
		case PRE : case POST : case DEFATTRIBUTE : case DEFMETHOD :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_PREPOST );
				if ( i != setTokens.end() )
					return true;
			}
		case INV :
			{
				TokenSet::const_iterator i = setTokens.find( CLASS_STEREOTYPES );
				if ( i != setTokens.end() )
					return true;
				break;
			}
	}
	TokenSet::const_iterator i = setTokens.find( iToken );
	return i != setTokens.end();
}

bool Contains( const TokenSet& setTokens1, const TokenSet& setTokens2 )
{
	if ( setTokens2.empty() )
		return setTokens1.empty();
	for ( TokenSet::const_iterator i = setTokens2.begin() ; i != setTokens2.end() ; ++i )
		if ( ! Contains( setTokens1, *i ) )
			return false;
	return true;
}