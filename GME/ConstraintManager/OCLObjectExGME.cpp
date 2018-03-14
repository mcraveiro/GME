//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLObjectExGME.cpp
//
//###############################################################################################################################################
#include "Solve4786.h"
#include "OCLObjectExGME.h"
#include "OCLCommonEx.h"
#include "OCLType.h"

namespace OclGmeCM
{

//##############################################################################################################################################
//
//	C L A S S : OclGmeCM::Object <<< + Any
//
//##############################################################################################################################################

	Object::Object( OclMeta::TypeManager* pManager, CComPtr<IMgaObject> value )
		: Any( pManager, "gme::Object" ), m_Value( value.p )
	{
		SetTypeName();
	}

	Object::Object( OclMeta::TypeManager* pManager, CComPtr<IMgaFolder> value )
		: Any( pManager, "gme::Folder" ), m_Value( value.p )
	{
		SetTypeName();
	}

	Object::Object( OclMeta::TypeManager* pManager, CComPtr<IMgaFCO> value )
		: Any( pManager, "gme::FCO" ), m_Value( value.p )
	{
		SetTypeName();
	}

	Object::Object( OclMeta::TypeManager* pManager, CComPtr<IMgaAtom> value )
		: Any( pManager, "gme::Atom" ), m_Value( value.p )
	{
		SetTypeName();
	}

	Object::Object( OclMeta::TypeManager* pManager, CComPtr<IMgaModel> value )
		: Any( pManager, "gme::Model" ), m_Value( value.p )
	{
		SetTypeName();
	}

	Object::Object( OclMeta::TypeManager* pManager, CComPtr<IMgaConnection> value )
		: Any( pManager, "gme::Connection" ), m_Value( value.p )
	{
		SetTypeName();
	}

	Object::Object( OclMeta::TypeManager* pManager, CComPtr<IMgaReference> value )
		: Any( pManager, "gme::Reference" ), m_Value( value.p )
	{
		SetTypeName();
	}

	Object::Object( OclMeta::TypeManager* pManager, CComPtr<IMgaSet> value )
		: Any( pManager, "gme::Set" ), m_Value( value.p )
	{
		SetTypeName();
	}

	void Object::SetTypeName()
	{
		if ( m_Value != NULL  &&  m_Value.p )
			m_strTypeName = "meta::" + OclCommonEx::GetObjectKind( m_Value );
	}

	Any* Object::Clone() const
	{
		Object* pObject = new Object( GetTypeManager(), m_Value );
		pObject->m_strTypeName = m_strTypeName;
		return pObject;
	}

	int Object::GetValue( CComPtr<IMgaObject>& value ) const
	{
		value = m_Value;
		return S_OK;
	}

	void Object::SetValue( CComPtr<IMgaObject> value )
	{
		m_Value = value.p;
		SetTypeName();
	}

	int Object::GetValue( CComPtr<IMgaFolder>& value ) const
	{
		if (m_Value == NULL)
		{
			value = NULL;
			return 0;
		}
		return m_Value->QueryInterface( &value );
	}

	void Object::SetValue( CComPtr<IMgaFolder> value )
	{
		m_Value = value.p;
		SetTypeName();
	}

	int Object::GetValue( CComPtr<IMgaFCO>& value ) const
	{
		if (m_Value == NULL)
		{
			value = NULL;
			return 0;
		}
		return m_Value->QueryInterface( &value );
	}

	void Object::SetValue( CComPtr<IMgaFCO> value )
	{
		m_Value = value.p;
		SetTypeName();
	}

	int Object::GetValue( CComPtr<IMgaModel>& value ) const
	{
		if (m_Value == NULL)
		{
			value = NULL;
			return 0;
		}
		return m_Value->QueryInterface( &value );
	}

	void Object::SetValue( CComPtr<IMgaModel> value )
	{
		m_Value = value.p;
		SetTypeName();
	}

	int Object::GetValue( CComPtr<IMgaAtom>& value ) const
	{
		if (m_Value == NULL)
		{
			value = NULL;
			return 0;
		}
		return m_Value->QueryInterface( &value );
	}

	void Object::SetValue( CComPtr<IMgaAtom> value )
	{
		m_Value = value.p;
		SetTypeName();
	}

	int Object::GetValue( CComPtr<IMgaReference>& value ) const
	{
		if (m_Value == NULL)
		{
			value = NULL;
			return 0;
		}
		return m_Value->QueryInterface( &value );
	}

