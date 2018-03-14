
#include "stdafx.h"
#include <math.h>
#include "ArHelper.h"


// --------------------------- CRect && CPoint


CRect InflatedRect(const CRect& rect, int a)
{
	CRect r = rect;
	r.InflateRect(a,a); 
	return r; 
}

CRect DeflatedRect(const CRect& rect, int a) 
{ 
	CRect r = rect; 
	r.DeflateRect(a,a); 
	return r; 
}

int IsPointNear(CPoint p1, CPoint p2, int nearness)
{
	return p2.x - nearness <= p1.x && p1.x <= p2.x + nearness &&
		   p2.y - nearness <= p1.y && p1.y <= p2.y + nearness;
}

int IsPointIn(CPoint point, CRect rect, int nearness)
{
	rect.InflateRect(nearness, nearness);
	return rect.PtInRect(point);
}

int IsRectIn(CRect r1, CRect r2)
{
	return r2.left <= r1.left && r1.right <= r2.right &&
		   r2.top <= r1.top && r1.bottom <= r2.bottom;
}

int IsRectClip(CRect r1, CRect r2)
{
	CRect rect;
	return rect.IntersectRect(&r1, &r2);
}

int IsPointNearHLine(CPoint p, long x1, long x2, long y, int nearness)
{
	ASSERT( x1 <= x2 );

	return x1 - nearness <= p.x && p.x <= x2 + nearness &&
		   y - nearness <= p.y && p.y <= y + nearness;
}

int IsPointNearVLine(CPoint p, long y1, long y2, long x, int nearness)
{
	ASSERT( y1 <= y2 );

	return y1 - nearness <= p.y && p.y <= y2 + nearness &&
		   x - nearness <= p.x && p.x <= x + nearness;
}

int DistanceFromHLine(CPoint p, long x1, long x2, long y)
{
	ASSERT( x1 <= x2 );

	return max(abs(p.y - y), max(x1 - p.x, p.x - x2));
}

int DistanceFromVLine(CPoint p, long y1, long y2, long x)
{
	ASSERT( y1 <= y2 );

	return max(abs(p.x - x), max(y1 - p.y, p.y - y2));
}

int IsPointNearLine(CPoint point, CPoint start, CPoint end, int nearness)
{
	ASSERT( 0 <= nearness );

	point -= start;
	end -= start;

	double x = end.x;
	double y = end.y;
	double u = point.x;
	double v = point.y;
	double xuyv = x * u + y * v;
	double x2y2 = x * x + y * y;

	if(xuyv < 0 || xuyv > x2y2)
		return 0;

	double expr1 = (x * v - y * u) ;
	expr1 *= expr1;
	double expr2 = nearness * nearness * x2y2;

	return expr1 <= expr2;
}

int IsLineMeetHLine(CPoint start, CPoint end, long x1, long x2, long y)
{
	ASSERT( x1 <= x2 );

	if( !((start.y <= y && y <= end.y) || (end.y <= y && y <= start.y )) )
		return 0;

	end -= start;
	x1 -= start.x;
	x2 -= start.x;
	y -= start.y;

	if( end.y == 0 )
		return y == 0 && (( x1 <= 0 && 0 <= x2 ) || (x1 <= end.x && end.x <= x2));

	long x = (long)(((float)end.x) / end.y) * y;
	return x1 <= x && x <= x2;
}

int IsLineMeetVLine(CPoint start, CPoint end, long y1, long y2, long x)
{
	ASSERT( y1 <= y2 );

	if( !((start.x <= x && x <= end.x) || (end.x <= x && x <= start.x )) )
		return 0;

	end -= start;
	y1 -= start.y;
	y2 -= start.y;
	x -= start.x;

	if( end.x == 0 )
		return x == 0 && (( y1 <= 0 && 0 <= y2 ) || (y1 <= end.y && end.y <= y2));

	long y = (long)(((float)end.y) / end.x) * x;
	return y1 <= y && y <= y2;
}

int IsLineClipRect(CPoint start, CPoint end, CRect rect)
{
	if( rect.PtInRect(start) || rect.PtInRect(end) )
		return 1;

	return IsLineMeetHLine(start, end, rect.left, rect.right -1, rect.top) ||
		IsLineMeetHLine(start, end, rect.left, rect.right -1, rect.bottom - 1) ||
		IsLineMeetVLine(start, end, rect.top, rect.bottom -1, rect.left) ||
		IsLineMeetVLine(start, end, rect.top, rect.bottom -1, rect.right -1);
}

int Intersect(long a1, long a2, long b1, long b2)
{
	return min(a1,a2) <= max(b1,b2) && min(b1,b2) <= max(a1,a2);
}

