//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	GMEConstraintEx.h
//
//###############################################################################################################################################

#ifndef GMEConstraintEx_h
#define GMEConstraintEx_h

#include "StdAfx.h"
#include "OCLConstraint.h"
#include "OCLCommon.h"
#include "OCLType.h"
#include "OCLRefCount.h"
#include "mga.h"
#include "meta.h"

namespace OclGme
{
	class ConstraintBase;
	class Constraint;
	class ConstraintFunction;

	typedef OclCommon::RCSmart<Constraint> SpConstraint;
	typedef OclCommon::RCSmart<ConstraintFunction> SpConstraintFunction;


	typedef std::vector<SpConstraint>			ConstraintVector;
	typedef std::vector<SpConstraintFunction> 	ConstraintFunctionVector;

//##############################################################################################################################################
//
//	C L A S S : OclGme::ConstraintBase <<< + Ocl::Constraint
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class ConstraintBase
		: public Ocl::Constraint
	{
		public :
			typedef enum Type { CT_CRITICAL_META = 0, CT_NON_CRITICAL_META = 1, CT_CRITICAL_USER = 2, CT_NON_CRITICAL_USER = 3, CT_CRITICAL_LIBRARY = 4, CT_NON_CRITICAL_LIBRARY = 5, CT_ATTRIBUTE_META = 6, CT_ATTRIBUTE_USER = 7, CT_METHOD_META = 8, CT_METHOD_USER = 9 };
			typedef enum Location { CL_META = 0, CL_PROJECT = 1, CL_LIBRARY };

		private :
			Type							m_eType;
			Location						m_eLocation;
			StringVector					m_vecLibraryPath;
		protected :
			CComPtr<IMgaConstraint>	m_spConstraint;
			CComPtr<IMgaRegNode>		m_spRegNode;
			CComPtr<IMgaFCO>			m_spFCO;

		public :
										ConstraintBase();
			virtual 						~ConstraintBase();

					Type 				GetType() const;
					Location			GetLocation() const;
					StringVector		GetLibraryPath() const;
			virtual std::string			GetFullName() const;
					bool	GetContextAndName( std::string& strContext, std::string& strName ) const;
		protected :
					void 				SetType( Type eType, const StringVector& vecLibraryPath = StringVector() );
	};

//##############################################################################################################################################
//
//	C L A S S : OclGme::Constraint <<< + OclGme::ConstraintBase
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Constraint
		: public ConstraintBase, public OclCommon::ReferenceCountable< Constraint >
	{
		public :
			typedef enum EnableInfo { CE_NONE = 0, CE_ENABLED = 1, CE_ENABLED_INHERITED = 2, CE_ENABLED_READONLY = 3, CE_DISABLED = 4, CE_DISABLED_INHERITED = 5, CE_DISABLED_READONLY = 6 };

		private :
			unsigned long 					m_ulEventMask;
			long							m_lPriority;
			constraint_depth_enum		m_eDepth;
			std::string 					m_strMessage;
			std::string						m_strExpression;

		public :
										Constraint( const std::string& strName, const std::string& strContextType, const std::string& strExpression, const std::string& strMessage, unsigned long ulEventMask, long lPriority, constraint_depth_enum eDepth, bool bFieldsMandatory = true );
										Constraint( const std::string& strContextType, CComPtr<IMgaFCO> spFCO, bool bFieldsMandatory = true );
										Constraint( const std::string& strContextType, CComPtr<IMgaConstraint> spConstraint );
										Constraint( CComPtr<IMgaRegNode> spRegNode, const StringVector& vecLibPath );
			virtual						~Constraint();

			unsigned long				GetEventMask() const;
			long 						GetPriority() const;
			constraint_depth_enum 	GetDepth() const;
			std::string 				GetMessage() const;
			std::string					GetExpression() const;

			EnableInfo					GetEnableInfo( CComPtr<IMgaObject> spObject ) const;
			Ocl::Constraint::State 		Write( CComPtr<IMgaRegNode> spRegNode );
	};

//##############################################################################################################################################
//
//	C L A S S : OclGme::ConstraintFunction <<< + OclGme::ConstraintBase, OclCommon::ReferenceCountable< ConstraintFunction >
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class ConstraintFunction
		: public ConstraintBase, public OclCommon::ReferenceCountable< ConstraintFunction >
	{

		private :
			std::string 					m_strParameterList;
			std::string						m_strExpression; // func body

		public :
									ConstraintFunction( const std::string& strName, const std::string& strContextType, const std::string& strParameterList, const std::string& strReturnType, bool bIsAttribute, const std::string& strExpression, bool bFieldsMandatory = true );
									ConstraintFunction( CComPtr<IMgaFCO> spFCO, bool bFieldsMandatory = true );
									ConstraintFunction( CComPtr<IMgaConstraint> spConstraint );
									ConstraintFunction( CComPtr<IMgaRegNode> spRegNode );

			std::string				GetParameterList() const;
			std::string 			GetExpression() const;

			Ocl::Constraint::State 	Write( CComPtr<IMgaRegNode> spRegNode );
		private :
			void					ReadFromOldFormat();
			void					ReadFromNewFormat();
	};

}; // namespace OclGme

#endif // GMEConstraintEx_h