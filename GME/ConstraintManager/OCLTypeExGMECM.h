//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLTypeExGMECM.h
//
//###############################################################################################################################################

#ifndef OCLTypeExGMECM_h
#define OCLTypeExGMECM_h

#include "OCLGMECMFacade.h"
#include "OCLTypeExBasic.h"
#include "OCLCommonEx.h"
#include "meta.h"

namespace OclGmeCM
{

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::String
//
//##############################################################################################################################################

	class TStringEx_MethodFactory
		: public OclBasic::TString_MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Collection
//
//##############################################################################################################################################

	class TCollectionEx_MethodFactory
		: public OclBasic::TCollection_MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Project
//
//##############################################################################################################################################

	class TProject_AttributeFactory
		: public OclImplementation::AttributeFactory
	{
		 public :
			 virtual void GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures );
	};

	class TProject_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   gme::ConnectionPoint
//
//##############################################################################################################################################

	class TConnectionPoint_AttributeFactory
		: public OclImplementation::AttributeFactory
	{
		 public :
			 virtual void GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures );
	};


	class TConnectionPoint_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Object
//
//##############################################################################################################################################

	class TObject_AttributeFactory
		: public OclImplementation::AttributeFactory
	{
		 public :
			 virtual void GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures );
	};


	class TObject_MethodFactory
		: public OclImplementation::MethodFactory
	{
		 public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Folder
//
//##############################################################################################################################################

	class TFolder_MethodFactory
		: public OclImplementation::MethodFactory
	{
		 public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   gme::FCO
//
//##############################################################################################################################################

	class TFCO_AttributeFactory
		: public OclImplementation::AttributeFactory
	{
		 public :
			 virtual void GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures );
	};


	class TFCO_MethodFactory
		: public OclImplementation::MethodFactory
	{
		 public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Model
//
//##############################################################################################################################################

	class TModel_MethodFactory
		: public OclImplementation::MethodFactory
	{
		 public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Atom
//
//##############################################################################################################################################

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Connection
//
//##############################################################################################################################################

	class TConnection_MethodFactory
		: public OclImplementation::MethodFactory
	{
		 public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Reference
//
//##############################################################################################################################################

	class TReference_MethodFactory
		: public OclImplementation::MethodFactory
	{
		 public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Set
//
//##############################################################################################################################################

	class TSet_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   D E R I V E D   O F   gme::FCO
//
//##############################################################################################################################################

	class TFCODerived_AttributeFactory
		: public OclImplementation::AttributeFactory
	{
		private :
			CComPtr<IMgaMetaFCO>		m_spMetaFCO;

		public :
							TFCODerived_AttributeFactory( CComPtr<IMgaMetaFCO> spMetaFCO );

			 virtual	void 	GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	G L O B A L   F A C T O R I E S
//
//##############################################################################################################################################

	class TypeFactory
		: public OclImplementation::TypeFactory
	{
		private :
			CComPtr<IMgaProject> m_spProject;

		public :
							TypeFactory( CComPtr<IMgaProject> spProject );

			virtual  void 	GetTypes( const std::string& strName, const std::string& strNSpace, std::vector<std::unique_ptr<OclMeta::Type>>& vecTypes, std::string& strNameRes );
		private :
					void 	GetStereotypes( const std::string& strName, std::vector<std::unique_ptr<OclMeta::Type>>& vecTypes );
					void 	GetDynamicTypes( const std::string& strName, std::vector<std::unique_ptr<OclMeta::Type>>& vecTypes, std::string& strNameResult );
	};

}; // namespace OclGmeCM

#endif // OCLTypeExGMECM_h