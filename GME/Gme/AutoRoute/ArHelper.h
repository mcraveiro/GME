
#ifndef __ARHELPER_H
#define __ARHELPER_H

#include "../GmeStd.h"
#include "Gme.h"
#include <vector>

#ifdef _DEBUG
//	#define _DEBUG_DEEP
#endif


// --------------------------- RoutingDirection && PathCustomizationType && CustomPathData

enum RoutingDirection
{
	Dir_None	= -1,
	Dir_Top		= 0,
	Dir_Right	= 1,
	Dir_Bottom	= 2,
	Dir_Left	= 3,
	Dir_Skew	= 4
};

// Type of edge customization
enum PathCustomizationType
{
	Invalid						= 0,
	SimpleEdgeDisplacement		= 1,
	CustomPointCustomization	= 2,
	NURBSEdgeCustomization		= 3,
	NumOfPathCustomizationTypes	= 4
};

// Version of edge data structure
#define CONNECTIONCUSTOMIZATIONDATAVERSION			0
#define EMPTYCONNECTIONCUSTOMIZATIONDATAMAGIC		"-1"

// Structure for storing edge customization data
class CustomPathData
{
private:
	long version;					// Version number of structure
	long aspect;					// Which aspect does customization refers to
	long edgeIndex;					// The index of the given edge in the edge array
	long edgeCount;					// The number of the edges in the array
	PathCustomizationType type;		// Type of the customization: simple displacement, NURBS curving, etc
	bool horizontalOrVerticalEdge;	// Tells if the displacement to an x or y coordinate
	long x;							// An x coordinate
	long y;							// An y coordinate

	std::vector<long> l;			// Additional long data
	std::vector<double> d;			// Additional double data

public:
	CustomPathData();
	CustomPathData& operator=(const CustomPathData& other);

	void Serialize(CString& outChannel);
	bool Deserialize(const CString& inChannel);

	long GetVersion(void) const;
	void SetVersion(long ver);
	long GetAspect(void) const;
	void SetAspect(long asp);
	long GetEdgeIndex(void) const;
	void SetEdgeIndex(long index);
	long GetEdgeCount(void) const;
	void SetEdgeCount(long count);
	PathCustomizationType GetType(void) const;
	void SetType(PathCustomizationType typ);
	bool IsHorizontalOrVertical(void) const;
	void SetHorizontalOrVertical(bool parity);
	long GetX(void) const;
	void SetX(long coord);
	long GetY(void) const;
	void SetY(long coord);

	long GetLongDataCount(void) const;
	long GetLongData(long index) const;
	void SetLongData(long index, long dat);
	void AddLongData(long dat);
	long GetDoubleDataCount(void) const;
	double GetDoubleData(long index) const;
	void SetDoubleData(long index, double dat);
	void AddDoubleData(double dat);
};


// --------------------------- CRect && CPoint

static const CPoint emptyPoint = CPoint(-100000, -100000);

CRect InflatedRect(const CRect& rect, int a);
CRect DeflatedRect(const CRect& rect, int a);

bool IsPointNear(const CPoint& p1, const CPoint& p2, int nearness = 0);
bool IsPointIn(const CPoint& point, const CRect& rect, int nearness = 0);
bool IsRectIn(const CRect& r1, const CRect& r2);
bool IsRectClip(const CRect& r1, const CRect& r2);

bool IsPointNearHLine(const CPoint& p, long x1, long x2, long y, int nearness = 0);
bool IsPointNearVLine(const CPoint& p, long y1, long y2, long x, int nearness = 0);
int DistanceFromHLine(const CPoint& p, long x1, long x2, long y);
int DistanceFromVLine(const CPoint& p, long y1, long y2, long x);
double DistanceSquareFromLine(const CPoint& start, const CPoint& end, const CPoint& pt);
bool IsOnEdge(const CPoint& start, const CPoint& end, const CPoint& pt, int nearness = 3);

bool IsPointNearLine(const CPoint& point, const CPoint& start, const CPoint& end, int nearness = 0);
bool IsLineMeetHLine(const CPoint& start, const CPoint& end, long x1, long x2, long y);
bool IsLineMeetVLine(const CPoint& start, const CPoint& end, long y1, long y2, long x);
bool IsLineClipRect(const CPoint& start, const CPoint& end, const CRect& rect);
bool Intersect(long first_x1, long first_x2, long second_x1, long second_x2);


