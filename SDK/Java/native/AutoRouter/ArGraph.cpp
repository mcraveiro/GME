
#include "stdafx.h"
#include <math.h>

#include "ArGraph.h"

#ifdef _DEBUG
// #define _DEBUG_DEEP
#endif

// --------------------------- CArPort


CArPort::CArPort():	stored_ref(NULL),
					owner(NULL),
					rect(0,0,0,0),
					attributes(ARPORT_Default)
{
	CalculateSelfPoints();
}

CArPort::~CArPort()
{
}

CArBox* CArPort::GetOwner() const
{
	ASSERT( owner != NULL );
	return owner;
}

void CArPort::SetOwner(CArBox* o)
{
	ASSERT( owner == NULL || o == NULL );
	owner = o;
}

// --- StartEndpoints

int CArPort::CanHaveStartEndPointOn(EArDir dir, int isstart) const
{
	int d = (int) dir;
	ASSERT( 0 <= d && d <= 3 );

	if( isstart )
		d += 4;

	return (attributes & (1 << d)) != 0;
}

int CArPort::CanHaveStartEndPoint(int isstart) const
{
	return (attributes & (isstart ? ARPORT_StartOnAll : ARPORT_EndOnAll)) != 0;
}

int CArPort::CanHaveStartEndPointHorizontal(int ishorizontal) const
{
	return (attributes & (ishorizontal ? ARPORT_StartEndHorizontal : ARPORT_StartEndVertical)) != 0;
}

EArDir CArPort::GetStartEndDirTo(CPoint point, int isstart, EArDir notthis) const
{
	ASSERT( !IsRectEmpty() );

	CSize offset = point - rect.CenterPoint();

	EArDir dir1 = GetMajorDir(offset);

	if( dir1 != notthis && CanHaveStartEndPointOn(dir1, isstart) )
		return dir1;

	EArDir dir2 = GetMinorDir(offset);

	if( dir2 != notthis && CanHaveStartEndPointOn(dir2, isstart) )
		return dir2;

	EArDir dir3 = ReverseDir(dir2);

	if( dir3 != notthis && CanHaveStartEndPointOn(dir3, isstart) )
		return dir3;

	EArDir dir4 = ReverseDir(dir1);

	if( dir4 != notthis && CanHaveStartEndPointOn(dir4, isstart) )
		return dir4;

	if( CanHaveStartEndPointOn(dir1, isstart) )
		return dir1;

	if( CanHaveStartEndPointOn(dir2, isstart) )
		return dir2;

	if( CanHaveStartEndPointOn(dir3, isstart) )
		return dir3;

	if( CanHaveStartEndPointOn(dir4, isstart) )
		return dir4;

	return Dir_Top;
}

int CArPort::CanCreateStartEndPointAt(CPoint point, int isstart, int nearness) const
{
	return CanHaveStartEndPoint(isstart) && IsPointIn(point, rect, nearness);
}

CPoint CArPort::CreateStartEndPointAt(CPoint point, int isstart) const
{
	ASSERT( !IsRectEmpty() );

	EArDir dir = Dir_None;

	CArFindNearestLine nearest(point);

	if( CanHaveStartEndPointOn(Dir_Top, isstart) && nearest.HLine(rect.left, rect.right-1, rect.top) )
		dir = Dir_Top;

	if( CanHaveStartEndPointOn(Dir_Right, isstart) && nearest.VLine(rect.top, rect.bottom-1, rect.right-1) )
		dir = Dir_Right;

	if( CanHaveStartEndPointOn(Dir_Bottom, isstart) && nearest.HLine(rect.left, rect.right-1, rect.bottom-1) )
		dir = Dir_Bottom;

	if( CanHaveStartEndPointOn(Dir_Left, isstart) && nearest.VLine(rect.top, rect.bottom-1, rect.left) )
		dir = Dir_Left;

	ASSERT( IsRightAngle(dir) );

	if( IsConnectToCenter() )
		return CreateStartEndPointOn(dir);

	if( point.x < rect.left )
		point.x = rect.left;
	else if( rect.right <= point.x )
		point.x = rect.right-1;

	if( point.y < rect.top )
		point.y = rect.top;
	else if( rect.bottom <= point.y )
		point.y = rect.bottom-1;

	switch(dir)
	{
	case Dir_Top:
		point.y = rect.top;
		break;

	case Dir_Right:
		point.x = rect.right-1;
		break;

	case Dir_Bottom:
		point.y = rect.bottom-1;
		break;

	case Dir_Left:
		point.x = rect.left;
		break;
	}

	return point;
}

CPoint CArPort::CreateStartEndPointOn(EArDir dir) const
{
	ASSERT( !IsRectEmpty() );
	ASSERT( IsRightAngle(dir) );

	switch( dir )
	{
	case Dir_Top:
		return CPoint((rect.left + rect.right)/2, rect.top);

	case Dir_Bottom:
		return CPoint((rect.left + rect.right)/2, rect.bottom-1);

	case Dir_Left:
		return CPoint(rect.left, (rect.top + rect.bottom)/2);
	}

	return CPoint(rect.right-1, (rect.top + rect.bottom)/2);
}

void CArPort::SetAttributes(unsigned int attr)
{
	ASSERT( owner == NULL );

	attributes = attr;
}

// --- Data

void CArPort::SetRect(CRect r)
{
	ASSERT( r.Width() >= 3 && r.Height() >= 3 );

	rect = r;
	CalculateSelfPoints();
}

void CArPort::ShiftBy(CSize offset)
{
	ASSERT( !rect.IsRectEmpty() );

	rect += offset;
	CalculateSelfPoints();
}

void CArPort::CalculateSelfPoints()
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

// --- Serialize

IMPLEMENT_SERIAL(CArPort, CObject, 0);

void CArPort::Serialize(CArchive& ar)
{
	ASSERT( stored_ref == NULL );

	if( ar.IsStoring() )
	{
		ar << this;
		ar << rect;
	}
	else
	{
		ar >> stored_ref;
		ar >> rect;
	}
}

// --- Debug

#ifdef _DEBUG

void CArPort::AssertValid() const
{
}

void CArPort::AssertValidStartEndPoint(CPoint point, EArDir dir, int isstart) const
{
	ASSERT( !IsRectEmpty() );

	if( dir == Dir_None )
	{
		dir = OnWhichEdge(point);
		ASSERT( IsRightAngle(dir) );
	}
	else
	{
		ASSERT( dir == OnWhichEdge(point) );
	}

	ASSERT( CanHaveStartEndPointOn(dir, isstart) );
}

#endif


// --------------------------- CArBox


CArBox::CArBox(): owner(NULL),
				  rect(0,0,0,0),
				  atomic(0)
{
	CalculateSelfPoints();
}

CArBox::~CArBox()
{
	DeleteAllPorts();
}

CArGraph* CArBox::GetOwner() const
{
	ASSERT( owner != NULL );
	return owner;
}

void CArBox::SetOwner(CArGraph* graph)
{
	ASSERT( owner == NULL || graph == NULL );

	owner = graph;
}

// --- Ports

CArPort* CArBox::CreatePort() const
{
	CArPort* port = new CArPort;
	ASSERT( port != NULL );

	return port;
}

void CArBox::Add(CArPort* port)
{
	ASSERT( port != NULL && port->owner == NULL );
	ASSERT( !atomic );

	port->SetOwner(this);
	ports.AddTail(port);

#ifdef _DEBUG
	AssertValidPort(port);
#endif
}

void CArBox::Delete(CArPort* port)
{
	ASSERT( port != NULL && port->owner == this );

#ifdef _DEBUG
	AssertValidPort(port);

	if( atomic )
		ASSERT( ports.GetCount() == 1 );
#endif

	POSITION pos = ports.Find(port);
	ASSERT( pos != NULL );
	ports.RemoveAt(pos);

	delete port;

	atomic = 0;
}

void CArBox::DeleteAllPorts()
{
	POSITION pos = ports.GetHeadPosition();
	while( pos != NULL )
		delete ports.GetNext(pos);

	ports.RemoveAll();

	atomic = 0;
}

CArPort* CArBox::GetPortAt(CPoint point, int nearness) const
{
	POSITION pos = ports.GetHeadPosition();
	while( pos != NULL )
	{
		CArPort* port = ports.GetNext(pos);
		if( port->IsPortAt(point, nearness) )
			return port;
	}

	return NULL;
}

