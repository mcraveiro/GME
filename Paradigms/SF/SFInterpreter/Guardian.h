#define MAXPARAMETERS 50
#define MAXSCRIPTS    50

class ScriptDescriptor
{
  protected:
	  CString  scriptName;
	  CString  modelName;
	  CString* paramNames;
	  CString* paramTypes;
	  int*     paramSizes;
	  int*     paramDatas;

	  int      numParameters;

  public:
	  ScriptDescriptor(CString& name, CString* params, CString* types, int numParams);
 	  ScriptDescriptor(CString model, CString& name, int numParams);

	  virtual ~ScriptDescriptor (void) {}
	  
	  CString  GetScriptName    (void);
	  CString  GetModelName     (void);
	  int      GetNumParameters (void);
	  CString* GetParameters    (void);
	  CString* GetDataTypes     (void);
	  int*     GetSizes         (void);
	  int*     GetDatas         (void);
	  void     AddParamLine     (CString paramName, CString paramType, int paramSize, int paramData);
};

class ScriptCollection
{
  protected:
	  ScriptDescriptor* scriptList[MAXSCRIPTS];
	  int               numScripts;

	  int EnforceList  (ScriptDescriptor* adrsDescriptor);
  public:
	  ScriptCollection  (void);
	  virtual ~ScriptCollection (void) {}

	  int      AddDescriptor (ScriptDescriptor* adrsDescriptor);
      CString  GetModelName  (int modelIndex);    

};