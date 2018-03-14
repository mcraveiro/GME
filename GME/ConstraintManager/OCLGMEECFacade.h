//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLGMEECFacade.h
//
//###############################################################################################################################################

#ifndef OCLGMEECFacade_h
#define OCLGMEECFacade_h

#include "GMEConstraintEx.h"

namespace OclGmeEC
{
	typedef CComObjPtr<IMgaFCO> SPFCO;
	typedef std::vector<SPFCO> FCOVector;

//##############################################################################################################################################
//
//	C L A S S : OclGmeEC::Facade
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Facade
	{
		private :
			CComPtr<IMgaProject>		m_spProject;
			OclTree::TreeManager*		m_pTreeManager;

		public :
			bool									m_bEnabled;
			bool									m_bEnabledEvents;
			bool									m_bEnabledInteractions;

		public :
												Facade();
												~Facade();
					void						Initialize( CComPtr<IMgaProject> spProject );
					void 						Finalize();

					CComPtr<IMgaProject>	GetProject() const;
					OclTree::TreeManager*	GetTreeManager() const;

					void 	CheckConstraint( CComPtr<IMgaFCO> spConstraint, OclGme::ConstraintVector& vecFaileds, bool bFieldsMandatory );
					void 	CheckAllConstraints( OclGme::ConstraintVector& vecFaileds );
					void	CheckConstraintFunction( CComPtr<IMgaFCO> spConstraintFunction, OclGme::ConstraintFunctionVector& vecFaileds, bool bFieldsMandatory );
					void 	CheckAllConstraintFunctions( OclGme::ConstraintFunctionVector& vecFaileds );

		private :
					bool 	IsConstraintDefined( CComPtr<IMgaFCO> spConstraint );
					bool 	IsConstraintFunctionDefined( CComPtr<IMgaFCO> spConstraintFunction );
	};

}; // namespace OclGmeEC

#endif // OCLGMEECFacade_h