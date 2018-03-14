
#include "stdafx.h"
#include "afxcoll.h"
#include "afxtempl.h"
#include "float.h"

#include "ArGraph.h"
#include "ArEdgeLs.h"

#ifdef _DEBUG
// #define _DEBUG_DEEP
#endif

#define EDLS_S (ED_SMALLGAP)
#define EDLS_R (ED_SMALLGAP+1)
#define EDLS_D (ED_MAXCOORD-ED_MINCOORD)

/*
	In this file every comments refer to the horizontal case, that is, each	edge is
	horizontal.
*/

/*
	Every SArEdge belongs to an edge of a CArPath, CArBox or CArPort. This edge is
	Represented by a CArPoint with its next point. The variable 'point' will refer
	to this CArPoint.

	The coordinates of an edge are 'x1', 'x2' and 'y' where x1/x2 is the x-coordinate
	of the left/right point, and y is the common y-coordinate of the points.

	The edges are ordered according to their y-coordinates. The first edge has
	the least y-coordinate (topmost), and its pointer is in 'order_fist'.
	We use the 'order' prefix in the variable names to refer to this order.

	We will walk from top to bottom (from the 'order_first' along the 'order_next').
	We keep track a "section" of some edges. If we have an infinite horizontal line,
	then the section consists of those edges that are above the line and not blocked
	by another edge which is closer to the line. Each edge in the section has
	a viewable portion from the line (the not blocked portion). The coordinates
	of this portion are 'section_x1' and 'section_x2'. We have an order of the edges
	belonging to the current section. The 'section_first' refers to the leftmost
	edge in the section, while the 'section_next' to the next from left to right.

	We say that the SArEdge E1 "precede" the SArEdge E2 if there is no other SArEdge which
	totally	blocks S1 from S2. So a section consists of the preceding edges of an 
	infinite edge. We say that E1 is "adjacent" to E2, if E1 is the nearest edge
	to E2 which precede it. Clearly, every edge has at most one adjacent precedence.

	The edges of any CArBox or CArPort are fixed. We will continually fix the edges
	of the CArPaths. But first we need some definition.

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

	The variables 'canstart' and 'canend' means that this egde can start and/or and
	a block. The top edgeof a box only canend, while a fixed edge of a path can both
	start and end of a block.

*/


// --------------------------- CArEdgeList


CArEdgeList::CArEdgeList(int h)
{
	owner = NULL;

	ishorizontal = (h != 0);

	Con_Order();
	Con_Section();
}

CArEdgeList::~CArEdgeList()
{
	Des_Order();
	Des_Section();
}

// --- Edges

SArEdge* CArEdgeList::CreateEdge() const
{
	SArEdge* edge = new SArEdge;
	ASSERT( edge != NULL );

	edge->owner = NULL;
	
	edge->startpoint_prev = NULL;
	edge->startpoint = NULL;
	edge->endpoint = NULL;
	edge->endpoint_next = NULL;

	edge->position_y =0;
	edge->position_x1 = 0;
	edge->position_x2 = 0;
	edge->bracket_closing = 0;
	edge->bracket_opening = 0;

	edge->order_prev = NULL;
	edge->order_next = NULL;

	edge->edge_fixed = 0;
	edge->edge_canpassed = 0;

	edge->position_y = 0;

	edge->block_prev = NULL;
	edge->block_next = NULL;
	edge->block_trace = NULL;

	edge->closest_prev = NULL;
	edge->closest_next = NULL;

	return edge;
}

void CArEdgeList::AddEdges(CArPath* path)
{
	ASSERT_VALID(path);
	ASSERT( path->GetOwner() == owner );
	ASSERT_VALID( path->GetStartPort() );
	ASSERT_VALID( path->GetEndPort() );

	CPoint* startpoint;
	CPoint* endpoint;

	POSITION pos = path->GetTailEdgePtrs(startpoint, endpoint);
	while( pos != NULL )
	{
		EArDir dir = GetDir(*endpoint - *startpoint);

#ifdef _DEBUG
		if( path->IsMoveable() )
			ASSERT( IsRightAngle(dir) );
#endif

		if( IsRightAngle(dir) && IsHorizontal(dir) == ishorizontal )
		{
			SArEdge* edge = CreateEdge();

			edge->owner = path;

			edge->startpoint = startpoint;
			edge->endpoint = endpoint;
			edge->startpoint_prev = path->GetPointBeforeEdge(pos);
			edge->endpoint_next = path->GetPointAfterEdge(pos);

			edge->edge_fixed = path->IsFixed() || 
				(edge->startpoint_prev == NULL && path->GetStartPort()->IsConnectToCenter()) ||
				(edge->endpoint_next == NULL && path->GetEndPort()->IsConnectToCenter());

			Position_LoadY(edge);
			Position_LoadB(edge);
			Insert(edge);
		}

		path->GetPrevEdgePtrs(pos, startpoint, endpoint);
	}
}