// --- Atomic Port

void CArBox::SetAtomicPort(CArPort* port)
{
	ASSERT( HasNoPort() );
	ASSERT( !IsRectEmpty() );
	ASSERT( !atomic );

	port->SetRect(rect);
	Add(port);

	atomic = 1;
}

CArPort* CArBox::GetAtomicPort() const
{
	ASSERT( atomic && ports.GetCount() == 1 );

	return ports.GetHead();
}

// --- Data

void CArBox::SetRect(CRect r)
{
	ASSERT( r.Width() >= 3 && r.Height() >= 3 );
	ASSERT( r.TopLeft().x >= ED_MINCOORD && r.TopLeft().y >= ED_MINCOORD );
	ASSERT( r.BottomRight().x <= ED_MAXCOORD && r.BottomRight().y <= ED_MAXCOORD );
	ASSERT( HasNoPort() || atomic );

	rect = r;
	CalculateSelfPoints();

	if( atomic )
	{
		ASSERT( ports.GetCount() == 1 );
		ports.GetHead()->SetRect(r);
	}
}

void CArBox::ShiftBy(CSize offset)
{
	rect += offset;

	POSITION pos = ports.GetHeadPosition();
	while( pos != NULL )
		ports.GetNext(pos)->ShiftBy(offset);

	CalculateSelfPoints();
}

void CArBox::CalculateSelfPoints()
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

// --- Serialize

IMPLEMENT_SERIAL(CArBox, CObject, 0);

void CArBox::Serialize(CArchive& ar)
{
	if( ar.IsStoring() )
		ar << rect;
	else
		ar >> rect;

	ports.Serialize(ar);

	if( ar.IsLoading() )
	{
		POSITION pos = ports.GetHeadPosition();
		while( pos != NULL )
			ports.GetNext(pos)->SetOwner(this);

		CalculateSelfPoints();
	}
}

CArPort* CArBox::ResolveStoredRef(CArPort* ref)
{
	POSITION pos = ports.GetHeadPosition();
	while( pos != NULL )
	{
		CArPort* port = ports.GetNext(pos);
		if( port->MatchStoredRef(ref) )
			return port;
	}

	return NULL;
}
		
void CArBox::ClearStoredRefs()
{
	POSITION pos = ports.GetHeadPosition();
	while( pos != NULL )
		ports.GetNext(pos)->ClearStoredRefs();
}

// --- Debug

#ifdef _DEBUG

void CArBox::AssertValid() const
{
	POSITION pos = ports.GetHeadPosition();
	while( pos != NULL )
	{
		CArPort* port = ports.GetNext(pos);
		AssertValidPort(port);
	}
}

void CArBox::AssertValidPort(const CArPort* port) const
{
	ASSERT_VALID(port);

	if( owner != NULL )
		ASSERT( port->GetOwner() == this );

	ASSERT( IsRectIn(port->rect, rect) );
}

#endif



// --------------------------- CArPath



CArPath::CArPath(): owner(NULL),
					ref_type(ref_type_valid),
					startport(NULL),
					endport(NULL),
					attributes(ARPATH_Default),
					state(ARPATHST_Default),
					extptr(NULL)
{
}

CArPath::~CArPath()
{
	DeleteAll();
}

CArGraph* CArPath::GetOwner() const
{
	ASSERT( owner != NULL );
	return owner;
}

void CArPath::SetOwner(CArGraph* graph)
{
	ASSERT( owner == NULL || graph == NULL );
	owner = graph;
}

// --- Ports

void CArPath::SetStartPort(CArPort* port)
{
	ASSERT( owner == NULL );
	startport = port;
}

void CArPath::SetEndPort(CArPort* port)
{
	ASSERT( owner == NULL );
	endport = port;
}

void CArPath::ClearPorts()
{
	ASSERT( owner == NULL );

	startport = NULL;
	endport = NULL;
}

CArPort* CArPath::GetStartPort() const
{
	ASSERT( startport != NULL );
	return startport;
}

CArPort* CArPath::GetEndPort() const
{
	ASSERT( endport != NULL );
	return endport;
}

// --- Points

void CArPath::AddTail(CPoint point)
{
	ASSERT( !IsConnected() );
	points.AddTail(point);
}

void CArPath::DeleteAll()
{
	points.RemoveAll();
	state = ARPATHST_Default;
}

CPoint CArPath::GetStartPoint() const
{
	ASSERT( points.GetCount() >= 2 );

	return points.GetHead();
}

CPoint CArPath::GetEndPoint() const
{
	ASSERT( points.GetCount() >= 2 );

	return points.GetTail();
}

CPoint CArPath::GetOutOfBoxStartPoint() const
{
	ASSERT( points.GetCount() >= 2 );

	POSITION pos = points.GetHeadPosition();
	CPoint p = points.GetNext(pos);
	EArDir d = GetDir(points.GetAt(pos) - p);
	ASSERT( IsRightAngle(d) );

	GetPointCoord(p, d) = GetRectOuterCoord(GetStartBox()->GetRect(), d);
	ASSERT( points.GetAt(pos) == p || GetDir(points.GetAt(pos) - p) == d ); 

	return p;
}

CPoint CArPath::GetOutOfBoxEndPoint() const
{
	ASSERT( points.GetCount() >= 2 );

	POSITION pos = points.GetTailPosition();
	CPoint p = points.GetPrev(pos);
	EArDir d = GetDir(points.GetAt(pos) - p);
	ASSERT( IsRightAngle(d) );

	GetPointCoord(p, d) = GetRectOuterCoord(GetEndBox()->GetRect(), d);
	ASSERT( points.GetAt(pos) == p || GetDir(points.GetAt(pos) - p) == d ); 

	return p;
}

POSITION CArPath::GetPointPosAt(CPoint point, int nearness) const
{
	POSITION pos = points.GetHeadPosition();
	while( pos != NULL )
	{
		POSITION oldpos = pos;
		if( IsPointNear(points.GetNext(pos), point, nearness) )
			return oldpos;
	}
	
	return NULL;
}

POSITION CArPath::GetEdgePosAt(CPoint point, int nearness) const
{
	CPoint a;
	CPoint b;

	POSITION pos = GetTailEdge(a, b);
	while( pos != NULL )
	{
		if( IsPointNearLine(point, a, b, nearness) )
			return pos;

		GetPrevEdge(pos, a, b);
	}

	return NULL;
}

void CArPath::SimplifyTrivially()
{
	ASSERT( !IsConnected() );

	if( points.GetCount() <= 2 )
		return;
	
	POSITION pos = points.GetHeadPosition();

	POSITION pos1 = pos;
	ASSERT( pos1 != NULL );
	CPoint p1 = points.GetNext(pos);

	POSITION pos2 = pos;
	ASSERT( pos2 != NULL );
	CPoint p2 = points.GetNext(pos);

	EArDir dir12 = GetDir(p2-p1);

	POSITION pos3 = pos;
	ASSERT( pos3 != NULL );
	CPoint p3 = points.GetNext(pos);

	EArDir dir23 = GetDir(p3-p2); 

	for(;;)
	{
		if( dir12 == Dir_None || dir23 == Dir_None ||
			(dir12 != Dir_Skew && dir23 != Dir_Skew &&
			(dir12 == dir23 || dir12 == ReverseDir(dir23)) ) )
		{
			points.RemoveAt(pos2);
			dir12 = GetDir(p3-p1);
		}
		else
		{
			pos1 = pos2;
			p1 = p2;
			dir12 = dir23;
		}

		if( pos == NULL )
			return;

		pos2 = pos3;
		p2 = p3;

		pos3 = pos;
		p3 = points.GetNext(pos);

		dir23 = GetDir(p3-p2);
	}

#ifdef _DEBUG
	AssertValidPoints();
#endif
}

// --- Edges

POSITION CArPath::GetHeadEdge(CPoint& start, CPoint& end) const
{
	if( points.GetCount() < 2 )
		return NULL;

	POSITION pos = points.GetHeadPosition();
	ASSERT( pos != NULL );

	start = points.GetNext(pos);
	ASSERT( pos != NULL );

	end = points.GetAt(pos);

	return pos;
}

