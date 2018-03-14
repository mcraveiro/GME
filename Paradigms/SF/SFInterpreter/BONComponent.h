#ifndef GME_INTERPRETER_H
#define GME_INTERPRETER_H

// #include <stdafx.h>
#include "Builder.h"
#include "Guardian.h"

#define NEW_BON_INVOKE

class CPrimitiveBuilder;
typedef CTypedPtrList<CPtrList, CPrimitiveBuilder*> CPrimitiveList;
class CSignalBuilder;
typedef CTypedPtrList<CPtrList, CSignalBuilder*> CSignalList;
class CParameterBuilder;
typedef CTypedPtrList<CPtrList, CParameterBuilder*>	CParameterList;
class CInputParameterBuilder;
typedef CTypedPtrList<CPtrList, CInputParameterBuilder*>	CInputParameterList;
typedef CTypedPtrMap<CMapStringToPtr, CString, CPrimitiveBuilder*> CPrimitiveTable;

class CComponent {
public:
	CComponent() : focusfolder(NULL) { ; } 
	CBuilderFolder *focusfolder;
	CBuilderFolderList selectedfolders;
	void InvokeEx(CBuilder &builder,CBuilderObject *focus, CBuilderObjectList &selected, long param);
//	void Invoke(CBuilder &builder, CBuilderObjectList &selected, long param);
protected:
	void CheckScriptTypes (CPrimitiveList* primitives);
};

//////////////////////////// Signal Flow ///////////////////////////

class CProcessingBuilder : public CBuilderModel
{
	DECLARE_CUSTOMMODEL(CProcessingBuilder, CBuilderModel)
public:
	virtual void Traverse(CPrimitiveList &primitives) { ASSERT(false); }
};

class CCompoundBuilder : public CProcessingBuilder
{
	DECLARE_CUSTOMMODEL(CCompoundBuilder, CProcessingBuilder)
public:
	void Traverse(CPrimitiveList &primitives);
	virtual void Initialize();
};

class CPrimitiveBuilder : public CProcessingBuilder
{
	DECLARE_CUSTOMMODEL(CPrimitiveBuilder, CProcessingBuilder)
public:
	virtual void Initialize();
	~CPrimitiveBuilder();
protected:
	CString actorName;
	int priority;
	CString script;
	int firing;
	int id;
	int outputPortNo;
	int inputPortNo;
	CParameterList parameters;
	CInputParameterList inputParameters;
	static CPrimitiveTable contextTable;
public:
	void SetPriority();
	void SetScript();
	void SetFiring();
	void SetActorName();
	CString &GetActorName()						{ return actorName; }
	CString &GetScriptName()                    { return script;    }
	void SetPorts(const CBuilderAtomList *ports);
	void Traverse(CPrimitiveList &primitives);

	static void WriteScriptRegs(FILE *fpt);
	static void WriteContextDefs(FILE *fpt);
	static void WriteScriptProto(FILE *fpt);
	void WriteContextDef(FILE *fpt);
	void WriteContextCreate(FILE *fpt,bool ext = true);
	void WriteSetContext(FILE *fpt);
	void WriteActorCreate(FILE *fpt);
	void WriteActorConnect(FILE *fpt);
};

class CSignalBuilder : public CBuilderAtom
{
	DECLARE_CUSTOMATOM(CSignalBuilder, CBuilderAtom)
	friend class CPrimitiveBuilder;
public:
	virtual void Initialize() { port = 0; }
protected:
	CSignalList connections;
	int yloc;
	int port;
public:
	void AddConnection(CSignalBuilder *signal)	{ connections.AddTail(signal); }
	void IncrementPort()						{ port++; }
	void SetLocation();
};

class CParameterBuilder : public CBuilderAtom
{
	DECLARE_CUSTOMATOM(CParameterBuilder, CBuilderAtom)
	friend class CPrimitiveBuilder;
public:
	virtual void Initialize();
	~CParameterBuilder();
protected:
	CString longName;
	bool global;
	int datatype;
	int size;
	CString initval;
//	int initval
	bool listed;
	static CParameterList instances;
public:
	int  GetDataType (void) { return datatype;}
	int  GetSize     (void) { return size;    }
	void AddToList();
	void WriteContextDef(FILE *fpt,const CString &fieldName);
	static void WriteGlobals(FILE *fpt,bool ext = true);
	void WriteGlobal(FILE *fpt,bool ext = true);
	void WriteContextInit(FILE *fpt,CString &actorName,const CString &fieldName);
};

class CInputParameterBuilder : public CBuilderAtom
{
	DECLARE_CUSTOMATOM(CInputParameterBuilder, CBuilderAtom)
	friend class CPrimitiveBuilder;
public:
	virtual void Initialize();
    int GetDataType (void) { return datatype; }
	int GetSize     (void) { return size;     }
protected:
	int datatype;
	int size;
	CParameterBuilder *source;
};

#endif // whole file