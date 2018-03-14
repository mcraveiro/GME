//######################################################################################################################################
//######################################################################################################################################
//
//	GRAMMAR FILE
//
//######################################################################################################################################
//######################################################################################################################################

#header
<<
	#include "Solve4786.h"
	#include "OCLTree.h"
	#include "OCLLexer.h"
	#include "OCLParserStatic.h"

	#undef DEBUG

	typedef ANTLRCommonToken ANTLRToken;

	#define ADDLINE( newid, node, oldid )	\
		{ pResult->m_mapPositions[ newid ] = node->m_mapPositions[ oldid ]; }

	#define INSLINE0( id ) \
		{ Position pos; pos.iLine = LT(1)->getLine(); pos.iColumn = -1; mapPositions.insert( PositionMap::value_type( id, pos ) ); }

	#define INSLINE1( id, line ) \
		{ Position pos; pos.iLine = line; pos.iColumn = -1; mapPositions.insert( PositionMap::value_type( id, pos ) ); }

	#pragma warning( disable : 4102 )

	#define IF_NO_EXC( expr ) \
		if ( m_ExceptionPool.IsEmpty() ) { \
			expr \
		}

	#define IF_EXC_NODE( node ) \
		if ( ! m_ExceptionPool.IsEmpty() && node ) { \
			delete node;	\
			node = NULL;	\
		}

	#define IF_EXC_NODES( nodes ) \
		{	\
			if ( ! m_ExceptionPool.IsEmpty() ) { \
				for ( unsigned int iiii = 0 ; iiii < nodes.size() ; iiii++ )	\
					if ( nodes[ iiii ] )	\
						delete nodes[ iiii ];	\
				nodes.clear();	\
			}	\
		}
>>

//######################################################################################################################################
//
//	 D E F A U L T   L E X E R   A N D   P A R S E R   D E F I N I T I O N
//
//######################################################################################################################################

#lexclass 	START

//######################################################################################################################################
//	TOKENS

//======================================================================================================================================
//	BASIC TOKENS

#token	INPUTEND				"@"				<< ; >>
#token								"[\ \t]+"			<< skip(); >>
#token								"\n"				<< skip(); newline(); >>
#token								"\r"					<< skip(); >>
#token								"//"					<< mode( COMMENT ); skip(); >>
#token								"\-\-"				<< mode( COMMENT ); skip(); >>

//======================================================================================================================================
//	PUNCTUATION, OPERATORS, BRACES.....

#token ARROWW					"\->"
#token DOUBLECOLON 			"::"
#token COLON 					":"
#token SEMICOLON 				";"
#token LEFT_BRACE 			"\{"
#token RIGHT_BRACE 			"\}"
#token LEFT_PARENTHESIS 	"\("
#token RIGHT_PARENTHESIS 	"\)"
#token LEFT_BRACKET 			"\["
#token RIGHT_BRACKET 		"\]"
#token POUND 					"#"
#token COMMA 					","
#token DELIMITER 				"\|"
#token ET 						"\@"
#token MULTIPLE 				"\*"
#token DIVIDE 					"\/"
#token DIVIDEINT				"div"
#token MODULO					"mod"
#token PLUS 						"\+"
#token MINUS 					"\-"
#token DOT 						"."
#token LEFT_CARET 			"<"
#token RIGHT_CARET 			">"
#token PERCENT					"\%"
#token EXCLAMATION			"\!"
#token EQUAL 					"="
#token NONEQUAL				"\!="
#token DOUBLEEQUAL			"=="
#token LESS						"<"
#token LESSEQUAL				"<="
#token LESSGREATER			"<>"
#token GREATEREQUAL			">="
#token GREATER					">"
#token ASSIGN					":="
#token SC_AND					"&&"
#token SC_OR						"\|\|"
#token SC_IMPLIES				"=>"
#token UNUSED					"UNUSED"

//======================================================================================================================================
//	KEYWORDS, CONSTANTS

#token CONTEXTT				"context"
#token INV 						"inv"
#token PRE 						"pre"
#token POST 						"post"
#token DEFATTRIBUTE			"defattribute"
#token DEFMETHOD				"defmethod"
#token IF 							"if"
#token THEN 						"then"
#token ELSE 						"else"
#token ENDIF 					"endif"
#token LET 						"let"
#token INN 						"in"
#token AND 						"and"
#token OR							"or"
#token XOR 						"xor"
#token NOT 						"not"
#token IMPLIES 					"implies"
#token TRUEE 					"true"
#token FALSEE 					"false"
#token UNDEFINED 				"undefined"
#token NULLL 						"null"
//udmoclpat_ocl_g_1 __please do not remove or change this line__

//======================================================================================================================================
//	GENERAL TOKENS

#token STRING					" \" ( ( ~[ \"\n\r ] ) | ( \\ ( [ ntbrf\\\" ] | [0-7] { [0-7] } | [0-3][0-7][0-7] ) ) | \\  )* \" "
#token IDENTIFIER 				" [a-zA-Z_] [a-zA-Z0-9_]* "
#token INTEGER					" [0-9]+ "
#token REAL						" ( [0-9]+ . [0-9]* | [0-9]* . [0-9]+ ) { [eE] { [\-\+] } [0-9]+ } "

//udmoclpat_ocl_g_2 __please do not remove or change this line__

//======================================================================================================================================
//	TOKEN CLASSES

#tokclass STEREOTYPES
{
	INV
	PRE
	POST
	DEFATTRIBUTE
	DEFMETHOD
}

#tokclass PREPOST
{
	PRE
	POST
	DEFATTRIBUTE
	DEFMETHOD
}

#tokclass CALL_OPERATORS
{
	DOT
	ARROWW
}

#tokclass LOGICAL_AND
{
	AND
	SC_AND
}

#tokclass LOGICAL_OR
{
	OR
	SC_OR
}

#tokclass LOGICAL_IMPLIES
{
	IMPLIES
	SC_IMPLIES
}

#tokclass LOGICAL_OPERATORS
{
	LOGICAL_AND
	LOGICAL_OR
	LOGICAL_IMPLIES
	XOR
}

#tokclass RELATIONAL_OPERATORS
{
	EQUAL
	DOUBLEEQUAL
	LESS
	LESSEQUAL
	LESSGREATER
	GREATEREQUAL
	GREATER
	NONEQUAL
}

#tokclass ADDITIVE_OPERATORS
{
	PLUS
	MINUS
}

#tokclass MULTIPLICATIVE_OPERATORS
{
	MULTIPLE
	DIVIDE
	MODULO
	DIVIDEINT
	PERCENT
}

#tokclass UNARY_OPERATORS
{
	MINUS
	NOT
}

#tokclass BOOLEAN
{
	TRUEE
	FALSEE
}

//######################################################################################################################################
//	PARSER AND ITS METHODS

class OCLParser
{
	<<
		private :
			OclTree::TreeManager* 			m_pManager;
			OCLLexer* 						m_pLexer;
			OclCommon::ExceptionPool		m_ExceptionPool;
			bool	 							m_bOnlyContext;
//udmoclpat_ocl_g_3 __please do not remove or change this line__
	>>

	<<
		public :
			static OclTree::Constraint* ParseConstraint( OclTree::TreeManager* pManager, const string& strConstraintArg, bool bOnlyContext, OclCommon::ExceptionPool& ePool )
			{
				InitializeTokenSets();
				string strConstraint( strConstraintArg );
				DLGStringInput input( ( const DLGChar* ) strConstraint.c_str() );
				OCLLexer lexer( &input );
				ANTLRTokenBuffer pipe( &lexer );
				_ANTLRTokenPtr token = new ANTLRToken;
				lexer.setToken( token );
				OCLParser parser( &pipe );

				parser.m_pManager = pManager;
				parser.m_pLexer = &lexer;
				parser.m_ExceptionPool.Clear();
				parser.init();

				int iParseSignal;
				OclTree::Constraint* pConstraint = NULL;
				parser.m_bOnlyContext = bOnlyContext;
				parser.constraint( &iParseSignal, pConstraint );

				ePool = parser.m_ExceptionPool;
				return pConstraint;
			}
	>>