POSITION CArPath::GetTailEdge(CPoint& start, CPoint& end) const
{
	if( points.GetCount() < 2 )
		return NULL;

	POSITION pos = points.GetTailPosition();
	ASSERT( pos != NULL );

	end = points.GetPrev(pos);
	ASSERT( pos != NULL );

	start = points.GetAt(pos);

	return pos;
}

void CArPath::GetNextEdge(POSITION& pos, CPoint& start, CPoint& end) const
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	points.GetNext(pos);
	ASSERT( pos != NULL );

	POSITION p = pos;
	start = points.GetNext(p);
	if( p == NULL )
		pos = NULL;
	else
		end = points.GetAt(p);
}

void CArPath::GetPrevEdge(POSITION& pos, CPoint& start, CPoint& end) const
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	end = points.GetPrev(pos);
	if( pos != NULL )
		start = points.GetAt(pos);
}

void CArPath::GetEdge(POSITION pos, CPoint& start, CPoint& end) const
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	start = points.GetNext(pos);
	ASSERT( pos != NULL );

	end = points.GetAt(pos);
}

POSITION CArPath::GetHeadEdgePtrs(CPoint*& start, CPoint*& end)
{
	if( points.GetCount() < 2 )
		return NULL;

	POSITION pos = points.GetHeadPosition();
	ASSERT( pos != NULL );

	start = &(points.GetNext(pos));
	ASSERT( pos != NULL );

	end = &(points.GetAt(pos));

	return pos;
}

POSITION CArPath::GetTailEdgePtrs(CPoint*& start, CPoint*& end)
{
	if( points.GetCount() < 2 )
		return NULL;

	POSITION pos = points.GetTailPosition();
	ASSERT( pos != NULL );

	end = &(points.GetPrev(pos));
	ASSERT( pos != NULL );

	start = &(points.GetAt(pos));

	return pos;
}

void CArPath::GetNextEdgePtrs(POSITION& pos, CPoint*& start, CPoint*& end)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	points.GetNext(pos);
	ASSERT( pos != NULL );

	POSITION p = pos;
	start = &(points.GetNext(p));
	if( p == NULL )
		pos = NULL;
	else
		end = &(points.GetAt(p));
}

void CArPath::GetPrevEdgePtrs(POSITION& pos, CPoint*& start, CPoint*& end)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	end = &(points.GetPrev(pos));
	if( pos != NULL )
		start = &(points.GetAt(pos));
}

void CArPath::GetEdgePtrs(POSITION pos, CPoint*& start, CPoint*& end)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	start = &(points.GetNext(pos));
	ASSERT( pos != NULL );

	end = &(points.GetAt(pos));
}

CPoint* CArPath::GetStartPoint(POSITION pos)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	return &(points.GetAt(pos));
}

CPoint* CArPath::GetEndPoint(POSITION pos)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	points.GetNext(pos);
	ASSERT( pos != NULL );

	return &(points.GetAt(pos));
}

CPoint* CArPath::GetPointBeforeEdge(POSITION pos)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	points.GetPrev(pos);
	if( pos == NULL )
		return NULL;

	return &(points.GetAt(pos));
}

CPoint* CArPath::GetPointAfterEdge(POSITION pos)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	points.GetNext(pos);
	ASSERT( pos != NULL );

	points.GetNext(pos);
	if( pos == NULL )
		return NULL;

	return &(points.GetAt(pos));
}

POSITION CArPath::GetEdgePosBeforePoint(POSITION pos) const
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	points.GetPrev(pos);
	return pos;
}

POSITION CArPath::GetEdgePosAfterPoint(POSITION pos) const
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	POSITION p = pos;
	points.GetNext(p);
	if( p == NULL )
		return NULL;

	return pos;
}

POSITION CArPath::GetEdgePosForStartPoint(CPoint* startpoint)
{
	POSITION pos = points.GetHeadPosition();
	while( pos != NULL )
	{
		if( &(points.GetNext(pos)) == startpoint )
		{
			ASSERT( pos != NULL );
			points.GetPrev(pos);
			break;
		}
	}

	ASSERT( pos != NULL );
	return pos;
}

// --- Data

CRect CArPath::GetSurroundRect() const
{
	CRect rect(INT_MAX,INT_MAX,INT_MIN,INT_MIN);

	POSITION pos = points.GetHeadPosition();
	while( pos != NULL )
	{
		CPoint point = points.GetNext(pos);

		rect.left = min(rect.left, point.x);
		rect.top = min(rect.top, point.y);
		rect.right = max(rect.right, point.x);
		rect.bottom = max(rect.bottom, point.y);
	}

	if( rect.left == INT_MAX || rect.top == INT_MAX ||
		rect.right == INT_MIN || rect.bottom == INT_MIN )
	{
		rect.SetRectEmpty();
	}

	return rect;
}

int CArPath::IsPathClip(CRect rect) const
{
	CPoint a;
	CPoint b;

	POSITION pos = GetTailEdge(a, b);
	while( pos != NULL )
	{
		if( IsLineClipRect(a, b, rect) )
			return 1;

		GetPrevEdge(pos, a, b);
	}

	return 0;
}

void CArPath::SetAttributes(unsigned int attr)
{
//	ASSERT( owner == NULL );

	attributes = attr;
}

void CArPath::SetState(int s)
{
	ASSERT( owner != NULL );

	state = s;

#ifdef _DEBUG
	AssertValid();
#endif
}

EArDir CArPath::GetEndDir() const
{
	unsigned int a = attributes & ARPATH_EndMask;
	return a == ARPATH_EndOnTop ? Dir_Top : 
	a == ARPATH_EndOnRight ? Dir_Right : 
	a == ARPATH_EndOnBottom ? Dir_Bottom :
	a == ARPATH_EndOnLeft ? Dir_Left : Dir_None;
}

EArDir CArPath::GetStartDir() const
{
	unsigned int a = attributes & ARPATH_StartMask;
	return a == ARPATH_StartOnTop ? Dir_Top : 
	a == ARPATH_StartOnRight ? Dir_Right : 
	a == ARPATH_StartOnBottom ? Dir_Bottom :
	a == ARPATH_StartOnLeft ? Dir_Left : Dir_None;
}

// --- Serialize

IMPLEMENT_SERIAL(CArPath, CObject, 0);

void CArPath::Serialize(CArchive& ar)
{
	ASSERT( HasRealReferences() );

	if( ar.IsStoring() )
	{
		ar << startport;
		ar << endport;
		
		ar << points.GetCount();
		POSITION pos = points.GetHeadPosition();
		while( pos != NULL )
			ar << points.GetNext(pos);
	}
	else
	{
		ASSERT( startport == NULL && endport == NULL && points.IsEmpty() );

		ar >> startport;
		ar >> endport;

		ref_type = ref_type_stored;

		int count;
		ar >> count;
		while( count-- > 0 )
		{
			CPoint point;
			ar >> point;
			AddTail(point);
		}
	}
}

void CArPath::LocateRefs()
{
	ASSERT( ref_type == ref_type_stored );
	ASSERT( owner != NULL );

	if( startport != NULL )
		startport = owner->ResolveStoredRef(startport);

	if( endport != NULL )
		endport = owner->ResolveStoredRef(endport);

	ref_type = ref_type_valid;

	ASSERT_VALID(this);
}

// --- Debug

#ifdef _DEBUG

void CArPath::AssertValid() const
{
	if( startport != NULL )
		ASSERT_VALID(startport);

	if( endport != NULL )
		ASSERT_VALID(endport);

	if( IsConnected() )
		ASSERT( !points.IsEmpty() );
	else
		ASSERT( points.IsEmpty() );
}

void CArPath::AssertValidPos(POSITION pos) const
{
	ASSERT( pos != NULL );

	POSITION p = points.GetHeadPosition();
	for(;;)
	{
		ASSERT( p != NULL );
		if( p == pos )
			return;

		points.GetNext(p);
	}
}

#pragma warning( disable : 4701 )

