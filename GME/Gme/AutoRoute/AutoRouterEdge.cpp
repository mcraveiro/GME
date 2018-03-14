
#include "stdafx.h"
#include "afxcoll.h"
#include "afxtempl.h"
#include "float.h"

#include <map>

#include "AutoRouterGraph.h"
#include "AutoRouterEdge.h"
#include "AutoRouterPath.h"
#include "AutoRouterBox.h"
#include "AutoRouterPort.h"

#define EDLS_S (ED_SMALLGAP)
#define EDLS_R (ED_SMALLGAP+1)
#define EDLS_D (ED_MAXCOORD-ED_MINCOORD)

/*
	In this file every comments refer to the horizontal case, that is, each	edge is
	horizontal.
*/

/*
	Every CAutoRouterEdge belongs to an edge of a CAutoRouterPath, CAutoRouterBox or CAutoRouterPort. This edge is
	Represented by a CAutoRouterPoint with its next point. The variable 'point' will refer
	to this CAutoRouterPoint.

	The coordinates of an edge are 'x1', 'x2' and 'y' where x1/x2 is the x-coordinate
	of the left/right point, and y is the common y-coordinate of the points.

	The edges are ordered according to their y-coordinates. The first edge has
	the least y-coordinate (topmost), and its pointer is in 'order_first'.
	We use the 'order' prefix in the variable names to refer to this order.

	We will walk from top to bottom (from the 'order_first' along the 'order_next').
	We keep track a "section" of some edges. If we have an infinite horizontal line,
	then the section consists of those edges that are above the line and not blocked
	by another edge which is closer to the line. Each edge in the section has
	a viewable portion from the line (the not blocked portion). The coordinates
	of this portion are 'section_x1' and 'section_x2'. We have an order of the edges
	belonging to the current section. The 'section_first' refers to the leftmost
	edge in the section, while the 'section_next' to the next from left to right.

	We say that the CAutoRouterEdge E1 "precede" the CAutoRouterEdge E2 if there is no other CAutoRouterEdge which
	totally	blocks S1 from S2. So a section consists of the preceding edges of an 
	infinite edge. We say that E1 is "adjacent" to E2, if E1 is the nearest edge
	to E2 which precede it. Clearly, every edge has at most one adjacent precedence.

	The edges of any CAutoRouterBox or CAutoRouterPort are fixed. We will continually fix the edges
	of the CAutoRouterPaths. But first we need some definition.

	We call a set of edges as a "block" if the topmost (first) and bottommost (last)
	edges of it are fixed while the edges between them are not. Furthermore, every
	edge is adjacent to	the next one in the order. Every edge in the block has an
	"index". The index of the first one (topmost) is 0, of the second is 1, and so on.
	We call the index of the last edge (# of edges - 1) as the index of the entire box.
	The "depth" of a block is the difference of the y-coordinates of the first and last
	edges of it. The "goal gap" of the block is the quotient of the depth and index
	of the block. If the difference of the y-coordinates of the adjacent edges in
	the block are all equal to the goal gap, then we say that the block is evenly
	distributed.

	So we search the block which has minimal goal gap. Then if it is not evenly
	distributed, then we shift the not fixed edges to the desired position. It is
	not hard to see	that if the block has minimal goal gap (among the all
	possibilities of blocks), then in this way we do not move any edges into boxes.
	Finally, we set the (inner) edges of the block to be fixed (except the topmost and 
	bottommost edges, since they are already fixed). And we again begin the search.
	If every edge is fixed, then we have finished. This is the basic idea. We will
	refine this algorithm.

	The variables related to the blocks are prefixed by 'block'. Note that the 
	variables of an edge are refer to that block in which this edge is inner! The
	'block_oldgap' is the goal gap of the block when it was last evenly distributed.

	The variables 'canstart' and 'canend' means that this egde can start and/or end
	a block. The top edge of a box only canend, while a fixed edge of a path can both
	start and end of a block.

*/


// --------------------------- CAutoRouterEdge

CAutoRouterEdge::CAutoRouterEdge()
{
	this->owner				= NULL;

	this->startpoint_prev	= NULL;
	this->startpoint		= NULL;
	this->endpoint			= NULL;
	this->endpoint_next		= NULL;

	this->position_y		= 0;
	this->position_x1		= 0;
	this->position_x2		= 0;
	this->bracket_closing	= 0;
	this->bracket_opening	= 0;

	this->order_prev		= NULL;
	this->order_next		= NULL;

	this->edge_fixed		= 0;
	this->edge_customFixed	= 0;
	this->edge_canpassed	= 0;

	this->position_y		= 0;

	this->block_prev		= NULL;
	this->block_next		= NULL;
	this->block_trace		= NULL;

	this->closest_prev		= NULL;
	this->closest_next		= NULL;
}

CAutoRouterEdge::~CAutoRouterEdge()
{
}

CObject* CAutoRouterEdge::GetOwner(void) const
{
	return owner;
}

void CAutoRouterEdge::SetOwner(CObject* owner)
{
	this->owner = owner;
}

CPoint CAutoRouterEdge::GetStartPointPrev(void) const
{
	return startpoint_prev == NULL ? emptyPoint : *startpoint_prev;
}

bool CAutoRouterEdge::IsStartPointPrevNull(void) const
{
	return startpoint_prev == NULL;
}

void CAutoRouterEdge::SetStartPointPrev(CPoint* point)
{
	startpoint_prev = point;
}

CPoint CAutoRouterEdge::GetStartPoint(void) const
{
	return startpoint == NULL ? emptyPoint : *startpoint;
}

bool CAutoRouterEdge::IsSameStartPointByPointer(const CPoint* point) const
{
	return startpoint == point;
}

bool CAutoRouterEdge::IsStartPointNull(void) const
{
	return startpoint == NULL;
}

void CAutoRouterEdge::SetStartPoint(CPoint* point)
{
	startpoint = point;
	RecalculateDirection();
}

void CAutoRouterEdge::SetStartPointX(int x)
{
	startpoint->x = x;
}

void CAutoRouterEdge::SetStartPointY(int y)
{
	startpoint->y = y;
}

CPoint CAutoRouterEdge::GetEndPoint(void) const
{
	return endpoint == NULL ? emptyPoint : *endpoint;
}

bool CAutoRouterEdge::IsEndPointNull(void) const
{
	return endpoint == NULL;
}

void CAutoRouterEdge::SetEndPoint(CPoint* point)
{
	endpoint = point;
	RecalculateDirection();
}

void CAutoRouterEdge::SetStartAndEndPoint(CPoint* startPoint, CPoint* endPoint)
{
	startpoint = startPoint;
	endpoint = endPoint;
	RecalculateDirection();
}

void CAutoRouterEdge::SetEndPointX(int x)
{
	endpoint->x = x;
}

void CAutoRouterEdge::SetEndPointY(int y)
{
	endpoint->y = y;
}

CPoint CAutoRouterEdge::GetEndPointNext(void) const
{
	return endpoint_next == NULL ? emptyPoint : *endpoint_next;
}

bool CAutoRouterEdge::IsEndPointNextNull(void) const
{
	return endpoint_next == NULL;
}

void CAutoRouterEdge::SetEndPointNext(CPoint* point)
{
	endpoint_next = point;
}

float CAutoRouterEdge::GetPositionY(void) const
{
	return position_y;
}

void CAutoRouterEdge::SetPositionY(float y)
{
	position_y = y;
}

void CAutoRouterEdge::AddToPositionY(float dy)
{
	position_y += dy;
}

int CAutoRouterEdge::GetPositionX1(void) const
{
	return position_x1;
}

void CAutoRouterEdge::SetPositionX1(int x1)
{
	position_x1 = x1;
}

int CAutoRouterEdge::GetPositionX2(void) const
{
	return position_x2;
}

void CAutoRouterEdge::SetPositionX2(int x2)
{
	position_x2 = x2;
}

bool CAutoRouterEdge::GetBracketClosing(void) const
{
	return bracket_closing;
}

void CAutoRouterEdge::SetBracketClosing(bool b)
{
	bracket_closing = b;
}

bool CAutoRouterEdge::GetBracketOpening(void) const
{
	return bracket_opening;
}