SArEdge* CArEdgeList::GetEdge(CPoint* startpoint, CPoint* endpoint) const
{
	ASSERT( startpoint != NULL && endpoint != NULL );

	SArEdge* edge = order_first;
	while( edge != NULL )
	{
		if( edge->startpoint == startpoint )
		{
			ASSERT( edge->endpoint == endpoint );
			break;
		}

		edge = edge->order_next;
	}

	ASSERT( edge != NULL );
	return edge;
}

SArEdge* CArEdgeList::GetEdgeAt(CPoint point, int nearness) const
{
	SArEdge* edge = order_first;
	while( edge )
	{
		if( IsPointNearLine(point, *edge->startpoint, *edge->endpoint, nearness) )
			return edge;

		edge = edge->order_next;
	}

	return NULL;
}

#ifdef _DEBUG

void CArEdgeList::AssertValidPathEdges(CArPath* path) const
{
	ASSERT( path != NULL );
	ASSERT( path->GetOwner() == owner );

	CPoint* startpoint;
	CPoint* endpoint;

	POSITION pos = path->GetTailEdgePtrs(startpoint, endpoint);
	while( pos != NULL )
	{
		EArDir dir = GetDir(*endpoint - *startpoint);

		if( path->IsMoveable() )
			ASSERT( IsRightAngle(dir) );

		if( IsRightAngle(dir) && IsHorizontal(dir) == ishorizontal )
		{
			SArEdge* edge = GetEdge(startpoint, endpoint);
			ASSERT( edge != NULL );

			ASSERT( edge->owner == path );

			ASSERT( edge->startpoint == startpoint );
			ASSERT( edge->endpoint == endpoint );
			ASSERT( edge->startpoint_prev == path->GetPointBeforeEdge(pos) );
			ASSERT( edge->endpoint_next == path->GetPointAfterEdge(pos) );
		}

		path->GetPrevEdgePtrs(pos, startpoint, endpoint);
	}

	SArEdge* edge = order_first;
	while( edge != NULL )
	{
		if( edge->owner == path )
		{
			POSITION pos = path->GetEdgePosForStartPoint(edge->startpoint);
			ASSERT( pos != NULL );
			ASSERT( path->GetStartPoint(pos)== edge->startpoint );
			ASSERT( path->GetEndPoint(pos)== edge->endpoint );
			ASSERT( path->GetPointBeforeEdge(pos)== edge->startpoint_prev );
			ASSERT( path->GetPointAfterEdge(pos)== edge->endpoint_next );
		}

		edge = edge->order_next;
	}
}

#endif

void CArEdgeList::AddEdges(CArPort* port)
{
	ASSERT_VALID(port);
	ASSERT( port->GetOwner()->GetOwner() == owner );

	if( port->IsConnectToCenter() || port->GetOwner()->IsAtomic() )
		return;

	for(int i = 0; i < 4; i++)
	{
		CPoint* startpoint_prev = port->selfpoints + ((i+3)%4);
		CPoint* startpoint = port->selfpoints + (i);
		CPoint* endpoint = port->selfpoints + ((i+1)%4);
		CPoint* endpoint_next = port->selfpoints + ((i+2)%4);

		EArDir dir = GetDir(*endpoint - *startpoint);
		ASSERT( IsRightAngle(dir) );

		if( IsHorizontal(dir) == ishorizontal && port->CanHaveStartEndPointHorizontal(ishorizontal) )
		{
			SArEdge* edge = CreateEdge();

			edge->owner = port;

			edge->startpoint = startpoint;
			edge->endpoint = endpoint;
			edge->startpoint_prev = startpoint_prev;
			edge->endpoint_next = endpoint_next;

			edge->edge_fixed = 1;

			Position_LoadY(edge);
			Position_LoadB(edge);

			if( edge->bracket_closing )
				edge->position_y += 0.999F;

			Insert(edge);
		}
	}
}

void CArEdgeList::AddEdges(CArBox* box)
{
	ASSERT_VALID(box);
	ASSERT( box->GetOwner() == owner );

	for(int i = 0; i < 4; i++)
	{
		CPoint* startpoint_prev = box->selfpoints + ((i+3)%4);
		CPoint* startpoint = box->selfpoints + (i);
		CPoint* endpoint = box->selfpoints + ((i+1)%4);
		CPoint* endpoint_next = box->selfpoints + ((i+2)%4);

		EArDir dir = GetDir(*endpoint - *startpoint);
		ASSERT( IsRightAngle(dir) );

		if( IsHorizontal(dir) == ishorizontal )
		{
			SArEdge* edge = CreateEdge();

			edge->owner = box;

			edge->startpoint = startpoint;
			edge->endpoint = endpoint;
			edge->startpoint_prev = startpoint_prev;
			edge->endpoint_next = endpoint_next;

			edge->edge_fixed = 1;

			Position_LoadY(edge);
			Position_LoadB(edge);

			if( edge->bracket_closing )
				edge->position_y += 0.999F;
		
			Insert(edge);
		}
	}
}