	<<
		private :
			string Consume( const TokenSet& setTokens, bool bFirstIdentifierUnused )
			{
				if ( setTokens.empty() )
					return "";
				bool bConsumeIt = true;
				string strConsumed("");
				while ( bConsumeIt ) {
					ANTLRTokenType iCurrent = LA(1);
					string strCurrent = string( LT(1)->getText() );
					if ( bFirstIdentifierUnused && iCurrent == IDENTIFIER )
						iCurrent = UNUSED;
					bFirstIdentifierUnused = false;
					if ( iCurrent == INPUTEND )
						return strConsumed;
					for ( TokenSet::const_iterator i = setTokens.begin() ; i != setTokens.end() && bConsumeIt; ++i )
						switch ( *i ) {
							case CLASS_MULTIPLICATIVE :	if ( set_el( iCurrent, MULTIPLICATIVE_OPERATORS_set ) ) bConsumeIt = false; break;
							case CLASS_ADDITIVE :		  	if ( set_el( iCurrent, ADDITIVE_OPERATORS_set ) ) bConsumeIt = false; break;
							case CLASS_RELATIONAL :		if ( set_el( iCurrent, RELATIONAL_OPERATORS_set ) ) bConsumeIt = false; break;
							case CLASS_PREPOST :			if ( set_el( iCurrent, PREPOST_set ) ) bConsumeIt = false; break;
							case CLASS_UNARY :			if ( set_el( iCurrent, UNARY_OPERATORS_set ) ) bConsumeIt = false; break;
							case CLASS_BOOLEAN :			if ( set_el( iCurrent, BOOLEAN_set ) ) bConsumeIt = false; break;
							case CLASS_LOGICAL :			if ( set_el( iCurrent, LOGICAL_OPERATORS_set ) ) bConsumeIt = false; break;
							case CLASS_STEREOTYPES :	if ( set_el( iCurrent, STEREOTYPES_set ) ) bConsumeIt = false; break;
							case CLASS_CALLKIND :			if ( set_el( iCurrent, CALL_OPERATORS_set ) ) bConsumeIt = false; break;
							default : 							if ( *i == iCurrent ) bConsumeIt = false; break;
						}
					if ( bConsumeIt ) {
						consume();
						strConsumed += strCurrent + " ";
					}
				}
				return strConsumed;
			}
	>>

	<<
		private :
			void AddException( const string& strMessage, const string& strParam1, const string& strParam2, int iLine )
			{
				OclCommon::Exception exp( OclCommon::Exception( OclCommon::Exception::ET_SYNTACTIC, strMessage, strParam1, strParam2, iLine ) );
				m_ExceptionPool.Add( exp );
			}
	>>

	<<
		private :
			void PrintErrorSelection( string strViables, const TokenSet& setRequired, const TokenSet& setFollow )
			{
				int iLine = LT(1)->getLine();
				int iCurrent = LA(1);
				string strToken = ( iCurrent == INPUTEND ) ? "InputEnd" : "\"" + string( LT(1)->getText() ) + "\"";
				if ( iCurrent == IDENTIFIER && ! Contains( setRequired, IDENTIFIER ) )
					iCurrent = UNUSED;

				string strConsumed = Consume( setFollow, iCurrent == UNUSED );
				string strMessage;
				if ( iCurrent == INPUTEND )
					strMessage = "At the end of expression ? cannot be parsed";
				else
					strMessage = "At Token [ ? ] ? cannot be parsed.";
				if ( ! strConsumed.empty() )
					strMessage += " Ignored tokens are [ \" " + strConsumed + "\" ].";
				if ( iCurrent == INPUTEND )
					AddException( strMessage, strViables, "", iLine );
				else
					AddException( strMessage, strToken, strViables, iLine );
			}
	>>

	<<
		private :
			void PrintErrorToken( const int iToken, const TokenSet& setFollow )
			{
				int iLine = LT(1)->getLine();
				int iCurrent = LA(1);
				string strToken = ( iCurrent == INPUTEND ) ? "InputEnd" : "\"" + string( LT(1)->getText() ) + "\"";
				if ( iCurrent == IDENTIFIER && iToken != IDENTIFIER )
					iCurrent = UNUSED;

				string strViables = PrintToken( iToken, true );

				if ( iToken != INPUTEND && Contains( setFollow, iToken ) ) {
					if ( iCurrent == INPUTEND )
						AddException( "? is missing at the end of expression", strViables, "", iLine );
					else
						AddException( "? is missing before Token [ ? ].", strViables, strToken, iLine );
				}
				else {
					string strConsumed = Consume( setFollow, iCurrent == UNUSED );
					if ( iToken == INPUTEND )
						AddException( "Superfluous tokens [ \" ?\" ] are ignored at the end of expression.", strConsumed, "", iLine );
					else {
						if ( iCurrent == INPUTEND )
							AddException( "? is missing at the end of expression", strViables, "", iLine );
						else {
							string strMessage = "At Token [ ? ] ? cannot be parsed.";
							if ( ! strConsumed.empty() )
								strMessage += " Ignored tokens are [ \" " + strConsumed + "\" ].";
							AddException( strMessage, strToken, strViables, iLine );
						}
					}
				}
			}
	>>

	<<
		private :
			bool PrintErrorIteration( const string& strSeparator, const TokenSet& setFirst, const TokenSet& setFollow )
			{
				int iLine = LT(1)->getLine();
				int iCurrent = LA(1);
				string strToken = "\"" + string( LT(1)->getText() ) + "\"";
				if ( iCurrent == IDENTIFIER && ! Contains( setFirst, IDENTIFIER ) )
					iCurrent = UNUSED;

				if ( ! Contains( setFollow, iCurrent ) && Contains( setFirst, iCurrent ) ) {
					if ( iCurrent == INPUTEND )
						AddException( "? is missing at the end of expression.", strSeparator, "", iLine );
					else
						AddException( "? is missing before Token [ ? ].", strSeparator, strToken, iLine );
					return true;
				}
				return false;
			}
	>>

	<<
		private :
			OclTree::Constraint* CreateConstraint( const string& strName, OclTree::ContextNode* pCNode, OclTree::TreeNode* pNode, const PositionMap& mapPositions )
			{
				OclTree::Constraint* pResult = m_pManager->CreateConstraint();
				pResult->m_strName = strName;
				pResult->m_pContext = pCNode;
				pResult->m_pExpression = pNode;

				pResult->m_mapPositions = mapPositions;
				ADDLINE( LID_NODE_START, pCNode, LID_NODE_START );
				return pResult;
			}
	>>

	<<
		private :
			OclTree::ContextNode* CreateContext( const string& strName, const string& strType, const string& strStereotype, const PositionMap& mapPositions, const OclCommon::FormalParameterVector& vecParameters = OclCommon::FormalParameterVector(), const string& strReturnType = "" )
			{
				OclTree::ContextNode* pResult = m_pManager->CreateContext();
				pResult->m_strName = strName;
				pResult->m_strType = strType;
				pResult->m_strStereotype = strStereotype;
				pResult->m_bClassifier = strReturnType.empty();
				pResult->m_vecParameters = vecParameters;
				pResult->m_strReturnType = strReturnType;
				pResult->m_mapPositions = mapPositions;
				return pResult;
			}
	>>

	<<
		private :
			OclTree::DeclarationNode* CreateDeclaration( const string& strName, const string& strType, OclTree::TreeNode* pSNode, const PositionMap& mapPositions )
			{
				OclTree::DeclarationNode* pResult = m_pManager->CreateDeclaration();
				pResult->m_strName = strName;
				pResult->m_strType = strType;
				pResult->m_pValueNode = pSNode;
				pResult->m_mapPositions = mapPositions;
				return pResult;
			}
	>>

	<<
		private :
			OclTree::IfThenElseNode* CreateIfThenElse( OclTree::TreeNode* pIfNode, OclTree::TreeNode* pThenNode, OclTree::TreeNode* pElseNode, const PositionMap& mapPositions )
			{
				OclTree::IfThenElseNode* pResult = m_pManager->CreateIfThenElse();
				pResult->m_pIfNode = pIfNode;
				pResult->m_pThenNode = pThenNode;
				pResult->m_pElseNode = pElseNode;
				pResult->m_mapPositions = mapPositions;
				return pResult;
			}
	>>