void CArPath::AssertValidPoints() const
{
	points.AssertValid();

	if( points.IsEmpty() )
		return;

#ifdef _DEBUG_DEEP
	TRACE("CArPath::AssertValidPoints (count=%d) START\n", points.GetCount());
#endif

	POSITION pos = points.GetHeadPosition();
	ASSERT( points.GetCount() >= 2 && pos != NULL );

	POSITION p1p = NULL;
	POSITION p2p = NULL;
	POSITION p3p;

	CPoint p1;
	CPoint p2;

	EArDir d12;
	EArDir d23;

	while( pos != NULL )
	{
		p3p = p2p;
		p2p = p1p;
		p1p = pos;

		p2 = p1;
		p1 = points.GetNext(pos);

#ifdef _DEBUG_DEEP
		TRACE("%p (%d,%d)\n", p1p, p1.x, p1.y);
#endif
		d23 = d12;
		if( p2p != NULL )
		{
			d12 = GetDir(p2 - p1);
			ASSERT( IsRightAngle(d12) );
		}

		if( p3p != NULL )
			ASSERT( AreInRightAngle(d12, d23) );
	}

#ifdef _DEBUG_DEEP
	TRACE("CArPath::AssertValidPoints END\n");
#endif

}

#endif


// --------------------------- CArGraph


CArGraph::CArGraph(): horizontal(1),
					  vertical(0)
{
	horizontal.owner = this;
	vertical.owner = this;
	CalculateSelfPoints();
	AddEdges(this);
}

CArGraph::~CArGraph()
{
	DeleteEdges(this);
	DeleteAll();
}

// --- Boxes

CArBox* CArGraph::CreateBox() const
{
	CArBox* box = new CArBox;
	ASSERT( box != NULL );

	return box;
}

void CArGraph::Add(CArBox* box)
{
	ASSERT( box != NULL );
	ASSERT( box->owner == NULL );

	DisconnectPathsClipping(box->GetRect());

	box->SetOwner(this);

	boxes.AddTail(box);

	AddBoxAndPortEdges(box);
}

void CArGraph::Remove(CArBox* box)
{
	ASSERT( box != NULL && box->owner == this );

	DeleteBoxAndPortEdges(box);

	POSITION pos = paths.GetHeadPosition();
	while( pos != NULL )
	{
		CArPath* path = paths.GetNext(pos);

		if( path->GetStartPort()->GetOwner() == box || path->GetEndPort()->GetOwner() == box )
			Delete(path);
	}

	box->SetOwner(NULL);

	pos = boxes.Find(box);
	ASSERT( pos != NULL );
	boxes.RemoveAt(pos);
}

void CArGraph::Delete(CArBox* box)
{
	if( box->owner != NULL )
	{
		ASSERT( box->owner == this );

		Remove(box);
	}

	delete box;
}

void CArGraph::DeleteAllBoxes()
{
	POSITION pos = boxes.GetHeadPosition();
	while( pos != NULL )
	{
		CArBox* box = boxes.GetNext(pos);

		DeleteBoxAndPortEdges(box);

		delete box;
	}

	boxes.RemoveAll();
}

void CArGraph::SetPortAttr(CArPort* port, unsigned int attr)
{
	ASSERT_VALID(port);

	DisconnectPathsFrom(port);
	port->SetAttributes(attr);
}

int CArGraph::IsRectClipBoxes(CRect rect) const
{
	POSITION pos = boxes.GetHeadPosition();
	while( pos != NULL )
	{
		if( IsRectClip(rect, boxes.GetNext(pos)->rect) )
			return 1;
	}

	return 0;
}

int CArGraph::IsLineClipBoxes(CPoint p1, CPoint p2) const
{
	CRect rect(p1, p2);
	rect.NormalizeRect();
	ASSERT( rect.left == rect.right || rect.top == rect.bottom );

	if( rect.left == rect.right)
		rect.right++;
	if( rect.top == rect.bottom )
		rect.bottom++;

	return IsRectClipBoxes(rect);
}

int CArGraph::CanBoxAt(CRect rect) const
{
	return !IsRectClipBoxes(InflatedRect(rect, 1));
}

int CArGraph::CanShiftBy(CArBox* box, CSize offset) const
{
	ASSERT_VALID(box);

	CRect rect = box->rect + offset;
	rect.InflateRect(1, 1);

	POSITION pos = boxes.GetHeadPosition();
	while( pos != NULL )
	{
		CArBox* b = boxes.GetNext(pos);
		if( b != box && IsRectClip(rect, b->rect) )
			return 0;
	}

	return 1;
}

void CArGraph::ShiftBy(CArBox* box, CSize offset)
{
	ASSERT_VALID(box);

	DeleteBoxAndPortEdges(box);
	box->ShiftBy(offset);
	AddBoxAndPortEdges(box);
	
	DisconnectPathsClipping(box->GetRect());
	DisconnectPathsFrom(box);
}

CArBox* CArGraph::GetBoxAt(CPoint point, int nearness) const
{
	POSITION pos = boxes.GetHeadPosition();
	while( pos != NULL )
	{
		CArBox* box = boxes.GetNext(pos);
		if( box->IsBoxAt(point, nearness) )
			return box;
	}

	return NULL;
}

CArPort* CArGraph::GetPortAt(CPoint point, int nearness) const
{
	POSITION pos = boxes.GetHeadPosition();
	while( pos != NULL )
	{
		CArPort* port = boxes.GetNext(pos)->GetPortAt(point, nearness);
		if( port != NULL )
			return port;
	}

	return NULL;
}

// --- Paths

CArPath* CArGraph::CreatePath() const
{
	CArPath* path = new CArPath;
	ASSERT( path != NULL );

	return path;
}

void CArGraph::Add(CArPath* path)
{
	ASSERT( path != NULL && path->owner == NULL );

	path->SetOwner(this);

	paths.AddTail(path);
	AddEdges(path);

#ifdef _DEBUG
	AssertValidPath(path);
#endif
}

void CArGraph::Remove(CArPath* path)
{
	ASSERT( path != NULL && path->owner == this );

	DeleteEdges(path);

	path->SetOwner(NULL);

	POSITION pos = paths.Find(path);
	ASSERT( pos != NULL );
	paths.RemoveAt(pos);

	path->SetOwner(NULL);
}

void CArGraph::Delete(CArPath* path)
{
	ASSERT( path != NULL );

	if( path->owner != NULL )
	{
		ASSERT( path->owner == this );

		Remove(path);
	}

	delete path;
}

void CArGraph::DeleteAllPaths()
{
	POSITION pos = paths.GetHeadPosition();
	while( pos != NULL )
	{
		CArPath* path = paths.GetNext(pos);

		DeleteEdges(path);
		path->SetOwner(NULL);
		delete path;
	}

	paths.RemoveAll();
}

CArPath* CArGraph::GetPathAt(CPoint point, int nearness) const
{
	POSITION pos = paths.GetHeadPosition();
	while( pos != NULL )
	{
		CArPath* path = paths.GetNext(pos);
		if( path->IsPathAt(point, nearness) )
			return path;
	}

	return NULL;
}

CArPath* CArGraph::GetPathPointAt(CPoint point, POSITION& pos, int nearness) const
{
	POSITION pathpos = paths.GetHeadPosition();
	while( pathpos != NULL )
	{
		CArPath* path = paths.GetNext(pathpos);
		ASSERT( path != NULL );
		pos = path->GetPointPosAt(point, nearness);
		if( pos != NULL )
			return path;
	}

	return NULL;
}

CArPath* CArGraph::GetPathEdgeAt(CPoint point, POSITION& pos, int nearness) const
{
	POSITION pathpos = paths.GetHeadPosition();
	while( pathpos != NULL )
	{
		CArPath* path = paths.GetNext(pathpos);
		ASSERT( path != NULL );
		pos = path->GetEdgePosAt(point, nearness);
		if( pos != NULL )
			return path;
	}

	return NULL;
}

SArEdge* CArGraph::GetPathEdgeAt(CArPath* path, POSITION pos) const
{
	ASSERT( path != NULL && pos != NULL );

	CPoint* start;
	CPoint* end;

	path->GetEdgePtrs(pos, start, end);
	ASSERT( start != NULL && end != NULL );

	if( start->y == end->y )
		return horizontal.GetEdge(start, end);
	else if( start->x == end->x )
		return vertical.GetEdge(start, end);

	return NULL;
}

