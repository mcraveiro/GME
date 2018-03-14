#pragma once

#include "AutoRouterPort.h"

class CAutoRouterBox;
class CAutoRouterPort;
class CAutoRouterPath;
class CAutoRouterGraph;


// --------------------------- SArEdge

class CAutoRouterEdgeList;

class CAutoRouterEdge
{
public:
	CAutoRouterEdge();
	virtual ~CAutoRouterEdge();

	CObject* GetOwner(void) const;
	void	SetOwner(CObject* owner);
	CPoint	GetStartPointPrev(void) const;
	bool	IsStartPointPrevNull(void) const;
	void	SetStartPointPrev(CPoint* point);
	CPoint	GetStartPoint(void) const;
	bool	IsSameStartPointByPointer(const CPoint* point) const;
	bool	IsStartPointNull(void) const;
	void	SetStartPoint(CPoint* point);
	void	SetStartPointX(int x);
	void	SetStartPointY(int y);
	CPoint	GetEndPoint(void) const;
	bool	IsEndPointNull(void) const;
	void	SetEndPoint(CPoint* point);
	void	SetStartAndEndPoint(CPoint* startPoint, CPoint* endPoint);
	void	SetEndPointX(int x);
	void	SetEndPointY(int y);
	CPoint	GetEndPointNext(void) const;
	bool	IsEndPointNextNull(void) const;
	void	SetEndPointNext(CPoint* point);

	float	GetPositionY(void) const;
	void	SetPositionY(float y);
	void	AddToPositionY(float dy);
	int		GetPositionX1(void) const;
	void	SetPositionX1(int x1);
	int		GetPositionX2(void) const;
	void	SetPositionX2(int x2);
	bool	GetBracketClosing(void) const;
	void	SetBracketClosing(bool b);
	bool	GetBracketOpening(void) const;
	void	SetBracketOpening(bool b);

	CAutoRouterEdge* GetOrderNext(void);
	void	SetOrderNext(CAutoRouterEdge* ordernext);
	CAutoRouterEdge* GetOrderPrev(void);
	void	SetOrderPrev(CAutoRouterEdge* orderprev);

	long	GetSectionX1(void);
	void	SetSectionX1(long x1);
	long	GetSectionX2(void);
	void	SetSectionX2(long x2);
	CAutoRouterEdge* GetSectionNext(void);
	CAutoRouterEdge** GetSectionNextPtr(void);
	void	SetSectionNext(CAutoRouterEdge* sectionnext);
	CAutoRouterEdge* GetSectionDown(void);
	CAutoRouterEdge** GetSectionDownPtr(void);
	void	SetSectionDown(CAutoRouterEdge* sectiondown);

	bool	GetEdgeFixed(void);
	void	SetEdgeFixed(bool ef);
	bool	GetEdgeCustomFixed(void);
	void	SetEdgeCustomFixed(bool ecf);
	bool	GetEdgeCanpassed(void);
	void	SetEdgeCanpassed(bool ecp);
	RoutingDirection	GetDirection(void);
	void	SetDirection(RoutingDirection dir);
	void	RecalculateDirection(void);

	CAutoRouterEdge* GetBlockPrev(void);
	void	SetBlockPrev(CAutoRouterEdge* bp);
	CAutoRouterEdge* GetBlockNext(void);
	void	SetBlockNext(CAutoRouterEdge* bn);
	CAutoRouterEdge* GetBlockTrace(void);
	void	SetBlockTrace(CAutoRouterEdge* bt);

	CAutoRouterEdge* GetClosestPrev(void);
	void	SetClosestPrev(CAutoRouterEdge* cp);
	CAutoRouterEdge* GetClosestNext(void);
	void	SetClosestNext(CAutoRouterEdge* cn);

private:
	CObject* owner;
	CPoint* startpoint_prev;
	CPoint* startpoint;
	CPoint* endpoint;
	CPoint* endpoint_next;

	float position_y;
	long position_x1;
	long position_x2;
	bool bracket_closing;
	bool bracket_opening;

	CAutoRouterEdge* order_next;
	CAutoRouterEdge* order_prev;

	long section_x1;
	long section_x2;
	CAutoRouterEdge* section_next;
	CAutoRouterEdge* section_down;

	bool edge_fixed;
	bool edge_customFixed;
	bool edge_canpassed;
	RoutingDirection edge_direction;

