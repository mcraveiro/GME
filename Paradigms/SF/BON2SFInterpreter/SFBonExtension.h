#ifndef SFBONEXTENSION_H
#define SFBONEXTENSION_H

#include "BON.h"
#include "BONImpl.h"
#include "Extensions.h"

///BUP
// add your class definitions here
///EUP

namespace SF_BON
{
class ProcessingImpl;
DECLARE_ABSTRACT_BONEXTENSION( BON::Model, ProcessingImpl, Processing );
class CompoundImpl;
DECLARE_BONEXTENSION( Processing, CompoundImpl, Compound );
class PrimitiveImpl;
DECLARE_BONEXTENSION( Processing, PrimitiveImpl, Primitive );
class ParameterBaseImpl;
DECLARE_BONEXTENSION( BON::Atom, ParameterBaseImpl, ParameterBase );
class InputParamImpl;
DECLARE_BONEXTENSION( ParameterBase, InputParamImpl, InputParam );
class ParamImpl;
DECLARE_BONEXTENSION( ParameterBase, ParamImpl, Param );
class SignalImpl;
DECLARE_BONEXTENSION( BON::Atom, SignalImpl, Signal );

typedef std::map<std::string, Primitive> PrimitiveTable;

//*******************************************************************
//   C  L  A  S  S   ProcessingImpl
//*******************************************************************
class ProcessingImpl :
  virtual public BON::ModelImpl 
{
public:
	//
	// kind and role getters
	virtual std::set<InputParam>            getInputParameters();
	virtual std::set<Signal>                getInputSignals();
	virtual std::set<ParameterBase>         getOutputParameters();
	virtual std::set<Signal>                getOutputSignals();
	virtual std::set<BON::Connection>       getPC();
	virtual std::set<Param>                 getParameters();
	virtual std::set<ParameterBase>         geteters();
	virtual std::set<Signal>                gets();

	///BUP
	public :
		void initialize() {}
		virtual void traverse(std::set<Primitive> &primitives) {}
	///EUP
};


//*******************************************************************
//   C  L  A  S  S   CompoundImpl
//*******************************************************************
class CompoundImpl :
  public ProcessingImpl 
{
public:
	//
	// kind and role getters
	virtual std::set<Compound>              getCompoundParts();
	virtual std::set<BON::Connection>       getDFC();
	virtual std::set<Processing>            getParts();
	virtual std::set<Primitive>             getPrimitiveParts();

	///BUP
	public :
		void initialize();
	virtual void traverse(std::set<Primitive> &primitives);
	///EUP
};


//*******************************************************************
//   C  L  A  S  S   PrimitiveImpl
//*******************************************************************
class PrimitiveImpl :
  public ProcessingImpl 
{
public:
	typedef enum
	{
		IFALL_Firing_Type,
		IFANY_Firing_Type
	} Firing_Type;
	//
	// attribute getters
	virtual PrimitiveImpl::Firing_Type      getFiring();
	virtual long        getPriority();
	virtual std::string getScript();

	///BUP

	private :
		int id;
		int outputPortNo;
		int inputPortNo;
		std::string actorName;

		std::set<Param> parameters;
		std::set<InputParam> inputParameters;
		static PrimitiveTable contextTable;
	public :
		void initialize();
		void setActorName();
		std::string getActorName() { return actorName; }
		void setPorts(const std::set<BON::FCO> &ports);
		void writeActorCreate(FILE *fpt);
		void writeActorConnect(FILE *fpt);
		void writeContextDef(FILE *fpt);
		void writeContextCreate(FILE *fpt,bool ext = true);
		void writeSetContext(FILE *fpt);

		static void writeContextDefs(FILE *fpt);
		static void writeScriptRegs(FILE *fpt);
		static void writeScriptProto(FILE *fpt);
	
		virtual void traverse(std::set<Primitive> &primitives);

	///EUP
};


//*******************************************************************
//   C  L  A  S  S   ParameterBaseImpl
//*******************************************************************
class ParameterBaseImpl :
  virtual public BON::AtomImpl 
{
public:
	typedef enum
	{
		Integer_DataType_Type,
		Double_DataType_Type,
		Character_DataType_Type,
		Pointer_DataType_Type
	} DataType_Type;
	//
	// attribute getters
	virtual ParameterBaseImpl::DataType_Type          getDataType();
	virtual long        getSize();

	///BUP
	public :
		int size;
		virtual void initialize();
	///EUP
};


//*******************************************************************
//   C  L  A  S  S   InputParamImpl
//*******************************************************************
class InputParamImpl :
  public ParameterBaseImpl 
{
public:

	///BUP
	friend class PrimitiveImpl;

	protected:
		Param source;
	public :
		virtual void initialize();
	///EUP
};


//*******************************************************************
//   C  L  A  S  S   ParamImpl
//*******************************************************************
class ParamImpl :
  public ParameterBaseImpl 
{
public:
	//
	// attribute getters
	virtual std::string getInitValue();
	virtual bool        isGlobal();

	///BUP
	friend class PrimitiveImpl;
	public :
		std::string longName;
		bool listed;
		static std::set<Param> instances;

	public :
		void addToList();
		void writeContextDef(FILE *fpt,const std::string &fieldName);
		void writeGlobal(FILE *fpt,bool ext = true);
		void writeContextInit(FILE *fpt,std::string &actorName,const std::string &fieldName);

		virtual void initialize();

		static void writeGlobals(FILE *fpt,bool ext = true);
	///EUP
};


//*******************************************************************
//   C  L  A  S  S   SignalImpl
//*******************************************************************
class SignalImpl :
  virtual public BON::AtomImpl 
{
public:

	///BUP
	friend class PrimitiveImpl;
	public :
		virtual void initialize() { port = 0; }
	protected:
		std::set<Signal> connections;
		int yloc;
		int port;
	public:
		void addConnection(Signal signal)		{ connections.insert(signal); }
		void incrementPort()					{ port++; }
		void setLocation();
	///EUP
};



}; // namespace SF_BON

///BUP
// add your additional class definitions here
///EUP

#endif // SFBONEXTENSION_H
