#ifndef GME_INTERPRETER_H
#define GME_INTERPRETER_H

#include "Builder.h"


#define NEW_BON_INVOKE
//#define DEPRECATED_BON_INVOKE_IMPLEMENTED

class GMEHandle
{
public:
	static HWND hwnd;
	static CBuilder *bldr;
};

class CComponent 
{
public:
	CComponent() : focusfolder(NULL) { ; } 
	CBuilderFolder *focusfolder;
	CBuilderFolderList selectedfolders;
	void InvokeEx(CBuilder &builder,CBuilderObject *focus, CBuilderObjectList &selected, long param);
//	void Invoke(CBuilder &builder, CBuilderObjectList &selected, long param);
};


// This is the Definition for the CBuilderEvent - extended from CBuilderAtom
class CBuilderEvent : public CBuilderAtom
{
	DECLARE_CUSTOMATOM (CBuilderEvent, CBuilderAtom)
public:
	virtual void Initialize();
	virtual ~CBuilderEvent();

	int GetDelay();

	void Highlight(CString eval, CBuilder *bldr, HWND handle);

private:
	int delay;
	CString GetOriginalIcon();
	void SaveOriginalIcon();
	CString orgicon;
};

typedef CTypedPtrList<CPtrList, CBuilderEvent*>									CBuilderEventList;


// Class Definition for the CSequence Class
class CSequence  
{
public:
	CSequence(CBuilder &builder);
	virtual ~CSequence();

	CBuilderEventList *GetSequence();


private:
	CBuilderFolder *root;
	const CBuilderEventList *events;

	bool GetEvents();
	int SingleSequence(const CBuilderEventList *inp, CBuilderEvent **startevent);

	void TraverseSequence(CBuilderEvent *node, CBuilderEventList *seq);

};




// This is the Definition for the CBuilderTransition - extended from CBuilderConnection
class CBuilderTransition : public CBuilderConnection
{
	DECLARE_CUSTOMCONNECTION (CBuilderTransition, CBuilderConnection)
public:
	virtual void Initialize();
	virtual ~CBuilderTransition();

	CString GetEvent();
	void Highlight(CString tval, CBuilder *bldr, HWND handle);

private:
	CString GetOriginalColor();
	void SaveOriginalColor();
	CString event;
	CString orgcol;

};


// This is the Definition for the CBuilderState - extended from CBuilderModel
class CBuilderState : public CBuilderModel
{
	DECLARE_CUSTOMMODEL (CBuilderState, CBuilderModel)
public:
	virtual void Initialize();
	virtual ~CBuilderState();

	CBuilderState * GetActiveSubstate();
	CBuilderState * TransitOnEvent(CBuilderEvent *evt);
	
	CString GetOriginalColor();
	void SaveOriginalColor();

	CString GetOriginalIcon();
	void SaveOriginalIcon();
	bool GetIsStartState();
	CBuilderTransition *lastincomingtransition;

private:
	CBuilderState *activesubstate;
	bool GetChildStartState(CBuilderState **start);
	bool GetChildFirstState(CBuilderAtom *start, CBuilderState **firststate);
	bool GetNextState(CBuilderEvent *event, CBuilderState **nextstate);


	CString orgcol;
	CString orgicon;
	bool isstart;
};


// Class Definition for CState
class CState  
{
public:
	CState(CBuilder &builder);
	virtual ~CState();
	
	bool GetRootState(CBuilderState **rootstate);

private:
	CBuilderFolder *root;
};


#endif // whole file