SArEdge* CArGraph::GetPathEdgeAt(CPoint point, int nearness) const
{
	POSITION pos;
	CArPath* path = GetPathEdgeAt(point, pos, nearness);
	if( path )
		return GetPathEdgeAt(path, pos);

	return NULL;
}

SArEdge* CArGraph::GetListEdgeAt(CPoint point, int nearness) const
{
	SArEdge* edge;

	edge = horizontal.GetEdgeAt(point, nearness);
	if( edge )
		return edge;

	return vertical.GetEdgeAt(point, nearness);
}

CArPath* CArGraph::AddPath(CArPort* startport, CArPort* endport)
{
//	ASSERT( startport != NULL && startport->GetOwner()->GetOwner() == this );
//	ASSERT( endport != NULL && endport->GetOwner()->GetOwner() == this );

	CArPath* path = CreatePath();
	path->SetStartPort(startport);
	path->SetEndPort(endport);
	Add(path);

	return path;
}

// --- Boxes && Paths

CRect CArGraph::GetSurroundRect() const
{
	CRect rect(0,0,0,0);

	POSITION pos;

	pos = boxes.GetHeadPosition();
	while( pos != NULL )
		rect |= boxes.GetNext(pos)->rect;

	pos = paths.GetHeadPosition();
	while( pos != NULL )
		rect |= paths.GetNext(pos)->GetSurroundRect();

	return rect;
}

int CArGraph::AutoRoute()
{
	ConnectAllDisconnectedPaths();

	int updated = 0;
	int last = 0;
	int c = 100;		// max # of total op
	int dm = 10;		// max # of distribution op
	int d = 0;

	while( c > 0 )
	{
		if( c > 0 )
		{
			if( last == 1 )
				break;

			c--;
			if( SimplifyPaths() )
			{
				updated = 1;
				last = 1;
			}
		}

		if( c > 0 )
		{
			if( last == 2 )
				break;

			c--;
			if( horizontal.Block_ScanBackward() )
			{
				updated = 1;

				do
					c--;
				while( c > 0 && horizontal.Block_ScanBackward() );

				if( last < 2 || last > 5 )
					d = 0;
				else if( ++d >= dm )
					break;
				
				last = 2;
			}
		}
		
		if( c > 0 )
		{
			if( last == 3 )
				break;

			c--;
			if( horizontal.Block_ScanForward() )
			{
				updated = 1;

				do
					c--;
				while( c > 0 && horizontal.Block_ScanForward() );

				if( last < 2 || last > 5 )
					d = 0;
				else if( ++d >= dm )
					break;
				
				last = 3;
			}
		}

		if( c > 0 )
		{
			if( last == 4 )
				break;

			c--;
			if( vertical.Block_ScanBackward() )
			{
				updated = 1;

				do
					c--;
				while( c > 0 && vertical.Block_ScanBackward() );

				if( last < 2 || last > 5 )
					d = 0;
				else if( ++d >= dm )
					break;
				
				last = 4;
			}
		}
		if( c > 0 )
		{
			if( last == 5 )
				break;

			c--;
			if( vertical.Block_ScanForward() )
			{
				updated = 1;

				do
					c--;
				while( c > 0 && vertical.Block_ScanForward() );

				if( last < 2 || last > 5 )
					d = 0;
				else if( ++d >= dm )
					break;
				
				last = 5;
			}
		}
		
		if( c > 0 )
		{
			if( last == 6 )
				break;

			c--;
			if( horizontal.Block_SwitchWrongs() )
			{
				updated = 1;
				last = 6;
			}
		}

		if( c > 0 )
		{
			if( last == 7 )
				break;

			c--;
			if( vertical.Block_SwitchWrongs() )
			{
				updated = 1;
				last = 7;
			}
		}

		if( last == 0 )
			break;
	}

	if( c <= 0 )
	{
		// MessageBeep(MB_ICONEXCLAMATION);
		updated = -1;
	}

	return updated;
}

// --- Navigation

CArBox* CArGraph::GetOutOfBox(CPoint& point, EArDir dir) const
{
	ASSERT( IsRightAngle(dir) );

	CArBox* boxby = NULL;

	POSITION pos = boxes.GetHeadPosition();
	while( pos != NULL )
	{
		CArBox* box = boxes.GetNext(pos);
		if( box->rect.PtInRect(point) )
		{
			CRect& rect = box->rect;
			boxby = box;
			pos = boxes.GetHeadPosition();

			GetPointCoord(point, dir) = GetRectOuterCoord(rect, dir);
		}
	}

	return boxby;
}

CArBox* CArGraph::GoToNextBox(CPoint& point, EArDir dir, long stophere) const
{
	ASSERT( IsRightAngle(dir) );
	ASSERT( GetPointCoord(point, dir) != stophere );

	CArBox* boxby = NULL;

	POSITION pos = boxes.GetHeadPosition();
	while( pos != NULL )
	{
		CArBox* box = boxes.GetNext(pos);
		CRect& rect = box->rect;

		if( IsPointInDirFrom(point, rect, ReverseDir(dir)) &&
			IsPointBetweenSides(point, rect, dir) &&
			IsCoordInDirFrom(stophere, GetRectOuterCoord(rect, ReverseDir(dir)), dir) )
		{
			stophere = GetRectOuterCoord(rect, ReverseDir(dir));
			boxby = box;
		}
	}

	GetPointCoord(point, dir) = stophere;
	return boxby;
}

void CArGraph::GetLimitsOfEdge(CPoint start, CPoint end, long& min, long& max) const
{
	long t;

	POSITION pos = boxes.GetHeadPosition();
	CArBox* box;

	if( start.y == end.y )
	{
		if( start.x > end.x )
		{
			t = start.x;
			start.x = end.x;
			end.x = t;
		}

		while( pos != NULL )
		{
			box = boxes.GetNext(pos);
			CRect& rect = box->rect;

			if(start.x < rect.right && rect.left <= end.x)
			{
				if( rect.bottom <= start.y && rect.bottom > min )
					min = rect.bottom;
				if( rect.top > start.y && rect.top < max )
					max = rect.top;
			}
		}
	}
	else
	{
		ASSERT( start.x == end.x );

		if( start.y > end.y )
		{
			t = start.y;
			start.y = end.y;
			end.y = t;
		}

		while( pos != NULL )
		{
			box = boxes.GetNext(pos);
			CRect& rect = box->rect;

			if(start.y < rect.bottom && rect.top <= end.y)
			{
				if( rect.right <= start.x && rect.right > min )
					min = rect.right;
				if( rect.left > start.x && rect.left < max )
					max = rect.left;
			}
		}
	}

	max--;
}

void CArGraph::Connect(CArPath* path)
{
	ASSERT( path != NULL && path->GetOwner() == this );

	CArPort* startport = path->GetStartPort();
	CArPort* endport = path->GetEndPort();

	EArDir startdir = path->GetStartDir();
	if( startdir == Dir_None ) 
		startdir = startport->GetStartEndDirTo(endport->GetCenter(), 1);

	EArDir enddir = path->GetEndDir();
	if( enddir == Dir_None )
		enddir = endport->GetStartEndDirTo(startport->GetCenter(), 0, startport == endport ? startdir : Dir_None );

	CPoint startpoint = startport->CreateStartEndPointOn(startdir);
	CPoint endpoint = endport->CreateStartEndPointOn(enddir);

	if( startpoint == endpoint )
		startpoint = StepOneInDir(startpoint, NextClockwiseDir(startdir));

	Connect(path, startpoint, endpoint);
}

void CArGraph::Connect(CArPath* path, CPoint startpoint, CPoint endpoint)
{
	ASSERT( path != NULL && path->GetOwner() == this );
	ASSERT( !path->IsConnected() );
	ASSERT( startpoint != endpoint );

	EArDir startdir = path->GetStartPort()->OnWhichEdge(startpoint);
	EArDir enddir = path->GetEndPort()->OnWhichEdge(endpoint);
	ASSERT( IsRightAngle(startdir) && IsRightAngle(enddir) );

	CPoint start = startpoint;
	GetOutOfBox(start, startdir);
	ASSERT( start != startpoint );

	CPoint end = endpoint;
	GetOutOfBox(end, enddir);
	ASSERT( end != endpoint );

	CPointList ret;
	const CPointList& points = path->points;
	ASSERT( points.IsEmpty() );

	ConnectPoints(ret, start, end, startdir, enddir);

	path->DeleteAll();
	path->AddTail(startpoint);

	POSITION pos = ret.GetHeadPosition();
	while( pos != NULL )
		path->AddTail(ret.GetNext(pos));

	path->AddTail(endpoint);

	path->SimplifyTrivially();
	SimplifyPathPoints(path);
	CenterStairsInPathPoints(path);
	path->SetState(ARPATHST_Connected);

	AddEdges(path);
}

