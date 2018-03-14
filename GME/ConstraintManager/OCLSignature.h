//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLSignature.h
//
//###############################################################################################################################################

#ifndef OCLSignature_h
#define OCLSignature_h

#include "OCLCommon.h"
#include "OCLFormalParameter.h"

namespace OclSignature
{
	class Feature;
	class ParametralFeature;
	class TypeableFeature;

	class Attribute;
	class Association;
	class Iterator;
	class Method;
	class Operator;
	class Function;

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Feature
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Feature
	{
		public :
			enum FeatureKind { FK_OPERATOR = 0, FK_METHOD = 1, FK_FUNCTION = 2, FK_ATTRIBUTE = 3, FK_ASSOCIATION = 4, FK_ITERATOR = 5 };

		private :
			std::string		m_strName;
			FeatureKind 	m_eKind;

		protected :
									Feature( const std::string& strName, FeatureKind eKind );
		public :
			virtual 					~Feature();

					std::string		GetName() const;
					FeatureKind 	GetKind() const;
			virtual std::string		Print() const;
		protected :
					bool			IsIdentical( const Feature& object ) const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclSignature::ParametralFeature
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class ParametralFeature
	{
		private :
			OclCommon::FormalParameterVector	m_vecParameters;

		protected :
													ParametralFeature( const OclCommon::FormalParameterVector& vecParameters );
		public :
			virtual 									~ParametralFeature();

					int 								GetParameterCount() const;
					int 								GetMinParameterCount() const;
					const OclCommon::FormalParameter& 		GetParameter( int i ) const;
		protected :
					OclCommon::FormalParameterVector& 	GetParameters();
			virtual std::string 					Print() const;
					bool 							IsIdentical( const ParametralFeature& object ) const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclMeta::TypeableFeature
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class TypeableFeature
	{
		private :
			std::string	m_strTypeName;

		protected :
							TypeableFeature( const std::string& strTypeName );
							TypeableFeature();
		public :
			virtual 			~TypeableFeature();

					std::string 	GetTypeName() const;
			virtual std::string 	Print() const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Attribute <<< + Feature , + TypeableFeature
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Attribute
		: 	public Feature,
			public TypeableFeature
	{
		public :
							Attribute( const std::string& strName, const std::string& strTypeName );
		protected :
							Attribute( const std::string& strName );
		public :
			virtual 			~Attribute();

			virtual std::string 	Print() const;
					bool			IsIdentical( const Attribute& object ) const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Assocation <<< + Feature , + TypeableFeature
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Association
		: 	public Feature,
			public TypeableFeature
	{
		private :
			std::string	m_strAcceptable;

		public :
							Association( const std::string& strName, const std::string& strTypeName, const std::string& strAcceptable );
		protected :
							Association( const std::string& strName, const std::string& strAcceptable );
		public :
			virtual 			~Association();

			virtual std::string 	Print() const;
					std::string 	GetAcceptableTypeName() const;
					bool			IsIdentical( const Association& object ) const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Iterator <<< + Feature , + TypeableFeature, + ParametralFeature
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Iterator
		:	public Feature,
			public TypeableFeature,
			public ParametralFeature
	{
		public :
							Iterator( const std::string& strName, const std::string& strTypeName, const std::string& strParameterType );
		protected :
							Iterator( const std::string& strName, const std::string& strParameterType );
		public :
			virtual 			~Iterator();

			virtual std::string 	Print() const;
					bool			IsIdentical( const Iterator& object ) const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Method <<< + Feature , + TypeableFeature, + ParametralFeature
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Method
		: 	public Feature,
			public TypeableFeature,
			public ParametralFeature
	{
		public :
							Method( const std::string& strName, const std::string& strTypeName, const StringVector& vecTypes );
		protected :
							Method( const std::string& strName, const OclCommon::FormalParameterVector& vecParameters );
		public :
			virtual 			~Method();

			virtual std::string 	Print() const;
					bool			IsIdentical( const Method& object ) const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Operator <<< + Feature , + ParametralFeature
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Operator
		: public Feature, public ParametralFeature
	{
		public :
							Operator( const std::string& strName, const std::string& strOperand1Type );
							Operator( const std::string& strName, const std::string& strOperand1Type, const std::string& strOperand2Type );
		public :
			virtual 			~Operator();

			virtual std::string 	Print() const;
					bool			IsIdentical( const Operator& object ) const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Function <<< + Feature , + ParametralFeature
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Function
		: public Feature, public ParametralFeature
	{
		public :
							Function( const std::string& strName, const StringVector& vecTypes );
		protected :
							Function( const std::string& strName, const OclCommon::FormalParameterVector& vecParameters );
		public :
			virtual 			~Function();

			virtual std::string 	Print() const;
					bool			IsIdentical( const Function& object ) const;
	};

}; // namespace OclSignature

#endif // OCLSignature_h

