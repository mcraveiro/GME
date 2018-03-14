// AutoRouterGraph.cpp : Implementation of CAutoRouterGraph

#include "stdafx.h"
#include "AutoRouterGraph.h"
#include "AutoRouterBox.h"
#include "AutoRouterPort.h"
#include "AutoRouterPath.h"
#include "AutoRouterEdge.h"


// --- CAutoRouterGraph

CAutoRouterGraph::CAutoRouterGraph():
	horizontal(true),
	vertical(false)
{
	horizontal.SetOwner(this);
	vertical.SetOwner(this);

	CalculateSelfPoints();
	AddSelfEdges();
}

// --- Boxes

void CAutoRouterGraph::Remove(CAutoRouterBox* box)
{
	DeleteBoxAndPortEdges(box);

	std::vector<CAutoRouterPath*>::iterator iter;
	iter = paths.begin();
	
	while(iter != paths.end())
	{
		bool iteratorChanged = false;

		CAutoRouterPath* path = *iter;

		CAutoRouterPort* startPort = path->GetStartPort();
		ASSERT(startPort != NULL);
		CAutoRouterBox* startbox = startPort->GetOwner();

		CAutoRouterPort* endPort = path->GetEndPort();
		ASSERT(endPort != NULL);
		CAutoRouterBox* endbox = endPort->GetOwner();

		if( (startbox == box || endbox == box) )
		{
			//DeletePath:
			if (path->HasOwner())
			{
				DeleteEdges(path);
				path->SetOwner(NULL);

				iter = paths.erase(iter);
				iteratorChanged = true;
			}

			path->Destroy();	// ??
		}

		if (!iteratorChanged)
			++iter;
	}

	box->SetOwner(NULL);

	std::vector<CAutoRouterBox*>::iterator iter2 = std::find(boxes.begin(), boxes.end(), box);

	if (iter2 != boxes.end())
	{
		boxes.erase(iter2);
	}
	else
	{
		//error
		ASSERT(false);
	}
}

void CAutoRouterGraph::DeleteAllBoxes()
{
	for (CAutoRouterBoxList::size_type i = 0; i < boxes.size(); i++)
	{
		//DeleteBoxAndPortEdges(boxes[i]);	// no need: there's a DeleteAllEdges in DeleteAll
		boxes[i]->Destroy();
		delete boxes[i];
	}

	boxes.clear();
}

CAutoRouterBox* CAutoRouterGraph::GetBoxAt(const CPoint& point, int nearness) const
{
	std::vector<CAutoRouterBox*>::const_iterator iter = boxes.begin();
	while (iter != boxes.end())
	{
		if ((*iter)->IsBoxAt(point, nearness))
			return (*iter);

		++iter;
	}

	return NULL;
}

void CAutoRouterGraph::SetPortAttr(CAutoRouterPort* port, unsigned int attr)
{
	DisconnectPathsFrom(port);
	port->SetAttributes(attr);
}

bool CAutoRouterGraph::IsRectClipBoxes(const CRect& rect) const
{
	for (CAutoRouterBoxList::size_type i = 0; i < boxes.size(); i++)
	{
		const CRect boxRect = boxes[i]->GetRect();
		if( IsRectClip(rect, boxRect) )
			return true;
	}
	return false;
}

bool CAutoRouterGraph::IsLineClipBoxes(const CPoint& p1, const CPoint& p2) const
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

bool CAutoRouterGraph::CanBoxAt(const CRect& rect) const
{
	return !IsRectClipBoxes(InflatedRect(rect, 1));
}

void CAutoRouterGraph::Add(CAutoRouterPath* path)
{
	ASSERT( path != NULL );
	ASSERT(!path->HasOwner());

	path->SetOwner(this);

	paths.push_back(path);

	AddEdges(path);

#ifdef _DEBUG
	AssertValidPath(path);
#endif
}

void CAutoRouterGraph::Remove(CAutoRouterPath* path)
{
	DeleteEdges(path);

	path->SetOwner(NULL);

	std::vector<CAutoRouterPath*>::iterator iter = std::find(paths.begin(), paths.end(), path);

	if (iter != paths.end())
	{
		paths.erase(iter);
	}
	else
	{
		//error
		ASSERT(false);
	}
}

void CAutoRouterGraph::DeleteAllPaths()
{
	std::vector<CAutoRouterPath*>::iterator iter;
	iter = paths.begin();

	while (iter != paths.end())
	{
		//DeleteEdges(*iter);	// no need: there's a DeleteAllEdges in DeleteAll

		(*iter)->SetOwner(NULL);
		(*iter)->Destroy();
		delete (*iter);
		++iter;
	}

	paths.clear();
}

CAutoRouterEdge* CAutoRouterGraph::GetListEdgeAt(const CPoint& point, int nearness) const
{
	CAutoRouterEdge* edge;

	edge = horizontal.GetEdgeAt(point, nearness);
	if( edge )
		return edge;

	return vertical.GetEdgeAt(point, nearness);
}

