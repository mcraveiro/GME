//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLObject.h
//
//###############################################################################################################################################

#ifndef OCLObject_h
#define OCLObject_h

#include "OCLCommon.h"
#include "OCLRefCount.h"
#include "unknwn.h"

namespace OclMeta {
	class Object;
};

namespace  OclImplementation {
	class Object;
	class CompoundObject;
	class ObjectIterator;
}

namespace OclMeta
{
	class TypeManager;

	typedef std::vector< Object > ObjectVector;

//##############################################################################################################################################
//
//	C L A S S : OclMeta::Object <<< + OclCommon::RCSmart< OclImplementation::Object >
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Object
		: protected OclCommon::RCSmart< OclImplementation::Object >
	{
		public :
			static const Object UNDEFINED;

		private :
			std::string m_strStaticTypeName;

		public :
											Object();
											Object( OclImplementation::Object* pImplementation );
											Object( const Object& object );
			Object& 						operator=( const Object& object );
			Object 							Clone() const;

			bool	 						Equals( const Object& object ) const;
			bool 							operator==( const Object& object ) const;
			bool 							operator!=( const Object& object ) const;

			std::string						GetTypeName() const;
			std::string 					GetStaticTypeName() const;
			void 							SetStaticTypeName( const std::string& strStaticTypeName );
			bool 							IsCompound() const;
			bool							IsComparable() const;
			bool 							IsUndefined() const;
			OclImplementation::Object* 	GetImplementation() const;
			std::string						Print() const;
			virtual IUnknown*				GetObject() const;
	};

}; // namespace OclMeta

namespace OclImplementation
{

//##############################################################################################################################################
//
//	A B S T R A C T   C L A S S : OclImplementation::Object <<< + OclCommon::ReferenceCountable< OclImplementation::Object >
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Object
		: public OclCommon::ReferenceCountable< Object >
	{
		protected :
			std::string m_strTypeName;
			OclMeta::TypeManager*	m_pTypeManager;

		protected :
			Object( OclMeta::TypeManager* pManager, const std::string& strTypeName )
				: OclCommon::ReferenceCountable<Object>(), m_pTypeManager( pManager ), m_strTypeName( strTypeName )
			{
			}

		public :
			virtual ~Object()
			{
			}

			std::string GetTypeName() const
			{
				return m_strTypeName;
			}

			OclMeta::TypeManager* GetTypeManager() const
			{
				return m_pTypeManager;
			}

			virtual bool IsCompound() const
			{
				return false;
			}
			
			virtual bool IsComparable() const
			{
				if (m_strTypeName == "ocl::Integer"  ||  m_strTypeName == "ocl::Real"  ||
					m_strTypeName == "ocl::String")
					return true; 
				return false;
			}

			virtual Object* Clone() const
			{
				return NULL;
			}

			virtual bool Equals( const Object& object ) const
			{
				return this == &object;
			}

			virtual std::string Print() const = 0;

			virtual IUnknown* GetObject() const {return NULL;};

			virtual bool IsUndefined() const
			{
				return false;
			}
	};

//##############################################################################################################################################
//
//	A B S T R A C T   C L A S S : OclImplementation::CompoundObject <<< + OclImplementation::Object
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class CompoundObject
		: public Object
	{
		protected :
			CompoundObject( OclMeta::TypeManager* pManager, const std::string& strTypeName )
				: Object( pManager, strTypeName )
			{
			}

		public :
			virtual ~CompoundObject()
			{
			}

			bool IsCompound() const
			{
				return true;
			}

			virtual void AddObject( const OclMeta::Object& object ) = 0;

			virtual ObjectIterator* GetIterator() = 0;
	};

//##############################################################################################################################################
//
//	P U R E   A B S T R A C T   C L A S S : OclImplementation::ObjectIterator
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class ObjectIterator
	{
		public :
			ObjectIterator()
			{
			}

			virtual ~ObjectIterator()
			{
			}

			virtual bool HasNext() = 0;

			virtual OclMeta::Object GetNext() = 0;
	};

}; // namespace OclImplementation

#endif // OCLObject_h
