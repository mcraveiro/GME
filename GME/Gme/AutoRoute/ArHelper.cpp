
#include "stdafx.h"
#include <math.h>
#include "ArHelper.h"


// --------------------------- CRect && CPoint

CustomPathData::CustomPathData():
	version						(CONNECTIONCUSTOMIZATIONDATAVERSION),
	aspect						(0),
	edgeIndex					(0),
	edgeCount					(-1),
	type						(SimpleEdgeDisplacement),
	horizontalOrVerticalEdge	(true),
	x							(0),
	y							(0)
{
}

CustomPathData& CustomPathData::operator=(const CustomPathData& other)
{
	if (this != &other)
	{
		this->version					= other.version;
		this->aspect					= other.aspect;
		this->edgeIndex					= other.edgeIndex;
		this->edgeCount					= other.edgeCount;
		this->type						= other.type;
		this->horizontalOrVerticalEdge	= other.horizontalOrVerticalEdge;
		this->x							= other.x;
		this->y							= other.y;
		this->l							= other.l;
		this->d							= other.d;
	}
	return *this;
}

void CustomPathData::Serialize(CString& outChannel)
{
	outChannel.Format(_T("%ld,%ld,%ld,%d,%ld"), GetVersion(), GetAspect(), GetEdgeIndex(),
											GetEdgeCount(), GetType());
	CString additionalDataStr;
	additionalDataStr.Format(_T(",%ld,%ld,%ld,%ld"), IsHorizontalOrVertical() ? 1 : 0,
												 GetX(), GetY(), GetLongDataCount());
	outChannel.Append(additionalDataStr);
	for(long i = 0; i < GetLongDataCount(); i++) {
		additionalDataStr.Format(_T(",%ld"), l[i]);
		outChannel.Append(additionalDataStr);
	}
	additionalDataStr.Format(_T(",%ld"), GetDoubleDataCount());
	outChannel.Append(additionalDataStr);
	for(long i = 0; i < GetDoubleDataCount(); i++) {
		additionalDataStr.Format(_T(",%lf"), d[i]);
		outChannel.Append(additionalDataStr);
	}
}

bool CustomPathData::Deserialize(const CString& inChannel)
{
	TRACE(_T("\tResulting token: %s\n"), inChannel);
	int curSubPos = 0;
	CString versionStr = inChannel.Tokenize(_T(","), curSubPos);
	SetVersion(_tcstol(versionStr, NULL, 10));
	ASSERT(GetVersion() == CONNECTIONCUSTOMIZATIONDATAVERSION);
	if (GetVersion() != CONNECTIONCUSTOMIZATIONDATAVERSION) {
		// TODO: Convert from older version to newer
		return false;
	}
	CString aspectStr = inChannel.Tokenize(_T(","), curSubPos);
	SetAspect(_tcstol(aspectStr, NULL, 10));
	CString edgeIndexStr = inChannel.Tokenize(_T(","), curSubPos);
	SetEdgeIndex(_tcstol(edgeIndexStr, NULL, 10));
	CString edgeCountStr = inChannel.Tokenize(_T(","), curSubPos);
	SetEdgeCount(_tcstol(edgeCountStr, NULL, 10));
	CString edgeCustomTypeStr = inChannel.Tokenize(_T(","), curSubPos);
	SetType((PathCustomizationType)_tcstol(edgeCustomTypeStr, NULL, 10));
	TRACE(_T("\tAsp %ld, Ind %ld, Cnt %d, Typ %ld"), GetAspect(), GetEdgeIndex(), GetEdgeCount(), GetType());
	CString directionStr = inChannel.Tokenize(_T(","), curSubPos);
	SetHorizontalOrVertical(_tcstol(directionStr, NULL, 10) != 0);
	CString positionStr = inChannel.Tokenize(_T(","), curSubPos);
	SetX(_tcstol(positionStr, NULL, 10));
	positionStr = inChannel.Tokenize(_T(","), curSubPos);
	SetY(_tcstol(positionStr, NULL, 10));
	positionStr = inChannel.Tokenize(_T(","), curSubPos);
	long numOfExtraLongData = _tcstol(positionStr, NULL, 10);
	ASSERT(numOfExtraLongData >= 0 && numOfExtraLongData <= 4);
	TRACE(_T(", Dir %ld, x %ld, y %ld, num %ld"), IsHorizontalOrVertical(), GetX(), GetY(), numOfExtraLongData);
	for(long i = 0; i < numOfExtraLongData; i++) {
		positionStr = inChannel.Tokenize(_T(","), curSubPos);
		AddLongData(_tcstol(positionStr, NULL, 10));
		TRACE(_T(", l%ld %ld"), i, l[i]);
	}
	TRACE(_T("\n"));

	positionStr = inChannel.Tokenize(_T(","), curSubPos);
	long numOfExtraDoubleData = _tcstol(positionStr, NULL, 10);
	ASSERT(numOfExtraDoubleData >= 0 && numOfExtraDoubleData <= 8);
	TRACE(_T(", num %ld"), numOfExtraDoubleData);
	for(long i = 0; i < numOfExtraDoubleData; i++) {
		positionStr = inChannel.Tokenize(_T(","), curSubPos);
		AddDoubleData(_ttof(positionStr));
		TRACE(_T(", l%ld %lf"), i, d[i]);
	}
	TRACE(_T("\n"));
	return true;
}