inline bool IsHorizontal(RoutingDirection dir) { return dir == Dir_Right || dir == Dir_Left; }
inline bool IsVertical(RoutingDirection dir) { return dir == Dir_Top || dir == Dir_Bottom; }
inline bool IsRightAngle(RoutingDirection dir) { return Dir_Top <= dir && dir <= Dir_Left; }
inline bool IsTopLeft(RoutingDirection dir) { return dir == Dir_Top || dir == Dir_Left; }
inline bool IsBottomRight(RoutingDirection dir) { return dir == Dir_Bottom || dir == Dir_Right; }
bool AreInRightAngle(RoutingDirection dir1, RoutingDirection dir2);
RoutingDirection NextClockwiseDir(RoutingDirection dir);
RoutingDirection PrevClockwiseDir(RoutingDirection dir);
RoutingDirection ReverseDir(RoutingDirection dir);

CPoint StepOneInDir(const CPoint& point, RoutingDirection dir);
long& GetRectCoord(CRect& rect, RoutingDirection dir);
long GetRectOuterCoord(const CRect& rect, RoutingDirection dir);
inline long& GetPointCoord(CPoint& point, bool ishorizontal) { return ishorizontal ? point.x : point.y; }
inline long& GetPointCoord(CPoint& point, RoutingDirection dir) { return IsHorizontal(dir) ? point.x : point.y; }
inline long ChooseInDir(long a, long b, RoutingDirection dir) { return IsTopLeft(dir) ? min(a,b) : max(a,b); }

RoutingDirection GetMajorDir(const CSize& offset);
RoutingDirection GetMinorDir(const CSize& offset);
RoutingDirection ExGetMajorDir(const CSize& offset);
RoutingDirection ExGetMinorDir(const CSize& offset);
RoutingDirection GetDir(const CSize& offset, RoutingDirection nodir = Dir_None);
RoutingDirection GetSkewDir(const CSize& offset, RoutingDirection nodir = Dir_None);
bool IsPointInDirFrom(const CPoint& point, const CPoint& from, RoutingDirection dir);
bool IsPointInDirFrom(const CPoint& point, const CRect& rect, RoutingDirection dir);
bool IsPointBetweenSides(const CPoint& point, const CRect& rect, bool ishorizontal = true);
inline bool IsPointBetweenSides(const CPoint& point, const CRect& rect, RoutingDirection dir) { return IsPointBetweenSides(point, rect, IsHorizontal(dir)); }
RoutingDirection PointOnSide(const CPoint& point, const CRect& rect);
bool IsCoordInDirFrom(long coord, long from, RoutingDirection dir);

RoutingDirection OnWhichEdge(const CRect& rect, const CPoint& point);

// --------------------------- CArFindNearestLine


class CArFindNearestLine
{
public:
	CArFindNearestLine(const CPoint& p): point(p), dist1(INT_MAX), dist2(INT_MAX) { }

	bool HLine(int x1, int x2, int y);
	bool VLine(int y1, int y2, int x);
	bool Was() { return dist1 < INT_MAX && dist2 < INT_MAX; }

public:
	CPoint point;
	int dist1;		// |(x,y)| = max(|x|,|y|)
	int dist2;		// |(x,y)| = |x| or |y|
};


// --------------------------- CPointListPath

class CPointListPath : public CPointList
{
public:
	POSITION GetHeadEdge(CPoint& start, CPoint& end) const;
	POSITION GetTailEdge(CPoint& start, CPoint& end) const;
	void GetNextEdge(POSITION& pos, CPoint& start, CPoint& end) const;
	void GetPrevEdge(POSITION& pos, CPoint& start, CPoint& end) const;
	void GetEdge(POSITION pos, CPoint& start, CPoint& end) const;

	POSITION GetHeadEdgePtrs(CPoint*& start, CPoint*& end);
	POSITION GetTailEdgePtrs(CPoint*& start, CPoint*& end);
	void GetNextEdgePtrs(POSITION& pos, CPoint*& start, CPoint*& end);
	void GetPrevEdgePtrs(POSITION& pos, CPoint*& start, CPoint*& end);
	void GetEdgePtrs(POSITION pos, CPoint*& start, CPoint*& end);
	CPoint* GetStartPoint(POSITION pos);
	CPoint* GetEndPoint(POSITION pos);
 	CPoint* GetPointBeforeEdge(POSITION pos);
	CPoint* GetPointAfterEdge(POSITION pos);

	POSITION GetEdgePosBeforePoint(POSITION pos) const;
	POSITION GetEdgePosAfterPoint(POSITION pos) const;
	POSITION GetEdgePosForStartPoint(const CPoint& startpoint);

// --- Debug

#ifdef _DEBUG
public:
	void AssertValidPos(POSITION pos) const;
	void DumpPoints(const CString& msg) const;
#endif
};


#endif//__ARHELPER_H