void CArEdgeList::AddEdges(CArGraph* graph)
{
	ASSERT_VALID(graph);
	ASSERT( graph == owner );

	for(int i = 0; i < 4; i++)
	{
		CPoint* startpoint_prev = graph->selfpoints + ((i+3)%4);
		CPoint* startpoint = graph->selfpoints + (i);
		CPoint* endpoint = graph->selfpoints + ((i+1)%4);
		CPoint* endpoint_next = graph->selfpoints + ((i+2)%4);

		EArDir dir = GetDir(*endpoint - *startpoint);
		ASSERT( IsRightAngle(dir) );

		if( IsHorizontal(dir) == ishorizontal )
		{
			SArEdge* edge = CreateEdge();

			edge->owner = graph;

			edge->startpoint = startpoint;
			edge->endpoint = endpoint;
			edge->startpoint_prev = startpoint_prev;
			edge->endpoint_next = endpoint_next;

			edge->edge_fixed = 1;

			Position_LoadY(edge);
			Insert(edge);
		}
	}
}

void CArEdgeList::DeleteEdges(CObject* object)
{
	ASSERT( object != NULL );

	SArEdge* edge = order_first;
	while( edge != NULL )
	{
		if( edge->owner == object )
		{
			SArEdge* next = edge->order_next;
			Delete(edge);
			edge = next;
		}
		else
			edge = edge->order_next;
	}
}

void CArEdgeList::DeleteAllEdges()
{
	while( order_first )
		Delete(order_first);

	ASSERT( order_last == NULL );
}

int CArEdgeList::IsEmpty() const
{
	ASSERT( (order_first != NULL) + (order_last != NULL) != 1 );

	return order_first == NULL;
}

// --- Position

long CArEdgeList::Position_GetRealY(const SArEdge* edge) const
{
	ASSERT( edge != NULL && edge->startpoint != NULL && edge->endpoint != NULL );

	if( ishorizontal )
	{
		ASSERT( edge->startpoint->y == edge->endpoint->y );
		return edge->startpoint->y;
	}

	ASSERT( edge->startpoint->x == edge->endpoint->x );
	return edge->startpoint->x;
}

void CArEdgeList::Position_SetRealY(SArEdge* edge, long y) const
{
	ASSERT( edge != NULL && edge->startpoint != NULL && edge->endpoint != NULL );

	if( ishorizontal )
	{
		ASSERT( edge->startpoint->y == edge->endpoint->y );
		edge->startpoint->y = y;
		edge->endpoint->y = y;
	}
	else
	{
		ASSERT( edge->startpoint->x == edge->endpoint->x );
		edge->startpoint->x = y;
		edge->endpoint->x = y;
	}
}

void CArEdgeList::Position_GetRealX(const SArEdge* edge, long& x1, long& x2) const
{
	ASSERT( edge != NULL && edge->startpoint != NULL && edge->endpoint != NULL );

	if( ishorizontal )
	{
		ASSERT( edge->startpoint->y == edge->endpoint->y );
		if( edge->startpoint->x < edge->endpoint->x )
		{
			x1 = edge->startpoint->x;
			x2 = edge->endpoint->x;
		}
		else
		{
			x1 = edge->endpoint->x;
			x2 = edge->startpoint->x;
		}
	}
	else
	{
		ASSERT( edge->startpoint->x == edge->endpoint->x );
		if( edge->startpoint->y < edge->endpoint->y )
		{
			x1 = edge->startpoint->y;
			x2 = edge->endpoint->y;
		}
		else
		{
			x1 = edge->endpoint->y;
			x2 = edge->startpoint->y;
		}
	}
}

void CArEdgeList::Position_GetRealO(const SArEdge* edge, long& o1, long& o2) const
{
	ASSERT( edge != NULL && edge->startpoint != NULL && edge->endpoint != NULL );

	if( ishorizontal )
	{
		ASSERT( edge->startpoint->y == edge->endpoint->y );
		if( edge->startpoint->x < edge->endpoint->x )
		{
			o1 = edge->startpoint_prev == NULL ? 0 : edge->startpoint_prev->y - edge->startpoint->y;
			o2 = edge->endpoint_next == NULL ? 0 : edge->endpoint_next->y - edge->endpoint->y;
		}
		else
		{
			o1 = edge->endpoint_next == NULL ? 0 : edge->endpoint_next->y - edge->endpoint->y;
			o2 = edge->startpoint_prev == NULL ? 0 : edge->startpoint_prev->y - edge->startpoint->y;
		}
	}
	else
	{
		ASSERT( edge->startpoint->x == edge->endpoint->x );
		if( edge->startpoint->y < edge->endpoint->y )
		{
			o1 = edge->startpoint_prev == NULL ? 0 : edge->startpoint_prev->x - edge->startpoint->x;
			o2 = edge->endpoint_next == NULL ? 0 : edge->endpoint_next->x - edge->endpoint->x;
		}
		else
		{
			o1 = edge->endpoint_next == NULL ? 0 : edge->endpoint_next->x - edge->endpoint->x;
			o2 = edge->startpoint_prev == NULL ? 0 : edge->startpoint_prev->x - edge->startpoint->x;
		}
	}
}

void CArEdgeList::Position_LoadY(SArEdge* edge) const
{
	ASSERT( edge != NULL && edge->order_next == NULL && edge->order_prev == NULL );

	edge->position_y = (float) Position_GetRealY(edge);
}