void CArGraph::ConnectPoints(CPointList& ret, CPoint start, CPoint end, EArDir hintstartdir, EArDir hintenddir)
{
	ASSERT( ret.IsEmpty() );
	ASSERT( !IsPointInBox(start) && !IsPointInBox(end) );

	CPoint thestart = start;

	while( start != end )
	{
		EArDir dir1 = ExGetMajorDir(end-start);
		EArDir dir2 = ExGetMinorDir(end-start);
		ASSERT( dir1 != Dir_None );

		ASSERT( dir1 == GetMajorDir(end-start) );
		ASSERT( dir2 == Dir_None || dir2 == GetMinorDir(end-start) );

		if( ret.IsEmpty() && dir2 == hintstartdir && dir2 != Dir_None )
		{
			dir2 = dir1;
			dir1 = hintstartdir;
		}

		ret.AddTail(start);
		CPoint old = start;

		CArBox* box = GoToNextBox(start, dir1, end);
		if( start == old )
		{
			ASSERT( box != NULL );
			CRect& rect = box->rect;

			if( dir2 == Dir_None )
				dir2 = NextClockwiseDir(dir1);

			ASSERT( dir1 != dir2 && dir1 != Dir_None && dir2 != Dir_None );

			if( IsPointInDirFrom(end, rect, dir2) )
			{
				ASSERT( !IsPointInDirFrom(start, rect, dir2) );
				GoToNextBox(start, dir2, end);
				ASSERT( IsPointInDirFrom(start, rect, dir2) );
			}
			else
			{
				ASSERT( IsPointBetweenSides(end, rect, dir1) );
				ASSERT( !IsPointIn(end, rect) );

				int rev = 0;

				if( ReverseDir(dir2) == hintenddir )
					rev = 1;
				else if( dir2 != hintenddir )
				{
					if( IsPointBetweenSides(thestart, rect, dir1) )
					{
						if(	IsPointInDirFrom(rect.TopLeft()+rect.BottomRight(), start + end, dir2) )
							rev = 1;
					}
					else
						if( IsPointInDirFrom(start, thestart, dir2) )
							rev = 1;
				}

				if( rev )
				{
					dir2 = ReverseDir(dir2);
				}

				GetPointCoord(start, dir2) = GetRectOuterCoord(rect, dir2);

				ASSERT( start != old );
				ret.AddTail(start);

				old = start;

				GetPointCoord(start, dir1) = GetRectOuterCoord(rect, dir1);
				ASSERT( IsPointInDirFrom(end, start, dir1) );
				if( GetPointCoord(start, dir1) != GetPointCoord(end, dir1) )
					GoToNextBox(start, dir1, end);
			}

			ASSERT( start != old );
		}
	}

	ret.AddTail(end);
}

void CArGraph::DisconnectAll()
{
	POSITION pos = paths.GetHeadPosition();
	while( pos != NULL )
		Disconnect(paths.GetNext(pos));
}

void CArGraph::Disconnect(CArPath* path)
{
	ASSERT( path != NULL && path->GetOwner() == this );

	if( path->IsConnected() )
		DeleteEdges(path);

	path->DeleteAll();
}

void CArGraph::DisconnectPathsClipping(CRect rect)
{
	POSITION pos = paths.GetTailPosition();
	while( pos != NULL )
	{
		CArPath* path = paths.GetPrev(pos);

		if( path->IsPathClip(rect) )
			Disconnect(path);
	}
}

void CArGraph::DisconnectPathsFrom(CArBox* box)
{
	ASSERT_VALID(box);

	POSITION pos = paths.GetTailPosition();
	while( pos != NULL )
	{
		CArPath* path = paths.GetPrev(pos);

		CArBox* startbox = path->GetStartPort()->GetOwner();
		CArBox* endbox = path->GetEndPort()->GetOwner();

		if( (startbox == box || endbox == box) )
			Disconnect(path);
	}
}

void CArGraph::DisconnectPathsFrom(CArPort* port)
{
	ASSERT_VALID(port);

	POSITION pos = paths.GetTailPosition();
	while( pos != NULL )
	{
		CArPath* path = paths.GetPrev(pos);

		CArPort* startport = path->GetStartPort();
		CArPort* endport = path->GetEndPort();

		if( (startport == port || endport == port) )
			Disconnect(path);
	}
}

// --- Edges

void CArGraph::AddAllEdges()
{
	ASSERT( horizontal.IsEmpty() && vertical.IsEmpty() );

	POSITION pos;

	pos = boxes.GetHeadPosition();
	while( pos != NULL )
		AddBoxAndPortEdges(boxes.GetNext(pos));

	pos = paths.GetHeadPosition();
	while( pos != NULL )
		AddEdges(paths.GetNext(pos));
}

void CArGraph::AddBoxAndPortEdges(CArBox* box)
{
	ASSERT( box != NULL );

	AddEdges(box);

	POSITION pos = box->ports.GetHeadPosition();
	while( pos != NULL )
		AddEdges(box->ports.GetNext(pos));
}

void CArGraph::DeleteBoxAndPortEdges(CArBox* box)
{
	ASSERT( box != NULL );

	DeleteEdges(box);

	POSITION pos = box->ports.GetHeadPosition();
	while( pos != NULL )
		DeleteEdges(box->ports.GetNext(pos));
}

// --- Path && Edges

int CArGraph::CanDeleteTwoEdgesAt(CArPath* path, POSITION pos)
{
#ifdef _DEBUG
	ASSERT( path->GetOwner() == this );
	ASSERT_VALID(path);
	ASSERT( path->IsConnected() );
	path->AssertValidPos(pos);
#ifdef _DEBUG_DEEP
	horizontal.AssertValidPathEdges(path);
	vertical.AssertValidPathEdges(path);
#endif
#endif

	CPointList& points = path->points;

	POSITION pointpos = pos;
	CPoint* point = &(points.GetNext(pos));
	POSITION npointpos = pos; if( npointpos == NULL ) return 0;
	CPoint* npoint = &(points.GetNext(pos));
	POSITION nnpointpos = pos; if( nnpointpos == NULL ) return 0;
	CPoint* nnpoint = &(points.GetNext(pos));
//	POSITION nnnpointpos = pos;
	pos = pointpos;
	points.GetPrev(pos);
	POSITION ppointpos = pos; if( ppointpos == NULL ) return 0;
	CPoint* ppoint = &(points.GetPrev(pos));
	POSITION pppointpos = pos; if( pppointpos == NULL ) return 0;
	CPoint* pppoint = &(points.GetAt(pos));

	ASSERT( pppointpos != NULL && ppointpos != NULL && pointpos != NULL && npointpos != NULL && nnpointpos != NULL );
	ASSERT( pppoint != NULL && ppoint != NULL && point != NULL && npoint != NULL && nnpoint != NULL );

	EArDir dir = GetDir(*npoint - *point);
	ASSERT( IsRightAngle(dir) );
	int ishorizontal = IsHorizontal(dir);

	CPoint newpoint;
	GetPointCoord(newpoint, ishorizontal) = GetPointCoord(*npoint, ishorizontal);
	GetPointCoord(newpoint, !ishorizontal) = GetPointCoord(*ppoint, !ishorizontal);

	ASSERT( GetDir(newpoint - *ppoint) == dir );

	if( IsLineClipBoxes(newpoint, *npoint) ) return 0;
	if( IsLineClipBoxes(newpoint, *ppoint) ) return 0;

	return 1;
}