void CAutoRouterEdge::SetBracketOpening(bool b)
{
	bracket_opening = b;
}

CAutoRouterEdge* CAutoRouterEdge::GetOrderNext(void)
{
	return order_next;
}

void CAutoRouterEdge::SetOrderNext(CAutoRouterEdge* ordernext)
{
	order_next = ordernext;
}

CAutoRouterEdge* CAutoRouterEdge::GetOrderPrev(void)
{
	return order_prev;
}

void CAutoRouterEdge::SetOrderPrev(CAutoRouterEdge* orderprev)
{
	order_prev = orderprev;
}

long CAutoRouterEdge::GetSectionX1(void)
{
	return section_x1;
}

void CAutoRouterEdge::SetSectionX1(long x1)
{
	section_x1 = x1;
}

long CAutoRouterEdge::GetSectionX2(void)
{
	return section_x2;
}

void CAutoRouterEdge::SetSectionX2(long x2)
{
	section_x2 = x2;
}

CAutoRouterEdge* CAutoRouterEdge::GetSectionNext(void)
{
	return section_next;
}

CAutoRouterEdge** CAutoRouterEdge::GetSectionNextPtr(void)
{
	return &section_next;
}

void CAutoRouterEdge::SetSectionNext(CAutoRouterEdge* sectionnext)
{
	section_next = sectionnext;
}

CAutoRouterEdge* CAutoRouterEdge::GetSectionDown(void)
{
	return section_down;
}

CAutoRouterEdge** CAutoRouterEdge::GetSectionDownPtr(void)
{
	return &section_down;
}

void CAutoRouterEdge::SetSectionDown(CAutoRouterEdge* sectiondown)
{
	section_down = sectiondown;
}

bool CAutoRouterEdge::GetEdgeFixed(void)
{
	return edge_fixed;
}

void CAutoRouterEdge::SetEdgeFixed(bool ef)
{
	edge_fixed = ef;
}

bool CAutoRouterEdge::GetEdgeCustomFixed(void)
{
	return edge_customFixed;
}

void CAutoRouterEdge::SetEdgeCustomFixed(bool ecf)
{
	edge_customFixed = ecf;
}

bool CAutoRouterEdge::GetEdgeCanpassed(void)
{
	return edge_canpassed;
}

void CAutoRouterEdge::SetEdgeCanpassed(bool ecp)
{
	edge_canpassed = ecp;
}

RoutingDirection CAutoRouterEdge::GetDirection(void)
{
	return edge_direction;
}

void CAutoRouterEdge::SetDirection(RoutingDirection dir)
{
	edge_direction = dir;
}

void CAutoRouterEdge::RecalculateDirection(void)
{
	ASSERT(startpoint != NULL && endpoint != NULL);
	edge_direction = GetDir(*endpoint - *startpoint);
}

CAutoRouterEdge* CAutoRouterEdge::GetBlockPrev(void)
{
	return block_prev;
}

void CAutoRouterEdge::SetBlockPrev(CAutoRouterEdge* bp)
{
	block_prev = bp;
}

CAutoRouterEdge* CAutoRouterEdge::GetBlockNext(void)
{
	return block_next;
}

void CAutoRouterEdge::SetBlockNext(CAutoRouterEdge* bn)
{
	block_next = bn;
}

CAutoRouterEdge* CAutoRouterEdge::GetBlockTrace(void)
{
	return block_trace;
}

void CAutoRouterEdge::SetBlockTrace(CAutoRouterEdge* bt)
{
	block_trace = bt;
}

CAutoRouterEdge* CAutoRouterEdge::GetClosestPrev(void)
{
	return closest_prev;
}

void CAutoRouterEdge::SetClosestPrev(CAutoRouterEdge* cp)
{
	closest_prev = cp;
}

CAutoRouterEdge* CAutoRouterEdge::GetClosestNext(void)
{
	return closest_next;
}

void CAutoRouterEdge::SetClosestNext(CAutoRouterEdge* cn)
{
	closest_next = cn;
}


// --------------------------- CAutoRouterEdgeList


CAutoRouterEdgeList::CAutoRouterEdgeList(bool h)
{
	owner = NULL;

	ishorizontal = (h != 0);

	Init_Order();
	Init_Section();
}

CAutoRouterEdgeList::~CAutoRouterEdgeList()
{
	Check_Order();
	Check_Section();
}

void CAutoRouterEdgeList::SetOwner(CAutoRouterGraph* o)
{
	owner = o;
}

// --- Edges

typedef std::pair<long,long> Long_Pair;

bool CAutoRouterEdgeList::AddEdges(CAutoRouterPath* path)
{
	ASSERT_VALID(path);
	ASSERT( path->GetOwner() == owner );
	ASSERT_VALID( path->GetStartPort() );
	ASSERT_VALID( path->GetEndPort() );

	bool isPathAutoRouted = path->IsAutoRouted();
	// Apply custom edge modifications - step 2, part 1
	// (Step 1: Move the desired edges
	//  Step 2: Fix the desired edges)
	bool hasCustomEdge = false;
	std::map<long,long> customizedIndexes;	// convert array to a map for easier lookup
	std::vector<int> indexes;
	path->GetCustomizedEdgeIndexes(indexes);

	if (isPathAutoRouted) {
		std::vector<int>::iterator ii = indexes.begin();
		while (ii != indexes.end()) {
			hasCustomEdge = true;
			customizedIndexes.insert(Long_Pair(*ii, 0));
			++ii;
		}
	} else {
		hasCustomEdge = true;
	}

	std::map<long,long>::const_iterator indIter;
	CPointListPath& pointList = path->GetPointList();
	long currEdgeIndex = pointList.GetSize() - 2;

	CPoint* startpoint = NULL;
	CPoint* endpoint = NULL;

	POSITION pos = pointList.GetTailEdgePtrs(startpoint, endpoint);
	while( pos != NULL )
	{
		RoutingDirection dir = GetDir(*endpoint - *startpoint);

		bool skipEdge = false;
		if (dir == Dir_None)
			skipEdge = true;
		bool isMovable = path->IsMoveable();
		if( !isMovable && dir != Dir_Skew)
		{
			bool goodAngle = IsRightAngle(dir);
			ASSERT( goodAngle );
			if (!goodAngle)
				skipEdge = true;
		}

		if( !skipEdge &&
			(//dir == Dir_Skew || -> do not add skew lines now, auto routing cannot do anything with it
			 IsRightAngle(dir) && IsHorizontal(dir) == ishorizontal) )
		{
			CAutoRouterEdge* edge = new CAutoRouterEdge();

			edge->SetOwner(path);

			edge->SetStartAndEndPoint(startpoint, endpoint);
			edge->SetStartPointPrev(pointList.GetPointBeforeEdge(pos));
			edge->SetEndPointNext(pointList.GetPointAfterEdge(pos));

			// Apply custom edge modifications - step 2, part 2
			if (hasCustomEdge)
			{
				bool isEdgeCustomFixed = false;
				if (isPathAutoRouted) {
					indIter = customizedIndexes.find(currEdgeIndex);
					isEdgeCustomFixed = (indIter != customizedIndexes.end());
				} else {
					isEdgeCustomFixed = true;
				}
				edge->SetEdgeCustomFixed(isEdgeCustomFixed);
			}
			else
			{
				edge->SetEdgeCustomFixed(dir == Dir_Skew);
			}

			CAutoRouterPort* startPort = path->GetStartPort();
			ASSERT(startPort != NULL);
			bool isStartPortConnectToCenter = startPort->IsConnectToCenter();

			CAutoRouterPort* endPort = path->GetEndPort();
			ASSERT(endPort != NULL);
			bool isEndPortConnectToCenter = endPort->IsConnectToCenter();
			bool isPathFixed = path->IsFixed();
			edge->SetEdgeFixed(edge->GetEdgeCustomFixed() || isPathFixed || 
				(edge->IsStartPointPrevNull() && isStartPortConnectToCenter) ||
				(edge->IsEndPointNextNull() && isEndPortConnectToCenter));

			if (dir != Dir_Skew)
			{
				Position_LoadY(edge);
				Position_LoadB(edge);
			}
			else
			{
				edge->SetPositionY(0.0);
				edge->SetBracketOpening(false);
				edge->SetBracketClosing(false);
			}
			Insert(edge);
		}

		pointList.GetPrevEdgePtrs(pos, startpoint, endpoint);
		currEdgeIndex--;
	}

	return true;
}