long CustomPathData::GetVersion(void) const
{
	return version;
}

void CustomPathData::SetVersion(long ver)
{
	version = ver;
}

long CustomPathData::GetAspect(void) const
{
	return aspect;
}

void CustomPathData::SetAspect(long asp)
{
	aspect = asp;
}

long CustomPathData::GetEdgeIndex(void) const
{
	return edgeIndex;
}

void CustomPathData::SetEdgeIndex(long index)
{
	edgeIndex = index;
}

long CustomPathData::GetEdgeCount(void) const
{
	return edgeCount;
}

void CustomPathData::SetEdgeCount(long count)
{
	edgeCount = count;
}

PathCustomizationType CustomPathData::GetType(void) const
{
	return type;
}

void CustomPathData::SetType(PathCustomizationType typ)
{
	type = typ;
}

bool CustomPathData::IsHorizontalOrVertical(void) const
{
	return horizontalOrVerticalEdge;
}

void CustomPathData::SetHorizontalOrVertical(bool parity)
{
	horizontalOrVerticalEdge = parity;
}

long CustomPathData::GetX(void) const
{
	return x;
}

void CustomPathData::SetX(long coord)
{
	x = coord;
}

long CustomPathData::GetY(void) const
{
	return y;
}

void CustomPathData::SetY(long coord)
{
	y = coord;
}

long CustomPathData::GetLongDataCount(void) const
{
	return (long)l.size();
}

long CustomPathData::GetLongData(long index) const
{
	return l[index];
}

void CustomPathData::SetLongData(long index, long dat)
{
	l[index] = dat;
}

void CustomPathData::AddLongData(long dat)
{
	l.push_back(dat);
}

long CustomPathData::GetDoubleDataCount(void) const
{
	return (long)d.size();
}

double CustomPathData::GetDoubleData(long index) const
{
	return d[index];
}

void CustomPathData::SetDoubleData(long index, double dat)
{
	d[index] = dat;
}

void CustomPathData::AddDoubleData(double dat)
{
	d.push_back(dat);
}


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

bool IsPointNear(const CPoint& p1, const CPoint& p2, int nearness)
{
	return p2.x - nearness <= p1.x && p1.x <= p2.x + nearness &&
		   p2.y - nearness <= p1.y && p1.y <= p2.y + nearness;
}

bool IsPointIn(const CPoint& point, const CRect& rect, int nearness)
{
	CRect tmpR = rect;
	tmpR.InflateRect(nearness, nearness);
	return tmpR.PtInRect(point) == TRUE;
}

bool IsRectIn(const CRect& r1, const CRect& r2)
{
	return r2.left <= r1.left && r1.right <= r2.right &&
		   r2.top <= r1.top && r1.bottom <= r2.bottom;
}