int IsOpeningBracket(CPoint* start_prev, CPoint* start, CPoint* end, CPoint* end_next, int ishorizontal)
{
	ASSERT( start != NULL && end != NULL );

#ifdef _DEBUG
	if( ishorizontal )
	{
		ASSERT( start->y == end->y );

		if( start_prev )
			ASSERT( start_prev->x == start->x );

		if( end_next )
			ASSERT( end_next->x == end->x );
	}
	else
	{
		ASSERT( start->x == end->x );

		if( start_prev )
			ASSERT( start_prev->y == start->y );

		if( end_next )
			ASSERT( end_next->y == end->y );
	}
#endif

	if( start_prev == NULL || end_next == NULL )
		return 0;

	return ishorizontal ?
		(start_prev->y  > start->y && end_next->y > end->y ) :
		(start_prev->x  > start->x && end_next->x > end->x );
}

int IsClosingBracket(CPoint* start_prev, CPoint* start, CPoint* end, CPoint* end_next, int ishorizontal)
{
	ASSERT( start != NULL && end != NULL );

#ifdef _DEBUG
	if( ishorizontal )
	{
		ASSERT( start->y == end->y );

		if( start_prev )
			ASSERT( start_prev->x == start->x );

		if( end_next )
			ASSERT( end_next->x == end->x );
	}
	else
	{
		ASSERT( start->x == end->x );

		if( start_prev )
			ASSERT( start_prev->y == start->y );

		if( end_next )
			ASSERT( end_next->y == end->y );
	}
#endif

	if( start_prev == NULL || end_next == NULL )
		return 0;

	return ishorizontal ?
		(start_prev->y  < start->y && end_next->y < end->y ) :
		(start_prev->x  < start->x && end_next->x < end->x );
}


// --------------------------- EArDir


int AreInRightAngle(EArDir dir1, EArDir dir2)
{
	ASSERT( IsRightAngle(dir1) && IsRightAngle(dir2) );
	return IsHorizontal(dir1) == IsVertical(dir2);
}

EArDir NextClockwiseDir(EArDir dir)
{
	if( IsRightAngle(dir) )
		return (EArDir) ((dir+1) % 4);

	return dir;
}

EArDir PrevClockwiseDir(EArDir dir)
{
	if( IsRightAngle(dir) )
		return (EArDir) ((dir+3) % 4);

	return dir;
}

EArDir ReverseDir(EArDir dir)
{
	if( IsRightAngle(dir) )
		return (EArDir) ((dir+2) % 4);

	return dir;
}

CPoint StepOneInDir(CPoint point, EArDir dir)
{
	ASSERT( IsRightAngle(dir) );
	CPoint p(point);

	switch(dir)
	{
	case Dir_Top:
		p.y--;
		break;

	case Dir_Right:
		p.x++;
		break;

	case Dir_Bottom:
		p.y++;
		break;

	case Dir_Left:
		p.x--;
		break;
	}

	return p;
}

long& GetRectCoord(CRect& rect, EArDir dir)
{
	ASSERT( IsRightAngle(dir) );

	switch( dir )
	{
	case Dir_Top: 
		return rect.top;

	case Dir_Right:
		return rect.right;

	case Dir_Bottom:
		return rect.bottom;
	}

	return rect.left;
}

long GetRectOuterCoord(CRect rect, EArDir dir)
{
	ASSERT( IsRightAngle(dir) );

	switch( dir )
	{
	case Dir_Top: 
		return rect.top-1;

	case Dir_Right:
		return rect.right;

	case Dir_Bottom:
		return rect.bottom;
	}

	return rect.left-1;
}

//	Indexes:
//				 04
//				1  5
//				3  7
//				 26

int GetDirTableIndex(CSize offset)
{
	return (offset.cx >= 0)*4 + (offset.cy >= 0)*2 + (abs(offset.cx) >= abs(offset.cy));
}

EArDir majordir_table[] =
{
	Dir_Top,
	Dir_Left,
	Dir_Bottom,
	Dir_Left,
	Dir_Top,
	Dir_Right,
	Dir_Bottom,
	Dir_Right
};

EArDir GetMajorDir(CSize offset)
{
	return majordir_table[GetDirTableIndex(offset)];
}

EArDir minordir_table[] =
{
	Dir_Left,
	Dir_Top,
	Dir_Left,
	Dir_Bottom,
	Dir_Right,
	Dir_Top,
	Dir_Right,
	Dir_Bottom
};

EArDir GetMinorDir(CSize offset)
{
	return minordir_table[GetDirTableIndex(offset)];
}

//	FG123
//	E   4
//	D 0 5
//	C   6
//  BA987