CAutoRouterEdge* CAutoRouterEdgeList::GetEdge(CAutoRouterPath* path, const CPoint& startpoint, const CPoint& endpoint) const
{
	CAutoRouterEdge* edge = order_first;
	while( edge != NULL )
	{
		if( edge->GetStartPoint() == startpoint )
		{
			if( edge->GetOwner() == path )
				break;
		}

		edge = edge->GetOrderNext();
	}

	ASSERT( edge != NULL );
	return edge;
}

CAutoRouterEdge* CAutoRouterEdgeList::GetEdgeByPointer(const CPoint* startpoint, const CPoint* endpoint) const
{
	CAutoRouterEdge* edge = order_first;
	while( edge != NULL )
	{
		if( edge->IsSameStartPointByPointer(startpoint) )
		{
			break;
		}

		edge = edge->GetOrderNext();
	}

	ASSERT( edge != NULL );
	return edge;
}

CAutoRouterEdge* CAutoRouterEdgeList::GetEdgeAt(const CPoint& point, int nearness) const
{
	CAutoRouterEdge* edge = order_first;
	while( edge )
	{
		if( IsPointNearLine(point, edge->GetStartPoint(), edge->GetEndPoint(), nearness) )
			return edge;

		edge = edge->GetOrderNext();
	}

	return NULL;
}

#ifdef _DEBUG

void CAutoRouterEdgeList::AssertValidPathEdges(CAutoRouterPath* path, CPointListPath& points) const
{
	ASSERT( path != NULL );
	CAutoRouterGraph* ownerGraph = path->GetOwner();
	ASSERT( ownerGraph == owner );

	CPoint* startpoint = NULL;
	CPoint* endpoint = NULL;

	POSITION pos = points.GetTailEdgePtrs(startpoint, endpoint);
	while( pos != NULL )
	{
		RoutingDirection dir = GetDir(*endpoint - *startpoint);

		if( path->IsMoveable() )
			ASSERT( IsRightAngle(dir) );

		if( IsRightAngle(dir) && IsHorizontal(dir) == ishorizontal )
		{
			CAutoRouterEdge* edge = GetEdgeByPointer(startpoint, endpoint);
			ASSERT( edge != NULL );

			ASSERT( edge->GetOwner() == path );

			ASSERT( edge->GetStartPoint() == *startpoint );
			ASSERT( edge->GetEndPoint() == *endpoint );
			if (edge->IsStartPointPrevNull())
				ASSERT( points.GetPointBeforeEdge(pos) == NULL );
			else
				ASSERT( edge->GetStartPointPrev() == *(points.GetPointBeforeEdge(pos)) );
			if (edge->IsEndPointNextNull())
				ASSERT( points.GetPointAfterEdge(pos) == NULL );
			else
				ASSERT( edge->GetEndPointNext() == *(points.GetPointAfterEdge(pos)) );
		}

		points.GetPrevEdgePtrs(pos, startpoint, endpoint);
	}

	CAutoRouterEdge* edge = order_first;
	while( edge != NULL )
	{
		if( edge->GetOwner() == path )
		{
			POSITION pos = points.GetEdgePosForStartPoint(edge->GetStartPoint());
			ASSERT( pos != NULL );
			ASSERT( *(points.GetStartPoint(pos)) == edge->GetStartPoint() );
			ASSERT( *(points.GetEndPoint(pos)) == edge->GetEndPoint() );
			if (edge->IsStartPointPrevNull())
				ASSERT( points.GetPointBeforeEdge(pos) == NULL );
			else
				ASSERT( *(points.GetPointBeforeEdge(pos)) == edge->GetStartPointPrev() );
			if (edge->IsEndPointNextNull())
				ASSERT( points.GetPointAfterEdge(pos) == NULL );
			else
				ASSERT( *(points.GetPointAfterEdge(pos)) == edge->GetEndPointNext() );
		}

		edge = edge->GetOrderNext();
	}
}

void CAutoRouterEdgeList::DumpEdges(const CString& headMsg)
{
#ifdef _DEBUG_DEEP
	TRACE0(headMsg + "\n");
	CAutoRouterEdge* edge = order_first;
	int i = 0;
	while( edge != NULL )
	{
		TRACE1("\t %ld.: ", i);
		TRACE2("sp %ld, %ld ", edge->IsStartPointNull() ? -1 : edge->GetStartPoint().x, edge->IsStartPointNull() ? -1 : edge->GetStartPoint().y);
		TRACE2("ep %ld, %ld ", edge->IsEndPointNull() ? -1 : edge->GetEndPoint().x, edge->IsEndPointNull() ? -1 : edge->GetEndPoint().y);
		TRACE2("spp %ld, %ld ", edge->IsStartPointPrevNull() ? -1 : edge->GetStartPointPrev().x, edge->IsStartPointPrevNull() ? -1 : edge->GetStartPointPrev().y);
		TRACE2("epn %ld, %ld\n", edge->IsEndPointNextNull() ? -1 : edge->GetEndPointNext().x, edge->IsEndPointNextNull() ? -1 : edge->GetEndPointNext().y);
		TRACE0("\t\t");
		TRACE3("py %f px1 %ld px2 %ld ", edge->GetPositionY(), edge->GetPositionX1(), edge->GetPositionX2());
		TRACE2("bc %ld bo %ld ", edge->GetBracketClosing(), edge->GetBracketOpening());
		TRACE2("sx1 %ld sx2 %ld ", edge->GetSectionX1(), edge->GetSectionX2());
		TRACE2("f %d cp %d\n", edge->GetEdgeFixed(), edge->GetEdgeCanpassed());

		edge = edge->GetOrderNext();
		i++;
	}
#endif
}

#endif

void CAutoRouterEdgeList::AddEdges(CAutoRouterPort* port)
{
	ASSERT_VALID(port);
	ASSERT( port->GetOwner()->GetOwner() == owner );

	if (port->IsConnectToCenter() || port->GetOwner()->IsAtomic())
		return;

	CPoint* selfpoints = port->GetSelfPoints();

	for(int i = 0; i < 4; i++)
	{
		CPoint* startpoint_prev = &(selfpoints[(i + 3) % 4]);
		CPoint* startpoint = &(selfpoints[i]);
		CPoint* endpoint = &(selfpoints[(i + 1) % 4]);
		CPoint* endpoint_next = &(selfpoints[(i + 2) % 4]);

		RoutingDirection dir = GetDir(*endpoint - *startpoint);
		ASSERT( IsRightAngle(dir) );

		bool canHaveStartEndPointHorizontal = port->CanHaveStartEndPointHorizontal(ishorizontal);
		if( IsHorizontal(dir) == ishorizontal && canHaveStartEndPointHorizontal )
		{
			CAutoRouterEdge* edge = new CAutoRouterEdge();

			edge->SetOwner(port);

			edge->SetStartAndEndPoint(startpoint, endpoint);
			edge->SetStartPointPrev(startpoint_prev);
			edge->SetEndPointNext(endpoint_next);

			edge->SetEdgeFixed(true);

			Position_LoadY(edge);
			Position_LoadB(edge);

			if( edge->GetBracketClosing() )
				edge->AddToPositionY(0.999F);

			Insert(edge);
		}
	}
}

void CAutoRouterEdgeList::AddEdges(CAutoRouterBox* box)
{
	ASSERT_VALID(box);
	ASSERT( box->GetOwner() == owner );

	CPoint* selfpoints = box->GetSelfPoints();

	for(int i = 0; i < 4; i++)
	{
		CPoint* startpoint_prev = &(selfpoints[(i + 3) % 4]);
		CPoint* startpoint = &(selfpoints[i]);
		CPoint* endpoint = &(selfpoints[(i + 1) % 4]);
		CPoint* endpoint_next = &(selfpoints[(i + 2) % 4]);

		RoutingDirection dir = GetDir(*endpoint - *startpoint);
		ASSERT( IsRightAngle(dir) );

		if( IsHorizontal(dir) == ishorizontal )
		{
			CAutoRouterEdge* edge = new CAutoRouterEdge();

			edge->SetOwner(box);

			edge->SetStartAndEndPoint(startpoint, endpoint);
			edge->SetStartPointPrev(startpoint_prev);
			edge->SetEndPointNext(endpoint_next);

			edge->SetEdgeFixed(true);

			Position_LoadY(edge);
			Position_LoadB(edge);

			if( edge->GetBracketClosing() )
				edge->AddToPositionY(0.999F);
		
			Insert(edge);
		}
	}
}

