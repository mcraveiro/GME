//###############################################################################################################################################
//
//	Meta and Builder Object Network V2.0 for GME
//	BON2Component.h
//
//###############################################################################################################################################

/*
	Copyright (c) Vanderbilt University, 2000-2004
	ALL RIGHTS RESERVED

	Vanderbilt University disclaims all warranties with regard to this
	software, including all implied warranties of merchantability
	and fitness.  In no event shall Vanderbilt University be liable for
	any special, indirect or consequential damages or any damages
	whatsoever resulting from loss of use, data or profits, whether
	in an action of contract, negligence or other tortious action,
	arising out of or in connection with the use or performance of
	this software.
*/

#ifndef BON2Component_h
#define BON2Component_h

#include "BON.h"
#include "Extensions.h"
#include "BONImpl.h"
#include "PrintDlg.h"
#include <ComponentConfig.h>

namespace BON
{
	// This Component is intended only for demonstration purposes. The calls are pretty dumb. !

	// The implementor always has to predeclarete the implementation classes. It is good practice that immediately after the BON::Extensions are declared.

	class ProcessingImplBase;

	class CompoundImpl;
	DECLARE_BONEXTENSION( Model, CompoundImpl, Compound );

	class PrimitiveImpl;
	DECLARE_BONEXTENSION( Model, PrimitiveImpl, Primitive );

	class ParameterImpl;
	DECLARE_BONEXTENSION( Atom, ParameterImpl, Parameter );

	class IOParameterImpl;
	DECLARE_BONEXTENSION( Atom, IOParameterImpl, IOParameter );

	class SignalImpl;
	DECLARE_BONEXTENSION( Atom, SignalImpl, Signal );

//###############################################################################################################################################
//
// 	C L A S S : BON::Component
//
//###############################################################################################################################################

	class Component
	{
		//==============================================================
		// IMPLEMENTOR SPECIFIC PART
		// Insert application specific members and method deifinitions here

		//==============================================================
		// BON2 SPECIFIC PART
		// Do not modify anything below

		// Member variables
		public :
			Project 	m_project;
			bool		m_bIsInteractive;

		public:
			Component();
			~Component();

		public:
			void initialize( Project& project );
			void finalize( Project& project );
			void invoke( Project& project, const std::set<FCO>& setModels, long lParam );
			void invokeEx( Project& project, FCO& currentFCO, const std::set<FCO>& setSelectedFCOs, long lParam );
			void objectInvokeEx( Project& project, Object& currentObject, const std::set<Object>& setSelectedObjects, long lParam );
			Util::Variant getParameter( const std::string& strName );
			void setParameter( const std::string& strName, const Util::Variant& varValue );

		#ifdef GME_ADDON
			void globalEventPerformed( globalevent_enum event );
			void objectEventPerformed( Object& object, unsigned long event, VARIANT v );
		#endif
	};

//###############################################################################################################################################
//
// 	C L A S S : BON::ProcessingBaseImpl
//
//###############################################################################################################################################

// This class demonstrate that the implementor is not forced to create BON::EX classes for implementations. They can be used in the inherited classes.

class ProcessingBaseImpl
	: public ModelImpl
{
	private :
		bool 						m_bParamsAccessed;
		std::vector<Parameter>	m_vecParams;

	public :
		void initialize();
		~ProcessingBaseImpl();
		std::vector<Parameter>& childParameters();
};

//###############################################################################################################################################
//
// 	C L A S S : BON::CompoundImpl
//
//###############################################################################################################################################

class CompoundImpl
	: public ProcessingBaseImpl
{
	public :
		int getHighPrimitivesCount();
};

//###############################################################################################################################################
//
// 	C L A S S : BON::PrimitiveImpl
//
//###############################################################################################################################################

class PrimitiveImpl
	: public ProcessingBaseImpl
{
	private :
		long m_iPriority;

	public :
		void initialize();
		long priority() const;
};

//###############################################################################################################################################
//
// 	C L A S S : BON::ParameterImpl
//
//###############################################################################################################################################

enum DataType { DT_Integer, DT_Real, DT_Character, DT_Pointer };

class ParameterImpl
	: public AtomImpl
{
	public :
		long 		m_iSize;
		DataType 	m_eType;

	public :
		void initialize();
		std::set<IOParameter> inputParameters();
		std::set<IOParameter> outputParameters();
		virtual std::string className() const;
};

//###############################################################################################################################################
//
// 	C L A S S : BON::IOParameterImpl
//
//###############################################################################################################################################

class IOParameterImpl
	: public ParameterImpl
{
	public :
		std::string className() const;
};

//###############################################################################################################################################
//
// 	C L A S S : BON::SignalImpl
//
//###############################################################################################################################################

class SignalImpl
	: public AtomImpl
{
	public :
		bool isMyParentPrimitive();
};

//###############################################################################################################################################
//
// 	C L A S S : BON::MyVisitor
//
//###############################################################################################################################################

// This is an example for Visitor Extension and usage

class MyVisitor
	: public Visitor
{
	private :
		CPrintDlg* m_pDlg;

	public :
		MyVisitor( CPrintDlg* pDlg );

		void visitObjectImpl( const Object& object );
		void visitFCOImpl( const FCO& object );
		void visitAtomImpl( const Atom& atom );
		void visitModelImpl( const Model& model );

	private :
		void PrintLine( const std::string& str );
		bool visitPrimitive( const Primitive& object );
		bool visitCompound( const Compound& object );
		bool visitParameter( const Parameter& object );
		bool visitIOParameter( const IOParameter& object );
		bool visitSignal( const Signal& object );
};

}; // namespace BON

#endif // Bon2Component_H