int ExGetDirTableIndex(CSize offset)
{
	return
		offset.cx > 0 ?
		(
			offset.cy > 0 ?
			(
				offset.cx > offset.cy ?
				(
					6
				) :
				(offset.cx < offset.cy ?
				(
					8
				) :
				(
					7
				))
			) :
			(offset.cy < 0 ?
			(
				offset.cx > -offset.cy ?
				(
					4
				) :
				(offset.cx < -offset.cy ?
				(
					2
				) :
				(
					3
				))
			) :
			(
				5
			))
		) :
		(offset.cx < 0 ?
		(
			offset.cy > 0 ?
			(
				-offset.cx > offset.cy ?
				(
					12
				) :
				(-offset.cx < offset.cy ?
				(
					10
				) :
				(
					11
				))
			) :
			(offset.cy < 0 ?
			(
				offset.cx < offset.cy ?
				(
					14
				) :
				(offset.cx > offset.cy ?
				(
					16
				) :
				(
					15
				))
			) :
			(
				13
			))
		) :
		(
			offset.cy > 0 ?
			(
				9
			) :
			(offset.cy < 0 ?
			(
				1
			) :
			(
				0
			))
		));
}

EArDir exmajordir_table[17] =
{
	Dir_None,
	Dir_Top,
	Dir_Top,
	Dir_Right,
	Dir_Right,
	Dir_Right,
	Dir_Right,
	Dir_Right,
	Dir_Bottom,
	Dir_Bottom,
	Dir_Bottom,
	Dir_Left,
	Dir_Left,
	Dir_Left,
	Dir_Left,
	Dir_Left,
	Dir_Top
};

EArDir ExGetMajorDir(CSize offset)
{
	return exmajordir_table[ExGetDirTableIndex(offset)];
}

EArDir exminordir_table[17] = 
{
	Dir_None,
	Dir_None,
	Dir_Right,
	Dir_Top,
	Dir_Top,
	Dir_None,
	Dir_Bottom,
	Dir_Bottom,
	Dir_Right,
	Dir_None,
	Dir_Left,
	Dir_Bottom,
	Dir_Bottom,
	Dir_None,
	Dir_Top,
	Dir_Top,
	Dir_Left
};

EArDir ExGetMinorDir(CSize offset)
{
	return exminordir_table[ExGetDirTableIndex(offset)];
}

EArDir GetDir(CSize offset, EArDir nodir)
{
	if( offset.cx == 0 )
	{
		if( offset.cy == 0 )
			return nodir;

		if( offset.cy < 0 )
			return Dir_Top;

		return Dir_Bottom;
	}

	if( offset.cy == 0 )
	{
		if( offset.cx > 0 )
			return Dir_Right;

		return Dir_Left;
	}

	return Dir_Skew;
}

int IsPointInDirFrom(CPoint point, CPoint from, EArDir dir)
{
	ASSERT( IsRightAngle(dir) );

	switch( dir )
	{
	case Dir_Top:
		return point.y <= from.y;

	case Dir_Right:
		return point.x >= from.x;

	case Dir_Bottom:
		return point.y >= from.y;

	case Dir_Left:
		return point.x <= from.x;
	}

	return 0;
}

int IsPointInDirFrom(CPoint point, CRect rect, EArDir dir)
{
	ASSERT( IsRightAngle(dir) );

	switch( dir )
	{
	case Dir_Top:
		return point.y < rect.top;

	case Dir_Right:
		return point.x >= rect.right;

	case Dir_Bottom:
		return point.y >= rect.bottom;

	case Dir_Left:
		return point.x < rect.left;
	}

	return 0;
}

int IsPointBetweenSides(CPoint point, CRect rect, int ishorizontal)
{
	if( ishorizontal )
		return rect.top <= point.y && point.y < rect.bottom;

	return rect.left <= point.x && point.x < rect.right;
}

int IsCoordInDirFrom(long coord, long from, EArDir dir)
{
	ASSERT( IsRightAngle(dir) );

	if( dir == Dir_Top || dir == Dir_Left )
		return coord <= from;

	return coord >= from;
}

EArDir OnWhichEdge(CRect rect, CPoint point)
{
	if( point.y == rect.top && rect.left < point.x && point.x < rect.right - 1 )
		return Dir_Top;

	if( point.y == rect.bottom - 1 && rect.left < point.x && point.x < rect.right - 1 )
		return Dir_Bottom;

	if( point.x == rect.left && rect.top < point.y && point.y < rect.bottom - 1 )
		return Dir_Left;

	if( point.x == rect.right - 1 && rect.top < point.y && point.y < rect.bottom - 1 )
		return Dir_Right;

	return Dir_None;
}


// --------------------------- CArFindNearestLine


int CArFindNearestLine::HLine(int x1, int x2, int y)
{
	ASSERT( x1 <= x2 );

	int d1 = DistanceFromHLine(point, x1, x2, y);
	int d2 = abs(point.y - y);

	if( d1 < dist1 || (d1 == dist1 && d2 < dist2) )
	{
		dist1 = d1;
		dist2 = d2;
		return 1;
	}

	return 0;
}

int CArFindNearestLine::VLine(int y1, int y2, int x)
{
	ASSERT( y1 <= y2 );

	int d1 = DistanceFromVLine(point, y1, y2, x);
	int d2 = abs(point.x - x);

	if( d1 < dist1 || (d1 == dist1 && d2 < dist2) )
	{
		dist1 = d1;
		dist2 = d2;
		return 1;
	}

	return 0;
}