void CAutoRouterEdgeList::AddEdges(CAutoRouterGraph* graph)
{
	ASSERT_VALID(graph);
	ASSERT( graph == owner );

	CPoint* selfpoints = graph->GetSelfPoints();

	for(int i = 0; i < 4; i++)
	{
		CPoint* startpoint_prev = &(selfpoints[(i + 3) % 4]);
		CPoint* startpoint = &(selfpoints[i]);
		CPoint* endpoint = &(selfpoints[(i + 1) % 4]);
		CPoint* endpoint_next = &(selfpoints[(i + 2) % 4]);

		RoutingDirection dir = GetDir(*endpoint - *startpoint);
		ASSERT( IsRightAngle(dir) );

		if( IsHorizontal(dir) == ishorizontal )
		{
			CAutoRouterEdge* edge = new CAutoRouterEdge();

			edge->SetOwner(graph);

			edge->SetStartAndEndPoint(startpoint, endpoint);
			edge->SetStartPointPrev(startpoint_prev);
			edge->SetEndPointNext(endpoint_next);

			edge->SetEdgeFixed(true);

			Position_LoadY(edge);
			Insert(edge);
		}
	}
}

void CAutoRouterEdgeList::DeleteEdges(CObject* object)
{
	CAutoRouterEdge* edge = order_first;
	while( edge != NULL )
	{
		if( edge->GetOwner() == object )
		{
			CAutoRouterEdge* next = edge->GetOrderNext();
			Delete(edge);
			edge = next;
		}
		else
			edge = edge->GetOrderNext();
	}
}

void CAutoRouterEdgeList::DeleteAllEdges()
{
	while( order_first )
		Delete(order_first);
}

bool CAutoRouterEdgeList::IsEmpty() const
{
	return order_first == NULL;
}

// --- Position

long CAutoRouterEdgeList::Position_GetRealY(const CAutoRouterEdge* edge) const
{
	if( ishorizontal )
	{
		ASSERT( edge->GetStartPoint().y == edge->GetEndPoint().y );
		return edge->GetStartPoint().y;
	}

	ASSERT( edge->GetStartPoint().x == edge->GetEndPoint().x );
	return edge->GetStartPoint().x;
}

void CAutoRouterEdgeList::Position_SetRealY(CAutoRouterEdge* edge, long y) const
{
	ASSERT( edge != NULL && !edge->IsStartPointNull() && !edge->IsEndPointNull() );

	if( ishorizontal )
	{
		ASSERT( edge->GetStartPoint().y == edge->GetEndPoint().y );
		edge->SetStartPointY(y);
		edge->SetEndPointY(y);
	}
	else
	{
		ASSERT( edge->GetStartPoint().x == edge->GetEndPoint().x );
		edge->SetStartPointX(y);
		edge->SetEndPointX(y);
	}
}

void CAutoRouterEdgeList::Position_GetRealX(const CAutoRouterEdge* edge, long& x1, long& x2) const
{
	ASSERT( edge != NULL && !edge->IsStartPointNull() && !edge->IsEndPointNull() );

	if( ishorizontal )
	{
		ASSERT( edge->GetStartPoint().y == edge->GetEndPoint().y );
		if( edge->GetStartPoint().x < edge->GetEndPoint().x )
		{
			x1 = edge->GetStartPoint().x;
			x2 = edge->GetEndPoint().x;
		}
		else
		{
			x1 = edge->GetEndPoint().x;
			x2 = edge->GetStartPoint().x;
		}
	}
	else
	{
		ASSERT( edge->GetStartPoint().x == edge->GetEndPoint().x );
		if( edge->GetStartPoint().y < edge->GetEndPoint().y )
		{
			x1 = edge->GetStartPoint().y;
			x2 = edge->GetEndPoint().y;
		}
		else
		{
			x1 = edge->GetEndPoint().y;
			x2 = edge->GetStartPoint().y;
		}
	}
}

void CAutoRouterEdgeList::Position_GetRealO(const CAutoRouterEdge* edge, long& o1, long& o2) const
{
	ASSERT( edge != NULL && !edge->IsStartPointNull() && !edge->IsEndPointNull() );

	if( ishorizontal )
	{
		ASSERT( edge->GetStartPoint().y == edge->GetEndPoint().y );
		if( edge->GetStartPoint().x < edge->GetEndPoint().x )
		{
			o1 = edge->IsStartPointPrevNull() ? 0 : edge->GetStartPointPrev().y - edge->GetStartPoint().y;
			o2 = edge->IsEndPointNextNull() ? 0 : edge->GetEndPointNext().y - edge->GetEndPoint().y;
		}
		else
		{
			o1 = edge->IsEndPointNextNull() ? 0 : edge->GetEndPointNext().y - edge->GetEndPoint().y;
			o2 = edge->IsStartPointPrevNull() ? 0 : edge->GetStartPointPrev().y - edge->GetStartPoint().y;
		}
	}
	else
	{
		ASSERT( edge->GetStartPoint().x == edge->GetEndPoint().x );
		if( edge->GetStartPoint().y < edge->GetEndPoint().y )
		{
			o1 = edge->IsStartPointPrevNull() ? 0 : edge->GetStartPointPrev().x - edge->GetStartPoint().x;
			o2 = edge->IsEndPointNextNull() ? 0 : edge->GetEndPointNext().x - edge->GetEndPoint().x;
		}
		else
		{
			o1 = edge->IsEndPointNextNull() ? 0 : edge->GetEndPointNext().x - edge->GetEndPoint().x;
			o2 = edge->IsStartPointPrevNull() ? 0 : edge->GetStartPointPrev().x - edge->GetStartPoint().x;
		}
	}
}

void CAutoRouterEdgeList::Position_LoadY(CAutoRouterEdge* edge) const
{
	ASSERT( edge != NULL && edge->GetOrderNext() == NULL && edge->GetOrderPrev() == NULL );

	edge->SetPositionY((float) Position_GetRealY(edge));
}

void CAutoRouterEdgeList::Position_LoadB(CAutoRouterEdge* edge) const
{
	ASSERT( edge != NULL );

	edge->SetBracketOpening(!edge->GetEdgeFixed() && Bracket_IsOpening(edge));
	edge->SetBracketClosing(!edge->GetEdgeFixed() && Bracket_IsClosing(edge));
}

void CAutoRouterEdgeList::PositionAll_StoreY() const
{
	CAutoRouterEdge* edge = order_first;
	while( edge )
	{
		Position_SetRealY(edge, (long) edge->GetPositionY());

		edge = edge->GetOrderNext();
	}
}

void CAutoRouterEdgeList::PositionAll_LoadX() const
{
	CAutoRouterEdge* edge = order_first;
	while( edge )
	{
		long ex1, ex2;
		Position_GetRealX(edge, ex1, ex2);
		edge->SetPositionX1(ex1);
		edge->SetPositionX2(ex2);

		edge = edge->GetOrderNext();
	}
}

#ifdef _DEBUG

void CAutoRouterEdgeList::AssertValidPositions() const
{
	CAutoRouterEdge* edge = order_first;
	while( edge )
	{
		long y = Position_GetRealY(edge);
		ASSERT( edge->GetPositionY() - 1 <= y && y <= edge->GetPositionY() + 1 );

		if( edge->GetOrderPrev() )
		{
			ASSERT( edge->GetOrderPrev()->GetPositionY() <= edge->GetPositionY() );
			ASSERT( Position_GetRealY(edge->GetOrderPrev()) <= y );
		}

		if( edge->GetOrderNext() )
		{
			ASSERT( edge->GetPositionY() <= edge->GetOrderNext()->GetPositionY() );
			ASSERT( y <= Position_GetRealY(edge->GetOrderNext()) );
		}

		edge = edge->GetOrderNext();
	}
}

#endif

// --- Order

