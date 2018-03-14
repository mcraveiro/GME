//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLFeature.h
//
//###############################################################################################################################################

#ifndef OCLFeature_h
#define OCLFeature_h

#include "OCLCommon.h"
#include "OCLFormalParameter.h"
#include "OCLSignature.h"
#include "OCLFeatureImplementation.h"

namespace OclMeta
{
	class Feature;
	class Attribute;
	class Association;
	class Iterator;
	class Method;
	class Operator;
	class Function;

	typedef std::vector< Attribute* > AttributeVector;
	typedef std::vector< Association* > AssociationVector;
	typedef std::vector< Iterator* > IteratorVector;
	typedef std::vector< Method* > MethodVector;
	typedef std::vector< Operator* > OperatorVector;
	typedef std::vector< Function* > FunctionVector;

	class Dependency;

	typedef std::set< Dependency > DependencySet;

//##############################################################################################################################################
//
//	C L A S S : OclMeta::Feature
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Feature
		: public OclSignature::Feature
	{
		private :
			bool						m_bDynamic;
			bool 						m_bDependence;
			TypeSeq					m_vecTypeReturn;

		protected :
			Feature( const std::string& strName, const OclSignature::Feature::FeatureKind eKind, const TypeSeq& vecTypeReturn, bool bDynamic, bool bDependence = false )
				: OclSignature::Feature( strName, eKind ), m_vecTypeReturn( vecTypeReturn ), m_bDynamic( bDynamic ), m_bDependence( bDependence )
			{
			}

		public :
			virtual ~Feature()
			{
			}

			bool IsDynamic() const
			{
				return m_bDynamic;
			}

			TypeSeq GetReturnTypeSeq() const
			{
				return m_vecTypeReturn;
			}

			void SetReturnTypeSeq( TypeSeq& vecType )
			{
				m_vecTypeReturn = vecType;
			}

			bool IsDependence() const
			{
				return m_bDependence;
			}
	};

//##############################################################################################################################################
//
//	T E M P L A T E   C L A S S : OclMeta::FeatureAppendix
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	template < class TImplementation >
	class FeatureAppendix
	{
		private :
			TImplementation*		m_pImplementation;

		protected :
			FeatureAppendix( TImplementation* pImplementation )
				: m_pImplementation( pImplementation )
			{
			}

		public :
			virtual ~FeatureAppendix()
			{
				delete m_pImplementation;
			}