// --- Boxes && Paths

CRect CAutoRouterGraph::GetSurroundRect(void) const
{
	CRect rect(0,0,0,0);

	for (CAutoRouterBoxList::size_type i = 0; i < boxes.size(); i++)
	{
		rect |= boxes[i]->GetRect();
	}

	for (CAutoRouterPathList::size_type i = 0; i < paths.size(); i++)
	{
		rect |= paths[i]->GetSurroundRect();
	}

	return rect;
}

CAutoRouterBox* CAutoRouterGraph::GetOutOfBox(CPoint& point, RoutingDirection dir) const
{
	ASSERT( IsRightAngle(dir) );

	CAutoRouterBox* boxby = NULL;

	std::vector<CAutoRouterBox*>::const_iterator iter = boxes.begin();

	while (iter != boxes.end())
	{
		const CRect boxRect = (*iter)->GetRect();
		if( boxRect.PtInRect(point) )
		{
			boxby = *iter;
			iter = boxes.begin();

			GetPointCoord(point, dir) = GetRectOuterCoord(boxRect, dir);
		}
		++iter;
	}

	return boxby;
}

CAutoRouterBox* CAutoRouterGraph::GoToNextBox(CPoint& point, RoutingDirection dir, long stophere) const
{
	ASSERT( IsRightAngle(dir) );
	ASSERT( GetPointCoord(point, dir) != stophere );

	CAutoRouterBox* boxby = NULL;

	std::vector<CAutoRouterBox*>::const_iterator iter = boxes.begin();

	while (iter != boxes.end())
	{
		const CRect boxRect = (*iter)->GetRect();
		if( IsPointInDirFrom(point, boxRect, ReverseDir(dir)) &&
			IsPointBetweenSides(point, boxRect, dir) &&
			IsCoordInDirFrom(stophere, GetRectOuterCoord(boxRect, ReverseDir(dir)), dir) )
		{
			stophere = GetRectOuterCoord(boxRect, ReverseDir(dir));
			boxby = *iter;
		}
		++iter;
	}

	GetPointCoord(point, dir) = stophere;

	return boxby;
}