void CAutoRouterEdgeList::Init_Order()
{
	order_first = NULL;
	order_last = NULL;
}

void CAutoRouterEdgeList::Check_Order()
{
	ASSERT( order_first == NULL && order_last == NULL );
}

void CAutoRouterEdgeList::InsertBefore(CAutoRouterEdge* edge, CAutoRouterEdge* before)
{
	ASSERT( edge != NULL && before != NULL && edge != before );
	ASSERT( edge->GetOrderNext() == NULL && edge->GetOrderPrev() == NULL );

	edge->SetOrderPrev(before->GetOrderPrev());
	edge->SetOrderNext(before);

	if( before->GetOrderPrev() )
	{
		ASSERT( before->GetOrderPrev()->GetOrderNext() == before );
		before->GetOrderPrev()->SetOrderNext(edge);

		ASSERT( order_first != before );
	}
	else
	{
		ASSERT( order_first == before );
		order_first = edge;
	}

	before->SetOrderPrev(edge);
}

void CAutoRouterEdgeList::InsertAfter(CAutoRouterEdge* edge, CAutoRouterEdge* after)
{
	ASSERT( edge != NULL && after != NULL && edge != after );
	ASSERT( edge->GetOrderNext() == NULL && edge->GetOrderPrev() == NULL );

	edge->SetOrderNext(after->GetOrderNext());
	edge->SetOrderPrev(after);

	if( after->GetOrderNext() )
	{
		ASSERT( after->GetOrderNext()->GetOrderPrev() == after );
		after->GetOrderNext()->SetOrderPrev(edge);

		ASSERT( order_last != after );
	}
	else
	{
		ASSERT( order_last == after );
		order_last = edge;
	}

	after->SetOrderNext(edge);
}

void CAutoRouterEdgeList::InsertLast(CAutoRouterEdge* edge)
{
	ASSERT( edge != NULL );
	ASSERT( edge->GetOrderPrev() == NULL && edge->GetOrderNext() == NULL );

	edge->SetOrderPrev(order_last);

	if( order_last )
	{
		ASSERT( order_last->GetOrderNext() == NULL );
		ASSERT( order_first != NULL );

		order_last->SetOrderNext(edge);
		order_last = edge;
	}
	else
	{
		ASSERT( order_first == NULL );

		order_first = edge;
		order_last = edge;
	}
}

void CAutoRouterEdgeList::Insert(CAutoRouterEdge* edge)
{
	ASSERT( edge != NULL );
	ASSERT( edge->GetOrderPrev() == NULL && edge->GetOrderNext() == NULL );

	float y = edge->GetPositionY();
	ASSERT( ED_MINCOORD <= y && y <= ED_MAXCOORD );

	CAutoRouterEdge* insert = order_first;
	// FIXME: std::sort would be better than insertion sort
	while( insert && insert->GetPositionY() < y )
		insert = insert->GetOrderNext();

	if( insert )
		InsertBefore(edge, insert);
	else
		InsertLast(edge);
}

void CAutoRouterEdgeList::Remove(CAutoRouterEdge* edge)
{
	ASSERT( edge != NULL );

	if( order_first == edge )
		order_first = edge->GetOrderNext();

	if( edge->GetOrderNext() )
		edge->GetOrderNext()->SetOrderPrev(edge->GetOrderPrev());

	if( order_last == edge )
		order_last = edge->GetOrderPrev();

	if( edge->GetOrderPrev() )
		edge->GetOrderPrev()->SetOrderNext(edge->GetOrderNext());

	edge->SetOrderNext(NULL);
	edge->SetOrderPrev(NULL);
}

void CAutoRouterEdgeList::Delete(CAutoRouterEdge* edge)
{
	ASSERT( edge != NULL );

	Remove(edge);

	edge->SetOwner(NULL);

	delete edge;
}

CAutoRouterEdge* CAutoRouterEdgeList::SlideButNotPassEdges(CAutoRouterEdge* edge, float y)
{
	ASSERT( edge != NULL );
	ASSERT( ED_MINCOORD < y && y < ED_MAXCOORD );

	float oldy = edge->GetPositionY();
	ASSERT( ED_MINCOORD < oldy && oldy < ED_MAXCOORD );

	if( oldy == y )
		return NULL;

	long x1 = edge->GetPositionX1();
	long x2 = edge->GetPositionX2();
	CAutoRouterEdge* ret = NULL;

	CAutoRouterEdge* insert = edge;
	if( oldy < y )
	{
		while( insert->GetOrderNext() )
		{
			insert = insert->GetOrderNext();

			if( y < insert->GetPositionY() )
				break;

			if( !insert->GetEdgeCanpassed() && Intersect(x1, x2, insert->GetPositionX1(), insert->GetPositionX2() ) )
			{
				ret = insert;
				y = insert->GetPositionY();
				break;
			}
		}

		if( edge != insert && insert->GetOrderPrev() != edge )
		{
			Remove(edge);
			InsertBefore(edge, insert);
		}
	}
	else
	{
		while( insert->GetOrderPrev() )
		{
			insert = insert->GetOrderPrev();

			if( y > insert->GetPositionY() )
				break;

			if( !insert->GetEdgeCanpassed() && Intersect(x1, x2, insert->GetPositionX1(), insert->GetPositionX2() ) )
			{
				ret = insert;
				y = insert->GetPositionY();
				break;
			}
		}

		if( edge != insert && insert->GetOrderNext() != edge )
		{
			Remove(edge);
			InsertAfter(edge, insert);
		}

	}

#ifdef _DEBUG
	if( edge->GetOrderNext() )
		ASSERT( y <= edge->GetOrderNext()->GetPositionY() );

	if( edge->GetOrderPrev() )
		ASSERT( edge->GetOrderPrev()->GetPositionY() <= y );
#endif

	edge->SetPositionY(y);

	return ret;
}

#ifdef _DEBUG

void CAutoRouterEdgeList::AssertValidOrder() const
{
	ASSERT( order_first != NULL && order_last != NULL );
	ASSERT( order_first->GetOrderPrev() == NULL );
	ASSERT( order_last->GetOrderNext() == NULL );

	float y = ED_MINCOORD;

	TRACE("CAutoRouterEdgeList::AssertValidOrder (horizontal=%d) START\n", ishorizontal);

	CAutoRouterEdge* edge = order_first;
	while( edge != order_last )
	{
		TRACE("edge=%p, position_y=%f\n", edge, edge->GetPositionY());

		ASSERT( edge != NULL );
		ASSERT( y <= edge->GetPositionY() );
		ASSERT( edge->GetOrderNext()->GetOrderPrev() == edge );

		y = edge->GetPositionY();
		edge = edge->GetOrderNext();
	}

	TRACE("edge=%p, position_y=%f\n", edge, edge->GetPositionY());
	TRACE("CAutoRouterEdgeList::AssertValidOrder (horizontal=%d) END\n", ishorizontal);

	ASSERT( y <= ED_MAXCOORD );
}

#endif

// --- Section

void CAutoRouterEdgeList::Init_Section()
{
	section_first = NULL;
	section_blocker = NULL;
	section_ptr2blocked = NULL;
}

void CAutoRouterEdgeList::Check_Section()
{
	ASSERT( section_blocker == NULL && section_ptr2blocked == NULL );
}

void CAutoRouterEdgeList::Section_Reset()
{
	ASSERT( section_blocker == NULL && section_ptr2blocked == NULL );

	section_first = NULL;
}

void CAutoRouterEdgeList::Section_BeginScan(CAutoRouterEdge* blocker)
{
	ASSERT( section_blocker == NULL && section_ptr2blocked == NULL );

	section_blocker = blocker;

	section_blocker->SetSectionX1(section_blocker->GetPositionX1());
	section_blocker->SetSectionX2(section_blocker->GetPositionX2());

	section_blocker->SetSectionNext(NULL);
	section_blocker->SetSectionDown(NULL);
}