void CArEdgeList::Position_LoadB(SArEdge* edge) const
{
	ASSERT( edge != NULL );

	edge->bracket_opening = !edge->edge_fixed && Bracket_IsOpening(edge);
	edge->bracket_closing = !edge->edge_fixed && Bracket_IsClosing(edge);
}

void CArEdgeList::PositionAll_StoreY() const
{
	SArEdge* edge = order_first;
	while( edge )
	{
		Position_SetRealY(edge, (long) edge->position_y);

		edge = edge->order_next;
	}
}

void CArEdgeList::PositionAll_LoadX() const
{
	SArEdge* edge = order_first;
	while( edge )
	{
		Position_GetRealX(edge, edge->position_x1, edge->position_x2);

		edge = edge->order_next;
	}
}

#ifdef _DEBUG

void CArEdgeList::AssertValidPositions() const
{
	SArEdge* edge = order_first;
	while( edge )
	{
		long y = Position_GetRealY(edge);
		ASSERT( edge->position_y - 1 <= y && y <= edge->position_y + 1 );

		if( edge->order_prev )
		{
			ASSERT( edge->order_prev->position_y <= edge->position_y );
			ASSERT( Position_GetRealY(edge->order_prev) <= y );
		}

		if( edge->order_next )
		{
			ASSERT( edge->position_y <= edge->order_next->position_y );
			ASSERT( y <= Position_GetRealY(edge->order_next) );
		}

		edge = edge->order_next;
	}
}

#endif

// --- Order

void CArEdgeList::Con_Order()
{
	order_first = NULL;
	order_last = NULL;
}

void CArEdgeList::Des_Order()
{
	ASSERT( order_first == NULL && order_last == NULL );
}

void CArEdgeList::InsertBefore(SArEdge* edge, SArEdge* before)
{
	ASSERT( edge != NULL && before != NULL && edge != before );
	ASSERT( edge->order_next == NULL && edge->order_prev == NULL );

	edge->order_prev = before->order_prev;
	edge->order_next = before;

	if( before->order_prev )
	{
		ASSERT( before->order_prev->order_next == before );
		before->order_prev->order_next = edge;

		ASSERT( order_first != before );
	}
	else
	{
		ASSERT( order_first == before );
		order_first = edge;
	}

	before->order_prev = edge;
}

void CArEdgeList::InsertAfter(SArEdge* edge, SArEdge* after)
{
	ASSERT( edge != NULL && after != NULL && edge != after );
	ASSERT( edge->order_next == NULL && edge->order_prev == NULL );

	edge->order_next = after->order_next;
	edge->order_prev = after;

	if( after->order_next )
	{
		ASSERT( after->order_next->order_prev == after );
		after->order_next->order_prev = edge;

		ASSERT( order_last != after );
	}
	else
	{
		ASSERT( order_last == after );
		order_last = edge;
	}

	after->order_next = edge;
}

void CArEdgeList::InsertLast(SArEdge* edge)
{
	ASSERT( edge != NULL );
	ASSERT( edge->order_prev == NULL && edge->order_next == NULL );

	edge->order_prev = order_last;

	if( order_last )
	{
		ASSERT( order_last->order_next == NULL );
		ASSERT( order_first != NULL );

		order_last->order_next = edge;
		order_last = edge;
	}
	else
	{
		ASSERT( order_first == NULL );

		order_first = edge;
		order_last = edge;
	}
}

void CArEdgeList::Insert(SArEdge* edge)
{
	ASSERT( edge != NULL );
	ASSERT( edge->order_prev == NULL && edge->order_next == NULL );

	float y = edge->position_y;
	ASSERT( ED_MINCOORD <= y && y <= ED_MAXCOORD );

	SArEdge* insert = order_first;
	while( insert && insert->position_y < y )
		insert = insert->order_next;

	if( insert )
		InsertBefore(edge, insert);
	else
		InsertLast(edge);
}

void CArEdgeList::Remove(SArEdge* edge)
{
	ASSERT( edge != NULL );

	if( order_first == edge )
		order_first = edge->order_next;

	if( edge->order_next )
		edge->order_next->order_prev = edge->order_prev;

	if( order_last == edge )
		order_last = edge->order_prev;

	if( edge->order_prev )
		edge->order_prev->order_next = edge->order_next;

	edge->order_next = NULL;
	edge->order_prev = NULL;
}

void CArEdgeList::Delete(SArEdge* edge)
{
	ASSERT( edge != NULL );

	Remove(edge);
	delete edge;
}