	CAutoRouterEdge* block_prev;
	CAutoRouterEdge* block_next;
	CAutoRouterEdge* block_trace;

	CAutoRouterEdge* closest_prev;
	CAutoRouterEdge* closest_next;
};

class CAutoRouterEdgeList
{
public:
	CAutoRouterEdgeList(bool ishorizontal);
	virtual ~CAutoRouterEdgeList();

	void SetOwner(CAutoRouterGraph* owner);

private:
	CAutoRouterGraph* owner;

// --- Edges

public:
	bool AddEdges(CAutoRouterPath* path);
	void AddEdges(CAutoRouterPort* port);
	void AddEdges(CAutoRouterBox* box);
	void AddEdges(CAutoRouterGraph* graph);
	void DeleteEdges(CObject* object);
	void DeleteAllEdges();

	bool IsEmpty() const;

private:
	bool ishorizontal;

public:
	CAutoRouterEdge* GetEdge(CAutoRouterPath* path, const CPoint& startpoint, const CPoint& endpoint) const;
	CAutoRouterEdge* GetEdgeByPointer(const CPoint* startpoint, const CPoint* endpoint) const;
	CAutoRouterEdge* GetEdgeAt(const CPoint& point, int nearness = 0) const;

#ifdef _DEBUG
public:
	void AssertValidPathEdges(CAutoRouterPath* path, CPointListPath& points) const;
	void DumpEdges(const CString& headMsg);
#endif _DEBUG

// --- Position

private:
	long Position_GetRealY(const CAutoRouterEdge* edge) const;
	void Position_SetRealY(CAutoRouterEdge* edge, long y) const;
	void Position_GetRealX(const CAutoRouterEdge* edge, long& x1, long& x2) const;
	void Position_GetRealO(const CAutoRouterEdge* edge, long& o1, long& o2) const;

	void Position_LoadY(CAutoRouterEdge* edge) const;
	void Position_LoadB(CAutoRouterEdge* edge) const;
	void PositionAll_StoreY() const;

	void PositionAll_LoadX() const;

#ifdef _DEBUG
private:
	void AssertValidPositions() const;
#endif

// --- Order

private:
	void Init_Order();
	void Check_Order();

public:
	void InsertBefore(CAutoRouterEdge* edge, CAutoRouterEdge* before);
	void InsertAfter(CAutoRouterEdge* edge, CAutoRouterEdge* after);
	void InsertLast(CAutoRouterEdge* edge);
	void Insert(CAutoRouterEdge* edge);
	void Remove(CAutoRouterEdge* edge);
	void Delete(CAutoRouterEdge* edge);

private:
	CAutoRouterEdge* SlideButNotPassEdges(CAutoRouterEdge* edge, float y);

	CAutoRouterEdge* order_first;
	CAutoRouterEdge* order_last;

#ifdef _DEBUG
private:
	void AssertValidOrder() const;
#endif

// --- Section

private:
	void Init_Section();
	void Check_Section();

	void Section_Reset();
	void Section_BeginScan(CAutoRouterEdge* blocker);
	bool Section_HasBlockedEdge();
	CAutoRouterEdge* Section_GetBlockedEdge();
	bool Section_IsImmediate();

	CAutoRouterEdge* section_first;
	CAutoRouterEdge* section_blocker;
	CAutoRouterEdge** section_ptr2blocked;

#ifdef _DEBUG
	void Section_AssertLevel(CAutoRouterEdge* level, long x1, long x2) const;
	void AssertValidSection() const;
	void AssertSectionReady() const;
#endif

// --- Bracket

	bool Bracket_IsClosing(const CAutoRouterEdge* edge) const;
	bool Bracket_IsOpening(const CAutoRouterEdge* edge) const;
	bool Bracket_IsSmallGap(const CAutoRouterEdge* blocked, const CAutoRouterEdge* blocker) const;

	bool Bracket_ShouldBeSwitched(const CAutoRouterEdge* edge, const CAutoRouterEdge* next) const;

// --- Block

public:
	bool Block_PushBackward(CAutoRouterEdge* blocked, CAutoRouterEdge* blocker);
	bool Block_PushForward(CAutoRouterEdge* blocked, CAutoRouterEdge* blocker);
	bool Block_ScanForward();
	bool Block_ScanBackward();

	bool Block_SwitchWrongs();
};