#define section_blocked (*section_ptr2blocked)
bool CAutoRouterEdgeList::Section_HasBlockedEdge()
{
	ASSERT( section_blocker != NULL );

	long b1 = section_blocker->GetSectionX1();
	long b2 = section_blocker->GetSectionX2();
	ASSERT( b1 <= b2 );

	if( section_ptr2blocked == NULL )
	{
		section_ptr2blocked = &section_first;
	}
	else
	{
		CAutoRouterEdge* current_edge = section_blocked;

		ASSERT( current_edge != NULL );

		CAutoRouterEdge* e = current_edge->GetSectionDown();
		CAutoRouterEdge* o = NULL;

		long a1 = current_edge->GetSectionX1();
		long a2 = current_edge->GetSectionX2();
		ASSERT( a1 <= a2 );

		ASSERT( b1 <= a2 &&  a1 <= b2 );							// not case 1 or 6

		if( a1 < b1 && b2 < a2 )									// case 3
		{
			section_ptr2blocked = current_edge->GetSectionDownPtr();
		}
		else if( b1 <= a1 && a2 <= b2 )								// case 4
		{
			if( e )
			{
				while( e->GetSectionNext() )
					e = e->GetSectionNext();

				e->SetSectionNext(current_edge->GetSectionNext());
				section_blocked = current_edge->GetSectionDown();
			}
			else
				section_blocked = current_edge->GetSectionNext();
		}
		else if( b1 <= a1 && b2 < a2 )								// case 5
		{
			ASSERT( a1 <= b2 );

			a1 = b2 + 1;

			while( e && e->GetSectionX1() <= a1 )
			{	
				ASSERT( e->GetSectionX1() <= e->GetSectionX2() );

				if( a1 <= e->GetSectionX2() )
					a1 = e->GetSectionX2() + 1;

				o = e;
				e = e->GetSectionNext();
			}

			if( o )
			{
				section_blocked = current_edge->GetSectionDown();
				o->SetSectionNext(current_edge);
				current_edge->SetSectionDown(e);
			}

			ASSERT( b2 < a1 );
			current_edge->SetSectionX1(a1);
		}
		else														// case 2
		{
			ASSERT( a1 < b1 && b1 <= a2 && a2 <= b2 );

			section_ptr2blocked = current_edge->GetSectionDownPtr();

			while( e )
			{
				o = e;
				e = e->GetSectionNext();

				if( o->GetSectionX2() + 1 < b1 && ( e == NULL || o->GetSectionX2() + 1 < e->GetSectionX1() ) )
					section_ptr2blocked = o->GetSectionNextPtr();
			}

			if( section_blocked )
			{
				ASSERT( o != NULL );
				o->SetSectionNext(current_edge->GetSectionNext());

				current_edge->SetSectionX2(
					(section_blocked->GetSectionX1() < b1 ? section_blocked->GetSectionX1() : b1) - 1);

				current_edge->SetSectionNext(section_blocked);
				section_blocked = NULL;
			}
			else
				current_edge->SetSectionX2(b1 - 1);

			section_ptr2blocked = current_edge->GetSectionNextPtr();
		}
	}

	ASSERT( section_ptr2blocked != NULL );
	while( section_blocked )
	{
		long a1 = section_blocked->GetSectionX1();
		long a2 = section_blocked->GetSectionX2();

		if( a2 < b1 )												// case 1
		{
			section_ptr2blocked = section_blocked->GetSectionNextPtr();

			ASSERT( section_ptr2blocked != NULL );
			continue;
		}
		else if( b2 < a1 )											// case 6
			break;
		
		if( a1 < b1 && b2 < a2 )									// case 3
		{
			long x = b1;

			CAutoRouterEdge* e = section_blocked->GetSectionDown();
			for(;;)
			{
				if( e == NULL || x < e->GetSectionX1() )
					return true;
				else if( x <= e->GetSectionX2() )
				{
					x = e->GetSectionX2() + 1;
					if( b2 < x )
						break;
				}

				e = e->GetSectionNext();
			}

			section_ptr2blocked = section_blocked->GetSectionDownPtr();
			continue;
		}

		return true;
	}

	ASSERT( section_blocker->GetSectionNext() == NULL && section_blocker->GetSectionDown() == NULL );

	section_blocker->SetSectionNext(section_blocked);
	section_blocked = section_blocker;

	section_blocker = NULL;
	section_ptr2blocked = NULL;

	return false;
}
#undef section_blocked

CAutoRouterEdge* CAutoRouterEdgeList::Section_GetBlockedEdge()
{
	ASSERT( section_blocker != NULL && section_ptr2blocked != NULL && *section_ptr2blocked != NULL );
#ifdef _DEBUG_DEEP
	AssertValidSection();
#endif

	return *section_ptr2blocked;
}

bool CAutoRouterEdgeList::Section_IsImmediate()
{
	ASSERT( section_blocker != NULL && section_ptr2blocked != NULL && *section_ptr2blocked != NULL );

	CAutoRouterEdge* section_blocked = *section_ptr2blocked;
	CAutoRouterEdge* e = section_blocked->GetSectionDown();

	long a1 = section_blocked->GetSectionX1();
	long a2 = section_blocked->GetSectionX2();
	long p1 = section_blocked->GetPositionX1();
	long p2 = section_blocked->GetPositionX2();
	long b1 = section_blocker->GetSectionX1();
	long b2 = section_blocker->GetSectionX2();

	ASSERT( b1 <= a2 && a1 <= b2 );									// not case 1 or 6

	// NOTE WE CHANGED THE CONDITIONS (A1<=B1 AND B2<=A2)
	// BECAUSE HERE WE NEED THIS!

	if( a1 <= b1 )
	{
		while( e != NULL && e->GetSectionX2() < b1 )
			e = e->GetSectionNext();

		if( b2 <= a2 )
			return e == NULL || b2 < e->GetSectionX1();				// case 3
			
		return e == NULL && a2 == p2;								// case 2
	}

	if( b2 <= a2 )
		return a1 == p1 && (e == NULL || b2 < e->GetSectionX1());	// case 5

	return e == NULL && a1 == p1 && a2 == p2;						// case 4
}

#ifdef _DEBUG

void CAutoRouterEdgeList::Section_AssertLevel(CAutoRouterEdge* level, long x1, long x2) const
{
	while( level )
	{
		ASSERT( level->GetPositionX1() <= level->GetSectionX1() && level->GetSectionX2() <= level->GetPositionX2() );
		ASSERT( x1 < level->GetSectionX1() && level->GetSectionX1() <= level->GetSectionX2() && level->GetSectionX2() < x2 );
		ASSERT( level->GetSectionNext() == NULL || level->GetSectionX2() < level->GetSectionNext()->GetSectionX1() );

		Section_AssertLevel(level->GetSectionDown(), level->GetSectionX1(), level->GetSectionX2());

		level = level->GetSectionNext();
	}
}

void CAutoRouterEdgeList::AssertValidSection() const
{
	Section_AssertLevel(section_first, ED_MINCOORD-1, ED_MAXCOORD+1);
}

void CAutoRouterEdgeList::AssertSectionReady() const
{
	ASSERT( section_blocker == NULL && section_ptr2blocked == NULL );
}

#endif

// --- Bracket

bool CAutoRouterEdgeList::Bracket_IsClosing(const CAutoRouterEdge* edge) const
{
	ASSERT( edge != NULL );
	ASSERT( !edge->IsStartPointNull() && !edge->IsEndPointNull() );

	CPoint start = edge->GetStartPoint();
	CPoint end = edge->GetEndPoint();

#ifdef _DEBUG
	if( ishorizontal )
	{
		ASSERT( start.y == end.y );

		if( !edge->IsStartPointPrevNull() )
			ASSERT( edge->GetStartPointPrev().x == start.x );

		if( !edge->IsEndPointNextNull() )
			ASSERT( edge->GetEndPointNext().x == end.x );
	}
	else
	{
		ASSERT( start.x == end.x );

		if( !edge->IsStartPointPrevNull() )
			ASSERT( edge->GetStartPointPrev().y == start.y );

		if( !edge->IsEndPointNextNull() )
			ASSERT( edge->GetEndPointNext().y == end.y );
	}
#endif

	if( edge->IsStartPointPrevNull() || edge->IsEndPointNextNull() )
		return false;

	return ishorizontal ?
		(edge->GetStartPointPrev().y < start.y && edge->GetEndPointNext().y < end.y ) :
		(edge->GetStartPointPrev().x < start.x && edge->GetEndPointNext().x < end.x );
}