void CAutoRouterGraph::GetLimitsOfEdge(const CPoint& startPt, const CPoint& endPt, long& min, long& max) const
{
	long t;
	CPoint start = startPt;
	CPoint end = endPt;

	std::vector<CAutoRouterBox*>::const_iterator iter = boxes.begin();

	if( start.y == end.y )
	{
		if( start.x > end.x )
		{
			t = start.x;
			start.x = end.x;
			end.x = t;
		}

		while( iter != boxes.end())
		{
			const CRect rect = (*iter)->GetRect();
			++iter;

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

		while( iter != boxes.end())
		{
			const CRect rect = (*iter)->GetRect();
			++iter;

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

bool CAutoRouterGraph::Connect(CAutoRouterPath* path)
{
	CAutoRouterPort* startport = path->GetStartPort();
	CAutoRouterPort* endport = path->GetEndPort();

	RoutingDirection startdir = path->GetStartDir();
	bool startportHasLimited = false;
	bool startportCanHave = true;
	if (startdir != Dir_None) {
		startportHasLimited = startport->HasLimitedDirs();
		startportCanHave = startport->CanHaveStartEndPointOn(startdir, true);
	}
	if( startdir == Dir_None ||							// recalc startdir if empty
		startportHasLimited && !startportCanHave)		// or is limited and userpref is invalid
	{
		startdir = startport->GetStartEndDirTo(endport->GetCenter(), true);
	}

	RoutingDirection enddir = path->GetEndDir();
	bool endportHasLimited = false;
	bool endportCanHave = true;
	if (enddir != Dir_None) {
		endportHasLimited = endport->HasLimitedDirs();
		endportCanHave = endport->CanHaveStartEndPointOn(enddir, false);
	}
	if( enddir == Dir_None ||							// like above
		endportHasLimited && !endportCanHave)
	{
		enddir = endport->GetStartEndDirTo(startport->GetCenter(), false, startport == endport ? startdir : Dir_None );
	}

	CPoint startpoint = startport->CreateStartEndPointOn(startdir);
	CPoint endpoint = endport->CreateStartEndPointOn(enddir);

	if( startpoint == endpoint )
		startpoint = StepOneInDir(startpoint, NextClockwiseDir(startdir));

	return Connect(path, startpoint, endpoint);
}

bool CAutoRouterGraph::Connect(CAutoRouterPath* path, CPoint& startpoint, CPoint& endpoint)
{
	ASSERT( path != NULL && path->GetOwner() == this );
	ASSERT( !path->IsConnected() );
	ASSERT( startpoint != endpoint );

	CAutoRouterPort* startPort = path->GetStartPort();
	ASSERT(startPort != NULL);
	RoutingDirection startdir = startPort->OnWhichEdge(startpoint);

	CAutoRouterPort* endPort = path->GetEndPort();
	ASSERT(endPort != NULL);
	RoutingDirection enddir = endPort->OnWhichEdge(endpoint);
	ASSERT( IsRightAngle(startdir) && IsRightAngle(enddir) );

	CPoint start = startpoint;
	GetOutOfBox(start, startdir);
	ASSERT( start != startpoint );

	CPoint end = endpoint;
	GetOutOfBox(end, enddir);
	ASSERT( end != endpoint );

	ASSERT( path->IsEmpty() );

	CPointListPath ret;
	bool isAutoRouted = path->IsAutoRouted();
	if (isAutoRouted)
		ConnectPoints(ret, start, end, startdir, enddir);

	if (!isAutoRouted)
	{
		CPointListPath ret2;
		path->ApplyCustomizationsBeforeAutoConnectPoints(ret2);

		if (ret2.GetCount() > 0)
		{
			ret.RemoveAll();
			POSITION pos = ret2.GetHeadPosition();
			while( pos != NULL )
			{
				ret.AddTail(ret2.GetNext(pos));
			}
		}
	}

	path->DeleteAll();

	path->AddTail(startpoint);
	POSITION pos = ret.GetHeadPosition();
	while( pos != NULL )
	{
		CPoint p = ret.GetNext(pos);
		path->AddTail(p);
	}
	path->AddTail(endpoint);

	if (isAutoRouted) {
		path->SimplifyTrivially();
		SimplifyPathPoints(path);
		CenterStairsInPathPoints(path, startdir, enddir);
	}
	path->SetState(ARPATHST_Connected);

	// Apply custom edge modifications - step 1
	// (Step 1: Move the desired edges - see in CAutoRouterGraph::Connect(CAutoRouterPath* path, CPoint& startpoint, CPoint& endpoint)
	//  Step 2: Fix the desired edges - see in CAutoRouterEdgeList::AddEdges(CAutoRouterPath* path))
	if (isAutoRouted)
		path->ApplyCustomizationsAfterAutoConnectPointsAndStuff();

	return AddEdges(path);
}

void CAutoRouterGraph::ConnectPoints(CPointListPath& ret, CPoint& start, CPoint& end, RoutingDirection hintstartdir, RoutingDirection hintenddir)
{
	ASSERT( ret.IsEmpty() );

	CPoint& thestart = start;
	POSITION retend = NULL;

	while( start != end )
	{
		RoutingDirection dir1 = ExGetMajorDir(end-start);
		RoutingDirection dir2 = ExGetMinorDir(end-start);
		ASSERT( dir1 != Dir_None );

		ASSERT( dir1 == GetMajorDir(end-start) );
		ASSERT( dir2 == Dir_None || dir2 == GetMinorDir(end-start) );

		if( retend == NULL && dir2 == hintstartdir && dir2 != Dir_None )
		{
			// i.e. std::swap(dir1, dir2);
			dir2 = dir1;
			dir1 = hintstartdir;
		}

		if (retend == NULL)
			retend = ret.AddTail(start);
		else
			retend = ret.InsertAfter(retend, start);
		CPoint old = start;

		CAutoRouterBox* box = GoToNextBox(start, dir1, end);
		if( start == old )
		{
			ASSERT( box != NULL );
			const CRect rect = box->GetRect();

			if( dir2 == Dir_None )
				dir2 = NextClockwiseDir(dir1);

			ASSERT( dir1 != dir2 && dir1 != Dir_None && dir2 != Dir_None );

			if( IsPointInDirFrom(end, rect, dir2) )
			{
				ASSERT( !IsPointInDirFrom(start, rect, dir2) );
				GoToNextBox(start, dir2, end);
				// this assert fails if two boxes are adjacent, and a connection wants to go between
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
						if(	IsPointInDirFrom(rect.TopLeft() + rect.BottomRight(), start + end, dir2) )
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
				ASSERT(retend != NULL);
				retend = ret.InsertAfter(retend, start);

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

void CAutoRouterGraph::DisconnectAll()
{
	std::vector<CAutoRouterPath*>::iterator iter;
	iter = paths.begin();
	
	while(iter != paths.end())
	{
		Disconnect(*iter);
		++iter;
	}
}

void CAutoRouterGraph::Disconnect(CAutoRouterPath* path)
{
	if( path->IsConnected() )
		DeleteEdges(path);

	path->DeleteAll();
}

void CAutoRouterGraph::DisconnectPathsClipping(const CRect& rect)
{
	std::vector<CAutoRouterPath*>::reverse_iterator iter;
	iter = paths.rbegin();

	while(iter != paths.rend())
	{
		if( (*iter)->IsPathClip(rect) )
			Disconnect(*iter);
		++iter;
	}
}

void CAutoRouterGraph::DisconnectPathsFrom(CAutoRouterBox* box)
{
	std::vector<CAutoRouterPath*>::reverse_iterator iter;
	iter = paths.rbegin();

	while(iter != paths.rend())
	{
		CAutoRouterPath* path = *iter;

		CAutoRouterPort* startPort = path->GetStartPort();
		ASSERT(startPort != NULL);
		CAutoRouterBox* startbox = startPort->GetOwner();
		ASSERT(startbox != NULL);

		CAutoRouterPort* endPort = path->GetEndPort();
		ASSERT(endPort != NULL);
		CAutoRouterBox* endbox = endPort->GetOwner();
		ASSERT(endbox != NULL);

		if( (startbox == box || endbox == box) )
			Disconnect(path);

		++iter;
	}
}

void CAutoRouterGraph::DisconnectPathsFrom(CAutoRouterPort* port)
{
	std::vector<CAutoRouterPath*>::reverse_iterator iter;
	iter = paths.rbegin();
	
	while(iter != paths.rend())
	{
		CAutoRouterPath* path = *iter;

		CAutoRouterPort* startport = path->GetStartPort();
		CAutoRouterPort* endport = path->GetEndPort();

		if( (startport == port || endport == port) )
			Disconnect(path);

		++iter;
	}
}

// --- Edges

void CAutoRouterGraph::AddSelfEdges(void)
{
	horizontal.AddEdges(this);
	vertical.AddEdges(this);
}

void CAutoRouterGraph::AddEdges(CAutoRouterGraph* graph)
{
	horizontal.AddEdges(graph);
	vertical.AddEdges(graph);
}

void CAutoRouterGraph::AddEdges(CAutoRouterBox* box)
{
	horizontal.AddEdges(box);
	vertical.AddEdges(box);
}

void CAutoRouterGraph::AddEdges(CAutoRouterPort* port)
{
	horizontal.AddEdges(port);
	vertical.AddEdges(port);
}

bool CAutoRouterGraph::AddEdges(CAutoRouterPath* path)
{
	return horizontal.AddEdges(path) && vertical.AddEdges(path);
}

void CAutoRouterGraph::DeleteEdges(CObject* object)
{
	horizontal.DeleteEdges(object);
	vertical.DeleteEdges(object);
}

void CAutoRouterGraph::AddAllEdges()
{
	ASSERT( horizontal.IsEmpty() && vertical.IsEmpty() );

	std::vector<CAutoRouterBox*>::iterator iter;
	iter = boxes.begin();

	while (iter != boxes.end())
	{
		AddBoxAndPortEdges(*iter);
		++iter;
	}

	std::vector<CAutoRouterPath*>::iterator iterP;
	iterP = paths.begin();

	while (iterP != paths.end())
	{
		AddEdges(*iterP);
		iterP++;
	}
}

void CAutoRouterGraph::DeleteAllEdges()
{
	horizontal.DeleteAllEdges();
	vertical.DeleteAllEdges();
}

void CAutoRouterGraph::AddBoxAndPortEdges(CAutoRouterBox* box)
{
	ASSERT( box != NULL );

	AddEdges(box);

	const CAutoRouterPortList& pl = box->GetPortList();
	std::vector<CAutoRouterPort*>::const_iterator ii = pl.begin();
	while( ii != pl.end() ) {
		AddEdges(*ii);
		++ii;
	}
}

void CAutoRouterGraph::DeleteBoxAndPortEdges(CAutoRouterBox* box)
{
	ASSERT( box != NULL );

	DeleteEdges(box);

	const CAutoRouterPortList& pl = box->GetPortList();
	std::vector<CAutoRouterPort*>::const_iterator ii = pl.begin();
	while( ii != pl.end() ) {
		DeleteEdges(*ii);
		++ii;
	}
}

CAutoRouterEdgeList& CAutoRouterGraph::GetEdgeList(bool ishorizontal)
{
	return ishorizontal ? horizontal : vertical;
}

// --- Path && Edges

bool CAutoRouterGraph::CanDeleteTwoEdgesAt(CAutoRouterPath* path, CPointListPath& points, POSITION pos) const
{
#ifdef _DEBUG
	ASSERT( path->GetOwner() == this );
	path->AssertValid();
	ASSERT( path->IsConnected() );
	points.AssertValidPos(pos);
#ifdef _DEBUG_DEEP
//	horizontal.AssertValidPathEdges(path, points);
//	vertical.AssertValidPathEdges(path, points);
#endif
#endif

	POSITION pointpos = pos;
	CPoint point = points.GetNext(pos);
	POSITION npointpos = pos;
	if( npointpos == NULL )
		return false;
	CPoint npoint = points.GetNext(pos);
	POSITION nnpointpos = pos;
	if( nnpointpos == NULL )
		return false;

	pos = pointpos;
	points.GetPrev(pos);
	POSITION ppointpos = pos; if( ppointpos == NULL ) return false;
	CPoint ppoint = points.GetPrev(pos);
	POSITION pppointpos = pos; if( pppointpos == NULL ) return false;
	if( npoint == point) return false; // direction of zero-length edges can't be determined, so don't delete them

	ASSERT( pppointpos != NULL && ppointpos != NULL && pointpos != NULL && npointpos != NULL && nnpointpos != NULL );

	RoutingDirection dir = GetDir(npoint - point);
	ASSERT( IsRightAngle(dir) );
	bool ishorizontal = IsHorizontal(dir);

	CPoint newpoint;
	GetPointCoord(newpoint, ishorizontal) = GetPointCoord(npoint, ishorizontal);
	GetPointCoord(newpoint, !ishorizontal) = GetPointCoord(ppoint, !ishorizontal);

	ASSERT( GetDir(newpoint - ppoint) == dir );

	if( IsLineClipBoxes(newpoint, npoint) ) return false;
	if( IsLineClipBoxes(newpoint, ppoint) ) return false;

	return true;
}

void CAutoRouterGraph::DeleteTwoEdgesAt(CAutoRouterPath* path, CPointListPath& points, POSITION pos)
{
#ifdef _DEBUG
	ASSERT( path->GetOwner() == this );
	path->AssertValid();
	ASSERT( path->IsConnected() );
	points.AssertValidPos(pos);
#ifdef _DEBUG_DEEP
//	horizontal.AssertValidPathEdges(path, points);
//	vertical.AssertValidPathEdges(path, points);
#endif
#endif

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

	RoutingDirection dir = GetDir(*npoint - *point);
	ASSERT( IsRightAngle(dir) );
	bool ishorizontal = IsHorizontal(dir);

	CPoint newpoint;
	GetPointCoord(newpoint, ishorizontal) = GetPointCoord(*npoint, ishorizontal);
	GetPointCoord(newpoint, !ishorizontal) = GetPointCoord(*ppoint, !ishorizontal);

	ASSERT( GetDir(newpoint - *ppoint) == dir );

	ASSERT( !IsLineClipBoxes(newpoint, *npoint) );
	ASSERT( !IsLineClipBoxes(newpoint, *ppoint) );

	CAutoRouterEdgeList& hlist = GetEdgeList(ishorizontal);
	CAutoRouterEdgeList& vlist = GetEdgeList(!ishorizontal);

	CAutoRouterEdge* ppedge = hlist.GetEdgeByPointer(pppoint, ppoint);
	CAutoRouterEdge* pedge = vlist.GetEdgeByPointer(ppoint, point);
	CAutoRouterEdge* nedge = hlist.GetEdgeByPointer(point, npoint);
	CAutoRouterEdge* nnedge = vlist.GetEdgeByPointer(npoint, nnpoint);

	ASSERT( ppedge != NULL && pedge != NULL && nedge != NULL && nnedge != NULL );

	vlist.Delete(pedge);
	hlist.Delete(nedge);

	points.RemoveAt(pointpos);
	points.RemoveAt(npointpos);
	points.SetAt(ppointpos, newpoint);

	ASSERT( ppedge->GetEndPoint() == *ppoint && ppedge->GetEndPointNext() == *point );
	ppedge->SetEndPointNext(nnpoint);

	ASSERT( nnedge->GetStartPoint() == *npoint && nnedge->GetStartPointPrev() == *point );
	nnedge->SetStartPoint(ppoint);
	nnedge->SetStartPointPrev(pppoint);

	if( nnnpointpos != NULL )
	{
		CAutoRouterEdge* nnnedge = hlist.GetEdgeByPointer(nnpoint, &(points.GetAt(nnnpointpos)));
		ASSERT( nnnedge != NULL );
		ASSERT( nnnedge->GetStartPointPrev() == *npoint && nnnedge->GetStartPoint() == *nnpoint );
		nnnedge->SetStartPointPrev(ppoint);
	}

	if( *nnpoint == newpoint )
		DeleteSamePointsAt(path, points, ppointpos);

#ifdef _DEBUG_DEEP
	path->AssertValid();
	horizontal.AssertValidPathEdges(path, points);
	vertical.AssertValidPathEdges(path, points);
#endif
}

void CAutoRouterGraph::DeleteSamePointsAt(CAutoRouterPath* path, CPointListPath& points, POSITION pos)
{
#ifdef _DEBUG
	ASSERT( path->GetOwner() == this );
	path->AssertValid();
	ASSERT( path->IsConnected() );
	points.AssertValidPos(pos);
#endif

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

	RoutingDirection dir = GetDir(*point - *ppoint);
	ASSERT( IsRightAngle(dir) );
	bool ishorizontal = IsHorizontal(dir);

	CAutoRouterEdgeList& hlist = GetEdgeList(ishorizontal);
	CAutoRouterEdgeList& vlist = GetEdgeList(!ishorizontal);

	CAutoRouterEdge* pedge = hlist.GetEdgeByPointer(ppoint, point);
	CAutoRouterEdge* nedge = vlist.GetEdgeByPointer(point, npoint);
	CAutoRouterEdge* nnedge = hlist.GetEdgeByPointer(npoint, nnpoint);

	ASSERT( pedge != NULL && nedge != NULL && nnedge != NULL );

	vlist.Delete(pedge);
	hlist.Delete(nedge);

	points.RemoveAt(pointpos);
	points.RemoveAt(npointpos);

	if( pppointpos != NULL )
	{
		CAutoRouterEdge* ppedge = vlist.GetEdgeByPointer(pppoint, ppoint);
		ASSERT( ppedge != NULL && ppedge->GetEndPoint() == *ppoint && ppedge->GetEndPointNext() == *point );
		ppedge->SetEndPointNext(nnpoint);
	}

	ASSERT( nnedge->GetStartPoint() == *npoint && nnedge->GetStartPointPrev() == *point );
	nnedge->SetStartPoint(ppoint);
	nnedge->SetStartPointPrev(pppoint);

	if( nnnpointpos != NULL )
	{
		CAutoRouterEdge* nnnedge = vlist.GetEdgeByPointer(nnpoint, &(points.GetAt(nnnpointpos)));
		ASSERT( nnnedge != NULL && nnnedge->GetStartPointPrev() == *npoint && nnnedge->GetStartPoint() == *nnpoint );
		nnnedge->SetStartPointPrev(ppoint);
	}

#ifdef _DEBUG_DEEP
	path->AssertValid();
//	horizontal.AssertValidPathEdges(path, points);
//	vertical.AssertValidPathEdges(path, points);
#endif
}

bool CAutoRouterGraph::SimplifyPaths()
{
	bool was = false;

	std::vector<CAutoRouterPath*>::iterator iter;
	iter = paths.begin();

	while (iter != paths.end())
	{
		CAutoRouterPath* path = *iter;
		++iter;

		if (path->IsAutoRouted()) {
			CPointListPath& pointList = path->GetPointList();
			POSITION pointpos = pointList.GetHeadPosition();

			while( pointpos != NULL )
			{
				if( CanDeleteTwoEdgesAt(path, pointList, pointpos) )
				{
					DeleteTwoEdgesAt(path, pointList, pointpos);
					was = true;
					break;
				}
				pointList.GetNext(pointpos);
			}
		}
	}

	return was;
}

void CAutoRouterGraph::CenterStairsInPathPoints(CAutoRouterPath* path, RoutingDirection hintstartdir, RoutingDirection hintenddir)
{
	ASSERT( path != NULL );
	ASSERT( !path->IsConnected() );

	CPointListPath& pointList = path->GetPointList();
	ASSERT( pointList.GetCount() >= 2 );

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

	RoutingDirection d12 = Dir_None;
	RoutingDirection d23 = Dir_None;
	RoutingDirection d34 = Dir_None;

	const CPoint outOfBoxStartPoint = path->GetOutOfBoxStartPoint(hintstartdir);
	const CPoint outOfBoxEndPoint = path->GetOutOfBoxEndPoint(hintenddir);

	POSITION pos = pointList.GetHeadPosition();
	ASSERT( pos != NULL );

	p1p = pos;
	p1 = pointList.GetNext(pos);

	while( pos != NULL )
	{
		p4p = p3p;
		p3p = p2p;
		p2p = p1p;
		p1p = pos;

		p4 = p3;
		p3 = p2;
		p2 = p1;
		p1 = pointList.GetNext(pos);

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
			bool h = IsHorizontal(d12);

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
					!IsLineClipBoxes(p1p == pointList.GetTailPosition() ? outOfBoxEndPoint : p1, np2) &&
					!IsLineClipBoxes(p4p == pointList.GetHeadPosition() ? outOfBoxStartPoint : p4, np3) )
				{
					p2 = np2;
					p3 = np3;
					pointList.SetAt(p2p, p2);
					pointList.SetAt(p3p, p3);
				}
			}
		}
	}

#ifdef _DEBUG
	path->AssertValidPoints();
#endif
}

void CAutoRouterGraph::SimplifyPathPoints(CAutoRouterPath* path)
{
	ASSERT( path != NULL );
	ASSERT( !path->IsConnected() );

	CPointListPath& pointList = path->GetPointList();
	ASSERT( pointList.GetCount() >= 2 );

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

	POSITION pos = pointList.GetHeadPosition();
	ASSERT( pos != NULL );

	p1p = pos;
	p1 = pointList.GetNext(pos);

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
		p1 = pointList.GetNext(pos);

		if( p5p != NULL )
		{
			ASSERT( p1p != NULL && p2p != NULL && p3p != NULL && p4p != NULL && p5p != NULL );
			ASSERT( p1 != p2 && p2 != p3 && p3 != p4 && p4 != p5 );

			RoutingDirection d = GetDir(p2 - p1);
			ASSERT( IsRightAngle(d) );
			bool h = IsHorizontal(d);

			CPoint np3;
			GetPointCoord(np3, h) = GetPointCoord(p5, h);
			GetPointCoord(np3, !h) = GetPointCoord(p1, !h);

			if( !IsLineClipBoxes(p2, np3) && !IsLineClipBoxes(np3, p4) )
			{
				pointList.RemoveAt(p2p);
				pointList.RemoveAt(p4p);
				pointList.SetAt(p3p, np3);
				if( np3 == p1 )
					pointList.RemoveAt(p1p);
				if( np3 == p5 )
					pointList.RemoveAt(p5p);

				p1p = NULL;
				p2p = NULL;
				p3p = NULL;
				p4p = NULL;

				pos = pointList.GetHeadPosition();
			}
		}
	}

#ifdef _DEBUG
	path->AssertValidPoints();
#endif
}

void CAutoRouterGraph::ConnectAllDisconnectedPaths()
{
	std::vector<CAutoRouterPath*>::iterator iter;

	bool success = false;
	bool giveup = false;
	while (!success && !giveup) {
		success = true;
		iter = paths.begin();
		while (iter != paths.end() && success)
		{
			CAutoRouterPath* path = *iter;

			if( !path->IsConnected() )
			{
				success = Connect(path);
				if (!success) {
					// Something is messed up, probably an existing edge customization results in a zero length edge
					// In that case we try to delete any customization for this path to recover from the problem
					if (path->AreTherePathCustomizations())
						path->RemovePathCustomizations();
					else
						giveup = true;
				}
			}

			++iter;
		}
		if (!success && !giveup)
			DisconnectAll();	// There was an error, delete halfway results to be able to start a new pass
	}
}

// --- SelfPoints

void CAutoRouterGraph::CalculateSelfPoints()
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

CAutoRouterBox* CAutoRouterGraph::CreateBox(void) const
{
	CAutoRouterBox* box = new CAutoRouterBox();
	ASSERT( box != NULL );

	return box;
}

void CAutoRouterGraph::AddBox(CAutoRouterBox* box)
{
	ASSERT(box != NULL);
	if (box == NULL)
		return;

	const CRect rect = box->GetRect();

	DisconnectPathsClipping(rect);

	box->SetOwner(this);

	boxes.push_back(box);

	AddBoxAndPortEdges(box);
}

void CAutoRouterGraph::DeleteBox(CAutoRouterBox* box)
{
	ASSERT(box != NULL);
	if (box == NULL)
		return;

	if( box->HasOwner() )
	{
		Remove(box);
	}
	
	box->Destroy();
	delete box;
}

void CAutoRouterGraph::ShiftBoxBy(CAutoRouterBox* box, const CPoint& offset)
{
	ASSERT(box != NULL);
	if (box == NULL)
		return;

	DeleteBoxAndPortEdges(box);
	box->ShiftBy(offset);
	AddBoxAndPortEdges(box);

	const CRect rect = box->GetRect();
	DisconnectPathsClipping(rect);
	DisconnectPathsFrom(box);
}

long CAutoRouterGraph::AutoRoute(long aspect)
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

				do {
					c--;
				} while( c > 0 && horizontal.Block_ScanBackward() );

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

				do {
					c--;
				} while( c > 0 && horizontal.Block_ScanForward() );

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

	// Check customized connection if there's any clip against boxes
	{
		std::vector<CAutoRouterPath*>::iterator pathiter;
		pathiter = paths.begin();

		HRESULT hr = S_OK;
		while (pathiter != paths.end())
		{
			CAutoRouterPath* path = *pathiter;

			if (path->IsAutoRouted()) {	// comment this if you want the check to run for fully customizable connections
				if (path->AreTherePathCustomizations())
				{
					const CRect startBoxRect = path->GetStartBox();
					const CRect endBoxRect = path->GetEndBox();

					std::vector<CAutoRouterBox*>::const_iterator boxiter = boxes.begin();

					while (boxiter != boxes.end())
					{
						const CRect boxRect = (*boxiter)->GetRect();
						bool isStartOrEndRect = (!startBoxRect.IsRectEmpty() && IsRectIn(startBoxRect, boxRect) ||
												 !endBoxRect.IsRectEmpty() && IsRectIn(endBoxRect, boxRect));
						if (path->IsPathClip(boxRect, isStartOrEndRect))
						{
							path->MarkPathCustomizationsForDeletion(aspect);
							updated = -2;
						}

						++boxiter;
					}
				}
			}

			++pathiter;
		}
	}

	return updated;
}

void CAutoRouterGraph::DeletePath(CAutoRouterPath* path)
{
	ASSERT(path != NULL);
	if (path == NULL)
		return;

	if( path->HasOwner() )
	{
		ASSERT( path->GetOwner() == this );

		Remove(path);
	}

	path->Destroy();
	delete path;
}

void CAutoRouterGraph::DeleteAll(bool addBackSelfEdges)
{
	DeleteAllPaths();
	DeleteAllBoxes();
	DeleteAllEdges();
	if (addBackSelfEdges)
		AddSelfEdges();
}

CAutoRouterPath* CAutoRouterGraph::GetPathAt(const CPoint& point, long nearness)
{
	std::vector<CAutoRouterPath*>::iterator iter;
	iter = paths.begin();

	while (iter != paths.end())
	{
		CAutoRouterPath* path = *iter;

		if( path->IsPathAt(point, nearness) )
			return path;

		++iter;
	}

	return NULL;
}

CAutoRouterPath* CAutoRouterGraph::AddPath(bool isAutoRouted, CAutoRouterPort* startport, CAutoRouterPort* endport)
{
	CAutoRouterPath* path = new CAutoRouterPath();

	path->SetAutoRouting(isAutoRouted);
	path->SetStartPort(startport);
	path->SetEndPort(endport);
	Add(path);

	return path;
}

bool CAutoRouterGraph::IsEdgeFixed(CAutoRouterPath* path, const CPoint& startpoint, const CPoint& endpoint)
{
	RoutingDirection d = GetDir(endpoint - startpoint);
	bool h = IsHorizontal(d);

	CAutoRouterEdgeList& elist = GetEdgeList(h);

	CAutoRouterEdge* edge = elist.GetEdge(path, startpoint, endpoint);
	if (edge != NULL)
		return edge->GetEdgeFixed() && !edge->GetEdgeCustomFixed();

	ASSERT(false);
	return true;
}

CPoint* CAutoRouterGraph::GetSelfPoints(void) const
{
	return (CPoint*)selfpoints;
}

void CAutoRouterGraph::Destroy(void)
{
	DeleteAll(false);

	horizontal.SetOwner(NULL);
	vertical.SetOwner(NULL);
}

#ifdef _DEBUG
void CAutoRouterGraph::AssertValid() const
{
	std::vector<CAutoRouterBox*>::const_iterator iter = boxes.begin();

	while (iter != boxes.end())
	{
		AssertValidBox(*iter);
		++iter;
	}

	std::vector<CAutoRouterPath*>::const_iterator iter2 = paths.begin();
	
	while(iter2 != paths.end())
	{
		AssertValidPath(*iter2);
		++iter2;
	}
}

void CAutoRouterGraph::AssertValidBox(CAutoRouterBox* box) const
{
	box->AssertValid();
	ASSERT( box->GetOwner() == this );

	std::vector<CAutoRouterBox*>::const_iterator iter = std::find(boxes.begin(), boxes.end(), box);
	ASSERT (iter != boxes.end());
}

void CAutoRouterGraph::AssertValidPath(CAutoRouterPath* path) const
{
	path->AssertValid();
	ASSERT( path->GetOwner() == this );

	std::vector<CAutoRouterPath*>::const_iterator iter = std::find(paths.begin(), paths.end(), path);
	ASSERT (iter != paths.end());

	CPointListPath& pointList = path->GetPointList();

	CAutoRouterPort* startPort = path->GetStartPort();
	ASSERT(startPort != NULL);
	startPort->AssertValid();
	CAutoRouterBox* ownerBox = startPort->GetOwner();
	CAutoRouterGraph* boxOwnerGraph = ownerBox->GetOwner();
	ASSERT( boxOwnerGraph == this );
	ownerBox->AssertValidPort(startPort);

	if( path->IsConnected() )
		startPort->AssertValidStartEndPoint(pointList.GetHead(), Dir_None, 1);

	CAutoRouterPort* endPort = path->GetEndPort();
	ASSERT(endPort != NULL);
	endPort->AssertValid();
	CAutoRouterBox* ownerBox2 = endPort->GetOwner();
	ASSERT( ownerBox2->GetOwner() == this );
	ownerBox2->AssertValidPort(endPort);

	if( path->IsConnected() )
	{
		endPort->AssertValidStartEndPoint(pointList.GetTail(), Dir_None, 0);
	}
	else
	{
		ASSERT( path->HasNoPoint() );
	}

	path->AssertValidPoints();

	if( !pointList.IsEmpty() )
	{
		ASSERT( pointList.GetCount() >= 2 );
		POSITION pos = pointList.GetHeadPosition();
		ASSERT( pos != NULL );

		ASSERT( IsPointInBox(pointList.GetNext(pos)) );

		while( pos != NULL )
		{
			CPoint p = pointList.GetNext(pos);
			if( pos != NULL )
				ASSERT( !IsPointInBox(p) );
			else
				ASSERT( IsPointInBox(p) );
		}
	}
}

void CAutoRouterGraph::DumpPaths(int pos, int c)
{
	TRACE2("Paths dump pos %ld, c %ld:\n", pos, c);
	std::vector<CAutoRouterPath*>::iterator iter;
	iter = paths.begin();
	int i = 0;

	while (iter != paths.end())
	{
		TRACE1("%ld. Path:\n", i);
		(*iter)->GetPointList().DumpPoints("DumpPaths");

		++iter;
		i++;
	}

	DumpEdgeLists();
}

void CAutoRouterGraph::DumpEdgeLists(void)
{
#ifdef _DEBUG_DEEP
	horizontal.DumpEdges("Horizontal edges:");
	vertical.DumpEdges("Vertical edges:");
#endif
}
#endif

// CAutoRouterGraph