SArEdge* CArEdgeList::SlideButNotPassEdges(SArEdge* edge, float y)
{
	ASSERT( edge != NULL );
	ASSERT( ED_MINCOORD < y && y < ED_MAXCOORD );

#ifdef _DEBUG
	if( !edge->edge_fixed && ((CArPath*)edge->owner)->IsHighLighted() )
		ASSERT(true);
#endif

	float oldy = edge->position_y;
	ASSERT( ED_MINCOORD < oldy && oldy < ED_MAXCOORD );

	if( oldy == y )
		return NULL;

	long x1 = edge->position_x1;
	long x2 = edge->position_x2;
	SArEdge* ret = NULL;

	SArEdge* insert = edge;
	if( oldy < y )
	{
		while( insert->order_next )
		{
			insert = insert->order_next;

			if( y < insert->position_y )
				break;

			if( !insert->edge_canpassed && Intersect(x1, x2, insert->position_x1, insert->position_x2 ) )
			{
				ret = insert;
				y = insert->position_y;
				break;
			}
		}

		if( edge != insert && insert->order_prev != edge )
		{
			Remove(edge);
			InsertBefore(edge, insert);
		}
	}
	else
	{
		while( insert->order_prev )
		{
			insert = insert->order_prev;

			if( y > insert->position_y )
				break;

			if( !insert->edge_canpassed && Intersect(x1, x2, insert->position_x1, insert->position_x2 ) )
			{
				ret = insert;
				y = insert->position_y;
				break;
			}
		}

		if( edge != insert && insert->order_next != edge )
		{
			Remove(edge);
			InsertAfter(edge, insert);
		}

	}

#ifdef _DEBUG
	if( edge->order_next )
		ASSERT( y <= edge->order_next->position_y );

	if( edge->order_prev )
		ASSERT( edge->order_prev->position_y <= y );
#endif

	edge->position_y = y;

	return ret;
}

#ifdef _DEBUG

void CArEdgeList::AssertValidOrder() const
{
	ASSERT( order_first != NULL && order_last != NULL );
	ASSERT( order_first->order_prev == NULL );
	ASSERT( order_last->order_next == NULL );

	float y = ED_MINCOORD;

	TRACE("CArEdgeList::AssertValidOrder (horizontal=%d) START\n", ishorizontal);

	SArEdge* edge = order_first;
	while( edge != order_last )
	{
		TRACE("edge=%p, position_y=%f\n", edge, edge->position_y);

		ASSERT( edge != NULL );
		ASSERT( y <= edge->position_y );
		ASSERT( edge->order_next->order_prev == edge );

		y = edge->position_y;
		edge = edge->order_next;
	}

	TRACE("edge=%p, position_y=%f\n", edge, edge->position_y);
	TRACE("CArEdgeList::AssertValidOrder (horizontal=%d) END\n", ishorizontal);

	ASSERT( y <= ED_MAXCOORD );
}

#endif

// --- Section

void CArEdgeList::Con_Section()
{
	section_first = NULL;
	section_blocker = NULL;
	section_ptr2blocked = NULL;
}

void CArEdgeList::Des_Section()
{
	ASSERT( section_blocker == NULL && section_ptr2blocked == NULL );
}

void CArEdgeList::Section_Reset()
{
	ASSERT( section_blocker == NULL && section_ptr2blocked == NULL );

	section_first = NULL;
}

void CArEdgeList::Section_BeginScan(SArEdge* blocker)
{
	ASSERT( section_blocker == NULL && section_ptr2blocked == NULL );

	section_blocker = blocker;

	section_blocker->section_x1 = section_blocker->position_x1;
	section_blocker->section_x2 = section_blocker->position_x2;

	section_blocker->section_next = NULL;
	section_blocker->section_down = NULL;
}

#define section_blocked (*section_ptr2blocked)
int CArEdgeList::Section_HasBlockedEdge()
{
	ASSERT( section_blocker != NULL );

	long b1 = section_blocker->section_x1;
	long b2 = section_blocker->section_x2;
	ASSERT( b1 <= b2 );

	if( section_ptr2blocked == NULL )
	{
		section_ptr2blocked = &section_first;
	}
	else
	{
		SArEdge* current_edge = section_blocked;

		ASSERT( current_edge != NULL );

		SArEdge* e = current_edge->section_down;
		SArEdge* o = NULL;

		long a1 = current_edge->section_x1;
		long a2 = current_edge->section_x2;
		ASSERT( a1 <= a2 );

		ASSERT( b1 <= a2 &&  a1 <= b2 );							// not case 1 or 6

		if( a1 < b1 && b2 < a2 )									// case 3
		{
			section_ptr2blocked = &(current_edge->section_down);
		}
		else if( b1 <= a1 && a2 <= b2 )								// case 4
		{
			if( e )
			{
				while( e->section_next )
					e = e->section_next;

				e->section_next = current_edge->section_next;
				section_blocked = current_edge->section_down;
			}
			else
				section_blocked = current_edge->section_next;
		}
		else if( b1 <= a1 && b2 < a2 )								// case 5
		{
			ASSERT( a1 <= b2 );

			a1 = b2 + 1;

			while( e && e->section_x1 <= a1 )
			{	
				ASSERT( e->section_x1 <= e->section_x2 );

				if( a1 <= e->section_x2 )
					a1 = e->section_x2 + 1;

				o = e;
				e = e->section_next;
			}

			if( o )
			{
				section_blocked = current_edge->section_down;
				o->section_next = current_edge;
				current_edge->section_down = e;
			}

			ASSERT( b2 < a1 );
			current_edge->section_x1 = a1;
		}
		else														// case 2
		{
			ASSERT( a1 < b1 && b1 <= a2 && a2 <= b2 );

			section_ptr2blocked = &(current_edge->section_down);

			while( e )
			{
				o = e;
				e = e->section_next;

				if( o->section_x2 + 1 < b1 && ( e == NULL || o->section_x2 + 1 < e->section_x1 ) )
					section_ptr2blocked = &(o->section_next);
			}

			if( section_blocked )
			{
				ASSERT( o != NULL );
				o->section_next = current_edge->section_next;

				current_edge->section_x2 = 
					(section_blocked->section_x1 < b1 ? section_blocked->section_x1 : b1) - 1;

				current_edge->section_next = section_blocked;
				section_blocked = NULL;
			}
			else
				current_edge->section_x2 = b1 - 1;

			section_ptr2blocked = &(current_edge->section_next);
		}
	}

	ASSERT( section_ptr2blocked != NULL );
	while( section_blocked )
	{
		long a1 = section_blocked->section_x1;
		long a2 = section_blocked->section_x2;

		if( a2 < b1 )												// case 1
		{
			section_ptr2blocked = &(section_blocked->section_next);

			ASSERT( section_ptr2blocked != NULL );
			continue;
		}
		else if( b2 < a1 )											// case 6
			break;
		
		if( a1 < b1 && b2 < a2 )									// case 3
		{
			long x = b1;

			SArEdge* e = section_blocked->section_down;
			for(;;)
			{
				if( e == NULL || x < e->section_x1 )
					return 1;
				else if( x <= e->section_x2 )
				{
					x = e->section_x2 + 1;
					if( b2 < x )
						break;
				}

				e = e->section_next;
			}

			section_ptr2blocked = &(section_blocked->section_down);
			continue;
		}

		return 1;
	}

	ASSERT( section_blocker->section_next == NULL && section_blocker->section_down == NULL );

	section_blocker->section_next = section_blocked;
	section_blocked = section_blocker;

	section_blocker = NULL;
	section_ptr2blocked = NULL;

	return 0;
}
#undef section_blocked

