// AutoRouterPort.cpp : Implementation of CAutoRouterPort
#include "stdafx.h"
#include "AutoRouterPort.h"
#include "AutoRouterBox.h"


CAutoRouterPort::CAutoRouterPort():	owner(NULL),
									limitedDirections( false),
									rect(0,0,0,0),
									attributes(ARPORT_Default)
{
	CalculateSelfPoints();
}

CAutoRouterPort::~CAutoRouterPort()
{
	this->SetOwner(NULL);
}

// Private methods

// --- Data

void CAutoRouterPort::CalculateSelfPoints()
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

CAutoRouterBox* CAutoRouterPort::GetOwner(void) const
{
	return owner;
}

bool CAutoRouterPort::HasOwner(void) const
{
	return owner != NULL;
}

void CAutoRouterPort::SetOwner(CAutoRouterBox* box)
{
	owner = box;
}

CRect CAutoRouterPort::GetRect(void) const
{
	return rect;
}

bool CAutoRouterPort::IsRectEmpty(void) const
{
	return rect.IsRectEmpty() == TRUE;
}

CPoint CAutoRouterPort::GetCenter(void) const
{
	return rect.CenterPoint();
}

void CAutoRouterPort::SetRect(const CRect& r)
{
	ASSERT( r.Width() >= 3 && r.Height() >= 3 );

	rect = r;
	CalculateSelfPoints();
}

void CAutoRouterPort::ShiftBy(const CPoint& offset)
{
	ASSERT( !rect.IsRectEmpty() );

	rect += offset;

	CalculateSelfPoints();
}

CPoint* CAutoRouterPort::GetSelfPoints(void) const
{
	return (CPoint*)selfpoints;
}

long CAutoRouterPort::GetAttributes(void) const
{
	return attributes;
}

void CAutoRouterPort::SetAttributes(long attr)
{
	attributes = (unsigned int)attr;
}

bool CAutoRouterPort::IsConnectToCenter(void) const
{
	return ((attributes & ARPORT_ConnectToCenter) != 0);
}

bool CAutoRouterPort::HasLimitedDirs(void) const
{
	return limitedDirections;
}

void CAutoRouterPort::SetLimitedDirs(bool ltd)
{
	limitedDirections = ltd;
}

bool CAutoRouterPort::IsPortAt(const CPoint& point, long nearness) const
{
	return IsPointIn(point, rect, nearness);
}

bool CAutoRouterPort::IsPortClip(const CRect& r) const
{
	return IsRectClip(rect, r);
}

bool CAutoRouterPort::IsPortIn(const CRect& r) const
{
	return IsRectIn(rect, r);
}

RoutingDirection CAutoRouterPort::OnWhichEdge(const CPoint& p) const
{
	return ::OnWhichEdge(rect, p);
}

bool CAutoRouterPort::CanHaveStartEndPointOn(RoutingDirection dir, bool isStart) const
{
	int d = (int) dir;
	ASSERT( 0 <= d && d <= 3 );

	if( isStart )
		d += 4;

	return ((attributes & (1 << d)) != 0);
}

bool CAutoRouterPort::CanHaveStartEndPoint(bool isStart) const
{
	return ((attributes & (isStart ? ARPORT_StartOnAll : ARPORT_EndOnAll)) != 0);
}

bool CAutoRouterPort::CanHaveStartEndPointHorizontal(bool ishorizontal) const
{
	return ((attributes & (ishorizontal ? ARPORT_StartEndHorizontal : ARPORT_StartEndVertical)) != 0);
}

RoutingDirection CAutoRouterPort::GetStartEndDirTo(const CPoint& point, bool isStart, RoutingDirection notthis) const
{
	ASSERT( !rect.IsRectEmpty() );

	CSize offset = point - rect.CenterPoint();

	bool canHave = false;

	RoutingDirection dir1 = GetMajorDir(offset);

	if (dir1 != notthis && CanHaveStartEndPointOn(dir1, isStart))
		return dir1;

	RoutingDirection dir2 = GetMinorDir(offset);

	if (dir2 != notthis && CanHaveStartEndPointOn(dir2, isStart))
		return dir2;

	RoutingDirection dir3 = ReverseDir(dir2);

	if (dir3 != notthis && CanHaveStartEndPointOn(dir3, isStart))
		return dir3;

	RoutingDirection dir4 = ReverseDir(dir1);

	if (dir4 != notthis && CanHaveStartEndPointOn(dir4, isStart))
		return dir4;

	if (CanHaveStartEndPointOn(dir1, isStart))
		return dir1;

	if (CanHaveStartEndPointOn(dir2, isStart))
		return dir2;

	if (CanHaveStartEndPointOn(dir3, isStart))
		return dir3;

	if (CanHaveStartEndPointOn(dir4, isStart))
		return dir4;

	return Dir_Top;
}