bool CAutoRouterEdgeList::Bracket_IsOpening(const CAutoRouterEdge* edge) const
{
	ASSERT( edge != NULL );
	ASSERT( !edge->IsStartPointNull() && !edge->IsEndPointNull() );

	CPoint start = edge->GetStartPoint();
	CPoint end = edge->GetEndPoint();
#ifdef _DEBUG
	if( ishorizontal )
	{
		ASSERT( start.y == end.y );

		if( !edge->IsStartPointPrevNull() )
			ASSERT( edge->GetStartPointPrev().x == start.x );

		if( !edge->IsEndPointNextNull() )
			ASSERT( edge->GetEndPointNext().x == end.x );
	}
	else
	{
		ASSERT( start.x == end.x );

		if( !edge->IsStartPointPrevNull() )
			ASSERT( edge->GetStartPointPrev().y == start.y );

		if( !edge->IsEndPointNextNull() )
			ASSERT( edge->GetEndPointNext().y == end.y );
	}
#endif

	if( edge->IsStartPointPrevNull() || edge->IsEndPointNextNull() )
		return false;

	return ishorizontal ?
		(edge->GetStartPointPrev().y > start.y && edge->GetEndPointNext().y > end.y ) :
		(edge->GetStartPointPrev().x > start.x && edge->GetEndPointNext().x > end.x );
}

bool CAutoRouterEdgeList::Bracket_IsSmallGap(const CAutoRouterEdge* blocked, const CAutoRouterEdge* blocker) const
{
	return Bracket_IsOpening(blocked) || Bracket_IsClosing(blocker);
}

bool CAutoRouterEdgeList::Bracket_ShouldBeSwitched(const CAutoRouterEdge* edge, const CAutoRouterEdge* next) const
{
	ASSERT( edge != NULL && next != NULL );

	long ex1, ex2, eo1, eo2;
	Position_GetRealX(edge, ex1, ex2);
	Position_GetRealO(edge, eo1, eo2);

	long nx1, nx2, no1, no2;
	Position_GetRealX(next, nx1, nx2);
	Position_GetRealO(next, no1, no2);

	int c1, c2;

	if( (nx1 < ex1 && ex1 < nx2 && eo1 > 0 ) || (ex1 < nx1 && nx1 < ex2 && no1 < 0) )
		c1 = +1;
	else if( ex1 == nx1 && eo1 == 0 && no1 == 0 )
		c1 = 0;
	else
		c1 = -9;

	if( (nx1 < ex2 && ex2 < nx2 && eo2 > 0 ) || (ex1 < nx2 && nx2 < ex2 && no2 < 0) )
		c2 = +1;
	else if( ex2 == nx2 && eo2 == 0 && no2 == 0 )
		c2 = 0;
	else
		c2 = -9;

	return (c1 + c2) > 0;
}

// --- Block

#define FLT_EQU(A,B)	(((A) - 0.1F < (B)) && ((B) < (A) + 0.1F))

#define S EDLS_S
#define R EDLS_R
#define D EDLS_D

inline float Block_GetF(float d, int b, int s)
{
	float f = d/(b+s);

	if( b == 0 && R <= f )
		f += (D-R);
	else if( S < f && s > 0 )
		f = ((D-S)*d - S*(D-R)*s) / ((D-S)*b + (R-S)*s);

	return f;
}

inline float Block_GetG(float d, int b, int s)
{
	float g = d/(b+s);

	if( S < g && b > 0 )
		g = ((R-S)*d + S*(D-R)*b) / ((D-S)*b + (R-S)*s);

	return g;
}

#undef S
#undef R
#undef D

bool CAutoRouterEdgeList::Block_PushBackward(CAutoRouterEdge* blocked, CAutoRouterEdge* blocker)
{
	bool modified = false;

	ASSERT( blocked != NULL && blocker != NULL );
	ASSERT( blocked->GetPositionY() <= blocker->GetPositionY() );
	ASSERT( blocked->GetBlockPrev() != NULL );

	float f = 0;
	float g = 0;

	CAutoRouterEdge* edge = blocked;
	CAutoRouterEdge* trace = blocker;

	float d = trace->GetPositionY() - edge->GetPositionY();
	ASSERT( d >= 0 );

	int s = (edge->GetBracketOpening() || trace->GetBracketClosing());
	int b = 1 - s;

	for(;;)
	{
		edge->SetBlockTrace(trace);
		trace = edge;
		edge = edge->GetBlockPrev();

		if( edge == NULL )
			break;

		float d2 = trace->GetPositionY() - edge->GetPositionY();
		ASSERT( d2 >= 0 );

		if( edge->GetBracketOpening() || trace->GetBracketClosing() )
		{
			g = Block_GetG(d,b,s);
			if( d2 <= g )
			{
				f = Block_GetF(d,b,s);
				break;
			}
			s++;
		}
		else
		{
			f = Block_GetF(d,b,s);
			if( d2 <= f )
			{
				g = Block_GetG(d,b,s);
				break;
			}
			b++;
		}

		d += d2;
	}

	if( b+s > 1 )
	{
		if( edge == NULL )
		{
			f = Block_GetF(d,b,s);
			g = Block_GetG(d,b,s);
		}

		ASSERT( FLT_EQU(d, f*b + g*s) );

		edge = trace;
		ASSERT( edge != NULL && edge != blocked );

		float y = edge->GetPositionY();

		do
		{
			ASSERT( edge != NULL && edge->GetBlockTrace() != NULL );
			trace = edge->GetBlockTrace();

			y += (edge->GetBracketOpening() || trace->GetBracketClosing()) ? g : f;

			if( y + 0.001F < trace->GetPositionY() )
			{
				modified = true;
				if( SlideButNotPassEdges(trace, y) )
					trace->SetBlockPrev(NULL);
			}

			edge = trace;
		} while( edge != blocked );
#ifdef _DEBUG
		y += (edge->GetBracketOpening() || blocker->GetBracketClosing()) ? g : f;
		ASSERT( FLT_EQU(y, blocker->GetPositionY()) );
#endif
	}

	return modified;
}

bool CAutoRouterEdgeList::Block_PushForward(CAutoRouterEdge* blocked, CAutoRouterEdge* blocker)
{
	bool modified = false;

	ASSERT( blocked != NULL && blocker != NULL );
	ASSERT( blocked->GetPositionY() >= blocker->GetPositionY() );
	ASSERT( blocked->GetBlockNext() != NULL );

	float f = 0;
	float g = 0;

	CAutoRouterEdge* edge = blocked;
	CAutoRouterEdge* trace = blocker;

	float d = edge->GetPositionY() - trace->GetPositionY();
	ASSERT( d >= 0 );

	int s = (trace->GetBracketOpening() || edge->GetBracketClosing());
	int b = 1 - s;

	for(;;)
	{
		edge->SetBlockTrace(trace);
		trace = edge;
		edge = edge->GetBlockNext();

		if( edge == NULL )
			break;

		float d2 = edge->GetPositionY() - trace->GetPositionY();
		ASSERT( d2 >= 0 );

		if( trace->GetBracketOpening() || edge->GetBracketClosing() )
		{
			g = Block_GetG(d,b,s);
			if( d2 <= g )
			{
				f = Block_GetF(d,b,s);
				break;
			}
			s++;
		}
		else
		{
			f = Block_GetF(d,b,s);
			if( d2 <= f )
			{
				g = Block_GetG(d,b,s);
				break;
			}
			b++;
		}

		d += d2;
	}

	if( b+s > 1 )
	{
		if( edge == NULL )
		{
			f = Block_GetF(d,b,s);
			g = Block_GetG(d,b,s);
		}

		ASSERT( FLT_EQU(d, f*b + g*s) );

		edge = trace;
		ASSERT( edge != NULL && edge != blocked );

		float y = edge->GetPositionY();

		do
		{
			ASSERT( edge != NULL && edge->GetBlockTrace() != NULL );
			trace = edge->GetBlockTrace();

			y -= (trace->GetBracketOpening() || edge->GetBracketClosing()) ? g : f;

			if( trace->GetPositionY() < y - 0.001F )
			{
				modified = true;
				if( SlideButNotPassEdges(trace, y) )
					trace->SetBlockNext(NULL);
			}

			edge = trace;
		} while( edge != blocked );
#ifdef _DEBUG
		y -= (blocker->GetBracketOpening() || edge->GetBracketClosing()) ? g : f;
		ASSERT( FLT_EQU(y, blocker->GetPositionY()) );
#endif
	}

	return modified;
}

