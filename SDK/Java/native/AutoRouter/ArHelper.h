
#ifndef __ARHELPER_H
#define __ARHELPER_H


// --------------------------- CRect && CPoint


CRect InflatedRect(const CRect& rect, int a);
CRect DeflatedRect(const CRect& rect, int a);

int IsPointNear(CPoint p1, CPoint p2, int nearness = 0);
int IsPointIn(CPoint point, CRect rect, int nearness = 0);
int IsRectIn(CRect r1, CRect r2);
int IsRectClip(CRect r1, CRect r2);

int IsPointNearHLine(CPoint p, long x1, long x2, long y, int nearness = 0);
int IsPointNearVLine(CPoint p, long y1, long y2, long x, int nearness = 0);
int DistanceFromHLine(CPoint p, long x1, long x2, long y);
int DistanceFromVLine(CPoint p, long y1, long y2, long x);

int IsPointNearLine(CPoint point, CPoint start, CPoint end, int nearness = 0);
int IsLineMeetHLine(CPoint start, CPoint end, long x1, long x2, long y);
int IsLineMeetVLine(CPoint start, CPoint end, long y1, long y2, long x);
int IsLineClipRect(CPoint start, CPoint end, CRect rect);
int Intersect(long first_x1, long first_x2, long second_x1, long second_x2);

int IsOpeningBracket(CPoint* start_prev, CPoint* start, CPoint* end, CPoint* end_next, int ishorizontal);
int IsClosingBracket(CPoint* start_prev, CPoint* start, CPoint* end, CPoint* end_next, int ishorizontal);


// --------------------------- EArDir


enum EArDir
{
	Dir_None = -1,
	Dir_Top,
	Dir_Right,
	Dir_Bottom,
	Dir_Left,
	Dir_Skew
};

inline int IsHorizontal(EArDir dir) { return dir == Dir_Right || dir == Dir_Left; }
inline int IsVertical(EArDir dir) { return dir == Dir_Top || dir == Dir_Bottom; }
inline int IsRightAngle(EArDir dir) { return Dir_Top <= dir && dir <= Dir_Left; }
inline int IsTopLeft(EArDir dir) { return dir == Dir_Top || dir == Dir_Left; }
inline int IsBottomRight(EArDir dir) { return dir == Dir_Bottom || dir == Dir_Right; }
int AreInRightAngle(EArDir dir1, EArDir dir2);
EArDir NextClockwiseDir(EArDir dir);
EArDir PrevClockwiseDir(EArDir dir);
EArDir ReverseDir(EArDir dir);

CPoint StepOneInDir(CPoint point, EArDir dir);
long& GetRectCoord(CRect& rect, EArDir dir);
long GetRectOuterCoord(CRect rect, EArDir dir);
inline long& GetPointCoord(CPoint& point, int ishorizontal) { return ishorizontal ? point.x : point.y; }
inline long& GetPointCoord(CPoint& point, EArDir dir) { return IsHorizontal(dir) ? point.x : point.y; }
inline long ChooseInDir(long a, long b, EArDir dir) { return IsTopLeft(dir) ? min(a,b) : max(a,b); }

EArDir GetMajorDir(CSize offset);
EArDir GetMinorDir(CSize offset);
EArDir ExGetMajorDir(CSize offset);
EArDir ExGetMinorDir(CSize offset);
EArDir GetDir(CSize offset, EArDir nodir = Dir_None);
int IsPointInDirFrom(CPoint point, CPoint from, EArDir dir);
int IsPointInDirFrom(CPoint point, CRect rect, EArDir dir);
int IsPointBetweenSides(CPoint point, CRect rect, int ishorizontal = 1);
inline int IsPointBetweenSides(CPoint point, CRect rect, EArDir dir) { return IsPointBetweenSides(point, rect, IsHorizontal(dir)); }
int IsCoordInDirFrom(long coord, long from, EArDir dir);

EArDir OnWhichEdge(CRect rect, CPoint point);


// --------------------------- CArFindNearestLine


class CArFindNearestLine
{
public:
	CArFindNearestLine(CPoint p): point(p), dist1(INT_MAX), dist2(INT_MAX) { }

	int HLine(int x1, int x2, int y);
	int VLine(int y1, int y2, int x);
	int Was() { return dist1 < INT_MAX && dist2 < INT_MAX; }

public:
	CPoint point;
	int dist1;		// |(x,y)| = max(|x|,|y|)
	int dist2;		// |(x,y)| = |x| or |y|
};


#endif//__ARHELPER_H