bool IsRectClip(const CRect& r1, const CRect& r2)
{
	CRect rect;
	return rect.IntersectRect(&r1, &r2) == TRUE;
}

bool IsPointNearHLine(const CPoint& p, long x1, long x2, long y, int nearness)
{
	ASSERT( x1 <= x2 );

	return x1 - nearness <= p.x && p.x <= x2 + nearness &&
		   y - nearness <= p.y && p.y <= y + nearness;
}

bool IsPointNearVLine(const CPoint& p, long y1, long y2, long x, int nearness)
{
	ASSERT( y1 <= y2 );

	return y1 - nearness <= p.y && p.y <= y2 + nearness &&
		   x - nearness <= p.x && p.x <= x + nearness;
}

int DistanceFromHLine(const CPoint& p, long x1, long x2, long y)
{
	ASSERT( x1 <= x2 );

	return max(abs(p.y - y), max(x1 - p.x, p.x - x2));
}

int DistanceFromVLine(const CPoint& p, long y1, long y2, long x)
{
	ASSERT( y1 <= y2 );

	return max(abs(p.x - x), max(y1 - p.y, p.y - y2));
}

double DistanceSquareFromLine(const CPoint& start, const CPoint& end, const CPoint& pt)
{
	//     |det(end-start start-pt)|
	// d = -------------------------
	//            |end-start|
	//
	double nom = abs((double)(end.x - start.x) * (start.y - pt.y) - (start.x - pt.x) * (end.y - start.y));
	double denom_square = (double)((end.x - start.x) * (end.x - start.x) + (end.y - start.y) * (end.y - start.y));
	double d_square = nom * nom / denom_square;
	return d_square;
}

bool IsOnEdge(const CPoint& start, const CPoint& end, const CPoint& pt, int nearness)
{
	if (start.x == end.x)			// vertical edge, horizontal move
	{
		if (abs(end.x - pt.x) <= nearness && pt.y <= max(end.y, start.y) + nearness && pt.y >= min(end.y, start.y) - nearness)
			return true;
	}
	else if (start.y == end.y)	// horizontal line, vertical move
	{
		if (abs(end.y - pt.y) <= nearness && pt.x <= max(end.x, start.x) + nearness && pt.x >= min(end.x, start.x) - nearness)
			return true;
	}
	else
	{
		// TODO: consider non-linear edges
		//
		// Is the point close to the edge?
		double d_square = DistanceSquareFromLine(start, end, pt);
		if (d_square <= nearness * nearness) {
			// Check not just if the point is on the line, but if it is on the line segment
			// point = m * start + (1 - m) * end
			//
			// m = (pt + end) / (start + end)
			// 0.0 <= m <= 1.0

			double m1 = ((double)pt.x - end.x) / (start.x - end.x);
			double m2 = ((double)pt.y - end.y) / (start.y - end.y);
			//ASSERT(abs(m2 - m1) < 2.0e-1);
			if (m1 >= 0.0 && m1 <= 1.0 && m2 >= 0.0 && m2 <= 1.0)
				return true;
		}
	}
	return false;
}

bool IsPointNearLine(const CPoint& point, const CPoint& start, const CPoint& end, int nearness)
{
	ASSERT( 0 <= nearness );

	// begin Zolmol
	// the routing may create edges that have start==end
	// thus confusing this algorithm
	if( end.x == start.x && end.y == start.y)
		return false;
	// end Zolmol

	CPoint point2 = point;
	point2 -= start;
	CPoint end2 = end;
	end2 -= start;

	double x = end2.x;
	double y = end2.y;
	double u = point2.x;
	double v = point2.y;
	double xuyv = x * u + y * v;
	double x2y2 = x * x + y * y;

	if(xuyv < 0 || xuyv > x2y2)
		return false;

	double expr1 = (x * v - y * u) ;
	expr1 *= expr1;
	double expr2 = nearness * nearness * x2y2;

	return expr1 <= expr2;
}