void CArGraph::DeleteTwoEdgesAt(CArPath* path, POSITION pos)
{
#ifdef _DEBUG
	ASSERT( path->GetOwner() == this );
	ASSERT_VALID(path);
	ASSERT( path->IsConnected() );
	path->AssertValidPos(pos);
#ifdef _DEBUG_DEEP
	horizontal.AssertValidPathEdges(path);
	vertical.AssertValidPathEdges(path);
#endif
#endif

	CPointList& points = path->points;

	POSITION pointpos = pos;
	CPoint* point = &(points.GetNext(pos));
	POSITION npointpos = pos;
	CPoint* npoint = &(points.GetNext(pos));
	POSITION nnpointpos = pos;
	CPoint* nnpoint = &(points.GetNext(pos));
	POSITION nnnpointpos = pos;
	pos = pointpos;
	points.GetPrev(pos);
	POSITION ppointpos = pos;
	CPoint* ppoint = &(points.GetPrev(pos));
	POSITION pppointpos = pos;
	CPoint* pppoint = &(points.GetAt(pos));

	ASSERT( pppointpos != NULL && ppointpos != NULL && pointpos != NULL && npointpos != NULL && nnpointpos != NULL );
	ASSERT( pppoint != NULL && ppoint != NULL && point != NULL && npoint != NULL && nnpoint != NULL );

	EArDir dir = GetDir(*npoint - *point);
	ASSERT( IsRightAngle(dir) );
	int ishorizontal = IsHorizontal(dir);

	CPoint newpoint;
	GetPointCoord(newpoint, ishorizontal) = GetPointCoord(*npoint, ishorizontal);
	GetPointCoord(newpoint, !ishorizontal) = GetPointCoord(*ppoint, !ishorizontal);

	ASSERT( GetDir(newpoint - *ppoint) == dir );

	ASSERT( !IsLineClipBoxes(newpoint, *npoint) );
	ASSERT( !IsLineClipBoxes(newpoint, *ppoint) );

	CArEdgeList& hlist = GetEdgeList(ishorizontal);
	CArEdgeList& vlist = GetEdgeList(!ishorizontal);

	SArEdge* ppedge = hlist.GetEdge(pppoint, ppoint);
	SArEdge* pedge = vlist.GetEdge(ppoint, point);
	SArEdge* nedge = hlist.GetEdge(point, npoint);
	SArEdge* nnedge = vlist.GetEdge(npoint, nnpoint);

	ASSERT( ppedge != NULL && pedge != NULL && nedge != NULL && nnedge != NULL );

	vlist.Delete(pedge);
	hlist.Delete(nedge);

	points.RemoveAt(pointpos);
	points.RemoveAt(npointpos);
	points.SetAt(ppointpos, newpoint);

	ASSERT( ppedge->endpoint == ppoint && ppedge->endpoint_next == point );
	ppedge->endpoint_next = nnpoint;

	ASSERT( nnedge->startpoint == npoint && nnedge->startpoint_prev == point );
	nnedge->startpoint = ppoint;
	nnedge->startpoint_prev = pppoint;

	if( nnnpointpos != NULL )
	{
		SArEdge* nnnedge = hlist.GetEdge(nnpoint, &(points.GetAt(nnnpointpos)));
		ASSERT( nnnedge != NULL );
		ASSERT( nnnedge->startpoint_prev == npoint && nnnedge->startpoint == nnpoint );
		nnnedge->startpoint_prev = ppoint;
	}

	if( *nnpoint == newpoint )
		DeleteSamePointsAt(path, ppointpos);

#ifdef _DEBUG_DEEP
	ASSERT_VALID(path);
	horizontal.AssertValidPathEdges(path);
	vertical.AssertValidPathEdges(path);
#endif
}

void CArGraph::DeleteSamePointsAt(CArPath* path, POSITION pos)
{
#ifdef _DEBUG
	ASSERT( path->GetOwner() == this );
	ASSERT_VALID(path);
	ASSERT( path->IsConnected() );
	path->AssertValidPos(pos);
#endif

	CPointList& points = path->points;

	POSITION pointpos = pos;
	CPoint* point = &(points.GetNext(pos));
	POSITION npointpos = pos;
	CPoint* npoint = &(points.GetNext(pos));
	POSITION nnpointpos = pos;
	CPoint* nnpoint = &(points.GetNext(pos));
	POSITION nnnpointpos = pos;
	pos = pointpos;
	points.GetPrev(pos);
	POSITION ppointpos = pos;
	CPoint* ppoint = &(points.GetPrev(pos));
	POSITION pppointpos = pos;
	CPoint* pppoint = pos == NULL ? NULL : &(points.GetAt(pos));

	ASSERT( ppointpos != NULL && pointpos != NULL && npointpos != NULL && nnpointpos != NULL );
	ASSERT( ppoint != NULL && point != NULL && npoint != NULL && nnpoint != NULL );
	ASSERT( *point == *npoint && *point != *ppoint );

	EArDir dir = GetDir(*point - *ppoint);
	ASSERT( IsRightAngle(dir) );
	int ishorizontal = IsHorizontal(dir);

	CArEdgeList& hlist = GetEdgeList(ishorizontal);
	CArEdgeList& vlist = GetEdgeList(!ishorizontal);

	SArEdge* pedge = hlist.GetEdge(ppoint, point);
	SArEdge* nedge = vlist.GetEdge(point, npoint);
	SArEdge* nnedge = hlist.GetEdge(npoint, nnpoint);

	ASSERT( pedge != NULL && nedge != NULL && nnedge != NULL );

	vlist.Delete(pedge);
	hlist.Delete(nedge);

	points.RemoveAt(pointpos);
	points.RemoveAt(npointpos);

	if( pppointpos != NULL )
	{
		SArEdge* ppedge = vlist.GetEdge(pppoint, ppoint);
		ASSERT( ppedge != NULL && ppedge->endpoint == ppoint && ppedge->endpoint_next == point );
		ppedge->endpoint_next = nnpoint;
	}

	ASSERT( nnedge->startpoint == npoint && nnedge->startpoint_prev == point );
	nnedge->startpoint = ppoint;
	nnedge->startpoint_prev = pppoint;

	if( nnnpointpos != NULL )
	{
		SArEdge* nnnedge = vlist.GetEdge(nnpoint, &(points.GetAt(nnnpointpos)));
		ASSERT( nnnedge != NULL && nnnedge->startpoint_prev == npoint && nnnedge->startpoint == nnpoint );
		nnnedge->startpoint_prev = ppoint;
	}

#ifdef _DEBUG_DEEP
	ASSERT_VALID(path);
	horizontal.AssertValidPathEdges(path);
	vertical.AssertValidPathEdges(path);
#endif
}

int CArGraph::SimplifyPaths()
{
	int was = 0;

	POSITION pathpos = paths.GetHeadPosition();
	while( pathpos != NULL )
	{
		CArPath* path = paths.GetNext(pathpos);
		ASSERT_VALID(path);

		CPointList& points = path->points;
		POSITION pointpos = points.GetHeadPosition();

		while( pointpos != NULL )
		{
			if( CanDeleteTwoEdgesAt(path, pointpos) )
			{
				DeleteTwoEdgesAt(path, pointpos);
				was = 1;
				break;
			}
			points.GetNext(pointpos);
		}
	}

	return was;
}