	<<
		private :
			OclTree::TypeCastNode* CreateTypeCast( OclTree::TreeNode* pThisNode, OclTree::TreeNode* pTypeNode, bool bIsDynamic, const PositionMap& mapPositions )
			{
				OclTree::TypeCastNode* pResult = m_pManager->CreateTypeCast();
				pResult->m_pThisNode = pThisNode;
				pResult->m_bIsDynamic = bIsDynamic;
				pResult->m_mapPositions = mapPositions;
				if ( pTypeNode->GetKind() != OclTree::TreeNode::NK_VARIABLE )
					AddException( "OclAsType operator can be used only with a Type.", "", "", pResult->m_mapPositions[ LID_NODE_START ].iLine );
				else
					pResult->m_strType = ( (OclTree::VariableNode*) pTypeNode)->m_strName;
				delete pTypeNode;
				return pResult;
			}
	>>

	<<
		private :
			OclTree::MethodNode* CreateMethod( const string& strCallOperator, const string& strName, OclTree::TreeNode* pThisNode, const OclTree::TreeNodeVector& vecArguments, const PositionMap& mapPositions )
			{
				OclTree::MethodNode* pResult = m_pManager->CreateMethod();
				pResult->m_strCallOperator = strCallOperator;
				pResult->m_strName = strName;
				pResult->m_vecArguments = vecArguments;
				pResult->m_pThisNode = pThisNode;
				pResult->m_mapPositions = mapPositions;
				return pResult;
			}
	>>

	<<
		private :
			OclTree::FunctionNode* CreateFunction( const string& strName, const OclTree::TreeNodeVector& vecArguments, const PositionMap& mapPositions )
			{
				OclTree::FunctionNode* pResult = m_pManager->CreateFunction();
				pResult->m_strName = strName;
				pResult->m_vecArguments = vecArguments;
				pResult->m_mapPositions = mapPositions;
				return pResult;
			}
	>>

	<<
		private :
			OclTree::OperatorNode* CreateOperator( const string& strName, OclTree::TreeNode* pOperand1, OclTree::TreeNode* pOperand2, const PositionMap& mapPositions )
			{
				OclTree::OperatorNode* pResult = m_pManager->CreateOperator();
				pResult->m_strName = strName;
				pResult->m_pOperandNode1 = pOperand1;
				pResult->m_pOperandNode2 = pOperand2;
				pResult->m_mapPositions = mapPositions;
				ADDLINE( LID_NODE_START, pOperand1, LID_NODE_START );
				return pResult;
			}
	>>

	<<
		private :
			OclTree::IteratorNode* CreateIterator( const string& strCallOperator, const string& strName, OclTree::TreeNode* pThisNode, OclTree::TreeNode* pArgumentNode, const StringVector& vecDeclarators, const string& strDeclType, const string& strAccName, const string& strAccType, OclTree::TreeNode* pAccuNode, const PositionMap& mapPositions )
			{
				OclTree::IteratorNode* pResult = m_pManager->CreateIterator();
				pResult->m_strCallOperator = strCallOperator;
				pResult->m_strName = strName;
				pResult->m_pArgumentNode = pArgumentNode;
				pResult->m_pThisNode = pThisNode;
				pResult->m_vecDeclarators = vecDeclarators;
				pResult->m_strDeclType = strDeclType;
				pResult->m_mapPositions = mapPositions;
				pResult->m_strAccuName = strAccName;
				pResult->m_strAccuType = strAccType;
				pResult->m_pAccuNode = pAccuNode;
				if ( ! strAccName.empty() && ! ( strName == "iterate" ) )
					AddException( "Only the generic iterator [ iterate ] can have accumulator!", "", "", pResult->m_mapPositions[ LID_ACCUMULATOR_NAME ].iLine );
				return pResult;
			}
	>>

	<<
		private :
			OclTree::VariableNode* CreateVariable( const string& strName, const PositionMap& mapPositions )
			{
				OclTree::VariableNode* pResult = m_pManager->CreateVariable();
				pResult->m_strName = strName;
				pResult->m_mapPositions = mapPositions;
				return pResult;
			}
	>>

	<<
		private :
			OclTree::AttributeNode* CreateAttribute( const string& strCallOperator, const string& strName, OclTree::TreeNode* pThisNode, const PositionMap& mapPositions )
			{
				OclTree::AttributeNode* pResult = m_pManager->CreateAttribute();
				pResult->m_strName = strName;
				pResult->m_pThisNode = pThisNode;
				pResult->m_strCallOperator = strCallOperator;
				pResult->m_mapPositions = mapPositions;
				return pResult;
			}
	>>

	<<
		private :
			OclTree::AssociationNode* CreateAssociation( const string& strCallOperator, const string& strName, OclTree::TreeNode* pThisNode, const string& strAcceptable, const PositionMap& mapPositions )
			{
				OclTree::AssociationNode* pResult = m_pManager->CreateAssociation();
				pResult->m_strName = strName;
				pResult->m_pThisNode = pThisNode;
				pResult->m_strCallOperator = strCallOperator;
				pResult->m_strAcceptable = strAcceptable;
				pResult->m_mapPositions = mapPositions;
				return pResult;
			}
	>>

	<<
		private :
			void SetThisForFeature( OclTree::TreeNode*& pResult, OclTree::TreeNode* pThisNode, int iCallOperatorLine )
			{
				if ( pResult ) {
					switch ( pResult->GetKind() ) {
						case OclTree::TreeNode::NK_ATTRIBUTE : {
							OclTree::AttributeNode* pCasted = ( OclTree::AttributeNode* ) pResult;
							pCasted->m_pThisNode = pThisNode;
							break;
						}
						case OclTree::TreeNode::NK_ASSOCIATION : {
							OclTree::AssociationNode* pCasted = ( OclTree::AssociationNode* ) pResult;
							pCasted->m_pThisNode = pThisNode;
							break;
						}
						case OclTree::TreeNode::NK_METHOD : {
							OclTree::MethodNode* pCasted = ( OclTree::MethodNode* ) pResult;
							pCasted->m_pThisNode = pThisNode;
							break;
						}
						case OclTree::TreeNode::NK_ITERATOR : {
							OclTree::IteratorNode* pCasted = ( OclTree::IteratorNode* ) pResult;
							pCasted->m_pThisNode = pThisNode;
							break;
						}
						case OclTree::TreeNode::NK_TYPECAST : {
							OclTree::TypeCastNode* pCasted = ( OclTree::TypeCastNode* ) pResult;
							pCasted->m_pThisNode = pThisNode;
							break;
						}
					}
					Position pos; pos.iLine = iCallOperatorLine; pos.iColumn = -1;
					pResult->m_mapPositions[ LID_CALL_OPERATOR ] = pos;
					if ( pThisNode )
						ADDLINE( LID_NODE_START, pThisNode, LID_NODE_START );
				}
			}
	>>

	<<
		private :
			OclTree::ObjectNode* CreateObject( const string& strType, const string& strValue, bool bCallable, const PositionMap& mapPositions )
			{
				OclTree::ObjectNode* pResult = m_pManager->CreateObject();
				pResult->m_strType = strType;
				pResult->m_strValue = strValue;
				pResult->m_bCallable = bCallable;
				pResult->m_mapPositions = mapPositions;
				return pResult;
			}
	>>