bool IsLineMeetHLine(const CPoint& start, const CPoint& end, long x1, long x2, long y)
{
	ASSERT( x1 <= x2 );

	if( !((start.y <= y && y <= end.y) || (end.y <= y && y <= start.y )) )
		return false;

	CPoint end2 = end;
	end2 -= start;
	x1 -= start.x;
	x2 -= start.x;
	y -= start.y;

	if( end2.y == 0 )
		return y == 0 && (( x1 <= 0 && 0 <= x2 ) || (x1 <= end2.x && end2.x <= x2));

	long x = (long)(((float)end2.x) / end2.y) * y;
	return x1 <= x && x <= x2;
}

bool IsLineMeetVLine(const CPoint& start, const CPoint& end, long y1, long y2, long x)
{
	ASSERT( y1 <= y2 );

	if( !((start.x <= x && x <= end.x) || (end.x <= x && x <= start.x )) )
		return false;

	CPoint end2 = end;
	end2 -= start;
	y1 -= start.y;
	y2 -= start.y;
	x -= start.x;

	if( end2.x == 0 )
		return x == 0 && (( y1 <= 0 && 0 <= y2 ) || (y1 <= end2.y && end2.y <= y2));

	long y = (long)(((float)end2.y) / end2.x) * x;
	return y1 <= y && y <= y2;
}

bool IsLineClipRect(const CPoint& start, const CPoint& end, const CRect& rect)
{
	if( rect.PtInRect(start) || rect.PtInRect(end) )
		return true;

	return IsLineMeetHLine(start, end, rect.left, rect.right - 1, rect.top) ||
		   IsLineMeetHLine(start, end, rect.left, rect.right - 1, rect.bottom - 1) ||
		   IsLineMeetVLine(start, end, rect.top, rect.bottom - 1, rect.left) ||
		   IsLineMeetVLine(start, end, rect.top, rect.bottom - 1, rect.right - 1);
}

bool Intersect(long a1, long a2, long b1, long b2)
{
	return min(a1,a2) <= max(b1,b2) && min(b1,b2) <= max(a1,a2);
}


// --------------------------- RoutingDirection


bool AreInRightAngle(RoutingDirection dir1, RoutingDirection dir2)
{
	ASSERT( IsRightAngle(dir1) && IsRightAngle(dir2) );
	return IsHorizontal(dir1) == IsVertical(dir2);
}

RoutingDirection NextClockwiseDir(RoutingDirection dir)
{
	if( IsRightAngle(dir) )
		return (RoutingDirection) ((dir+1) % 4);

	return dir;
}

RoutingDirection PrevClockwiseDir(RoutingDirection dir)
{
	if( IsRightAngle(dir) )
		return (RoutingDirection) ((dir+3) % 4);

	return dir;
}

RoutingDirection ReverseDir(RoutingDirection dir)
{
	if( IsRightAngle(dir) )
		return (RoutingDirection) ((dir+2) % 4);

	return dir;
}

CPoint StepOneInDir(const CPoint& point, RoutingDirection dir)
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

long& GetRectCoord(CRect& rect, RoutingDirection dir)
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

long GetRectOuterCoord(const CRect& rect, RoutingDirection dir)
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

int GetDirTableIndex(const CSize& offset)
{
	return (offset.cx >= 0)*4 + (offset.cy >= 0)*2 + (abs(offset.cx) >= abs(offset.cy));
}

RoutingDirection majordir_table[] =
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

RoutingDirection GetMajorDir(const CSize& offset)
{
	return majordir_table[GetDirTableIndex(offset)];
}

RoutingDirection minordir_table[] =
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

RoutingDirection GetMinorDir(const CSize& offset)
{
	return minordir_table[GetDirTableIndex(offset)];
}

//	FG123
//	E   4
//	D 0 5
//	C   6
//  BA987

int ExGetDirTableIndex(const CSize& offset)
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

RoutingDirection exmajordir_table[17] =
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

RoutingDirection ExGetMajorDir(const CSize& offset)
{
	return exmajordir_table[ExGetDirTableIndex(offset)];
}

RoutingDirection exminordir_table[17] = 
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

RoutingDirection ExGetMinorDir(const CSize& offset)
{
	return exminordir_table[ExGetDirTableIndex(offset)];
}

