//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLException.h
//
//###############################################################################################################################################

#ifndef OCLException_h
#define OCLException_h

#include "OCLCommon.h"

//###############################################################################################################################################
//
//	P R E D E F I N E D   C O D E S
//
//###############################################################################################################################################

#define EX_TYPE_DOESNT_EXIST	100
#define EX_TYPE_AMBIGUOUS	101
#define EX_BASETYPE_DOESNT_EXIST 102
#define EX_OPERATOR_AMBIGUOUS 103
#define EX_OPERATOR_DOESNT_EXIST 104
#define EX_FUNCTION_AMBIGUOUS 105
#define EX_FUNCTION_DOESNT_EXIST 106
#define EX_ATTRIBUTE_AMBIGUOUS 107
#define EX_ATTRIBUTE_DOESNT_EXIST 108
#define EX_ASSOCIATION_AMBIGUOUS 109
#define EX_ASSOCIATION_DOESNT_EXIST 110
#define EX_METHOD_AMBIGUOUS 111
#define EX_METHOD_DOESNT_EXIST 112
#define EX_ITERATOR_AMBIGUOUS 113
#define EX_ITERATOR_DOESNT_EXIST 114
#define EX_INVALID_TYPE			115
#define EX_TYPE_ISNT_COMPOUND	116
#define EX_VARIABLE_ALREADY_EXISTS	117
#define EX_BOOLEAN_REQUIRED 118
#define EX_IF_TYPE_MISMATCH 119
#define EX_TYPE_ISNT_A 120
#define EX_OPERATOR_NOT_IMPLEMENTED 121
#define EX_INVALID_CALL 122
#define EX_ITERATOR_NOT_IMPLEMENTED 123
#define EX_OBJECT_CALL_PROHIBITED 124
#define EX_METHOD_ITERATOR_AMBIGUOUS 125
#define EX_METHOD_NOT_IMPLEMENTED 126
#define EX_FUNCTION_NOT_IMPLEMENTED 127
#define EX_ASSOCIATION_NOT_IMPLEMENTED 128
#define EX_ATTRIBUTE_NOT_IMPLEMENTED 129
#define EX_ATTRIBUTE_ASSOCIATION_AMBIGUOUS 130
#define EX_VARIABLE_DOESNT_EXIST 131
#define EX_CANNOT_CREATE_OBJECT 132
#define EX_CANNOT_CREATE_COLLECTION 133
#define EX_ATTRIBUTE_INNERATTRIBUTE_AMBIGUOUS 134
#define EX_TYPE_NOT_COMPOUND_ITERATOR 135
#define EX_TYPE_COMPOUND_ASSOCIATION 136
#define EX_ITERATOR_ONE_ARGUMENT 137
#define EX_CONSTRAINT_DEF_RETURN_MISSING 138
#define EX_CONSTRAINT_DEF_RETURN_MISMATCH 139
#define EX_CONSTRAINT_DEF_FAILED 140
#define EX_ASSIGNMENT_TYPE_MISMATCH 141
#define EX_CAST_TYPE_MISMATCH 142
#define EX_IMPL_ITER_DOESNT_EXIST 143
#define EX_FUNC_IMPL_METH_ITER_DOESNT_EXIST 144
#define EX_IMPL_ASSOC_DOESNT_EXIST 145
#define EX_VAR_IMPL_ATTRIB_ASSOC_DOESNT_EXIST 146
#define EX_STOP_EVALUATION 147

//###############################################################################################################################################
//
//	M A C R O S
//
//###############################################################################################################################################

	#define THROWOCL0( type, code_or_message )	\
		throw OclCommon::Exception( OclCommon::Exception:: type, code_or_message )

	#define THROWOCL1( type, code_or_message, param1 )	\
		throw OclCommon::Exception( OclCommon::Exception:: type, code_or_message, param1 )

	#define THROWOCL2( type, code_or_message, param1, param2 )	\
		throw OclCommon::Exception( OclCommon::Exception:: type, code_or_message, param1, param2 )

	#define THROWOCL3( type, code_or_message, param1, param2, param3 )	\
		throw OclCommon::Exception( OclCommon::Exception:: type, code_or_message, param1, param2, param3 )

