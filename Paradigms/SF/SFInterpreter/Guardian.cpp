#include <stdafx.h>
#include "Guardian.h"

ScriptDescriptor::ScriptDescriptor(CString& name, CString* params, CString* types, int numParams)
{
  scriptName    = name;
  paramNames    = params;
  paramTypes    = types;
  numParameters = numParams;
}

ScriptDescriptor::ScriptDescriptor(CString model, CString& name, int numParams)
{
  modelName  = model;
  scriptName = name;
  paramNames = new CString[numParams];
  paramTypes = new CString[numParams];
  paramSizes = new int    [numParams];
  paramDatas = new int    [numParams];
  numParameters = -1;
}

void ScriptDescriptor::AddParamLine(CString paramName, CString paramType, int paramSize, int paramData)
{
  ++numParameters;
  paramNames[numParameters] = paramName;
  paramTypes[numParameters] = paramType;
  paramSizes[numParameters] = paramSize;
  paramDatas[numParameters] = paramData;
}

CString ScriptDescriptor::GetModelName (void)
{ return modelName; }

CString ScriptDescriptor::GetScriptName (void)
{ return scriptName; }

int ScriptDescriptor::GetNumParameters (void)
{ return numParameters; }


CString* ScriptDescriptor::GetParameters (void)
{ return paramNames; }

CString* ScriptDescriptor::GetDataTypes (void)
{ return paramTypes; }

int* ScriptDescriptor::GetSizes (void)
{ return paramSizes; }

int* ScriptDescriptor::GetDatas (void)
{ return paramDatas;}


ScriptCollection::ScriptCollection (void)
{ 
  numScripts = 0; 
}

int ScriptCollection::EnforceList (ScriptDescriptor* adrsDescriptor)
{
	if (numScripts == 0) { return 1; }
    

	for (int i = 0; i < numScripts; ++i)
	{
	 if (scriptList[i]->GetScriptName() == adrsDescriptor->GetScriptName()) 
	 {
    	if ( scriptList[i]->GetNumParameters() == adrsDescriptor->GetNumParameters()) 
		{
		  bool     syntaxCorrect;

		  CString* goodNames = scriptList[i]->GetParameters();
		  CString* goodTypes = scriptList[i]->GetDataTypes();

		  CString* checkNames = adrsDescriptor->GetParameters();
		  CString* checkTypes = adrsDescriptor->GetDataTypes();

		  int*     goodSizes = scriptList[i]->GetSizes();
		  int*     goodDatas = scriptList[i]->GetDatas();

		  int*     checkSizes = adrsDescriptor->GetSizes();
		  int*     checkDatas = adrsDescriptor->GetDatas();

          for (int j =0; j <= scriptList[i]->GetNumParameters(); ++j)
		  {
		    syntaxCorrect = false;
   		    for (int k = 0; k <= adrsDescriptor->GetNumParameters(); ++k)
			{
			  if ((goodNames[j] == checkNames[k]) && ( goodTypes[j] == checkTypes[k]) && (goodSizes[j] == checkSizes[k]) && (goodDatas[j] == checkDatas[k])) 
			  {   syntaxCorrect = true;}
	 
			}
		  
			if (syntaxCorrect == false) {   return -1 - j; }
			
		  }
		       return 0;
		}
	    else {   return -1; }

	 }
	}
	return 1;
}

int ScriptCollection::AddDescriptor (ScriptDescriptor* adrsDescriptor)
{

  int ret=0;
  ret = EnforceList(adrsDescriptor);
  if (ret == 1)
  { scriptList[numScripts] = adrsDescriptor; 
    ++numScripts;
	return 0;
  }
  else if (ret == 0)
  {
	return 0;
  }
  else { 
    return (-(ret+1))+1; 
  }
}

CString ScriptCollection::GetModelName (int modelIndex)
{ return scriptList[modelIndex]->GetModelName(); }