RoutingDirection GetDir(const CSize& offset, RoutingDirection nodir)
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

RoutingDirection GetSkewDir(const CSize& offset, RoutingDirection nodir)
{
	if (offset.cx == 0 || abs(offset.cy) > abs(offset.cx))
	{
		if (offset.cy == 0)
			return nodir;

		if (offset.cy < 0)
			return Dir_Top;

		return Dir_Bottom;
	}

	if (offset.cy == 0 || abs(offset.cx) >= abs(offset.cy))
	{
		if (offset.cx > 0)
			return Dir_Right;

		return Dir_Left;
	}

	ASSERT(false);
	return Dir_Skew;
}

bool IsPointInDirFrom(const CPoint& point, const CPoint& from, RoutingDirection dir)
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

	return false;
}

bool IsPointInDirFrom(const CPoint& point, const CRect& rect, RoutingDirection dir)
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

	return false;
}

bool IsPointBetweenSides(const CPoint& point, const CRect& rect, bool ishorizontal)
{
	if( ishorizontal )
		return rect.top <= point.y && point.y < rect.bottom;

	return rect.left <= point.x && point.x < rect.right;
}

RoutingDirection PointOnSide(const CPoint& point, const CRect& rect)
{
	int dleft = DistanceFromVLine(point, rect.top, rect.bottom, rect.left);
	int dtop = DistanceFromHLine(point, rect.left, rect.right, rect.top);
	int dright = DistanceFromVLine(point, rect.top, rect.bottom, rect.right);
	int dbottom = DistanceFromHLine(point, rect.left, rect.right, rect.bottom);

	if (dleft < 3)
		return Dir_Left;
	if (dtop < 3)
		return Dir_Top;
	if (dright < 3)
		return Dir_Right;
	if (dbottom < 3)
		return Dir_Bottom;

	return GetSkewDir(point - rect.CenterPoint());
}

bool IsCoordInDirFrom(long coord, long from, RoutingDirection dir)
{
	ASSERT( IsRightAngle(dir) );

	if( dir == Dir_Top || dir == Dir_Left )
		return coord <= from;

	return coord >= from;
}

RoutingDirection OnWhichEdge(const CRect& rect, const CPoint& point)
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


bool CArFindNearestLine::HLine(int x1, int x2, int y)
{
	ASSERT( x1 <= x2 );

	int d1 = DistanceFromHLine(point, x1, x2, y);
	int d2 = abs(point.y - y);

	if( d1 < dist1 || (d1 == dist1 && d2 < dist2) )
	{
		dist1 = d1;
		dist2 = d2;
		return true;
	}

	return false;
}

bool CArFindNearestLine::VLine(int y1, int y2, int x)
{
	ASSERT( y1 <= y2 );

	int d1 = DistanceFromVLine(point, y1, y2, x);
	int d2 = abs(point.x - x);

	if( d1 < dist1 || (d1 == dist1 && d2 < dist2) )
	{
		dist1 = d1;
		dist2 = d2;
		return true;
	}

	return false;
}


// --------------------------- CPointListPath


POSITION CPointListPath::GetHeadEdge(CPoint& start, CPoint& end) const
{
	if( GetCount() < 2 )
		return NULL;

	POSITION pos = GetHeadPosition();
	ASSERT( pos != NULL );

	start = GetNext(pos);
	ASSERT( pos != NULL );

	end = GetAt(pos);

	return pos;
}

POSITION CPointListPath::GetTailEdge(CPoint& start, CPoint& end) const
{
	if( GetCount() < 2 )
		return NULL;

	POSITION pos = GetTailPosition();
	ASSERT( pos != NULL );

	end = GetPrev(pos);
	ASSERT( pos != NULL );

	start = GetAt(pos);

	return pos;
}

void CPointListPath::GetNextEdge(POSITION& pos, CPoint& start, CPoint& end) const
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	GetNext(pos);
	ASSERT( pos != NULL );

	POSITION p = pos;
	start = GetNext(p);
	if( p == NULL )
		pos = NULL;
	else
		end = GetAt(p);
}