SArEdge* CArEdgeList::Section_GetBlockedEdge()
{
	ASSERT( section_blocker != NULL && section_ptr2blocked != NULL && *section_ptr2blocked != NULL );
#ifdef _DEBUG_DEEP
	AssertValidSection();
#endif

	return *section_ptr2blocked;
}

int CArEdgeList::Section_IsImmediate()
{
	ASSERT( section_blocker != NULL && section_ptr2blocked != NULL && *section_ptr2blocked != NULL );

	SArEdge* section_blocked = *section_ptr2blocked;
	SArEdge* e = section_blocked->section_down;

	long a1 = section_blocked->section_x1;
	long a2 = section_blocked->section_x2;
	long p1 = section_blocked->position_x1;
	long p2 = section_blocked->position_x2;
	long b1 = section_blocker->section_x1;
	long b2 = section_blocker->section_x2;

	ASSERT( b1 <= a2 && a1 <= b2 );									// not case 1 or 6

	// NOTE WE CHANGED THE CONDITIONS (A1<=B1 AND B2<=A2)
	// BECAUSE HERE WE NEED THIS!

	if( a1 <= b1 )
	{
		while( e != NULL && e->section_x2 < b1 )
			e = e->section_next;

		if( b2 <= a2 )
			return e == NULL || b2 < e->section_x1;					// case 3
			
		return e == NULL && a2 == p2;								// case 2
	}

	if( b2 <= a2 )
		return a1 == p1 && (e == NULL || b2 < e->section_x1);		// case 5

	return e == NULL && a1 == p1 && a2 == p2;						// case 4
}

#ifdef _DEBUG

void CArEdgeList::Section_AssertLevel(SArEdge* level, long x1, long x2) const
{
	while( level )
	{
		ASSERT( level->position_x1 <= level->section_x1 && level->section_x2 <= level->position_x2 );
		ASSERT( x1 < level->section_x1 && level->section_x1 <= level->section_x2 && level->section_x2 < x2 );
		ASSERT( level->section_next == NULL || level->section_x2 < level->section_next->section_x1 );

		Section_AssertLevel(level->section_down, level->section_x1, level->section_x2);

		level = level->section_next;
	}
}

void CArEdgeList::AssertValidSection() const
{
	Section_AssertLevel(section_first, ED_MINCOORD-1, ED_MAXCOORD+1);
}

void CArEdgeList::AssertSectionReady() const
{
	ASSERT( section_blocker == NULL && section_ptr2blocked == NULL );
}

#endif

// --- Bracket

int CArEdgeList::Bracket_IsClosing(const SArEdge* edge) const
{
	ASSERT( edge != NULL && edge->startpoint != NULL && edge->endpoint != NULL );

	return IsClosingBracket(edge->startpoint_prev, edge->startpoint, edge->endpoint, edge->endpoint_next, ishorizontal);
}

int CArEdgeList::Bracket_IsOpening(const SArEdge* edge) const
{
	ASSERT( edge != NULL && edge->startpoint != NULL && edge->endpoint != NULL );

	return IsOpeningBracket(edge->startpoint_prev, edge->startpoint, edge->endpoint, edge->endpoint_next, ishorizontal);
}