bool CAutoRouterEdgeList::Block_ScanForward()
{
#ifdef _DEBUG_DEEP
	AssertValidOrder();
	AssertSectionReady();
	AssertValidPositions();
#endif

	PositionAll_LoadX();

	bool modified = false;

	Section_Reset();
	CAutoRouterEdge* blocker = order_first;
	while( blocker )
	{
		CAutoRouterEdge* bmin = NULL;
		CAutoRouterEdge* smin = NULL;
		float bmin_f = ED_MINCOORD - 1;
		float smin_f = ED_MINCOORD - 1;

		Section_BeginScan(blocker);
		while( Section_HasBlockedEdge() )
		{
			if( Section_IsImmediate() )
			{
				CAutoRouterEdge* blocked = Section_GetBlockedEdge();
				ASSERT( blocked != NULL );

				if( blocked->GetBlockPrev() != NULL )
					modified |= Block_PushBackward(blocked, blocker);

				if( !blocker->GetEdgeFixed() )
				{
					if( blocked->GetBracketOpening() || blocker->GetBracketClosing() )
					{
						if( smin_f < blocked->GetPositionY() )
						{
							smin_f = blocked->GetPositionY();
							smin = blocked;
						}
					}
					else
					{
						if( bmin_f < blocked->GetPositionY() )
						{
							bmin_f = blocked->GetPositionY();
							bmin = blocked;
						}
					}
				}
			}
		}

		if( bmin )
		{
			if( smin )
			{
				blocker->SetClosestPrev(smin_f > bmin_f ? smin : bmin);

				bmin_f = blocker->GetPositionY() - bmin_f;
				smin_f = Block_GetF(blocker->GetPositionY() - smin_f, 0, 1);

				blocker->SetBlockPrev(smin_f < bmin_f ? smin : bmin);
			}
			else
			{
				blocker->SetBlockPrev(bmin);
				blocker->SetClosestPrev(bmin);
			}
		}
		else
		{
			blocker->SetBlockPrev(smin);
			blocker->SetClosestPrev(smin);
		}

#ifdef _DEBUG
		if( !blocker->GetEdgeFixed() )
		{
			CAutoRouterPath* ownerPath = static_cast<CAutoRouterPath*>(blocker->GetOwner());
			if (ownerPath->IsHighLighted())
			{
				ASSERT(false);
				break;
			}
		}
#endif

		blocker = blocker->GetOrderNext();
	}

	PositionAll_StoreY();

#ifdef _DEBUG_DEEP
	AssertValidOrder();
	AssertSectionReady();
	AssertValidPositions();
#endif

	return modified;
}

bool CAutoRouterEdgeList::Block_ScanBackward()
{
#ifdef _DEBUG_DEEP
	AssertValidOrder();
	AssertSectionReady();
	AssertValidPositions();
#endif

	PositionAll_LoadX();

	bool modified = false;

	Section_Reset();
	CAutoRouterEdge* blocker = order_last;
	while( blocker )
	{
		CAutoRouterEdge* bmin = NULL;
		CAutoRouterEdge* smin = NULL;
		float bmin_f = ED_MAXCOORD + 1;
		float smin_f = ED_MAXCOORD + 1;

		Section_BeginScan(blocker);
		while( Section_HasBlockedEdge() )
		{
			if( Section_IsImmediate() )
			{
				CAutoRouterEdge* blocked = Section_GetBlockedEdge();
				ASSERT( blocked != NULL );

				if( blocked->GetBlockNext() != NULL )
					modified |= Block_PushForward(blocked, blocker);

				if( !blocker->GetEdgeFixed() )
				{
					if( blocker->GetBracketOpening() || blocked->GetBracketClosing() )
					{
						if( smin_f > blocked->GetPositionY() )
						{
							smin_f = blocked->GetPositionY();
							smin = blocked;
						}
					}
					else
					{
						if( bmin_f > blocked->GetPositionY() )
						{
							bmin_f = blocked->GetPositionY();
							bmin = blocked;
						}
					}
				}
			}
		}

		if( bmin )
		{
			if( smin )
			{
				blocker->SetClosestNext(smin_f < bmin_f ? smin : bmin);

				bmin_f = bmin_f - blocker->GetPositionY();
				smin_f = Block_GetF(smin_f - blocker->GetPositionY(), 0, 1);

				blocker->SetBlockNext(smin_f < bmin_f ? smin : bmin);
			}
			else
			{
				blocker->SetBlockNext(bmin);
				blocker->SetClosestNext(bmin);
			}
		}
		else
		{
			blocker->SetBlockNext(smin);
			blocker->SetClosestNext(smin);
		}

#ifdef _DEBUG
		if( !blocker->GetEdgeFixed() )
		{
			CAutoRouterPath* ownerPath = static_cast<CAutoRouterPath*>(blocker->GetOwner());
			if (ownerPath->IsHighLighted())
			{
				ASSERT(false);
				break;
			}
		}
#endif

		blocker = blocker->GetOrderPrev();
	}

	PositionAll_StoreY();

#ifdef _DEBUG_DEEP
	AssertValidOrder();
	AssertSectionReady();
	AssertValidPositions();
#endif

	return modified;
}

bool CAutoRouterEdgeList::Block_SwitchWrongs()
{
	bool was = false;

	PositionAll_LoadX();

	CAutoRouterEdge* second = order_first;
	while( second != NULL )
	{
		if( second->GetClosestPrev() != NULL && second->GetClosestPrev()->GetClosestNext() != second &&
			second->GetClosestNext() != NULL && second->GetClosestNext()->GetClosestPrev() == second )
			
		{
			ASSERT( !second->GetEdgeFixed() );

			CAutoRouterEdge* edge = second;
			CAutoRouterEdge* next = edge->GetClosestNext();

			while( next != NULL && edge == next->GetClosestPrev() )
			{
				ASSERT( edge != NULL && !edge->GetEdgeFixed() );
				ASSERT( next != NULL && !next->GetEdgeFixed() );

				float ey = edge->GetPositionY();
				float ny = next->GetPositionY();

				ASSERT( ey <= ny );

				if( ey + 1 <= ny && Bracket_ShouldBeSwitched(edge, next) )
				{
					was = true;

					ASSERT( !edge->GetEdgeCanpassed() && !next->GetEdgeCanpassed() );
					edge->SetEdgeCanpassed(true);
					next->SetEdgeCanpassed(true);

					int a = SlideButNotPassEdges(edge, (ny+ey)/2 + 0.001F) != NULL;
					a |= SlideButNotPassEdges(next, (ny+ey)/2 - 0.001F) != NULL;

					if( a )
					{
						edge->SetClosestPrev(NULL);
						edge->SetClosestNext(NULL);
						next->SetClosestPrev(NULL);
						next->SetClosestNext(NULL);

						edge->SetEdgeCanpassed(false);
						next->SetEdgeCanpassed(false);
						break;
					}

					if( edge->GetClosestPrev() != NULL && edge->GetClosestPrev()->GetClosestNext() == edge )
						edge->GetClosestPrev()->SetClosestNext(next);

					if( next->GetClosestNext() != NULL && next->GetClosestNext()->GetClosestPrev() == next)
						next->GetClosestNext()->SetClosestPrev(edge);

					edge->SetClosestNext(next->GetClosestNext());
					next->SetClosestNext(edge);
					next->SetClosestPrev(edge->GetClosestPrev());
					edge->SetClosestPrev(next);

					edge->SetEdgeCanpassed(false);
					next->SetEdgeCanpassed(false);

					ASSERT( !Bracket_ShouldBeSwitched(next, edge) );

					if( next->GetClosestPrev() != NULL && next->GetClosestPrev()->GetClosestNext() == next )
						edge = next->GetClosestPrev();
					else
						next = edge->GetClosestNext();
				}
				else
				{
					edge = next;
					next = next->GetClosestNext();
				}
			}
		}

		second = second->GetOrderNext();
	}

	if( was )
		PositionAll_StoreY();

	return was;
}