namespace OclCommon
{
	//class Exception;
	class ExceptionPool;

//###############################################################################################################################################
//
//	C L A S S : Exception
//
//===============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//###############################################################################################################################################

	class Exception
	{
		public :
			enum ExceptionType { ET_SYNTACTIC = 0, ET_SEMANTIC = 1, ET_RUNTIME = 2, ET_WARNING = 3 };

		private :
			int					m_iCode;
			std::string			m_strMessage;
			ExceptionType		m_eType;
			StringVector 		m_vecParameters;
			int 					m_iLine;
			int 					m_iColumn;

		public :
			Exception()
				: m_iCode( -1 ), m_iLine( -1 ), m_iColumn( -1 ), m_strMessage( "" ), m_eType( ET_SEMANTIC )
			{
			}

			Exception( ExceptionType eType, int iCode, int iLine = -1, int iColumn = -1 )
				: m_iCode( iCode ), m_iLine( iLine ), m_iColumn( iColumn ), m_strMessage( "" ), m_eType( eType )
			{
			}

			Exception( ExceptionType eType, int iCode, const std::string& param, int iLine = -1, int iColumn = -1 )
				: m_iCode( iCode ), m_strMessage( "" ), m_iLine( iLine ), m_iColumn( iColumn ), m_eType( eType )
			{
				m_vecParameters.push_back( param );
			}

			Exception( ExceptionType eType, int iCode, const std::string& param1, const std::string& param2, int iLine = -1, int iColumn = -1 )
				: m_iCode( iCode ), m_strMessage( "" ), m_iLine( iLine ), m_iColumn( iColumn ), m_eType( eType )
			{
				m_vecParameters.push_back( param1 );
				m_vecParameters.push_back( param2 );
			}

			Exception( ExceptionType eType, int iCode, const std::string& param1, const std::string& param2, const std::string& param3, int iLine = -1, int iColumn = -1 )
				: m_iCode( iCode ), m_strMessage( "" ), m_iLine( iLine ), m_iColumn( iColumn ), m_eType( eType )
			{
				m_vecParameters.push_back( param1 );
				m_vecParameters.push_back( param2 );
				m_vecParameters.push_back( param3 );
			}

			Exception( ExceptionType eType, int iCode, const StringVector& vecParameters, int iLine = -1, int iColumn = -1 )
				: m_iCode( iCode ), m_vecParameters( vecParameters ), m_strMessage( "" ), m_iLine( iLine ), m_iColumn( iColumn ), m_eType( eType )
			{
			}

			Exception( ExceptionType eType, const std::string& strMessage, int iLine = -1, int iColumn = -1 )
				: m_iCode( -1 ), m_strMessage( strMessage ), m_iLine( iLine ), m_iColumn( iColumn ), m_eType( eType )
			{
			}

			Exception( ExceptionType eType, const std::string& strMessage, const std::string& param, int iLine = -1, int iColumn = -1 )
				: m_iCode( -1 ), m_strMessage( strMessage ), m_iLine( iLine ), m_iColumn( iColumn ), m_eType( eType )
			{
				m_vecParameters.push_back( param );
			}

			Exception( ExceptionType eType, const std::string& strMessage, const std::string& param1, const std::string& param2, int iLine = -1, int iColumn = -1 )
				: m_iCode( -1 ), m_strMessage( strMessage ), m_iLine( iLine ), m_iColumn( iColumn ), m_eType( eType )
			{
				m_vecParameters.push_back( param1 );
				m_vecParameters.push_back( param2 );
			}

			Exception( ExceptionType eType, const std::string& strMessage, const std::string& param1, const std::string& param2, const std::string& param3, int iLine = -1, int iColumn = -1 )
				: m_iCode( -1 ), m_strMessage( strMessage ), m_iLine( iLine ), m_iColumn( iColumn ), m_eType( eType )
			{
				m_vecParameters.push_back( param1 );
				m_vecParameters.push_back( param2 );
				m_vecParameters.push_back( param3 );
			}


			Exception( ExceptionType eType, const std::string& strMessage, const StringVector& vecParameters, int iLine = -1, int iColumn = -1 )
				: m_iCode( -1 ), m_vecParameters( vecParameters ), m_strMessage( strMessage ), m_iLine( iLine ), m_iColumn( iColumn ), m_eType( eType )
			{
			}