int CArEdgeList::Bracket_IsSmallGap(const SArEdge* blocked, const SArEdge* blocker) const
{
	return Bracket_IsOpening(blocked) || Bracket_IsClosing(blocker);
}

int CArEdgeList::Bracket_ShouldBeSwitched(const SArEdge* edge, const SArEdge* next) const
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

int CArEdgeList::Block_PushBackward(SArEdge* blocked, SArEdge* blocker)
{
	int modified = 0;

	ASSERT( blocked != NULL && blocker != NULL );
	ASSERT( blocked->position_y <= blocker->position_y );
	ASSERT( blocked->block_prev != NULL );

	float f = 0;
	float g = 0;

	SArEdge* edge = blocked;
	SArEdge* trace = blocker;

	float d = trace->position_y - edge->position_y;
	ASSERT( d >= 0 );

	int s = (edge->bracket_opening || trace->bracket_closing);
	int b = 1 - s;

	for(;;)
	{
		edge->block_trace = trace;
		trace = edge;
		edge = edge->block_prev;

		if( edge == NULL )
			break;

		float d2 = trace->position_y - edge->position_y;
		ASSERT( d2 >= 0 );

		if( edge->bracket_opening || trace->bracket_closing )
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

		float y = edge->position_y;

		do
		{
			ASSERT( edge != NULL && edge->block_trace != NULL );
			trace = edge->block_trace;

			y += (edge->bracket_opening || trace->bracket_closing) ? g : f;

			if( y + 0.001F < trace->position_y )
			{
				modified = 1;
				if( SlideButNotPassEdges(trace, y) )
					trace->block_prev = NULL;
			}

			edge = trace;
		} while( edge != blocked );
#ifdef _DEBUG
		y += (edge->bracket_opening || blocker->bracket_closing) ? g : f;
		ASSERT( FLT_EQU(y, blocker->position_y) );
#endif
	}

	return modified;
}

int CArEdgeList::Block_PushForward(SArEdge* blocked, SArEdge* blocker)
{
	int modified = 0;

	ASSERT( blocked != NULL && blocker != NULL );
	ASSERT( blocked->position_y >= blocker->position_y );
	ASSERT( blocked->block_next != NULL );

	float f = 0;
	float g = 0;

	SArEdge* edge = blocked;
	SArEdge* trace = blocker;

	float d = edge->position_y - trace->position_y;
	ASSERT( d >= 0 );

	int s = (trace->bracket_opening || edge->bracket_closing);
	int b = 1 - s;

	for(;;)
	{
		edge->block_trace = trace;
		trace = edge;
		edge = edge->block_next;

		if( edge == NULL )
			break;

		float d2 = edge->position_y - trace->position_y;
		ASSERT( d2 >= 0 );

		if( trace->bracket_opening || edge->bracket_closing )
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

		float y = edge->position_y;

		do
		{
			ASSERT( edge != NULL && edge->block_trace != NULL );
			trace = edge->block_trace;

			y -= (trace->bracket_opening || edge->bracket_closing) ? g : f;

			if( trace->position_y < y - 0.001F )
			{
				modified = 1;
				if( SlideButNotPassEdges(trace, y) )
					trace->block_next = NULL;
			}

			edge = trace;
		} while( edge != blocked );
#ifdef _DEBUG
		y -= (blocker->bracket_opening || edge->bracket_closing) ? g : f;
		ASSERT( FLT_EQU(y, blocker->position_y) );
#endif
	}

	return modified;
}

int CArEdgeList::Block_ScanForward()
{
#ifdef _DEBUG_DEEP
	AssertValidOrder();
	AssertSectionReady();
	AssertValidPositions();
#endif

	PositionAll_LoadX();

	int modified = 0;

	Section_Reset();
	SArEdge* blocker = order_first;
	while( blocker )
	{
		SArEdge* bmin = NULL;
		SArEdge* smin = NULL;
		float bmin_f = ED_MINCOORD - 1;
		float smin_f = ED_MINCOORD - 1;

		Section_BeginScan(blocker);
		while( Section_HasBlockedEdge() ) if( Section_IsImmediate() )
		{
			SArEdge* blocked = Section_GetBlockedEdge();
			ASSERT( blocked != NULL );

			if( blocked->block_prev != NULL )
				modified |= Block_PushBackward(blocked, blocker);

			if( !blocker->edge_fixed )
			{
				if( blocked->bracket_opening || blocker->bracket_closing )
				{
					if( smin_f < blocked->position_y )
					{
						smin_f = blocked->position_y;
						smin = blocked;
					}
				}
				else
				{
					if( bmin_f < blocked->position_y )
					{
						bmin_f = blocked->position_y;
						bmin = blocked;
					}
				}
			}
		}

		if( bmin )
		{
			if( smin )
			{
				blocker->closest_prev = smin_f > bmin_f ? smin : bmin;

				bmin_f = blocker->position_y - bmin_f;
				smin_f = Block_GetF(blocker->position_y - smin_f, 0, 1);

				blocker->block_prev = smin_f < bmin_f ? smin : bmin;
			}
			else
			{
				blocker->block_prev = bmin;
				blocker->closest_prev = bmin;
			}
		}
		else
		{
			blocker->block_prev = smin;
			blocker->closest_prev = smin;
		}

#ifdef _DEBUG
		if( !blocker->edge_fixed && ((CArPath*)blocker->owner)->IsHighLighted() )
			break;
#endif

		blocker = blocker->order_next;
	}

	PositionAll_StoreY();

#ifdef _DEBUG_DEEP
	AssertValidOrder();
	AssertSectionReady();
	AssertValidPositions();
#endif

	return modified;
}

