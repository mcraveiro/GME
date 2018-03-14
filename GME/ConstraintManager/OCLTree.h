//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLTree.h
//
//###############################################################################################################################################

#ifndef OCLTree_h
#define OCLTree_h

#include "StdAfx.h"
#include "OCLCommon.h"
#include "OCLException.h"
#include "OCLObject.h"
#include "OCLContext.h"
#include "OCLType.h"
#include "OCLFormalParameter.h"
#include "OclViolation.h"

//udmoclpat_ocltree_h_1 __please do not remove or change this line__

namespace OclTree
{
	class TreeManager;

	class ObjectNodeAdaptor;
	class CollectionNodeAdaptor;
	class ConstraintFactory;

	class TreeNode;

	class ObjectNode;
	class CollectionNode;

	class DeclarationNode;
	class VariableNode;
	class TypeCastNode;
	class IfThenElseNode;

	class AttributeNode;
	class AssociationNode;
	class IteratorNode;
	class MethodNode;
	class OperatorNode;
	class FunctionNode;

	class ContextNode;
	class Constraint;
//udmoclpat_ocltree_h_2 __please do not remove or change this line__

	typedef OclCommon::ContextStack< TypeSeq > TypeContextStack;

	class TypeContext
	{
		public :
			TypeContextStack			m_ctxTypes;
			OclCommon::ExceptionPool	m_poolExceptions;
			StringVector					m_vecImplicits;
			std::string						m_strStereotype;
			OclMeta::DependencySet		m_setDependencies;
			std::string					m_namespace;

		public :
			TypeContext( const std::string& pNamesp )
				: m_strStereotype( "inv" )
				, m_namespace( pNamesp)
			{
			}

			TypeContext( const TypeContext& context )
				: m_ctxTypes( context.m_ctxTypes ), m_poolExceptions( context.m_poolExceptions ), m_vecImplicits( context.m_vecImplicits ), m_strStereotype( context.m_strStereotype ), m_setDependencies( context.m_setDependencies )
				, m_namespace( context.m_namespace)
			{
			}

			TypeContext& operator=( const TypeContext& context )
			{
				if ( this != &context ) {
					m_ctxTypes = context.m_ctxTypes;
					m_poolExceptions = context.m_poolExceptions;
					m_vecImplicits = context.m_vecImplicits;
					m_strStereotype = context.m_strStereotype;
					m_setDependencies = context.m_setDependencies; //FIXED, was m_setDependencies = m_setDependencies;
					m_namespace = context.m_namespace;
				}
				return *this;
			}
	};

	typedef OclCommon::ContextStack< OclMeta::Object > ObjectContextStack;

/*	transferred to OclViolation
	struct Violation {
		bool						bIsException;
		std::string						strMessage;
		std::string						strSignature;
		Position					position;
		StringVector				vecVariables;
		StringVector				vecObjects;
	};

	typedef std::vector< Violation >	ViolationVector;
*/
	struct ObjectContext {
		ObjectContextStack	oCtx;
		ViolationVector		vecViolations;
		bool					bDoSnapshot;
		int						iViolationCount;
		bool 					m_bHasException;
		bool 					m_bShortCircuitLogical;
		bool 					m_bShortCircuitIterator;
		bool 					m_bEnableTracking;
	};

