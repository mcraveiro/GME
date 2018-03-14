#include "stdafx.h"
#include <stdlib.h>
#define COLOR "0x710071"
#define ICON "focusstate.bmp"
#define EVTCOL "eventsel.bmp"
#define TRANSITCOL "0xFF0000"

#include "BONComponent.h"


HWND GMEHandle::hwnd;
CBuilder * GMEHandle::bldr;

void CComponent::InvokeEx(CBuilder &builder,CBuilderObject *focus, CBuilderObjectList &selected, long param) 
{
	builder.BeginTransaction();
	GMEHandle::bldr = &builder;
	
	
	CBuilderFolder *fldr = builder.GetRootFolder();
	
	// Searching for the main window based on its name was not a good idea
	// Especially, if the child windows were maximized (main window's title changes)
	// CString winname = "HFSM - " + fldr->GetName();
	// GMEHandle::hwnd = FindWindow(0, winname);

	GMEHandle::hwnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
	

	CSequence inp(builder);
	const CBuilderEventList *inpseq = inp.GetSequence();

	if(inpseq!=NULL && !inpseq->IsEmpty())
	{
		CState StateMachine(builder);
		CBuilderState *fsm = NULL;
		StateMachine.GetRootState(&fsm);
		
		POSITION pos = inpseq->GetHeadPosition();

		if(fsm != NULL)
		{
			AfxMessageBox("Ready to Start Simulation");
			CBuilderState *currstate = fsm->GetActiveSubstate();
			
			builder.CommitTransaction();
			builder.BeginTransaction();
			RedrawWindow(GMEHandle::hwnd, 0, 0, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
			
			
			
			while(pos)
			{
				CBuilderEvent *evt = inpseq->GetNext(pos);

				int delay=0;

				delay = evt->GetDelay() * 1000;
				if(delay>0)
					Sleep(delay);
				else
					AfxMessageBox("Next Event: - " + evt->GetName());

				
				evt->Highlight(EVTCOL, GMEHandle::bldr, GMEHandle::hwnd);

				CBuilderState *nextstate = currstate->TransitOnEvent(evt);
				
				if(nextstate != NULL)
					currstate = nextstate;
				else
					currstate = currstate;
	

				builder.CommitTransaction();
				builder.BeginTransaction();
				RedrawWindow(GMEHandle::hwnd, 0, 0, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
				
			}

			AfxMessageBox("Simulation Over. Final State:- " + currstate->GetName());
			
			CBuilderState *final = currstate;
			while(final != NULL)
			{
				CString col = "color";
				final->SetRegistryValue(col, currstate->GetOriginalColor());
				CString ico = "icon";
				final->SetRegistryValue(ico, currstate->GetOriginalIcon());

				final = (CBuilderState *)final->GetParent();					
			}
		}		
	}	
	
	builder.CommitTransaction();

}


//********* Implementation for CBuilderEvent
IMPLEMENT_CUSTOMATOM(CBuilderEvent, CBuilderAtom, "Events")

void CBuilderEvent::Initialize()
{
	CBuilderAtom::Initialize();
}

CBuilderEvent::~CBuilderEvent()
{

}

int CBuilderEvent::GetDelay()
{
	this->GetAttribute("Delay", delay);
	return delay;
}

CString CBuilderEvent::GetOriginalIcon()
{
	return orgicon;
}

void CBuilderEvent::SaveOriginalIcon()
{
	CString ico = "icon";
	CString val = "";
	this->GetRegistryValue(ico, val);
	orgicon = val;
}

void CBuilderEvent::Highlight(CString eval, CBuilder *bldr, HWND handle)
{
	CString ico = "icon";

	this->SaveOriginalIcon();
	this->SetRegistryValue(ico, eval);

	bldr->CommitTransaction();
	bldr->BeginTransaction();
	RedrawWindow(handle, 0, 0, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

	Sleep(300);

	eval = this->GetOriginalIcon();
	this->SetRegistryValue(ico, eval);

	bldr->CommitTransaction();
	bldr->BeginTransaction();
	RedrawWindow(handle, 0, 0, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}


//********* Implementation for CSequence
CSequence::CSequence(CBuilder &builder)
{
	root = builder.GetRootFolder();
	events = NULL;
}

CSequence::~CSequence()
{

}

// This function verifies if a sequence has been defined, & if so, returns a list of all the events defined
bool CSequence::GetEvents()
{
	const CBuilderModelList *rmodels = root->GetRootModels();
	POSITION pos = rmodels->GetHeadPosition();
	
	bool found = false;
	CBuilderModel *container;

	while(pos)
	{
		container = rmodels->GetNext(pos);

		if(container->GetKindName()=="InputSequence")
		{
			found = true;
			break;
		}
	}

	if(found)
	{
		const CBuilderAtomList *allevents = container->GetAtoms("Events");
		events = (CBuilderEventList *)allevents;
		return true;
	}
	else
	{
		AfxMessageBox("No Events/Sequence Defined - Cannot Continue");
		return false;
	}
}

// This function returns the Input Sequence in the ordered fashion
CBuilderEventList * CSequence::GetSequence()
{
	CBuilderEventList *sequence = new CBuilderEventList;
	
	if(GetEvents())
	{		
		if(!events->IsEmpty())
		{
			CBuilderEvent *start=NULL;

			int seqcount = SingleSequence(events, &start);
			
			if(seqcount != 1)
			{
				AfxMessageBox("Please Specify Exactly a Single Sequence");
			}
			// Specify a Single Sequence Ends
			else
			{
				if(start!=NULL)
				{
					sequence->AddTail(start);
					TraverseSequence(start, sequence);
				}
			}
		}
		else
			AfxMessageBox("No Valid Events");
	}
	return sequence;
}

// This function checks if only a single sequence has been given
int CSequence::SingleSequence(const CBuilderEventList *inp, CBuilderEvent **startevent)
{
	POSITION pos = inp->GetHeadPosition();
	
	// Specify a Single Sequence
	int count=0;
	
	while(pos)
	{
		CBuilderEvent *fevt = inp->GetNext(pos);
		
		CBuilderObjectList temp;
		fevt->GetInConnectedObjects("Sequence", temp);
		
		if(temp.GetCount()==0)
		{
			count++;
			*startevent = fevt;
		}
	}
	return count;
}

// This function traverses thru the sequence from the start till end!!
void CSequence::TraverseSequence(CBuilderEvent *node, CBuilderEventList *seq)
{
	CBuilderObjectList temp;
	CBuilderEvent *tempnode;

	tempnode = node;
	
	while(tempnode->GetOutConnectedObjects("Sequence", temp))
	{
		if (temp.IsEmpty())
			break;

		POSITION pos = temp.GetHeadPosition();
		
		CBuilderEvent *destnode = (CBuilderEvent *)temp.GetNext(pos);

		seq->AddTail(destnode);
		tempnode = destnode;
		temp.RemoveAll();
	}
}


//********* Implementation for CBuilderState
IMPLEMENT_CUSTOMMODEL(CBuilderState, CBuilderModel, "State")

void CBuilderState::Initialize()
{
	this->activesubstate = NULL;
	CBuilderModel::Initialize();
}

CBuilderState::~CBuilderState()
{

}

bool CBuilderState::GetChildStartState(CBuilderState **start)
{
	const CBuilderModelList *modlist = this->GetModels();
	if(modlist==NULL || modlist->IsEmpty())
	{
		return false;
	}
	else
	{
		POSITION pos = modlist->GetHeadPosition();

		while(pos)
		{
			CBuilderState *state = (CBuilderState *)modlist->GetNext(pos);
			if(state->GetIsStartState())
			{
				*start = state;
				return true;
			}
		}
		return false;
	}
}


// Returns the ActiveSubstate, if any.
CBuilderState * CBuilderState::GetActiveSubstate()
{
	CString col = "color";
	CString val = COLOR;
	this->SaveOriginalColor();
	this->SetRegistryValue(col, val);

	CString ico = "icon";
	val = ICON;
	this->SaveOriginalIcon();
	this->SetRegistryValue(ico, val);

	CBuilderState *start=NULL;
	if(this->GetChildStartState(&start))
	{
		// The next line recurssively evaluates the currentstate, & finally returns the lowermost active state
		this->activesubstate = start->GetActiveSubstate();//firststate->GetActiveSubstate();
	}
	
	if(activesubstate != NULL)
		return activesubstate;
	else
		return this;
}

// This returns the next state to which we should transit on the specified event
CBuilderState * CBuilderState::TransitOnEvent(CBuilderEvent *evt)
{

	CBuilderState *nstate = NULL;
	if(this->GetNextState(evt, &nstate))
	{
		// Found - Transition Possible
		CString col = "color";
		CString val = this->GetOriginalColor();
		this->SetRegistryValue(col, val);

		CString ico = "icon";
		val = this->GetOriginalIcon();
		this->SetRegistryValue(ico, val);

		CBuilderState *nextactivestate=NULL;
		
		nextactivestate = nstate->GetActiveSubstate();
		return nextactivestate;
	}
	else
	{
		// Some Outgoing Connections Present or Last State in the machine - time to go to the parent
		CBuilderState *par = (CBuilderState *)this->GetParent();
		if(par != NULL)
		{
			nstate = par->TransitOnEvent(evt);
			if(nstate != NULL)
			{
				CString col = "color";
				CString val = this->GetOriginalColor();
				this->SetRegistryValue(col, val);

				CString ico = "icon";
				val = this->GetOriginalIcon();
				this->SetRegistryValue(ico, val);

				return nstate;
			}
		}
		return NULL;
	}
}

// Gets a state connected by the specified event, if any
bool CBuilderState::GetNextState(CBuilderEvent *event, CBuilderState **nextstate)
{
	const CBuilderConnectionList *connlist = this->GetOutConnections("Transition");
	
	if(connlist!=NULL)
	{
		POSITION pos = connlist->GetHeadPosition();
		while(pos)
		{
			CBuilderTransition *conn = (CBuilderTransition *)connlist->GetNext(pos);

			if(conn->GetEvent() == event->GetName())
			{
				conn->Highlight(TRANSITCOL, GMEHandle::bldr, GMEHandle::hwnd);

				*nextstate = (CBuilderState *)conn->GetDestination();
				(*nextstate)->lastincomingtransition = conn;
				return true;
			}
		}
	}
	return false;
}

CString CBuilderState::GetOriginalColor()
{
	return orgcol;
}

void CBuilderState::SaveOriginalColor()
{
	CString col = "color";
	CString val = "";
	this->GetRegistryValue(col, val);
	orgcol = val;
}

CString CBuilderState::GetOriginalIcon()
{
	return orgicon;
}

void CBuilderState::SaveOriginalIcon()
{
	CString ico = "icon";
	CString val = "";
	this->GetRegistryValue(ico, val);
	orgicon = val;
}

bool CBuilderState::GetIsStartState()
{
	this->GetAttribute("StartState", isstart);
	return isstart;
}



//********* Implementation for CState
CState::CState(CBuilder &builder)
{
	root = builder.GetRootFolder();
}

CState::~CState()
{

}

bool CState::GetRootState(CBuilderState **rootstate)
{
	const CBuilderModelList *rmodels = root->GetRootModels();
	POSITION pos = rmodels->GetHeadPosition();
	
	bool found = false;
	CBuilderModel *stcontainer;
	
	while(pos)
	{
		stcontainer = rmodels->GetNext(pos);

		if(stcontainer->GetKindName()=="State")				// Search for State containing Input Sequence
		{
			*rootstate = (CBuilderState *)stcontainer;
			found = true;
			break;
		}
	}	
	
	if(!found)
	{
		AfxMessageBox("No State Machine Defined");
		return false;
	}
	return true;
}


//********* Implementation for CBuilderTransition

IMPLEMENT_CUSTOMCONNECTION(CBuilderTransition, CBuilderConnection, "Transition")

void CBuilderTransition::Initialize()
{
	CBuilderConnection::Initialize();
}

CBuilderTransition::~CBuilderTransition()
{

}

CString CBuilderTransition::GetEvent()
{
	this->GetAttribute("Event", event);
	return event;
}


CString CBuilderTransition::GetOriginalColor()
{
	return orgcol;
}

void CBuilderTransition::SaveOriginalColor()
{
	CString col = "color";
	CString val = "";
	this->GetRegistryValue(col, val);
	orgcol = val;
}

void CBuilderTransition::Highlight(CString tval, CBuilder *bldr, HWND handle)
{
	CString col = "color";

	this->SaveOriginalColor();
	this->SetRegistryValue(col, tval);

	bldr->CommitTransaction();
	bldr->BeginTransaction();
	RedrawWindow(handle, 0, 0, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

	Sleep(250);

	tval = this->GetOriginalColor();
	this->SetRegistryValue(col, tval);

	bldr->CommitTransaction();
	bldr->BeginTransaction();
	RedrawWindow(handle, 0, 0, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}