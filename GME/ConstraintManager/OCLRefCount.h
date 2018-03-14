//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLRefCount.h
//
//###############################################################################################################################################

#ifndef OCLRefCount_h
#define OCLRefCount_h

namespace OclCommon
{
	template < class TRefCount > 	class ReferenceCountable;
	template < class TRefCount > 	class RCSmart;
	template < class TObject > 		class Smart;

//###############################################################################################################################################
//
//	T E M P L A T E   C L A S S : ReferenceCountable
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	template < class TRefCount >
	class ReferenceCountable
	{
		private :
			int	m_iRefCount;

		protected :
			ReferenceCountable()
				: m_iRefCount( 0 )
			{
			}

		public :
			virtual ~ReferenceCountable()
			{
			}

		friend class RCSmart< TRefCount >;
	};

//###############################################################################################################################################
//
//	T E M P L A T E   C L A S S : RCSmart
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	template < class TRefCount >
	class RCSmart
	{
		private :
			TRefCount* m_p;

		public :
			RCSmart()
				: m_p( 0 )
			{
			}

			RCSmart( TRefCount* q )
				: m_p( q )
			{
				IncRef();
			}

			RCSmart( const RCSmart<TRefCount>& q )
				: m_p( q.m_p )
			{
				IncRef();
			}

			virtual ~RCSmart()
			{
				DecRef();
			}

			RCSmart<TRefCount>& operator=( const RCSmart<TRefCount>& q )
			{
				if ( *this != q ) {
					DecRef();
					m_p = q.m_p;
					IncRef();
				}
				return *this;
			}

			RCSmart<TRefCount>& operator=( TRefCount* q )
			{
				if ( m_p != q ) {
					DecRef();
					m_p = q;
					IncRef();
				}
				return *this;
			}

			bool operator==( const RCSmart<TRefCount>& q ) const
			{
				return m_p == q.m_p;
			}

			bool operator!=( const RCSmart<TRefCount>& q ) const
			{
				return ! ( *this == q );
			}

			operator TRefCount*() const
			{
				return m_p;
			}

			TRefCount& operator *() const
			{
				return *m_p;
			}

			TRefCount* operator->() const
			{
				return m_p;
			}


			TRefCount* Ptr() const
			{
				return m_p;
			}

			bool IsNull() const
			{
				return m_p == 0;
			}

		private :
			void IncRef()
			{
				if  ( m_p )
					m_p->m_iRefCount++;
			}

			void DecRef()
			{
				if ( m_p )
					if ( --m_p->m_iRefCount == 0 ) {
						delete m_p;
						m_p = 0;
					}
			}
	};

//###############################################################################################################################################
//
//	T E M P L A T E   C L A S S : Smart
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	template < class TObject >
	class Smart
	{
		private :
			TObject* 	m_p;

		public :
			Smart()
				: m_p( 0 )
			{
			}

			Smart( TObject* q )
				: m_p( q )
			{
			}

			~Smart()
			{
				if ( m_p )
					delete m_p;
			}

			bool operator==( const Smart<TObject>& q ) const
			{
				return m_p == q.m_p;
			}

			bool operator!=( const Smart<TObject>& q ) const
			{
				return ! ( *this == q );
			}

			bool operator==( const TObject* const q ) const
			{
				return m_p == q;
			}

			bool operator!=( const TObject* const q ) const
			{
				return ! ( *this == q );
			}

			operator TObject*() const
			{
				return m_p;
			}

			TObject& operator *() const
			{
				return *m_p;
			}

			TObject* operator ->() const
			{
				return m_p;
			}

			TObject* Ptr() const
			{
				return m_p;
			}

			bool IsNull() const
			{
				return m_p == 0;
			}
	};

}; // namespace OclCommon

#endif // OCLRefCount_h