#pragma warning(disable: 4700)
void CArGraph::CenterStairsInPathPoints(CArPath* path)
{
	ASSERT( path != NULL );
	ASSERT( !path->IsConnected() );
	CPointList& points = path->points;
	ASSERT( points.GetCount() >= 2 );

#ifdef _DEBUG
	path->AssertValidPoints();
#endif

	CPoint p1;
	CPoint p2;
	CPoint p3;
	CPoint p4;

	POSITION p1p = NULL;
	POSITION p2p = NULL;
	POSITION p3p = NULL;
	POSITION p4p = NULL;

	EArDir d12 = Dir_None;
	EArDir d23 = Dir_None;
	EArDir d34 = Dir_None;

	POSITION pos = points.GetHeadPosition();
	ASSERT( pos != NULL );

	p1p = pos;
	p1 = points.GetNext(pos);

	while( pos != NULL )
	{
		p4p = p3p;
		p3p = p2p;
		p2p = p1p;
		p1p = pos;

		p4 = p3;
		p3 = p2;
		p2 = p1;
		p1 = points.GetNext(pos);

		d34 = d23;
		d23 = d12;

		if( p2p != NULL )
		{
			d12 = GetDir(p2 - p1);
#ifdef _DEBUG
			ASSERT( IsRightAngle(d12) );
			if( p3p != NULL )
				ASSERT( AreInRightAngle(d12, d23) );
#endif
		}

		if( p4p != NULL && d12 == d34 )
		{
			ASSERT( p1p != NULL && p2p != NULL && p3p != NULL && p4p != NULL );

			CPoint np2 = p2;
			CPoint np3 = p3;
			int h = IsHorizontal(d12);

			long p4x = GetPointCoord(p4, h);
			long p3x = GetPointCoord(p3, h);
			long p1x = GetPointCoord(p1, h);

			if( p1x < p4x )
			{
				long t = p1x;
				p1x = p4x;
				p4x = t;
			}

			if( p4x < p3x && p3x < p1x )
			{
				long m = (p4x + p1x)/2;
				GetPointCoord(np2, h) = m;
				GetPointCoord(np3, h) = m;

				GetLimitsOfEdge(np2, np3, p4x, p1x);

				m = (p4x + p1x)/2;
				GetPointCoord(np2, h) = m;
				GetPointCoord(np3, h) = m;

				if( !IsLineClipBoxes(np2, np3) &&
					!IsLineClipBoxes(p1p == points.GetTailPosition() ? path->GetOutOfBoxEndPoint() : p1, np2) &&
					!IsLineClipBoxes(p4p == points.GetHeadPosition() ? path->GetOutOfBoxStartPoint() : p4, np3) )
				{
					p2 = np2;
					p3 = np3;
					points.SetAt(p2p, p2);
					points.SetAt(p3p, p3);
				}
			}
		}
	}

#ifdef _DEBUG
	path->AssertValidPoints();
#endif
}

void CArGraph::SimplifyPathPoints(CArPath* path)
{
	ASSERT( path != NULL );
	ASSERT( !path->IsConnected() );
	CPointList& points = path->points;
	ASSERT( points.GetCount() >= 2 );

#ifdef _DEBUG
	path->AssertValidPoints();
#endif

	CPoint p1;
	CPoint p2;
	CPoint p3;
	CPoint p4;
	CPoint p5;

	POSITION p1p = NULL;
	POSITION p2p = NULL;
	POSITION p3p = NULL;
	POSITION p4p = NULL;
	POSITION p5p = NULL;

	POSITION pos = points.GetHeadPosition();
	ASSERT( pos != NULL );

	p1p = pos;
	p1 = points.GetNext(pos);

	while( pos != NULL )
	{
		p5p = p4p;
		p4p = p3p;
		p3p = p2p;
		p2p = p1p;
		p1p = pos;

		p5 = p4;
		p4 = p3;
		p3 = p2;
		p2 = p1;
		p1 = points.GetNext(pos);

		if( p5p != NULL )
		{
			ASSERT( p1p != NULL && p2p != NULL && p3p != NULL && p4p != NULL && p5p != NULL );
			ASSERT( p1 != p2 && p2 != p3 && p3 != p4 && p4 != p5 );

			EArDir d = GetDir(p2 - p1);
			ASSERT( IsRightAngle(d) );
			int h = IsHorizontal(d);

			CPoint np3;
			GetPointCoord(np3, h) = GetPointCoord(p5, h);
			GetPointCoord(np3, !h) = GetPointCoord(p1, !h);

			if( !IsLineClipBoxes(p2, np3) && !IsLineClipBoxes(np3, p4) )
			{
				points.RemoveAt(p2p);
				points.RemoveAt(p4p);
				points.SetAt(p3p, np3);
				if( np3 == p1 )
					points.RemoveAt(p1p);
				if( np3 == p5 )
					points.RemoveAt(p5p);

				p1p = NULL;
				p2p = NULL;
				p3p = NULL;
				p4p = NULL;

				pos = points.GetHeadPosition();
			}
		}
	}

#ifdef _DEBUG
	path->AssertValidPoints();
#endif
}
#pragma warning(default: 4700)

void CArGraph::ConnectAllDisconnectedPaths()
{
	POSITION pos = paths.GetHeadPosition();
	while( pos != 0 )
	{
		CArPath* path = paths.GetNext(pos);

		if( !path->IsConnected() )
			Connect(path);
	}
}

// --- SelfPoints

void CArGraph::CalculateSelfPoints()
{
	selfpoints[0].x = ED_MINCOORD;
	selfpoints[0].y = ED_MINCOORD;

	selfpoints[1].x = ED_MAXCOORD;
	selfpoints[1].y = ED_MINCOORD;

	selfpoints[2].x = ED_MAXCOORD;
	selfpoints[2].y = ED_MAXCOORD;

	selfpoints[3].x = ED_MINCOORD;
	selfpoints[3].y = ED_MAXCOORD;
}

// --- Serialize

IMPLEMENT_SERIAL(CArGraph, CObject, 0);

void CArGraph::Serialize(CArchive& ar)
{
	if( ar.IsLoading() )
	{
		ASSERT( IsEmpty() );
		POSITION pos;

		boxes.Serialize(ar);
		pos = boxes.GetHeadPosition();
		while( pos != NULL )
			boxes.GetNext(pos)->SetOwner(this);
		
		paths.Serialize(ar);
		pos = paths.GetHeadPosition();
		while( pos != NULL )
			paths.GetNext(pos)->SetOwner(this);

		LocateRefs();
		ClearStoredRefs();
		AddAllEdges();
	}
	else
	{
		boxes.Serialize(ar);
		paths.Serialize(ar);
	}
}

CArPort* CArGraph::ResolveStoredRef(CArPort* stored_ref)
{
	POSITION pos = boxes.GetHeadPosition();
	while( pos != NULL )
	{
		CArPort* ptr = boxes.GetNext(pos)->ResolveStoredRef(stored_ref);
		if( ptr != NULL )
			return ptr;
	}

	return NULL;
}

void CArGraph::LocateRefs()
{
	POSITION pos = paths.GetHeadPosition();
	while( pos != NULL )
		paths.GetNext(pos)->LocateRefs();
}

void CArGraph::ClearStoredRefs()
{
	POSITION pos = boxes.GetHeadPosition();
	while( pos != NULL )
		boxes.GetNext(pos)->ClearStoredRefs();
}

// --- Debug

#ifdef _DEBUG

void CArGraph::AssertValid() const
{
	POSITION pos;

	pos = boxes.GetHeadPosition();
	while( pos != NULL )
	{
		CArBox* box = boxes.GetNext(pos);
		AssertValidBox(box);
	}

	pos = paths.GetHeadPosition();
	while( pos != NULL )
	{
		CArPath* path = paths.GetNext(pos);
		AssertValidPath(path);
	}
}

void CArGraph::AssertValidBox(CArBox* box) const
{
	ASSERT_VALID(box);
	ASSERT( box->owner == this );
	ASSERT( boxes.Find(box) != NULL );
}

void CArGraph::AssertValidPath(CArPath* path) const
{
	ASSERT_VALID(path);
	ASSERT( path->owner == this );
	ASSERT( paths.Find(path) != NULL );

	CArPort* port = path->GetStartPort();
	ASSERT_VALID(port);
	ASSERT( port->GetOwner()->GetOwner() == this );
	port->GetOwner()->AssertValidPort(port);
	if( path->IsConnected() )
		port->AssertValidStartEndPoint(path->points.GetHead(), Dir_None, 1);

	port = path->GetEndPort();
	ASSERT_VALID(port);
	ASSERT( port->GetOwner()->GetOwner() == this );
	port->GetOwner()->AssertValidPort(port);
	if( path->IsConnected() )
		port->AssertValidStartEndPoint(path->points.GetTail(), Dir_None, 0);

	if( !path->IsConnected() )
		ASSERT( path->HasNoPoint() );

	path->AssertValidPoints();

	CPointList& points = path->points;
	if( !points.IsEmpty() )
	{
		ASSERT( points.GetCount() >= 2 );
		POSITION pos = points.GetHeadPosition();
		ASSERT( pos != NULL );

		ASSERT( IsPointInBox(points.GetNext(pos)) );

		while( pos != NULL )
		{
			CPoint p = points.GetNext(pos);
			if( pos != NULL )
				ASSERT( !IsPointInBox(p) );
			else
				ASSERT( IsPointInBox(p) );
		}
	}
}

#endif
