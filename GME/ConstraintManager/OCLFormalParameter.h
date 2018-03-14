//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLFormalParameter.h
//
//###############################################################################################################################################

#ifndef OCLFormalParameter_h
#define OCLFormalParameter_h

#include "OCLCommon.h"

namespace OclCommon
{
	class FormalParameter;
	typedef std::vector< FormalParameter > FormalParameterVector;

//##############################################################################################################################################
//
//	C L A S S : OclMeta::FormalParameter
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class FormalParameter
	{
		private :
			std::string	m_strName;
			std::string	m_strType;
			bool	m_bRequired;

		public :
			FormalParameter( const std::string& strName, const std::string& strType, bool bRequired )
				: m_strName( strName ), m_strType( strType ), m_bRequired( bRequired )
			{
			}

			FormalParameter( const std::string& strType, bool bRequired )
				: m_strName( "" ), m_strType( strType ), m_bRequired( bRequired )
			{
			}

			FormalParameter()
				: m_strName( "" ), m_strType( "" ), m_bRequired( false )
			{
			}

			FormalParameter( const FormalParameter& object )
				: m_strName( object.m_strName ), m_strType( object.m_strType ), m_bRequired( object.m_bRequired )
			{
			}

			FormalParameter& operator=( const FormalParameter& object )
			{
				if ( this != &object ) {
					m_strName = object.m_strName;
					m_strType = object.m_strType;
					m_bRequired = object.m_bRequired;
				}
				return *this;
			}

			bool IsIdentical( const FormalParameter& object ) const
			{
				return m_strType == object.m_strType;
			}

			bool IsIdentical( const std::string& strType ) const
			{
				return m_strType == strType;
			}

			bool operator==( const FormalParameter& object ) const
			{
				return IsIdentical( object ) && m_strName == object.m_strName && m_bRequired == object.m_bRequired;
			}

			bool operator!=( const FormalParameter& object ) const
			{
				return ! ( *this == object );
			}

			bool IsRequired() const
			{
				return m_bRequired;
			}

			std::string GetName() const
			{
				return m_strName;
			}

			std::string GetTypeName() const
			{
				return m_strType;
			}
	};

}; // namespace OclCommon

#endif // OCLFormalParameter_h