			Exception( ExceptionType eType, const Exception& e )
				: m_iCode( e.m_iCode ), m_vecParameters( e.m_vecParameters ), m_iLine( e.m_iLine ), m_iColumn( e.m_iColumn ), m_strMessage( e.m_strMessage ), m_eType( e.m_eType )
			{
			}

			Exception& operator=( const Exception& e )
			{
				if ( this != &e ) {
					m_iCode = e.m_iCode;
					m_vecParameters = e.m_vecParameters;
					m_iLine = e.m_iLine;
					m_strMessage = e.m_strMessage;
					m_eType = e.m_eType;
				}
				return *this;
			}

			int GetLine()
			{
				return m_iLine;
			}

			int GetColumn()
			{
				return m_iColumn;
			}

			void SetLine( int iLine )
			{
				m_iLine = iLine;
			}

			void SetColumn( int iColumn )
			{
				m_iColumn = iColumn;
			}

			std::string GGetMessage();//z!

			int GetCode()
			{
				return m_iCode;
			}

			ExceptionType GetType()
			{
				return m_eType;
			}
	};

//##############################################################################################################################################
//
//	C L A S S : ExceptionPool
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class ExceptionPool
	{
		private :
			typedef std::vector< Exception > ExceptionVector;

			ExceptionVector	m_vecExceptions;
			bool				m_bHasError;

		public :
			ExceptionPool()
				: m_bHasError( false )
			{
			}

			ExceptionPool( const ExceptionPool& object )
				: m_vecExceptions( object.m_vecExceptions ), m_bHasError( object.m_bHasError )
			{
			}

			ExceptionPool& operator=( const ExceptionPool& object )
			{
				if ( this != &object ) {
					m_vecExceptions = object.m_vecExceptions;
					m_bHasError = object.m_bHasError;
				}
				return *this;
			}

			void Add( Exception& e )
			{
				m_vecExceptions.push_back( e );
				if ( e.GetType() != Exception::ET_WARNING )
					m_bHasError = true;
			}

			void Add( ExceptionPool& ePool )
			{
				for ( unsigned int i = 0 ; i < ePool.m_vecExceptions.size() ; i++ )
					Add( ePool.m_vecExceptions[ i ] );
			}

			int Size() const
			{
				return m_vecExceptions.size();
			}

			Exception& GetAt( int iPos ) const
			{
				return ((ExceptionPool*)this)->m_vecExceptions[ iPos ];
			}

			bool IsEmpty()
			{
				return m_vecExceptions.empty();
			}

			void Clear( int from = -1, int to = -1 )
			{
				if ( m_vecExceptions.empty() )
					return;
				if ( from < 0 )
					from = 0;
				if ( from > (int) m_vecExceptions.size() )
					return;
				if ( to < 0 || to >= (int) m_vecExceptions.size() )
					to = m_vecExceptions.size() - 1;
				if ( from == 0 && to == (int) m_vecExceptions.size() - 1 )
					m_vecExceptions.clear();
				else {
					ExceptionVector vecTemp( m_vecExceptions );
					m_vecExceptions.clear();
					int i;
					for ( i = 0 ; i < from ; i++ )
						m_vecExceptions.push_back( vecTemp[ i ] );
					for ( i = to ; i < (int) vecTemp.size() ; i++ )
						m_vecExceptions.push_back( vecTemp[ i ] );
				}
				m_bHasError = false;
				for ( unsigned int i = 0 ; i < m_vecExceptions.size() ; i++ )
					if ( m_vecExceptions[ i ].GetType() != Exception::ET_WARNING ) {
						m_bHasError = true;
						break;
					}
			}

			bool HasError() const
			{
				return m_bHasError;
			}

			void SetLine( int iLine )
			{
				for ( unsigned int i = 0 ; i < m_vecExceptions.size() ; i++ )
					m_vecExceptions[ i ].SetLine( iLine );
			}

			void SetColumn( int iColumn )
			{
				for ( unsigned int i = 0 ; i < m_vecExceptions.size() ; i++ )
					m_vecExceptions[ i ].SetColumn( iColumn );
			}
	};

}; // namespace OclCommon

#endif // OCLException_h


