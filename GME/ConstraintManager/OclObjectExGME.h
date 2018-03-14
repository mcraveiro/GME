//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLObjectExGME.h
//
//###############################################################################################################################################

#ifndef OCLObjectExGME_h
#define OCLObjectExGME_h

#include "StdAfx.h"
#include "OCLCommon.h"
#include "OCLObject.h"
#include "mga.h"

	#define CREATE_GMEPROJECT( pManager, project )	\
		OclMeta::Object( new OclGmeCM::Project( pManager, project ) )

	#define CREATE_GMEOBJECT( pManager, object ) 	\
		OclMeta::Object( new OclGmeCM::Object( pManager, object ) )

	#define CREATE_GMECONNECTIONPOINT( pManager, object )	\
		OclMeta::Object( new OclGmeCM::ConnectionPoint( pManager, object ) )

	#define DECL_GMEOBJECT( varName, var2Name )		\
		CComPtr<IMgaObject> varName;					\
		if ( ! var2Name.IsUndefined() )						\
			( ( OclGmeCM::Object* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_GMEFOLDER( varName, var2Name )		\
		CComPtr<IMgaFolder> varName;					\
		if ( ! var2Name.IsUndefined() )						\
			( ( OclGmeCM::Object* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_GMEFCO( varName, var2Name )		\
		CComPtr<IMgaFCO> varName;					\
		if ( ! var2Name.IsUndefined() )						\
			( ( OclGmeCM::Object* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_GMESET( varName, var2Name )		\
		CComPtr<IMgaSet> varName;					\
		if ( ! var2Name.IsUndefined() )						\
			( ( OclGmeCM::Object* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_GMEATOM( varName, var2Name )		\
		CComPtr<IMgaAtom> varName;					\
		if ( ! var2Name.IsUndefined() )						\
			( ( OclGmeCM::Object* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_GMEMODEL( varName, var2Name )		\
		CComPtr<IMgaModel> varName;					\
		if ( ! var2Name.IsUndefined() )						\
			( ( OclGmeCM::Object* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_GMEREFERENCE( varName, var2Name )		\
		CComPtr<IMgaReference> varName;					\
		if ( ! var2Name.IsUndefined() )						\
			( ( OclGmeCM::Object* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_GMECONNECTION( varName, var2Name )		\
		CComPtr<IMgaConnection> varName;					\
		if ( ! var2Name.IsUndefined() )						\
			( ( OclGmeCM::Object* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_GMECONNECTIONPOINT( varName, var2Name )		\
		CComPtr<IMgaConnPoint> varName;					\
		if ( ! var2Name.IsUndefined() )						\
			( ( OclGmeCM::ConnectionPoint* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_GMEPROJECT( varName, var2Name )		\
		CComPtr<IMgaProject> varName;					\
		if ( ! var2Name.IsUndefined() )						\
			( ( OclGmeCM::Project* ) var2Name.GetImplementation() )->GetValue( varName )

namespace OclGmeCM
{
	typedef OclImplementation::Object Any;

//##############################################################################################################################################
//
//	C L A S S : OclGmeCM::Object <<< + Any
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Object
		: public Any
	{
		private :
			CComPtr<IMgaObject>	m_Value;

		public :
							Object( OclMeta::TypeManager* pManager, CComPtr<IMgaObject> value );
							Object( OclMeta::TypeManager* pManager, CComPtr<IMgaFCO> value );
							Object( OclMeta::TypeManager* pManager, CComPtr<IMgaFolder> value );
							Object( OclMeta::TypeManager* pManager, CComPtr<IMgaAtom> value );
							Object( OclMeta::TypeManager* pManager, CComPtr<IMgaModel> value );
							Object( OclMeta::TypeManager* pManager, CComPtr<IMgaConnection> value );
							Object( OclMeta::TypeManager* pManager, CComPtr<IMgaReference> value );
							Object( OclMeta::TypeManager* pManager, CComPtr<IMgaSet> value );
		private :
							void SetTypeName();
		public :
			virtual 	Any* 	Clone() const;

					int		GetValue( CComPtr<IMgaObject>& value ) const;
					void 	SetValue( CComPtr<IMgaObject> value );
					int		GetValue( CComPtr<IMgaFolder>& value ) const;
					void	SetValue( CComPtr<IMgaFolder> value );
					int		GetValue( CComPtr<IMgaFCO>& value ) const;
					void	SetValue( CComPtr<IMgaFCO> value );
					int		GetValue( CComPtr<IMgaModel>& value ) const;
					void	SetValue( CComPtr<IMgaModel> value );
					int		GetValue( CComPtr<IMgaAtom>& value ) const;
					void	SetValue( CComPtr<IMgaAtom> value );
					int		GetValue( CComPtr<IMgaReference>& value ) const;
					void	SetValue( CComPtr<IMgaReference> value );
					int		GetValue( CComPtr<IMgaConnection>& value ) const;
					void	SetValue( CComPtr<IMgaConnection> value );
					int		GetValue( CComPtr<IMgaSet>& value ) const;
					void	SetValue( CComPtr<IMgaSet> value );

			virtual 	bool 	Equals( const Any& object ) const;
					bool 	operator==( const Object& object ) const;
					bool 	operator!=( const Object& object ) const;

			virtual std::string 	Print() const;
			virtual IUnknown* GetObject() const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclGmeCM::ConnectionPoint <<< + Any
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class ConnectionPoint
		: public Any
	{
		private :
			CComPtr<IMgaConnPoint>	m_Value;

		public :
							ConnectionPoint( OclMeta::TypeManager* pManager, CComPtr<IMgaConnPoint> value );
			virtual 	Any* 	Clone() const;

					int		GetValue( CComPtr<IMgaConnPoint>& value ) const;
					void	SetValue( CComPtr<IMgaConnPoint> value );

			virtual 	bool 	Equals( const Any& object ) const;
					bool 	operator==( const ConnectionPoint& object ) const;
					bool 	operator!=( const ConnectionPoint& object ) const;

			virtual std::string 	Print() const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclGmeCM::Project <<< + Any
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Project
		: public Any
	{
		private :
			CComPtr<IMgaProject>	m_Value;

		public :
							Project( OclMeta::TypeManager* pManager, CComPtr<IMgaProject> value );
			virtual 	Any* 	Clone() const;

					int		GetValue( CComPtr<IMgaProject>& value ) const;
					void	SetValue( CComPtr<IMgaProject> value );

			virtual 	bool 	Equals( const Any& object ) const;
					bool 	operator==( const Project& object ) const;
					bool 	operator!=( const Project& object ) const;

			virtual std::string 	Print() const;
	};

}; // namespace OclGmeCM

#endif // OCLObjectExGME_h