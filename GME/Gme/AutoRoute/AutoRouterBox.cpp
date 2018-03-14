// AutoRouterBox.cpp : Implementation of CAutoRouterBox

#include "stdafx.h"
#include "AutoRouterBox.h"
#include "AutoRouterPort.h"
#include "AutoRouterGraph.h"


CAutoRouterBox::CAutoRouterBox():
	owner(NULL),
	rect(0, 0, 0, 0),
	atomic(false)
{
	CalculateSelfPoints();
}

void CAutoRouterBox::CalculateSelfPoints()
{
	selfpoints[0].x = rect.left;
	selfpoints[0].y = rect.top;

	selfpoints[1].x = rect.right - 1;
	selfpoints[1].y = rect.top;

	selfpoints[2].x = rect.right - 1;
	selfpoints[2].y = rect.bottom - 1;

	selfpoints[3].x = rect.left;
	selfpoints[3].y = rect.bottom - 1;
}

void CAutoRouterBox::DeleteAllPorts()
{
	for (CAutoRouterPortList::size_type i = 0; i < ports.size(); i++)
	{
		ports[i]->SetOwner(NULL);
		delete ports[i];
	}

	ports.clear();

	atomic = false;
}

CAutoRouterGraph* CAutoRouterBox::GetOwner(void) const
{
	return owner;
}

bool CAutoRouterBox::HasOwner(void) const
{
	return owner != NULL;
}

void CAutoRouterBox::SetOwner(CAutoRouterGraph* graph)
{
	owner = graph;
}

CAutoRouterPort* CAutoRouterBox::CreatePort(void) const
{
	CAutoRouterPort* port = new CAutoRouterPort();
	ASSERT( port != NULL );

	return port;
}

bool CAutoRouterBox::HasNoPort(void) const
{
	return ports.size() == 0;
}

long CAutoRouterBox::GetPortCount(void) const
{
	return ports.size();
}

bool CAutoRouterBox::IsAtomic(void) const
{
	return atomic;
}

void CAutoRouterBox::AddPort(CAutoRouterPort* port)
{
	ASSERT(port != NULL);
	if (port == NULL)
		return;

	port->SetOwner(this);

	ports.push_back(port);

#ifdef _DEBUG
	AssertValidPort(port);
#endif
}

void CAutoRouterBox::DeletePort(CAutoRouterPort* port)
{
	ASSERT(port != NULL);
	if (port == NULL)
		return;

#ifdef _DEBUG
	CAutoRouterBox* ownerBox = port->GetOwner();
	ASSERT( ownerBox == this );

	AssertValidPort(port);

	if( atomic )
		ASSERT( ports.size() == 1 );
#endif

	std::vector<CAutoRouterPort*>::iterator iter = std::find(ports.begin(), ports.end(), port);

	if (iter == ports.end())
	{
		//error
		return;
	}

	(*iter)->SetOwner(NULL);

	delete *iter;

	ports.erase(iter);

	atomic = false;
}

const CAutoRouterPortList& CAutoRouterBox::GetPortList(void) const
{
	return ports;
}

CRect CAutoRouterBox::GetRect(void) const
{
	return rect;
}

bool CAutoRouterBox::IsRectEmpty(void) const
{
	return rect.IsRectEmpty() == TRUE;
}

void CAutoRouterBox::SetRect(const CRect& r)
{
	ASSERT( r.Width() >= 3 && r.Height() >= 3 );
	ASSERT( r.TopLeft().x >= ED_MINCOORD && r.TopLeft().y >= ED_MINCOORD );
	ASSERT( r.BottomRight().x <= ED_MAXCOORD && r.BottomRight().y <= ED_MAXCOORD );
	ASSERT( ports.size() == 0 || atomic );

	rect = r;
	CalculateSelfPoints();

	if( atomic )
	{
		ASSERT( ports.size() == 1 );
		ports[0]->SetRect(r);
	}
}

void CAutoRouterBox::SetRectByPoint(const CPoint& point)
{
	ShiftBy(point);
}

void CAutoRouterBox::ShiftBy(const CPoint& offset)
{
	rect += offset;

	for (CAutoRouterPortList::size_type i = 0; i < ports.size(); i++)
	{
		ports[i]->ShiftBy(offset);
	}

	CalculateSelfPoints();
}

CPoint* CAutoRouterBox::GetSelfPoints(void) const
{
	return (CPoint*)selfpoints;
}

bool CAutoRouterBox::IsBoxAt(const CPoint& point, long nearness) const
{
	return IsPointIn(point, rect, nearness);
}

bool CAutoRouterBox::IsBoxClip(const CRect& r) const
{
	return IsRectClip(rect, r);
}

bool CAutoRouterBox::IsBoxIn(const CRect& r) const
{
	return IsRectIn(rect, r);
}

void CAutoRouterBox::Destroy(void)
{
	//ideally it could be placed in the finaldestruct(), 
	//but the deleted ports hold references to this box, 
	//so the finaldestruct() is not called until the ports are deleted
	
	this->SetOwner(NULL);
	DeleteAllPorts();
}

// --- Debug

#ifdef _DEBUG

void CAutoRouterBox::AssertValid() const
{
	for (CAutoRouterPortList::size_type i = 0; i < ports.size(); i++)
	{
		AssertValidPort(ports[i]);
	}
}

void CAutoRouterBox::AssertValidPort(CAutoRouterPort* port) const
{
	port->AssertValid();

	if( owner != NULL ) {
		CAutoRouterBox* ownerBox = port->GetOwner();
		ASSERT( ownerBox == this );
	}

	CRect r = port->GetRect();
	ASSERT( IsRectIn(r, rect) );
}

#endif

// CAutoRouterBox