int CArEdgeList::Block_ScanBackward()
{
#ifdef _DEBUG_DEEP
	AssertValidOrder();
	AssertSectionReady();
	AssertValidPositions();
#endif

	PositionAll_LoadX();

	int modified = 0;

	Section_Reset();
	SArEdge* blocker = order_last;
	while( blocker )
	{
		SArEdge* bmin = NULL;
		SArEdge* smin = NULL;
		float bmin_f = ED_MAXCOORD + 1;
		float smin_f = ED_MAXCOORD + 1;

		Section_BeginScan(blocker);
		while( Section_HasBlockedEdge() ) if( Section_IsImmediate() )
		{
			SArEdge* blocked = Section_GetBlockedEdge();
			ASSERT( blocked != NULL );

			if( blocked->block_next != NULL )
				modified |= Block_PushForward(blocked, blocker);

			if( !blocker->edge_fixed )
			{
				if( blocker->bracket_opening || blocked->bracket_closing )
				{
					if( smin_f > blocked->position_y )
					{
						smin_f = blocked->position_y;
						smin = blocked;
					}
				}
				else
				{
					if( bmin_f > blocked->position_y )
					{
						bmin_f = blocked->position_y;
						bmin = blocked;
					}
				}
			}
		}

		if( bmin )
		{
			if( smin )
			{
				blocker->closest_next = smin_f < bmin_f ? smin : bmin;

				bmin_f = bmin_f - blocker->position_y;
				smin_f = Block_GetF(smin_f - blocker->position_y, 0, 1);

				blocker->block_next = smin_f < bmin_f ? smin : bmin;
			}
			else
			{
				blocker->block_next = bmin;
				blocker->closest_next = bmin;
			}
		}
		else
		{
			blocker->block_next = smin;
			blocker->closest_next = smin;
		}

#ifdef _DEBUG
		if( !blocker->edge_fixed && ((CArPath*)blocker->owner)->IsHighLighted() )
			break;
#endif

		blocker = blocker->order_prev;
	}

	PositionAll_StoreY();

#ifdef _DEBUG_DEEP
	AssertValidOrder();
	AssertSectionReady();
	AssertValidPositions();
#endif

	return modified;
}

int CArEdgeList::Block_SwitchWrongs()
{
	int was = 0;

	PositionAll_LoadX();

	SArEdge* second = order_first;
	while( second != NULL )
	{
		if( second->closest_prev != NULL && second->closest_prev->closest_next != second &&
			second->closest_next != NULL && second->closest_next->closest_prev == second )
			
		{
			ASSERT( !second->edge_fixed );

			SArEdge* edge = second;
			SArEdge* next = edge->closest_next;

			while( next != NULL && edge == next->closest_prev )
			{
				ASSERT( edge != NULL && !edge->edge_fixed );
				ASSERT( next != NULL && !next->edge_fixed );

				float ey = edge->position_y;
				float ny = next->position_y;

				ASSERT( ey <= ny );

				if( ey + 1 <= ny && Bracket_ShouldBeSwitched(edge, next) )
				{
					was = 1;

					ASSERT( !edge->edge_canpassed && !next->edge_canpassed );
					edge->edge_canpassed = 1;
					next->edge_canpassed = 1;

					int a = SlideButNotPassEdges(edge, (ny+ey)/2 + 0.001F) != NULL;
					a |= SlideButNotPassEdges(next, (ny+ey)/2 - 0.001F) != NULL;

					if( a )
					{
						edge->closest_prev = NULL;
						edge->closest_next = NULL;
						next->closest_prev = NULL;
						next->closest_next = NULL;

						edge->edge_canpassed = 0;
						next->edge_canpassed = 0;
						break;
					}

					if( edge->closest_prev != NULL && edge->closest_prev->closest_next == edge )
						edge->closest_prev->closest_next = next;

					if( next->closest_next != NULL && next->closest_next->closest_prev == next)
						next->closest_next->closest_prev = edge;

					edge->closest_next = next->closest_next;
					next->closest_next = edge;
					next->closest_prev = edge->closest_prev;
					edge->closest_prev = next;

					edge->edge_canpassed = 0;
					next->edge_canpassed = 0;

					ASSERT( !Bracket_ShouldBeSwitched(next, edge) );

					if( next->closest_prev != NULL && next->closest_prev->closest_next == next )
						edge = next->closest_prev;
					else
						next = edge->closest_next;
				}
				else
				{
					edge = next;
					next = next->closest_next;
				}
			}
		}

		second = second->order_next;
	}

	if( was )
		PositionAll_StoreY();

	return was;
}