void CPointListPath::GetPrevEdge(POSITION& pos, CPoint& start, CPoint& end) const
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	end = GetPrev(pos);
	if( pos != NULL )
		start = GetAt(pos);
}

void CPointListPath::GetEdge(POSITION pos, CPoint& start, CPoint& end) const
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	start = GetNext(pos);
	ASSERT( pos != NULL );

	end = GetAt(pos);
}

POSITION CPointListPath::GetHeadEdgePtrs(CPoint*& start, CPoint*& end)
{
	if( GetCount() < 2 )
		return NULL;

	POSITION pos = GetHeadPosition();
	ASSERT( pos != NULL );

	start = &(GetNext(pos));
	ASSERT( pos != NULL );

	end = &(GetAt(pos));

	return pos;
}

POSITION CPointListPath::GetTailEdgePtrs(CPoint*& start, CPoint*& end)
{
	if( GetCount() < 2 )
		return NULL;

	POSITION pos = GetTailPosition();
	ASSERT( pos != NULL );

	end = &(GetPrev(pos));
	ASSERT( pos != NULL );

	start = &(GetAt(pos));

	return pos;
}

void CPointListPath::GetNextEdgePtrs(POSITION& pos, CPoint*& start, CPoint*& end)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	start = &(GetNext(pos));
	if (pos != NULL)
		end = &(GetAt(pos));
}

void CPointListPath::GetPrevEdgePtrs(POSITION& pos, CPoint*& start, CPoint*& end)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	end = &(GetPrev(pos));
	if( pos != NULL )
		start = &(GetAt(pos));
}

void CPointListPath::GetEdgePtrs(POSITION pos, CPoint*& start, CPoint*& end)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	start = &(GetNext(pos));
	ASSERT( pos != NULL );

	end = &(GetAt(pos));
}

CPoint* CPointListPath::GetStartPoint(POSITION pos)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	return &(GetAt(pos));
}

CPoint* CPointListPath::GetEndPoint(POSITION pos)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	GetNext(pos);
	ASSERT( pos != NULL );

	return &(GetAt(pos));
}

CPoint* CPointListPath::GetPointBeforeEdge(POSITION pos)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	GetPrev(pos);
	if( pos == NULL )
		return NULL;

	return &(GetAt(pos));
}

CPoint* CPointListPath::GetPointAfterEdge(POSITION pos)
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	GetNext(pos);
	ASSERT( pos != NULL );

	GetNext(pos);
	if( pos == NULL )
		return NULL;

	return &(GetAt(pos));
}

POSITION CPointListPath::GetEdgePosBeforePoint(POSITION pos) const
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	GetPrev(pos);
	return pos;
}

POSITION CPointListPath::GetEdgePosAfterPoint(POSITION pos) const
{
#ifdef _DEBUG
	AssertValidPos(pos);
#endif

	POSITION p = pos;
	GetNext(p);
	if( p == NULL )
		return NULL;

	return pos;
}

POSITION CPointListPath::GetEdgePosForStartPoint(const CPoint& startpoint)
{
	POSITION pos = GetHeadPosition();
	while( pos != NULL )
	{
		if( GetNext(pos) == startpoint )
		{
			ASSERT( pos != NULL );
			GetPrev(pos);
			break;
		}
	}

	ASSERT( pos != NULL );
	return pos;
}

#ifdef _DEBUG

void CPointListPath::AssertValidPos(POSITION pos) const
{
	ASSERT( pos != NULL );

	POSITION p = GetHeadPosition();
	for(;;)
	{
		ASSERT( p != NULL );
		if( p == pos )
			return;

		GetNext(p);
	}
}

void CPointListPath::DumpPoints(const CString& msg) const
{
	TRACE(msg);
	TRACE(_T(", points dump begin:\n"));
	POSITION pos = GetHeadPosition();
	int i = 0;
	while(pos != NULL) {
		CPoint p = GetNext(pos);
		TRACE(_T("%ld.: (%ld, %ld)\n"), i, p.x, p.y);
		i++;
	}
	TRACE(_T("points dump end.\n"));
}

#endif