	typedef std::vector< TreeNode* > TreeNodeVector;

//##############################################################################################################################################
//
//	A B S T R A C T   C L A S S : OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class TreeNode
	{
		public :	typedef enum NodeKind { NK_OBJECT = 0, NK_COLLECTION = 1, NK_DECLARATION = 2,  NK_VARIABLE = 3, NK_IFTHENELSE = 4, NK_OPERATOR = 5, NK_FUNCTION = 6, NK_ATTRIBUTE = 7, NK_ASSOCIATION = 8, NK_METHOD = 9,  NK_ITERATOR = 10, NK_CONTEXT = 11, NK_TYPECAST = 12, NK_CONSTRAINT = 13, NK_EXTENSION = 14 };

		private : 				TreeManager* 	m_pManager;
						const 	NodeKind			m_eKind;
		public :					TypeSeq			m_vecType;
								PositionMap		m_mapPositions;
								bool				m_bTester;
								bool				m_bSelfTester;

		public :
											TreeNode( TreeManager* pManager, NodeKind eKind );
			virtual 							~TreeNode();

					TreeManager* 		GetTreeManager() const;
					NodeKind				GetKind() const;

			virtual std::string 			Print( const std::string& strTabs ) const = 0;
					bool					Check( TypeContext& context );
			virtual 	OclMeta::Object 		Evaluate( ObjectContext& context ) = 0;

			 		bool 					ParseTypeSeq( TypeContext& context, const Position& position, std::string& strType, TypeSeq& vecType ) const;
			 		bool					CastType( TypeContext& context, const Position& position, const TypeSeq& vecTypeFrom, const TypeSeq& vecTypeTo ) const;
			 		VariableNode*			CreateThis( TypeContext& context, int iImplicitPos ) const;
			 		int						GetLastExceptionCode( TypeContext& context ) const;

			 		TypeSeq				GetParametralTypeSeq( const TypeSeq& vecType1, const TypeSeq& vecType2, const TypeSeq& vecTypeReturn );
			 		OclMeta::Feature* 	CheckAmbiguity( const std::vector<OclMeta::Type*>& vecTypes, const std::vector<OclSignature::Feature*>& vecSignatures, std::vector<int>& vecAmbiguities, int& iPrecedence, OclCommon::ExceptionPool& exAmbiguity );

			 		void 					AddViolation( ObjectContext& context, int iLine, const std::string& strSignature, const std::string& strMessage = "" );
					void					AddViolation(ObjectContext& context, Violation &violation);
			 		bool					IsBooleanReturned();
					OclMeta::Object 		CheckFalseResult( ObjectContext& context, OclMeta::Object spObject, int iLine, const std::string& strSignature );
					OclMeta::Object 		EvaluateCast( ObjectContext& context, OclMeta::Object spObject, int iLine, const std::string& strSignature, const std::string& strTypeName = "", bool bOnlyTest = true );

			protected :
			virtual 	void					CheckInitialize();
			virtual 	bool					CheckImplementation( TypeContext& context ) = 0;
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::ObjectNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class ObjectNode
		: public TreeNode
	{
		public :		std::string		m_strType;
						std::string 	m_strValue;
						bool 	m_bCallable;

													ObjectNode( TreeManager* pManager );

					virtual 	std::string				Print( const std::string& strTabs ) const;
					virtual 	bool					CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 		Evaluate( ObjectContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::CollectionNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class CollectionNode
		: public TreeNode
	{
		public :	std::string 	m_strType;
					TreeNodeVector 	m_vecNodes;

													CollectionNode( TreeManager* pManager );
													~CollectionNode();

					virtual 	std::string 			Print( const std::string& strTabs ) const;
					virtual 	bool 					CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 		Evaluate( ObjectContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::DeclarationNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class DeclarationNode
		: public TreeNode
	{
		public :	std::string 				m_strType;
					std::string 				m_strName;
					TreeNode* 		m_pValueNode;
					TreeNode* 		m_pInnerNode;
					TypeSeq			m_vecTypeDecl;

													DeclarationNode( TreeManager* pManager );
													~DeclarationNode();

					virtual 	std::string 			Print( const std::string& strTabs ) const;
					virtual 	OclMeta::Object 		Evaluate( ObjectContext& context );

					virtual 	void					CheckInitialize();
					virtual 	bool					CheckImplementation( TypeContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::TypeCastNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class TypeCastNode
		: public TreeNode
	{
		public :	std::string 	m_strType;
					TreeNode* 		m_pThisNode;
					bool			m_bIsDynamic;

													TypeCastNode( TreeManager* pManager );
													~TypeCastNode();

					virtual 	std::string 		Print( const std::string& strTabs ) const;
					virtual 	void				CheckInitialize();
					virtual 	bool					CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 		Evaluate( ObjectContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::IfThenElseNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class IfThenElseNode
		: public TreeNode
	{
		public :
					TreeNode* 	m_pIfNode;
					TreeNode* 	m_pThenNode;
					TreeNode* 	m_pElseNode;

													IfThenElseNode( TreeManager* pManager );
													~IfThenElseNode();

					virtual 	std::string 			Print( const std::string& strTabs ) const;
					virtual 	bool					CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 		Evaluate( ObjectContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::OperatorNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class OperatorNode
		: public TreeNode
	{
		public :
					std::string	m_strName;
					TreeNode* 	m_pOperandNode1;
					TreeNode* 	m_pOperandNode2;
					std::string 			m_strAssignVarName;

													OperatorNode( TreeManager* pManager );
													~OperatorNode();

					virtual 	std::string 			Print( const std::string& strTabs ) const;
					virtual 	void					CheckInitialize();
					virtual 	bool					CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 		Evaluate( ObjectContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::IteratorNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class LevelCounter
	{
		public:
			LevelCounter(int *count)
			{
				if (count)
				{
					counter = count;
					(*counter)++;
				}
			}
			~LevelCounter()
			{
				if (counter)
				{
					(*counter)--;
				}
			}

		private:
			int *counter;
	};

	class IteratorNode
		: public TreeNode
	{
		// recursion corrected: terge
		private:
			static int m_iteratorLevel;

		public :
					std::string 			m_strName;
					TreeNode* 				m_pThisNode;
					TreeNode*				m_pArgumentNode;

					StringVector 			m_vecDeclarators;
					std::string				m_strDeclType;
					TypeSeq					m_vecTypeDecl;

					std::string 			m_strAccuType;
					std::string 			m_strAccuName;
					TypeSeq 				m_vecTypeAccu;
					TreeNode* 				m_pAccuNode;

					std::string				m_strCallOperator;

												IteratorNode( TreeManager* pManager );
												~IteratorNode();

					virtual 	std::string 		Print( const std::string& strTabs ) const;
					virtual 	void				CheckInitialize();
					virtual 	bool				CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 	Evaluate( ObjectContext& context );
							bool				Iterate( ObjectContext& context, OclImplementation::Iterator* pIIterator, OclMeta::Object& objectThis, OclMeta::ObjectVector& vecArguments, int iDeclNum );
		private :
					bool 						CheckIterator( TypeContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::MethodNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class MethodNode
		: public TreeNode
	{
		private:
			static int	m_stackLevel;

		public :	std::string 			m_strName;
					TreeNode* 				m_pThisNode;
					TreeNodeVector			m_vecArguments;
					std::string				m_strCallOperator;

					IteratorNode*				m_pIterator;

												MethodNode( TreeManager* pManager );
												~MethodNode();

					virtual 	std::string 		Print( const std::string& strTabs ) const;
					virtual 	void				CheckInitialize();
					virtual 	bool				CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 	Evaluate( ObjectContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::FunctionNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class FunctionNode
		: public TreeNode
	{
		public :	std::string				m_strName;
					TreeNodeVector			m_vecArguments;

					MethodNode*				m_pMethod;

												FunctionNode( TreeManager* pManager );
												~FunctionNode();

					virtual 	std::string 		Print( const std::string& strTabs ) const;
					virtual 	void				CheckInitialize();
					virtual 	bool				CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 	Evaluate( ObjectContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::AssociationNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class AssociationNode
		: public TreeNode
	{
		public :	std::string 				m_strName;
					std::string					m_strAcceptable;
					TreeNode*					m_pThisNode;
					std::string 				m_strCallOperator;

												AssociationNode( TreeManager* pManager );
												~AssociationNode();

					virtual 	std::string 		Print( const std::string& strTabs ) const;
					virtual 	void				CheckInitialize();
					virtual 	bool				CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 	Evaluate( ObjectContext& context );
		private :
							bool 				CheckAssociation( TypeContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::AttributeNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class AttributeNode
		: public TreeNode
	{
		public :	std::string 				m_strName;
					TreeNode*					m_pThisNode;
					std::string 				m_strCallOperator;

					AssociationNode*			m_pAssociation;
					IteratorNode*				m_pCollector;

												AttributeNode( TreeManager* pManager );
												~AttributeNode();

					virtual 	std::string 		Print( const std::string& strTabs ) const;
					virtual 	void				CheckInitialize();
					virtual 	bool				CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 	Evaluate( ObjectContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::VariableNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class VariableNode
		: public TreeNode
	{
		public :	std::string 			m_strName;

					AttributeNode*			m_pAttribute;
					ObjectNode*				m_pType;

												VariableNode( TreeManager* pManager );
												~VariableNode();

					virtual 	std::string 		Print( const std::string& strTabs ) const;
					virtual 	void				CheckInitialize();
					virtual 	bool				CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 	Evaluate( ObjectContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::ContextNode <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class ContextNode
		: public TreeNode
	{
		public :		std::string 						m_strName;
						std::string 						m_strType;
						bool								m_bClassifier;
						std::string							m_strStereotype;
						OclMeta::DependencySet			m_setDependencies;

						OclCommon::FormalParameterVector 	m_vecParameters;
						std::string							m_strReturnType;
						TypeSeq								m_vecTypeReturn;

													ContextNode( TreeManager* pManager );

					virtual 	std::string 			Print( const std::string& strTabs ) const;
					virtual 	void					CheckInitialize();
					virtual 	bool					CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 		Evaluate( ObjectContext& context );
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::Constraint <<< + OclTree::TreeNode
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Constraint
		: public TreeNode
	{
		public :		std::string 	m_strName;
						ContextNode* 	m_pContext;
						TreeNode* 		m_pExpression;

													Constraint( TreeManager* pManager );
					virtual							~Constraint();

					virtual 	std::string 			Print( const std::string& strTabs ) const;
					virtual 	bool					CheckImplementation( TypeContext& context );
					virtual 	OclMeta::Object 		Evaluate( ObjectContext& context );
	};

//##############################################################################################################################################
//
//	A B S T R A C T   C L A S S : OclTree::ObjectNodeAdaptor
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class ObjectNodeAdaptor
	{
		public : 									ObjectNodeAdaptor() {}
					virtual 							~ObjectNodeAdaptor() {}

					virtual std::string 			Print( const std::string& strTabs, ObjectNode* pNode ) const;
					virtual bool 					Check( TypeContext& context, ObjectNode* pNode ) const;
					virtual OclMeta::Object 		Evaluate( ObjectContext& context, ObjectNode* pNode ) const;
	};

//##############################################################################################################################################
//
//	P U R E   A B S T R A C T   C L A S S : OclTree::CollectionNodeAdaptor
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class CollectionNodeAdaptor
	{
		public :
												CollectionNodeAdaptor() {}
			virtual 								~CollectionNodeAdaptor() {}

			virtual std::string 				Print( const std::string& strTabs, CollectionNode* pNode ) const;
			virtual bool 						Check( TypeContext& context, CollectionNode* pNode ) const;
			virtual OclMeta::Object	 		Evaluate( ObjectContext& context, CollectionNode* pNode ) const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclTree::TreeManager
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class TreeManager
	{
		private :		OclMeta::TypeManager* 		m_pTypeManager;
						ObjectNodeAdaptor* 			m_pOAdaptor;
						CollectionNodeAdaptor* 		m_pCAdaptor;

		public :
													TreeManager( OclMeta::TypeManager* pTypeManager, ObjectNodeAdaptor* pOAdaptor, CollectionNodeAdaptor* pCAdaptor );
			virtual									~TreeManager();

						OclMeta::TypeManager* 	GetTypeManager() const;

						ObjectNode* 				CreateObject() const;
						CollectionNode* 			CreateCollection() const;

						DeclarationNode* 			CreateDeclaration() const;
						VariableNode* 			CreateVariable() const;
						TypeCastNode*			CreateTypeCast() const;
						IfThenElseNode*			CreateIfThenElse() const;

						OperatorNode*			CreateOperator() const;
						IteratorNode* 			CreateIterator() const;
						MethodNode* 				CreateMethod() const;
						FunctionNode* 			CreateFunction() const;
						AssociationNode* 			CreateAssociation() const;
						AttributeNode* 			CreateAttribute() const;

						ContextNode* 				CreateContext() const;
						Constraint* 				CreateConstraint() const;

//udmoclpat_ocltree_h_3 __please do not remove or change this line__

		friend class ObjectNode;
		friend class CollectionNode;
	};

//udmoclpat_ocltree_h_4 __please do not remove or change this line__

}; // namespace OclTree


#endif // OclTree_h