	<<
		private :
			OclTree::CollectionNode* CreateCollection( const string& strType, const OclTree::TreeNodeVector& vecElements, const PositionMap& mapPositions )
			{
				OclTree::CollectionNode* pResult = m_pManager->CreateCollection();
				pResult->m_strType = strType;
				pResult->m_vecNodes = vecElements;
				pResult->m_mapPositions = mapPositions;
				return pResult;
			}
	>>
	
//udmoclpat_ocl_g_4 __please do not remove or change this line__

//######################################################################################################################################
//	RULES

//======================================================================================================================================
// 	rootExpression

rootExpression [ OclTree::TreeNode*& pNode ] :
	expression [ First_inputend, $pNode ]
	wrap_inputend
	<< IF_EXC_NODE( $pNode ) >>
;

//======================================================================================================================================
// 	constraint

constraint [ OclTree::Constraint*& pConstraint ] :
	<<
		string strName; OclTree::TreeNode* pNode = NULL; OclTree::ContextNode* pCNode = NULL;
		PositionMap mapPositions;
	>>
	contextDeclaration [ First_name, pCNode ]
	name [ First_colon, strName ]
	<< INSLINE0( LID_CONSTRAINT_NAME ); >>
	wrap_colon [ First_expression ]
	<< if ( m_bOnlyContext ) { IF_NO_EXC( $pConstraint = CreateConstraint( strName, pCNode, pNode, mapPositions ); ); return; } >>
	expression [ First_inputend, pNode ] //udmoclpat_ocl_g_5 __please do not remove or change this line__
	wrap_inputend
	<< IF_EXC_NODE( pNode ) >>
	<< IF_EXC_NODE( pCNode ) >>
	<< IF_NO_EXC( $pConstraint = CreateConstraint( strName, pCNode, pNode, mapPositions ); ) >>
;

//======================================================================================================================================
// 	contextDeclaration

contextDeclaration [ const TokenSet& setFollow, OclTree::ContextNode*& pCNode ] :
	<<
		PositionMap mapPositions;
	>>
	wrap_context [ First_contextDeclarationHelper ]
	<< INSLINE0( LID_NODE_START ); >>
	contextDeclarationHelper [ setFollow, $pCNode, mapPositions ]
;

//======================================================================================================================================
// 	contextDeclarationHelper

operationContextTester :
	IDENTIFIER
	(
		DOUBLECOLON
		IDENTIFIER
	) *
	LEFT_PARENTHESIS
;

contextDeclarationHelper [ const TokenSet& setFollow, OclTree::ContextNode*& pCNode, PositionMap& mapPositions ] :
	(
		( operationContextTester ) ?
		operationContext [ setFollow, $pCNode, mapPositions ]
		|
		classifierContext [ setFollow, $pCNode, mapPositions ]
	)
;
exception default :
	<<	PrintErrorSelection( "Context", First_name, setFollow ); >>

//======================================================================================================================================
// 	classifierContext

classifierContextWithNameTester :
	IDENTIFIER
	COLON
;

classifierContext [ const TokenSet& setFollow, OclTree::ContextNode*& pCNode, PositionMap& mapPositions ] :
	<<
		string strName("self"); string strType; string strNewType;
		bool bSubRule1In = false;
	>>
	name [ Union( Union( First_colon, DOUBLECOLON ), INV ), strType ]
	{
		(
			wrap_colon [ First_typeName ]
			typeName [ First_inv, strNewType ]
			<< strName = strType; strType = strNewType; >>
			|
			wrap_doublecolon [ First_typeName ]
			typeName[ First_inv, strNewType ]
			<< strType = strType + "::" + strNewType; >>
		)
		<< bSubRule1In = true; >>
	}
	<< INSLINE0( LID_CONTEXT_TYPE ); >>
	wrap_inv [ setFollow ]
	<< INSLINE0( LID_CONTEXT_STEREOTYPE ); >>
	<< IF_NO_EXC( $pCNode = CreateContext( strName, strType, "inv", mapPositions ); ) >>
;

//======================================================================================================================================
// 	operationContext

operationContext [ const TokenSet& setFollow, OclTree::ContextNode*& pCNode, PositionMap& mapPositions ] :
	<<
		OclCommon::FormalParameterVector vecParameters; string strType(""); string strName; string strReturnType; string strStereotype;
		bool bSubRule1In = false; bool bSubRule2In = false;
	>>
	typeName [ First_left_parenthesis, strName ]
	<< INSLINE0( LID_CONTEXT_TYPE ); >>
	<< INSLINE0( LID_FEATURE_NAME ); >>
	<<
		size_t iPos = strName.rfind( ':' );
		if ( iPos != string::npos )
			{ strType = strName.substr( 0, iPos - 1 ); strName = strName.substr( iPos + 1, strName.length() - iPos - 1 ); }
		else
			AddException( "Invalid operation name. Type and name of operation must be specified.", "", "", LT(1)->getLine() );
	>>
	wrap_left_parenthesis [ Union( First_formalParameterList, RIGHT_PARENTHESIS ) ]
	{
		formalParameterList [ ( First_right_parenthesis ), vecParameters, mapPositions ]
		<< bSubRule1In = true; >>
	}
	wrap_right_parenthesis [ Union( First_colon, CLASS_PREPOST ) ]
	{
		wrap_colon [ First_typeName ]
		typeName [ First_prePost, strReturnType ]
		<< INSLINE0( LID_CONTEXT_RETURNTYPE ); >>
		<< bSubRule2In = true; >>
	}
	prePost [ setFollow, strStereotype ]
	<< INSLINE0( LID_CONTEXT_STEREOTYPE ); >>
	<< IF_NO_EXC( $pCNode = CreateContext( strName, strType, strStereotype, mapPositions, vecParameters, strReturnType ); ) >>
;

//======================================================================================================================================
// 	formalParameterList

formalParameterList [ const TokenSet& setFollow, OclCommon::FormalParameterVector& vecParameters, PositionMap& mapPositions ] :
	<<
		string strName; string strType; int iPrmCnt = 0;
	>>
	formalParameter [ Union( Union( setFollow, SEMICOLON ), First_formalParameter ), strName, strType, $mapPositions, iPrmCnt ]
	<<
		$vecParameters.push_back( OclCommon::FormalParameter( strName, strType, true ) );
	>>
	(
		wrap_semicolon [ First_formalParameter ]
		formalParameter [ Union( Union( setFollow, SEMICOLON ), First_formalParameter ), strName, strType, $mapPositions, iPrmCnt ]
		<<
			$vecParameters.push_back( OclCommon::FormalParameter( strName, strType, true ) );
		>>
	) *
	<< if ( PrintErrorIteration( "\";\"", First_formalParameter, setFollow ) ) formalParameterList( &_signal, setFollow, vecParameters, mapPositions ); >>
;

//======================================================================================================================================
// 	formalParameter

formalParameter [ const TokenSet& setFollow, string& strName , string& strType, PositionMap& mapPositions, int& iPrmCnt ] :
	name [ First_colon, $strName ]
	<< INSLINE0( LID_PARAMETER_NAME + $iPrmCnt ); >>
	wrap_colon [ First_typeName ]
	typeName [ setFollow, $strType ]
	<< INSLINE0( LID_PARAMETER_TYPE + $iPrmCnt++ ); >>
;

//udmoclpat_ocl_g_6 __please do not remove or change this line__

//======================================================================================================================================
// 	expression

expression [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		OclTree::DeclarationNode* pDNode = NULL;
	>>
	(
		letExpression [ First_expression, pDNode ]
		expression [ setFollow, $pNode ] //udmoclpat_ocl_g_7 __please do not remove or change this line__
		<< IF_EXC_NODE( pDNode ) >>
		<< IF_EXC_NODE( pNode ) >>
		<< IF_NO_EXC( pDNode->m_pInnerNode = $pNode; $pNode = pDNode; ) >>
		|
		implicationExpression [ setFollow, $pNode ]
	)
;
exception default :
	<< PrintErrorSelection( "Expression", First_expression, setFollow ); >>

//======================================================================================================================================
// 	letExpression

letExpression [ const TokenSet& setFollow, OclTree::DeclarationNode*& pDNode ] :
	<<
		string strName; string strType(""); OclTree::TreeNode* pSNode = NULL;
		PositionMap mapPositions;
	>>
	wrap_let [ First_name ]
	<< INSLINE0( LID_NODE_START ); >>
	name [ Union( First_colon, EQUAL ), strName ]
	<< INSLINE0( LID_VARIABLE_NAME ); >>
	{
		wrap_colon [ First_typeName ]
		typeName [ First_equal, strType ]
		<< INSLINE0( LID_VARIABLE_TYPE ); >>
	}
	wrap_equal [ First_implicationExpression ]
	implicationExpression [ First_in, pSNode ]
	wrap_in [ setFollow ]
	<< IF_EXC_NODE( pSNode ) >>
	<< IF_NO_EXC( $pDNode = CreateDeclaration( strName, strType, pSNode, mapPositions ); ) >>
;

//======================================================================================================================================
// 	ifExpression

ifExpression [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		OclTree::TreeNode* pIfNode = NULL; OclTree::TreeNode* pThenNode = NULL; OclTree::TreeNode* pElseNode = NULL;
		PositionMap mapPositions;
	>>
	wrap_if [ First_expression ]
	<< INSLINE0( LID_NODE_START ); >>
	expression [ First_then, pIfNode ]
	wrap_then [ First_expression ]
	expression [ First_else, pThenNode ] //udmoclpat_ocl_g_8 __please do not remove or change this line__
	wrap_else [ First_expression ]
	expression [ First_endif, pElseNode ] //udmoclpat_ocl_g_9 __please do not remove or change this line__
	wrap_endif [ setFollow ]
	<< IF_EXC_NODE( pIfNode ) >>
	<< IF_EXC_NODE( pThenNode ) >>
	<< IF_EXC_NODE( pElseNode ) >>
	<< IF_NO_EXC( $pNode = CreateIfThenElse( pIfNode, pThenNode, pElseNode, mapPositions ); ) >>
;

//======================================================================================================================================
// 	implicationExpression

implicationExpression [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		OclTree::TreeNode* pSNode = NULL; string strOp;
		PositionMap mapPositions;
	>>
	orExpression [ Union( Union( setFollow, CLASS_LOGICAL_IMPLIES ), First_orExpression ), $pNode ]
	<< IF_EXC_NODE( $pNode ) >>
	{
		implicationExpressionRight [ Union( Union( setFollow, CLASS_LOGICAL_IMPLIES ), First_orExpression ), pSNode, strOp, mapPositions ]
		<< IF_EXC_NODE( pSNode ) >>
		<< IF_EXC_NODE( $pNode ) >>
		<< IF_NO_EXC( $pNode = CreateOperator( strOp, $pNode, pSNode, mapPositions ); ) pSNode = NULL; >>
	}
	<< if ( PrintErrorIteration( "BinaryOperator", First_orExpression, setFollow ) ) implicationExpression( &_signal, setFollow, $pNode ); >>
;

implicationExpressionRight [ const TokenSet& setFollow, OclTree::TreeNode*& pNode, string& strOp, PositionMap& mapPositions ] :
	<<
		OclTree::TreeNode* pSNode = NULL; string strOp2;
	>>
	wrap_implies [ First_orExpression, $strOp ]
	<< INSLINE0( LID_FEATURE_NAME ); >>
	orExpression [ Union( Union( setFollow, CLASS_LOGICAL_IMPLIES ), First_orExpression ), $pNode ]
	<< IF_EXC_NODE( $pNode ) >>
	{
		implicationExpressionRight [ Union( Union( setFollow, CLASS_LOGICAL_IMPLIES ), First_orExpression ), pSNode, strOp2, mapPositions ]
		<< IF_EXC_NODE( pSNode ) >>
		<< IF_EXC_NODE( $pNode ) >>
		<< IF_NO_EXC( $pNode = CreateOperator( strOp2, $pNode, pSNode, mapPositions ); ) pSNode = NULL; >>
	}
	<< if ( PrintErrorIteration( "BinaryOperator", First_orExpression, setFollow ) ) implicationExpression( &_signal, setFollow, $pNode ); >>
;

//======================================================================================================================================
// 	orExpression

orExpression [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		OclTree::TreeNode* pSNode = NULL; string strOp;
		PositionMap mapPositions;
	>>
	xorExpression [ Union( Union( setFollow, CLASS_LOGICAL_OR ), First_xorExpression ), $pNode ]
	<< IF_EXC_NODE( $pNode ) >>
	(
		wrap_or [ First_xorExpression, strOp ]
		<< INSLINE0( LID_FEATURE_NAME ); >>
		xorExpression [ Union( Union( setFollow, CLASS_LOGICAL_OR ), First_xorExpression ), pSNode ]
		<< IF_EXC_NODE( pSNode ) >>
		<< IF_NO_EXC( $pNode = CreateOperator( strOp, $pNode, pSNode, mapPositions ); ) pSNode = NULL; >>
	) *
;

//======================================================================================================================================
// 	xorExpression

xorExpression [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		OclTree::TreeNode* pSNode = NULL;
		PositionMap mapPositions;
	>>
	andExpression [ Union( Union( setFollow, XOR ), First_andExpression ), $pNode ]
	<< IF_EXC_NODE( $pNode ) >>
	(
		wrap_xor [ First_andExpression ]
		<< INSLINE0( LID_FEATURE_NAME ); >>
		andExpression [ Union( Union( setFollow, XOR ), First_andExpression ), pSNode ]
		<< IF_EXC_NODE( pSNode ) >>
		<< IF_NO_EXC( $pNode = CreateOperator( "xor", $pNode, pSNode, mapPositions ); ) pSNode = NULL; >>
	) *
;

//======================================================================================================================================
// 	andExpression

andExpression [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		OclTree::TreeNode* pSNode = NULL; string strOp;
		PositionMap mapPositions;
	>>
	relationalExpression [ Union( Union( setFollow, CLASS_LOGICAL_AND ), First_relationalExpression ), $pNode ]
	<< IF_EXC_NODE( $pNode ) >>
	(
		wrap_and [ First_relationalExpression, strOp ]
		<< INSLINE0( LID_FEATURE_NAME ); >>
		relationalExpression [ Union( Union( setFollow, CLASS_LOGICAL_AND ), First_relationalExpression ), pSNode ]
		<< IF_EXC_NODE( pSNode ) >>
		<< IF_NO_EXC( $pNode = CreateOperator( strOp, $pNode, pSNode, mapPositions ); ) pSNode = NULL; >>
	) *
;

//======================================================================================================================================
// 	relationalExpression

relationalExpression [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		OclTree::TreeNode* pSNode = NULL; string strOp;
		PositionMap mapPositions;
	>>
	additiveExpression [ Union( Union( setFollow, CLASS_RELATIONAL ), First_additiveExpression ), $pNode ]
	<< IF_EXC_NODE( $pNode ) >>
	(
		relationalOperator [ First_additiveExpression, strOp ]
		<< INSLINE0( LID_FEATURE_NAME ); >>
		additiveExpression [ Union( Union( setFollow, CLASS_RELATIONAL ), First_additiveExpression ), pSNode ]
		<< IF_EXC_NODE( pSNode ) >>
		<< IF_NO_EXC( $pNode = CreateOperator( strOp, $pNode, pSNode, mapPositions ); ) pSNode = NULL; >>
	) *
;

//======================================================================================================================================
// 	additiveExpression

additiveExpression [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		OclTree::TreeNode* pSNode = NULL; string strOp;
		PositionMap mapPositions;
	>>
	multiplicativeExpression [ Union( Union( setFollow, CLASS_ADDITIVE ), First_multiplicativeExpression ), $pNode ]
	<< IF_EXC_NODE( $pNode ) >>
	(
		addOperator [ First_multiplicativeExpression, strOp ]
		<< INSLINE0( LID_FEATURE_NAME ); >>
		multiplicativeExpression [ Union( Union( setFollow, CLASS_ADDITIVE ), First_multiplicativeExpression ), pSNode ]
		<< IF_EXC_NODE( pSNode ) >>
		<< IF_NO_EXC( $pNode = CreateOperator( strOp, $pNode, pSNode, mapPositions ); ) pSNode = NULL; >>
	) *
;

//======================================================================================================================================
// 	multiplicativeExpression

multiplicativeExpression [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		OclTree::TreeNode* pSNode = NULL; string strOp;
		PositionMap mapPositions;
	>>
	unaryExpression [ Union( Union( setFollow, CLASS_MULTIPLICATIVE ), First_unaryExpression ), $pNode ]
	<< IF_EXC_NODE( $pNode ) >>
	(
		multiplyOperator [ First_unaryExpression, strOp ]
		<< INSLINE0( LID_FEATURE_NAME ); >>
		unaryExpression [ Union( Union( setFollow, CLASS_MULTIPLICATIVE ), First_unaryExpression ), pSNode ]
		<< IF_EXC_NODE( pSNode ) >>
		<< IF_NO_EXC( $pNode = CreateOperator( strOp, $pNode, pSNode, mapPositions ); ) pSNode = NULL; >>
	) *
;

//======================================================================================================================================
// 	unaryExpression

unaryExpression [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		string strOp;
		PositionMap mapPositions;
		bool bSubRule1In = false;
	>>
	{
		unaryOperator [ First_postfixExpression, strOp ]
		<< INSLINE0( LID_NODE_START ); >>
		<< INSLINE0( LID_FEATURE_NAME ); >>
		<< bSubRule1In = true; >>
	}
	postfixExpression [ setFollow, $pNode ]
	<< IF_EXC_NODE( $pNode ) >>
	<< IF_NO_EXC( if ( ! strOp.empty() ) $pNode = CreateOperator( strOp, $pNode, NULL, mapPositions ); )>>
;

//======================================================================================================================================
// 	postfixExpression

primaryExpressionTester :
	BOOLEAN
	|
	STRING
	|
	REAL
	|
	INTEGER
	|
	POUND
	|
	IDENTIFIER
	(
		DOUBLECOLON
		IDENTIFIER
	) *
	LEFT_BRACE
	|
	LEFT_PARENTHESIS
	|
	IF
;

postfixExpression [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		OclTree::TreeNode* pSelf = NULL;
	>>
	(
		(
			( primaryExpressionTester ) ?
			primaryExpression [ Union( setFollow, CLASS_CALLKIND ), $pNode ]
			<< IF_EXC_NODE( $pNode ) >>
			<< IF_NO_EXC ( pSelf = $pNode; ) >>
			|
			featureCall [ Union( setFollow, CLASS_CALLKIND ), "", $pNode ]
			<< IF_EXC_NODE( $pNode ) >>
			<< IF_NO_EXC ( pSelf = $pNode; ) >>
		)
		exception default :
			<< PrintErrorSelection( "PrimaryExpression or FeatureCall", First_postfixExpression, Union( setFollow, CLASS_CALLKIND ) ); >>
	)
	featureCallWithThis [ setFollow, pSelf, $pNode ]
;

featureCallWithThis [ const TokenSet& setFollow, OclTree::TreeNode* pSelf, OclTree::TreeNode*& pNode ] :
	<<
		string strCallOperator;
		int iLine = -1;
	>>
	(
		callOperator [ First_featureCall, strCallOperator ]
		<< iLine = LT(1)->getLine(); >>
		featureCall [ Union( setFollow, CLASS_CALLKIND ), strCallOperator, $pNode ]
		<< IF_NO_EXC ( SetThisForFeature( $pNode, pSelf, iLine ); pSelf = $pNode; ) >>
	) *
	<< if ( PrintErrorIteration( "\".\" \"->\"", First_featureCall, setFollow ) ) featureCallWithThis( &_signal, setFollow, $pNode, $pNode ); >>
;

//======================================================================================================================================
// 	primaryExpression

primaryExpression [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	(
		literalCollection [ setFollow, $pNode ]
		|
		literal [ setFollow, $pNode ]
		|
		wrap_left_parenthesis [ First_expression ]
		expression [ First_right_parenthesis, $pNode ]
		wrap_right_parenthesis [ setFollow ]
		|
		ifExpression [ setFollow, $pNode ]
	)
;
exception default :
	<< PrintErrorSelection( "Expression", First_primaryExpression, setFollow ); >>

//======================================================================================================================================
// 	featureCall

featureCall [ const TokenSet& setFollow, const string& strCallOperator, OclTree::TreeNode*& pNode ] :
	<<
		string strName, strRole;
		PositionMap mapPositions; bool bWasInRule = false; bool bOperation = false;
	>>
	typeName [ Union( First_qualifiers, Union( First_featureCallParameters, setFollow ) ), strName ]
	<< INSLINE0( LID_FEATURE_NAME ); >>
	<< INSLINE0( LID_NODE_START ); >>
	{
		(
			featureCallParameters [ setFollow, strName, $strCallOperator, $pNode, mapPositions ]
			<< bOperation = true; >>
			|
			qualifiers [ setFollow, strRole, mapPositions ]
		)
		<< bWasInRule = true; >>
	}
	<< IF_NO_EXC (
		if ( ! bOperation ) {
			if ( ! bWasInRule )
				if ( ! $strCallOperator.empty() )
					$pNode = CreateAttribute( $strCallOperator, strName, NULL, mapPositions );
				else
					$pNode = CreateVariable( strName, mapPositions );
			else
				$pNode = CreateAssociation( $strCallOperator, strRole, NULL, strName, mapPositions );
		}
	) >>
;

//======================================================================================================================================
// 	featureCallParameters

featureCallParameters [ const TokenSet& setFollow, const string& strName, const string& strCallOperator, OclTree::TreeNode*& pNode, PositionMap& mapPositions ] :
	wrap_left_parenthesis [ First_featureCallParametersHelper ]
	featureCallParametersHelper [ First_right_parenthesis, $strName, $strCallOperator, $pNode, mapPositions ]
	wrap_right_parenthesis [ setFollow ]
;

//======================================================================================================================================
// 	featureCallParametersHelper

iteratorMethodCallTester :
	IDENTIFIER
	(
		COMMA
		IDENTIFIER
	) *
	(
		COLON
		|
		DELIMITER
		|
		SEMICOLON
	)
;

featureCallParametersHelper [ const TokenSet& setFollow, const string& strName, const string& strCallOperator, OclTree::TreeNode*& pNode, PositionMap& mapPositions ] :
	<<
		OclTree::TreeNodeVector vecArguments;
	>>
	(
		( iteratorMethodCallTester ) ?
		iteratorMethodCall [ setFollow, $strName, $strCallOperator, $pNode, mapPositions ]
		|
		(
			{
				actualParameterList [ setFollow, vecArguments ]
			}
			<< IF_EXC_NODES( vecArguments ) >>
			<< IF_NO_EXC (
				if ( $strName == "oclAsType" && vecArguments.size() == 1 )
					$pNode = CreateTypeCast( NULL, vecArguments[ 0 ], $strName == "oclAsKind", mapPositions );
				else
					if ( ! $strCallOperator.empty() )
						$pNode = CreateMethod( $strCallOperator, $strName, NULL, vecArguments, mapPositions );
					else
						$pNode = CreateFunction( $strName, vecArguments, mapPositions );
			) >>
		)
	)
;
exception default :
	<< PrintErrorSelection( "ParameterList", First_featureCallParametersHelper, setFollow ); >>

//======================================================================================================================================
// 	iteratorMethodCall

iteratorMethodCall [ const TokenSet& setFollow, const string& strName, const string& strCallOperator, OclTree::TreeNode*& pNode, PositionMap& mapPositions ] :
	<<
		OclTree::TreeNode* pArgNode = NULL; StringVector vecDeclarators; string strDeclType; string strAccName; string strAccType; OclTree::TreeNode* pAccNode = NULL;
	>>
	iteratorDeclaration [ First_expression, vecDeclarators, strDeclType, strAccName, strAccType, pAccNode, mapPositions ]
	<< IF_EXC_NODE( pAccNode ) >>
	expression [ setFollow, pArgNode ]  //udmoclpat_ocl_g_10 __please do not remove or change this line__
	<< IF_EXC_NODE( pArgNode ) >>
	<< IF_NO_EXC( $pNode = CreateIterator( $strCallOperator, $strName, NULL, pArgNode, vecDeclarators, strDeclType, strAccName, strAccType, pAccNode, mapPositions ); ) >>
;

//======================================================================================================================================
// 	iteratorDeclaration

iteratorDeclaration [ const TokenSet& setFollow, StringVector& vecDeclarators, string& strDeclType, string& strAccName, string& strAccType, OclTree::TreeNode*& pAccNode, PositionMap& mapPositions ] :
	<<
		string strName1, strName2, strType; bool bWasInRule1 = false; bool bWasInRule2 = false;
		int iLine = -1; int iLine2 = -1;
	>>
	// identifier
	name [ Union( Union( Union( Union( First_delimiter, COMMA ), SEMICOLON ), COLON ), EQUAL ), strName1 ]
	<< iLine = LT(1)->getLine(); >>
	{
		(
			// two iterator
			wrap_comma [ First_name ]
			name [ Union( First_delimiter, COLON ), strName2 ]
			<< vecDeclarators.push_back( strName1 ); vecDeclarators.push_back( strName2 ); >>
			<< INSLINE1( LID_DECLARATOR_NAME , iLine ); >>
			<< INSLINE0( LID_DECLARATOR_NAME + 1 ); >>
			{
				wrap_colon [ First_typeName ]
				typeName [ First_delimiter, $strDeclType ]
				<< INSLINE0( LID_DECLARATOR_TYPE ); >>
			}
			|
			// iterator, accumulator
			wrap_semicolon [ First_name ]
			name [ Union( First_colon, EQUAL ), strAccName ]
			<< vecDeclarators.push_back( strName1 ); >>
			<< INSLINE1( LID_DECLARATOR_NAME , iLine ); >>
			<< INSLINE0( LID_ACCUMULATOR_NAME ); >>
			{
				wrap_colon [ First_typeName ]
				typeName [ First_equal, $strAccType ]
				<< INSLINE0( LID_ACCUMULATOR_TYPE ); >>
			}
			wrap_equal [ First_expression ]
			expression [ First_delimiter, $pAccNode ]
			|
			// accumulator
			wrap_equal [ First_expression ]
			expression [ First_delimiter, $pAccNode ]
			<< strAccName = strName1; >>
			<< INSLINE1( LID_ACCUMULATOR_NAME , iLine ); >>
			|
			// identifier and typeName
			wrap_colon [ First_typeName ]
			typeName [ Union( Union( First_delimiter, SEMICOLON ), EQUAL ), strType ]
			<< iLine2 = LT( 1 )->getLine(); >>
			{
				(
					// accumulator
					wrap_equal [ First_expression ]
					expression [ First_delimiter, $pAccNode ]
					<< strAccType = strType; strAccName = strName1; >>
					<< INSLINE1( LID_ACCUMULATOR_NAME , iLine ); >>
					<< INSLINE1( LID_ACCUMULATOR_TYPE , iLine2 ); >>
					|
					// iterator , accumulator
					wrap_semicolon [ First_name ]
					name [ Union( First_colon, EQUAL ), strAccName ]
					<< vecDeclarators.push_back( strName1 ); >>
					<< INSLINE1( LID_DECLARATOR_NAME , iLine ); >>
					<< INSLINE1( LID_DECLARATOR_TYPE , iLine2 ); >>
					<< INSLINE0( LID_ACCUMULATOR_NAME ); >>
					{
						wrap_colon [ First_typeName ]
						typeName [ First_equal, strAccType ]
						<< INSLINE0( LID_ACCUMULATOR_TYPE ); >>
					}
					<< strDeclType = strType; >>
					wrap_equal [ First_expression ]
					expression [ First_delimiter, $pAccNode ]
				)
				<< bWasInRule2 = true; >>
			}
			<< if ( ! bWasInRule2 ) { vecDeclarators.push_back( strName1 ); strDeclType = strType; } >>
		)
		<< bWasInRule1 = true; >>
	}
	<< if ( ! bWasInRule1 ) { vecDeclarators.push_back( strName1 ); } >>
	wrap_delimiter [ setFollow ]
;

//======================================================================================================================================
// 	actualParameterList

actualParameterList [ const TokenSet& setFollow, OclTree::TreeNodeVector& vecArguments ] :
	<<
		OclTree::TreeNode* pNode = NULL;
	>>
	expression [ Union( Union( setFollow, COMMA ), First_expression ), pNode ]
	<< IF_EXC_NODE( pNode ) >>
	<< IF_NO_EXC( $vecArguments.push_back( pNode ); ) >>
	(
		wrap_comma [ First_expression ]
		expression [ Union( Union( setFollow, COMMA ), First_expression ), pNode ]
		<< IF_EXC_NODE( pNode ) >>
		<< IF_NO_EXC( $vecArguments.push_back( pNode ); ) >>
	) *
	<< if ( PrintErrorIteration( "\",\"", First_expression, setFollow ) ) actualParameterList( &_signal, setFollow, vecArguments ); >>
;

//======================================================================================================================================
// 	qualifiers
qualifiers [ const TokenSet& setFollow, string& strRole, PositionMap& mapPositions ] :
	wrap_left_bracket [ First_name ]
	name [ First_right_bracket, $strRole ]
	<< INSLINE0( LID_ROLE_NAME ); >>
	wrap_right_bracket [ setFollow ]
;

/*
qualifiers [ const TokenSet& setFollow, OclTree::TreeNodeVector& vecArguments ] :
	wrap_left_bracket [ First_actualParameterList ]
	actualParameterList [ First_right_bracket, vecArguments ]
	wrap_right_bracket [ setFollow ]
;
*/

//======================================================================================================================================
// 	literal

literal [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		string strValue; string strType; bool bCallable = false;
		PositionMap mapPositions;
	>>
	(
		wrap_string[ setFollow, strValue ]
		<< strType = "ocl::String"; bCallable = true; >>
		|
		wrap_real [ setFollow, strValue ]
		<< strType = "ocl::Real"; >>
		|
		wrap_integer [ setFollow, strValue ]
		<< strType = "ocl::Integer"; >>
		|
		wrap_pound [ First_name ]
		name [ setFollow, strValue ]
		<< strType = "ocl::Enumeration"; >>
		|
		wrap_boolean [ setFollow, strValue ]
		<< strType = "ocl::Boolean"; >>
	)
	<< INSLINE0( LID_NODE_START ); >>
	<< IF_NO_EXC( $pNode = CreateObject( strType, strValue, bCallable, mapPositions ); ) >>
;
exception default :
	<< PrintErrorSelection( "Literal", First_literal, setFollow ); >>

//======================================================================================================================================
// 	literalCollection

literalCollection [ const TokenSet& setFollow, OclTree::TreeNode*& pNode ] :
	<<
		string strType; OclTree::TreeNodeVector vecElements;
		PositionMap mapPositions;
	>>
	typeName [ First_left_brace, strType ]
	<< INSLINE0( LID_NODE_START ); >>
	wrap_left_brace [ Union( First_expressionListOrRange, RIGHT_BRACE ) ]
	{
		expressionListOrRange [ First_right_brace, vecElements ]
	}
	wrap_right_brace [ setFollow ]
	<< IF_EXC_NODES( vecElements ) >>
	<< IF_NO_EXC( $pNode = CreateCollection( strType, vecElements, mapPositions ); ) >>
;

//======================================================================================================================================
// 	expressionListOrRange

expressionListOrRange [ const TokenSet& setFollow, OclTree::TreeNodeVector& vecElements ] :
	<<
		OclTree::TreeNode* pNode = NULL;
	>>
	expression [ Union( Union( setFollow, COMMA ), First_expression ), pNode ]
	<< IF_EXC_NODE( pNode ) >>
	<< IF_NO_EXC( vecElements.push_back( pNode ); ) >>
	(
		wrap_comma [ First_expression ]
		expression [ Union( Union( setFollow, COMMA ), First_expression ), pNode ]
		<< IF_EXC_NODE( pNode ) >>
		<< IF_NO_EXC( vecElements.push_back( pNode ); ) >>
	) *
	<< if ( PrintErrorIteration( "\",\"", First_expression, setFollow ) ) expressionListOrRange( &_signal, setFollow, vecElements ); >>
;

//======================================================================================================================================
// 	typeName

typeName [ const TokenSet& setFollow, string& strName ] :
	<<
		string strTemp;
	>>
	name [ Union( setFollow, DOUBLECOLON ), strName ]
	(
		wrap_doublecolon [ First_name ]
		name [ Union( setFollow, DOUBLECOLON ), strTemp ]
		<< strName += "::" + strTemp; >>
	) *
;

//======================================================================================================================================
// 	name

name [ const TokenSet& setFollow, string& strName ] :
	id:IDENTIFIER
	<< strName = string( id->getText() ); >>
;
exception default : << PrintErrorToken( IDENTIFIER, setFollow ); >>

//======================================================================================================================================
// 	WRAP RULES

wrap_colon [ const TokenSet& setFollow ] :
	COLON
;
exception default : << PrintErrorToken( COLON, setFollow ); >>

wrap_doublecolon [ const TokenSet& setFollow ] :
	DOUBLECOLON
;
exception default : << PrintErrorToken( DOUBLECOLON, setFollow ); >>

wrap_context [ const TokenSet& setFollow ] :
	CONTEXTT
;
exception default : << PrintErrorToken( CONTEXTT, setFollow ); >>

wrap_inv [ const TokenSet& setFollow ] :
	INV
;
exception default : << PrintErrorToken( INV, setFollow ); >>

wrap_left_parenthesis [ const TokenSet& setFollow ] :
	LEFT_PARENTHESIS
;
exception default : << PrintErrorToken( LEFT_PARENTHESIS, setFollow ); >>

wrap_right_parenthesis [ const TokenSet& setFollow ] :
	RIGHT_PARENTHESIS
;
exception default : << PrintErrorToken( RIGHT_PARENTHESIS, setFollow ); >>

prePost [ const TokenSet& setFollow, string& strStereotypes ] :
	pp:PREPOST
	<< strStereotypes = string( pp->getText() ); >>
;
exception default : << PrintErrorToken( CLASS_PREPOST, setFollow ); >>

wrap_semicolon [ const TokenSet& setFollow ] :
	SEMICOLON
;
exception default : << PrintErrorToken( SEMICOLON, setFollow ); >>

wrap_let [ const TokenSet& setFollow ] :
	LET
;
exception default : << PrintErrorToken( LET, setFollow ); >>

wrap_equal [ const TokenSet& setFollow ] :
	EQUAL
;
exception default : << PrintErrorToken( EQUAL, setFollow ); >>

wrap_in [ const TokenSet& setFollow ] :
	INN
;
exception default : << PrintErrorToken( INN, setFollow ); >>

wrap_if [ const TokenSet& setFollow ] :
	IF
;
exception default : << PrintErrorToken( IF, setFollow ); >>

wrap_then [ const TokenSet& setFollow ] :
	THEN
;
exception default : << PrintErrorToken( THEN, setFollow ); >>

wrap_else [ const TokenSet& setFollow ] :
	ELSE
;
exception default : << PrintErrorToken( ELSE, setFollow ); >>

wrap_endif [ const TokenSet& setFollow ] :
	ENDIF
;
exception default : << PrintErrorToken( ENDIF, setFollow ); >>

wrap_implies [ const TokenSet& setFollow, string& strOp ] :
	op:LOGICAL_IMPLIES
	<< strOp = string( op->getText() ); >>
;
exception default : << PrintErrorToken( CLASS_LOGICAL_IMPLIES, setFollow ); >>

wrap_or [ const TokenSet& setFollow, string& strOp ] :
	op:LOGICAL_OR
	<< strOp = string( op->getText() ); >>
;
exception default : << PrintErrorToken( CLASS_LOGICAL_OR, setFollow ); >>

wrap_xor [ const TokenSet& setFollow ] :
	XOR
;
exception default : << PrintErrorToken( XOR, setFollow ); >>

wrap_and [ const TokenSet& setFollow, string& strOp ] :
	op:LOGICAL_AND
	<< strOp = string( op->getText() ); >>
;
exception default : << PrintErrorToken( CLASS_LOGICAL_AND, setFollow ); >>

relationalOperator [ const TokenSet& setFollow, string& strOp ] :
	op:RELATIONAL_OPERATORS
	<< strOp = string( op->getText() ); >>
;
exception default : << PrintErrorToken( CLASS_RELATIONAL, setFollow ); >>

addOperator [ const TokenSet& setFollow, string& strOp ] :
	op:ADDITIVE_OPERATORS
	<< strOp = string( op->getText() ); >>
;
exception default : << PrintErrorToken( CLASS_ADDITIVE, setFollow ); >>

multiplyOperator [ const TokenSet& setFollow, string& strOp ] :
	op:MULTIPLICATIVE_OPERATORS
	<< strOp = string( op->getText() ); >>
;
exception default : << PrintErrorToken( CLASS_MULTIPLICATIVE, setFollow ); >>

unaryOperator [ const TokenSet& setFollow, string& strOp ] :
	op:UNARY_OPERATORS
	<< strOp = string( op->getText() ); >>
;
exception default : << PrintErrorToken( CLASS_UNARY, setFollow ); >>

callOperator [ const TokenSet& setFollow, string& strCallOperator ] :
	op:CALL_OPERATORS
	<< strCallOperator = string( op->getText() ); >>
;
exception default : << PrintErrorToken( CLASS_CALLKIND, setFollow ); >>

wrap_comma [ const TokenSet& setFollow ] :
	COMMA
;
exception default : << PrintErrorToken( COMMA, setFollow ); >>

wrap_delimiter [ const TokenSet& setFollow ] :
	DELIMITER
;
exception default : << PrintErrorToken( DELIMITER, setFollow ); >>

wrap_left_bracket [ const TokenSet& setFollow ] :
	LEFT_BRACKET
;
exception default : << PrintErrorToken( LEFT_BRACKET, setFollow ); >>

wrap_right_bracket [ const TokenSet& setFollow ] :
	RIGHT_BRACKET
;
exception default : << PrintErrorToken( RIGHT_BRACKET, setFollow ); >>

wrap_left_brace [ const TokenSet& setFollow ] :
	LEFT_BRACE
;
exception default : << PrintErrorToken( LEFT_BRACE, setFollow ); >>

wrap_right_brace [ const TokenSet& setFollow ] :
	RIGHT_BRACE
;
exception default : << PrintErrorToken( RIGHT_BRACE, setFollow ); >>

wrap_string [ const TokenSet& setFollow, string& strValue ] :
	str:STRING
	<< strValue = string( str->getText() ); strValue  = strValue.substr( 1, strValue.length() - 2 ); >>
;
exception default : << PrintErrorToken( STRING, setFollow ); >>

wrap_real [ const TokenSet& setFollow, string& strValue ] :
	r:REAL
	<< strValue = string( r->getText() ); >>
;
exception default : << PrintErrorToken( REAL, setFollow ); >>

wrap_integer [ const TokenSet& setFollow, string& strValue ] :
	i:INTEGER
	<< strValue = string( i->getText() ); >>
;
exception default : << PrintErrorToken( INTEGER, setFollow ); >>

wrap_pound  [ const TokenSet& setFollow ] :
	POUND
;
exception default : << PrintErrorToken( POUND, setFollow ); >>

wrap_boolean [ const TokenSet& setFollow, string& strValue ] :
	b:BOOLEAN
	<< strValue = string( b->getText() ); >>
;
exception default : << PrintErrorToken( CLASS_BOOLEAN, setFollow ); >>

wrap_inputend :
	INPUTEND
;
exception default : << PrintErrorToken( INPUTEND, First_inputend ); >>

//udmoclpat_ocl_g_11 __please do not remove or change this line__

} // END Of OCLParser

//######################################################################################################################################
//
//	 C O M M E N T   L E X E R   D E F I N I T I O N
//
//######################################################################################################################################

#lexclass	COMMENT

//======================================================================================================================================
//	BASIC TOKENS

#token								"@"				<< ; >> // TODO; End of string
#token								"[\n\r]"				<< mode( START ); skip(); newline(); >>
#token								"~[\n\r]+"			<< skip(); >>

//udmoclpat_ocl_g_12 __please do not remove or change this line__
