//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLGMECMFacade.h
//
//###############################################################################################################################################

#ifndef OCLGMECMFacade_h
#define OCLGMECMFacade_h

#include "GMEConstraintEx.h"
#include "OCLCommonEx.h"

namespace OclGmeCM
{
	struct FuncItem
	{
		std::string	text;
		int		serial;
	};

	typedef std::map<std::string, FuncItem> FuncDesc;

	struct EvaluationRecord
	{
		OclMeta::Object				spObject;
		OclTree::ViolationVector	vecViolations;
		OclGme::SpConstraint		spConstraint;
		FuncDesc					calledFunctions;
	};

	struct EvaluationInfo
	{
		bool	bEnabledSCLogical;
		bool	bEnabledSCIterator;
		bool	bEnabledTracking;
		int		iViolationCount;
		int		iModelDepth;
	};

	typedef std::vector< EvaluationRecord > EvaluationRecordVector;

	class ConstraintDefinitionFactory;

//##############################################################################################################################################
//
//	C L A S S : OclGmeCM::Facade
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Facade
	{
		private :
			CComPtr<IMgaProject>				m_spProject;
			OclTree::TreeManager*				m_pTreeManager;

			OclGme::ConstraintFunctionVector	m_vecMetaConstraintFunctions;
			OclGme::ConstraintFunctionVector	m_vecUserConstraintFunctions;

			bool									m_bViolationDlgExpanded;
			EvaluationInfo							m_infoEvaluation;

		public :
			OclGme::ConstraintVector				m_vecMetaConstraints;
			OclGme::ConstraintVector				m_vecUserConstraints;

		public :
			bool									m_bEnabled;
			bool									m_bEnabledEvents;
			bool									m_bEnabledInteractions;

		public :
														Facade();
														~Facade();
					void								Initialize( CComPtr<IMgaProject> spProject );
					void 								Finalize();

					CComPtr<IMgaMetaProject> 	GetMetaProject() const;
					CComPtr<IMgaProject>			GetProject() const;
					OclTree::TreeManager*			GetTreeManager() const;

					EvaluationInfo						GetEvaluationInfo() const;
					void								SetEvaluationInfo( const EvaluationInfo& info );

					void								LoadMetaConstraintFunctions( OclGme::ConstraintFunctionVector& vecFaileds );
					void								LoadUserConstraintFunctions( OclGme::ConstraintFunctionVector& vecFaileds );
					void								LoadMetaConstraints( OclGme::ConstraintVector& vecFaileds );
					void								LoadUserConstraints( OclGme::ConstraintVector& vecFaileds );

					HRESULT 							EvaluateAll(IUnknown **punk);
					HRESULT 							Evaluate( CComPtr<IMgaObject> spObject, IUnknown **punk);
					HRESULT								Evaluate( const OclCommonEx::ObjectVector& vecObjects, IUnknown **punk);
					HRESULT 							Evaluate( CComPtr<IMgaObject> spObject, unsigned long ulCurrentEventMask);
					HRESULT 							EvaluateConstraints( EvaluationRecordVector& vecInputs, bool bShowProgress, IUnknown **punk );
		private :
					void								updateLineno(EvaluationRecord &rec);
					bool								getConstraintFunctionText(std::string &name, std::string &text);
					void								addFunctionTexts(EvaluationRecord &rec);
					void 								LoadConstraintFunctions( OclGme::ConstraintFunctionVector& vecFounds, OclGme::ConstraintFunctionVector& vecFaileds, OclGme::ConstraintFunctionVector& vecSucceededs );
					std::string							GetSignature( OclGme::SpConstraintFunction spCF );
					void								LoadConstraints( OclGme::ConstraintVector& vecFounds, OclGme::ConstraintVector& vecFaileds, OclGme::ConstraintVector& vecSucceededs );

					void 								GetObjectConstraints( CComPtr<IMgaObject> spObject, EvaluationRecordVector& vecInputs, unsigned long ulCurrentEventMask );
					void 								SortByPriority( EvaluationRecordVector& vecInputs ) const;
					void 								CollectConstraints( CComPtr<IMgaObject> spObject, bool bRecursive, EvaluationRecordVector& vecInputs );
					void 								CollectConstraints( CComPtr<IMgaObject> spObject, EvaluationRecordVector& vecInputs );
					void 								CollectConstraints( CComPtr<IMgaObject> spObject, unsigned long ulCurrentEventMask, int iCheckedLevels, EvaluationRecordVector& vecInputs );

		friend ConstraintDefinitionFactory;
	};

}; // namespace OclGmeCM

#endif // OCLGMECMFacade_h