bool CAutoRouterPort::CanCreateStartEndPointAt(const CPoint& point, bool isStart, long nearness) const
{
	return CanHaveStartEndPoint(isStart) && IsPointIn(point, rect, nearness);
}

CPoint CAutoRouterPort::CreateStartEndPointAt(const CPoint& p, bool isStart) const
{
	ASSERT( !rect.IsRectEmpty() );

	CPoint point = p;

	RoutingDirection dir = Dir_None;

	CArFindNearestLine nearest(point);

	VARIANT_BOOL canHave = VARIANT_FALSE;

	if (CanHaveStartEndPointOn(Dir_Top, isStart) && nearest.HLine(rect.left, rect.right - 1, rect.top))
		dir = Dir_Top;

	if (CanHaveStartEndPointOn(Dir_Right, isStart) && nearest.VLine(rect.top, rect.bottom - 1, rect.right - 1))
		dir = Dir_Right;

	if (CanHaveStartEndPointOn(Dir_Bottom, isStart) && nearest.HLine(rect.left, rect.right - 1, rect.bottom - 1))
		dir = Dir_Bottom;

	if (CanHaveStartEndPointOn(Dir_Left, isStart) && nearest.VLine(rect.top, rect.bottom - 1, rect.left))
		dir = Dir_Left;

	ASSERT( IsRightAngle(dir) );

	if (IsConnectToCenter())
		return CreateStartEndPointOn(dir);

	if( point.x < rect.left )
		point.x = rect.left;
	else if( rect.right <= point.x )
		point.x = rect.right - 1;

	if( point.y < rect.top )
		point.y = rect.top;
	else if( rect.bottom <= point.y )
		point.y = rect.bottom - 1;

	switch(dir)
	{
	case Dir_Top:
		point.y = rect.top;
		break;

	case Dir_Right:
		point.x = rect.right - 1;
		break;

	case Dir_Bottom:
		point.y = rect.bottom - 1;
		break;

	case Dir_Left:
		point.x = rect.left;
		break;
	}

	return point;
}

static inline LONG RoundToHalfGrid(LONG left, LONG right)
{
	return ((right + left) / 2) / GME_GRID_SIZE * GME_GRID_SIZE + (GME_GRID_SIZE / 2);
}

CPoint CAutoRouterPort::CreateStartEndPointOn(RoutingDirection dir) const
{
	ASSERT( !rect.IsRectEmpty() );
	ASSERT( IsRightAngle(dir) );

	switch( dir )
	{
	case Dir_Top:
		return CPoint(RoundToHalfGrid(rect.left, rect.right), rect.top);

	case Dir_Bottom:
		return CPoint(RoundToHalfGrid(rect.left, rect.right), rect.bottom - 1);

	case Dir_Left:
		return CPoint(rect.left, RoundToHalfGrid(rect.top, rect.bottom));
	}

	return CPoint(rect.right - 1, RoundToHalfGrid(rect.top, rect.bottom));
}

CPoint CAutoRouterPort::CreateStartEndPointTo(const CPoint& point, bool isStart) const
{
	RoutingDirection dir = GetStartEndDirTo(point, isStart, Dir_None);
	return CreateStartEndPointOn(dir);
}

#ifdef _DEBUG
void CAutoRouterPort::AssertValid() const
{
}

void CAutoRouterPort::AssertValidStartEndPoint(const CPoint& point, RoutingDirection dir, bool isStart)
{
	ASSERT( !rect.IsRectEmpty() );

	RoutingDirection comDir = OnWhichEdge(point);
	if( dir == Dir_None )
	{
		dir = comDir;
		ASSERT( IsRightAngle(dir) );
	}
	else
	{
		ASSERT( dir == comDir );
	}

	ASSERT( CanHaveStartEndPointOn(dir, isStart) );
}
#endif