			TImplementation* GetImplementation() const
			{
				return m_pImplementation;
			}
	};

//##############################################################################################################################################
//
//	C L A S S : OclMeta::Attribute <<< + OclMeta::Feature, + OclSignature::Attribute, OclMeta::FeatureAppendix<OclImplementation::Attribute>
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Attribute
		: 	public OclMeta::Feature,
			public OclSignature::Attribute,
			public OclMeta::FeatureAppendix< OclImplementation::Attribute >
	{
		public :
			Attribute( const std::string& strName, const TypeSeq& vecTypeReturn, OclImplementation::Attribute* pImplementation, bool bDynamic, bool bDependence = false )
				: OclMeta::Feature( strName, OclSignature::Feature::FK_ATTRIBUTE, vecTypeReturn, bDynamic, bDependence ), OclSignature::Attribute( strName ), OclMeta::FeatureAppendix<OclImplementation::Attribute>( pImplementation )
			{
			}

			bool IsIdentical( const Attribute& object ) const
			{
				return ((OclSignature::Attribute)*this).IsIdentical( object );
			}
	};

//##############################################################################################################################################
//
//	C L A S S : OclMeta::Association <<< + OclMeta::Feature, + OclSignature::Association, OclMeta::FeatureAppendix<OclImplementation::Association>
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Association
		: 	public OclMeta::Feature,
			public OclSignature::Association,
			public OclMeta::FeatureAppendix< OclImplementation::Association >
	{
		public :
			Association( const std::string& strName, const std::string& strAcceptable, TypeSeq& vecTypeReturn, OclImplementation::Association* pImplementation, bool bDynamic )
				: OclMeta::Feature( strName, OclSignature::Feature::FK_ASSOCIATION, vecTypeReturn, bDynamic ), OclSignature::Association( strName, strAcceptable ), OclMeta::FeatureAppendix<OclImplementation::Association>( pImplementation )
			{
			}

			bool IsIdentical( const Association& object ) const
			{
				return ((OclSignature::Association)*this).IsIdentical( object );
			}
	};

//##############################################################################################################################################
//
//	C L A S S : OclMeta::Iterator <<< + OclMeta::Feature, + OclSignature::Iterator, OclMeta::FeatureAppendix<OclImplementation::Iterator>
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Iterator
		: 	public OclMeta::Feature,
			public OclSignature::Iterator,
			public OclMeta::FeatureAppendix< OclImplementation::Iterator >
	{
		public :
			Iterator( const std::string& strName, const std::string& strParameterType, TypeSeq& vecTypeReturn, OclImplementation::Iterator* pImplementation, bool bDynamic )
				: OclMeta::Feature( strName, OclSignature::Feature::FK_ITERATOR, vecTypeReturn, bDynamic ), OclSignature::Iterator( strName, strParameterType ), OclMeta::FeatureAppendix<OclImplementation::Iterator>( pImplementation )
			{
			}

			bool IsIdentical( const Iterator& object ) const
			{
				return ((OclSignature::Iterator)*this).IsIdentical( object );
			}
	};

//##############################################################################################################################################
//
//	C L A S S : OclMeta::Method <<< + OclMeta::Feature, + OclSignature::Method, OclMeta::FeatureAppendix<OclImplementation::Method>
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Method
		: 	public OclMeta::Feature,
			public OclSignature::Method,
			public OclMeta::FeatureAppendix< OclImplementation::Method >
	{
		public :
			Method( const std::string& strName, const OclCommon::FormalParameterVector& vecParameters, TypeSeq& vecTypeReturn, OclImplementation::Method* pImplementation, bool bDynamic, bool bDependence = false )
				: OclMeta::Feature( strName, OclSignature::Feature::FK_METHOD, vecTypeReturn, bDynamic, bDependence ), OclSignature::Method( strName, vecParameters ), OclMeta::FeatureAppendix<OclImplementation::Method>( pImplementation )
			{
			}

			bool IsIdentical( const Method& object ) const
			{
				return ((OclSignature::Method)*this).IsIdentical( object );
			}
	};

//##############################################################################################################################################
//
//	C L A S S : OclMeta::Operator
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Operator
		: 	public OclMeta::Feature,
			public OclSignature::Operator,
			public OclMeta::FeatureAppendix< OclImplementation::Operator >
	{
		public :
			Operator( const std::string& strName, const std::string& strOperand1Type, const TypeSeq& vecTypeReturn, OclImplementation::Operator* pImplementation, bool bDynamic )
				: OclMeta::Feature( strName, OclSignature::Feature::FK_OPERATOR, vecTypeReturn, bDynamic ), OclSignature::Operator( strName, strOperand1Type ), OclMeta::FeatureAppendix<OclImplementation::Operator>( pImplementation )
			{
			}

			Operator( const std::string& strName, const std::string& strOperand1Type, const std::string& strOperand2Type, const TypeSeq& vecTypeReturn, OclImplementation::Operator* pImplementation, bool bDynamic )
				: OclMeta::Feature( strName, OclSignature::Feature::FK_OPERATOR, vecTypeReturn, bDynamic ), OclSignature::Operator( strName, strOperand1Type, strOperand2Type ), OclMeta::FeatureAppendix<OclImplementation::Operator>( pImplementation )
			{
			}

			bool IsIdentical( const Operator& object ) const
			{
				return ((OclSignature::Operator)*this).IsIdentical( object );
			}
	};

//##############################################################################################################################################
//
//	C L A S S : OclMeta::Function <<< + OclMeta::Feature, + OclSignature::Function, OclMeta::FeatureAppendix<OclImplementation::Function>
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Function
		: 	public OclMeta::Feature,
			public OclSignature::Function,
			public OclMeta::FeatureAppendix< OclImplementation::Function >
	{
		public :
			Function( const std::string& strName, const OclCommon::FormalParameterVector& vecParameters, TypeSeq& vecTypeReturn, OclImplementation::Function* pImplementation, bool bDynamic )
				: OclMeta::Feature( strName, OclSignature::Feature::FK_FUNCTION, vecTypeReturn, bDynamic ), OclSignature::Function( strName, vecParameters ), OclMeta::FeatureAppendix<OclImplementation::Function>( pImplementation )
			{
			}

			bool IsIdentical( const Function& object ) const
			{
				return ((OclSignature::Function)*this).IsIdentical( object );
			}
	};

//##############################################################################################################################################
//
//	C L A S S : OclMeta::Dependency
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Dependency
	{
		public :
			Position 	m_position;
			std::string		m_strSignature;
			bool 		m_bFailed;
			bool 		m_bChecked;


			Dependency( const std::string& strSignature, const Position& position )
				: m_strSignature( strSignature), m_position( position ), m_bFailed( false ), m_bChecked( false )
			{
			}

			Dependency( const Dependency& d )
				: m_strSignature( d.m_strSignature ), m_position( d.m_position ), m_bFailed( d.m_bFailed ), m_bChecked( d.m_bChecked )
			{
			}

			Dependency& operator=( const Dependency& d )
			{
				if ( this != &d ) {
					m_strSignature = d.m_strSignature;
					m_position = d.m_position;
					m_bFailed = d.m_bFailed;
					m_bChecked = d.m_bChecked;
				}
				return *this;
			}

			bool operator==( const Dependency& d ) const
			{
				return m_strSignature == d.m_strSignature;
			}

			bool operator!=( const Dependency& d ) const
			{
				return ! ( *this == d );
			}

			bool operator<( const Dependency& d ) const
			{
				return m_strSignature < d.m_strSignature;
			}

			std::string Print() const
			{
				std::string strOut = "c: ";
				strOut += ( m_bChecked ) ? "1 " : "0 ";
				strOut += "f: ";
				strOut += ( m_bFailed ) ? "1 " : "0 ";
				return strOut + "s: " + m_strSignature;
			}

			static bool IsChecked( const DependencySet& ds )
			{
				for ( DependencySet::const_iterator i = ds.begin() ; i != ds.end() ; ++i )
					if ( ! (*i).m_bChecked )
						return false;
				return true;
			}

			static bool IsFailed( const DependencySet& ds )
			{
				for ( DependencySet::const_iterator i = ds.begin() ; i != ds.end() ; ++i )
					if ( ! (*i).m_bChecked )
						return false;
					else
						if ( (*i).m_bFailed )
							return true;
				return false;
			}

			static void SetChecked( DependencySet& ds, DependencySet::iterator i, bool bFailed )
			{
				Dependency d = *i;
				d.m_bFailed = bFailed;
				d.m_bChecked = true;
				ds.erase( i );
				ds.insert( d );
			}

			static DependencySet::iterator LookUp( DependencySet& ds, const std::string& strSignature )
			{
				Position pos;
				DependencySet::iterator it = ds.find( Dependency( strSignature, pos ) );
				return it;
			}

			static std::string Print( const DependencySet& setDependencies )
			{
				std::string strOut;
				for ( DependencySet::const_iterator it = setDependencies.begin() ; it != setDependencies.end() ; ++it )
					strOut += (*it).Print() + "\r\n";
				return strOut;
			}
	};

}; // namespace OclMeta

#endif // OCLFeature_h