	void Object::SetValue( CComPtr<IMgaReference> value )
	{
		m_Value = value.p;
		SetTypeName();
	}

	int Object::GetValue( CComPtr<IMgaConnection>& value ) const
	{
		if (m_Value == NULL)
		{
			value = NULL;
			return 0;
		}
		return m_Value->QueryInterface( &value );
	}

	void Object::SetValue( CComPtr<IMgaConnection> value )
	{
		m_Value = value.p;
		SetTypeName();
	}

	int Object::GetValue( CComPtr<IMgaSet>& value ) const
	{
		if (m_Value == NULL)
		{
			value = NULL;
			return 0;
		}
		return m_Value->QueryInterface( &value );
	}

	void Object::SetValue( CComPtr<IMgaSet> value )
	{
		m_Value = value.p;
		SetTypeName();
	}

	bool Object::Equals( const Any& object ) const
	{
		if ( GetTypeManager()->IsTypeA( object.GetTypeName(), "gme::Object" ) >= 0 )
			return m_Value == ( ( Object* ) &object )->m_Value;
		else
			return false;
	}

	bool Object::operator==( const Object& object ) const
	{
		return Equals( object );
	}
	bool Object::operator!=( const Object& object ) const
	{
		return ! ( *this == object );
	}

	std::string Object::Print() const
	{
		return OclCommonEx::GetObjectDescription( m_Value );
	}

	IUnknown* Object::GetObject() const
	{
		if (m_Value == NULL)
			return NULL;
		IUnknown* ret = NULL;
		m_Value.QueryInterface(&ret);
		return ret;
	}

//##############################################################################################################################################
//
//	C L A S S : OclGmeCM::ConnectionPoint <<< + Any
//
//##############################################################################################################################################

	ConnectionPoint::ConnectionPoint( OclMeta::TypeManager* pManager, CComPtr<IMgaConnPoint> value )
		: Any( pManager, "gme::ConnectionPoint" ), m_Value( value )
	{
	}

	Any* ConnectionPoint::Clone() const
	{
		return new ConnectionPoint( GetTypeManager(), m_Value );
	}

	int ConnectionPoint::GetValue( CComPtr<IMgaConnPoint>& value ) const
	{
		value = m_Value;
		return S_OK;
	}

	void ConnectionPoint::SetValue( CComPtr<IMgaConnPoint> value )
	{
		m_Value = value;
	}

	bool ConnectionPoint::Equals( const Any& object ) const
	{
		if ( GetTypeManager()->IsTypeA( object.GetTypeName(), "gme::ConnectionPoint" ) >= 0 )
			return m_Value == ( ( ConnectionPoint* ) &object )->m_Value;
		else
			return false;
	}

	bool ConnectionPoint::operator==( const ConnectionPoint& object ) const
	{
		return Equals( object );
	}

	bool ConnectionPoint::operator!=( const ConnectionPoint& object ) const
	{
		return ! ( *this == object );
	}

	std::string ConnectionPoint::Print() const
	{
		return OclCommonEx::GetConnectionPointDescription( m_Value );
	}

//##############################################################################################################################################
//
//	C L A S S : OclGmeCM::Project <<< + Any
//
//##############################################################################################################################################

	Project::Project( OclMeta::TypeManager* pManager, CComPtr<IMgaProject> value )
		: Any( pManager, "gme::Project" ), m_Value( value )
	{
	}

	Any* Project::Clone() const
	{
		return new Project( GetTypeManager(), m_Value );
	}

	int Project::GetValue( CComPtr<IMgaProject>& value ) const
	{
		value = m_Value;
		return S_OK;
	}

	void Project::SetValue( CComPtr<IMgaProject> value )
	{
		m_Value = value;
	}

	bool Project::Equals( const Any& object ) const
	{
		if ( GetTypeManager()->IsTypeA( object.GetTypeName(), "gme::Project" ) >= 0 )
			return m_Value == ( ( Project* ) &object )->m_Value;
		else
			return false;
	}

	bool Project::operator==( const Project& object ) const
	{
		return Equals( object );
	}

	bool Project::operator!=( const Project& object ) const
	{
		return ! ( *this == object );
	}

	std::string Project::Print() const
	{
		CString strName;
		COMTHROW( m_Value->get_Name( PutOut( strName ) ) );
		return std::string("gme::Project { name: ") + std::string(CStringA(strName)) + "; }";
	}

}; // namespace OclGmeCM