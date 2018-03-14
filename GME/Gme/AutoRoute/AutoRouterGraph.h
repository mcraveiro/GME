// AutoRouterGraph.h : Declaration of the CAutoRouterGraph

#pragma once
#include "../resource.h"       // main symbols

#include "ArHelper.h" 
#include "AutoRouterPath.h"
#include "AutoRouterEdge.h"
#include "AutoRouterPort.h"
#include "AutoRouterBox.h"

#include <vector>
#include <algorithm>


typedef std::vector<CAutoRouterBox*> CAutoRouterBoxList;
typedef std::vector<CAutoRouterPath*> CAutoRouterPathList;

typedef CList<CPoint, CPoint&> CPointList;


// CAutoRouterGraph
class CAutoRouterGraph : public CObject
{
public:
	CAutoRouterGraph();

//box:

private:
	void Remove(CAutoRouterBox* box);

	void DeleteAllBoxes();
	const CAutoRouterBoxList& GetBoxList() const { return boxes; }
	bool HasNoBox() const { return boxes.size() == 0; }
	int GetBoxCount() const { return boxes.size(); }

	void SetPortAttr(CAutoRouterPort* port, unsigned int attr);

	bool IsRectClipBoxes(const CRect& rect) const;
	bool IsLineClipBoxes(const CPoint& p1, const CPoint& p2) const;
	bool CanBoxAt(const CRect& rect) const;
	CAutoRouterBox* GetBoxAt(const CPoint& point, int nearness = 0) const;

	CAutoRouterBox* AddAtomicPort(const CRect& rect, unsigned int attr);

//path:
	void Add(CAutoRouterPath* path);
	void Remove(CAutoRouterPath* path);

	void DeleteAllPaths();
	bool HasNoPath() const { return paths.size() == 0; }
	int GetPathCount() const { return paths.size(); }

	CAutoRouterEdge* GetListEdgeAt(const CPoint& point, int nearness = 0) const;

// --- Boxes && Paths (FOR EXTERNAL USE)

	bool IsEmpty() const { return (boxes.size() == 0) && (paths.size() == 0); }

	CRect GetSurroundRect(void) const;

// --- Navigation

	CAutoRouterBox* GetOutOfBox(CPoint& point, RoutingDirection dir) const;
	CAutoRouterBox* GoToNextBox(CPoint& point, RoutingDirection dir, long stophere) const;
	CAutoRouterBox* GoToNextBox(CPoint& point, RoutingDirection dir, CPoint& stophere) const 
		{ return GoToNextBox(point, dir, GetPointCoord(stophere, dir)); }

	CAutoRouterBox* GoToNextBox(CPoint& point, RoutingDirection dir, long stop1, long stop2) const
		{ return GoToNextBox(point, dir, ChooseInDir(stop1, stop2, ReverseDir(dir))); }
	CAutoRouterBox* GoToNextBox(CPoint& point, RoutingDirection dir, CPoint& stop1, CPoint& stop2) const
		{ return GoToNextBox(point, dir, GetPointCoord(stop1, dir), GetPointCoord(stop2, dir)); }

	void GetLimitsOfEdge(const CPoint& start, const CPoint& end, long& min, long& max) const;
	bool IsPointInBox(const CPoint& point) const { return GetBoxAt(point) != NULL; }

	bool Connect(CAutoRouterPath* path);
	bool Connect(CAutoRouterPath* path, CPoint& startpoint, CPoint& endpoint);

	void ConnectPoints(CPointListPath& ret, CPoint& startpoint, CPoint& endpoint, RoutingDirection hintstartdir, RoutingDirection hintenddir);

	void DisconnectAll();
	void Disconnect(CAutoRouterPath* path);

	void DisconnectPathsClipping(const CRect& rect);
	void DisconnectPathsFrom(CAutoRouterBox* box);
	void DisconnectPathsFrom(CAutoRouterPort* port);

// --- Edges

	void AddSelfEdges(void);
	void AddEdges(CAutoRouterGraph* graph);
	void AddEdges(CAutoRouterBox* box);
	void AddEdges(CAutoRouterPort* port);
	bool AddEdges(CAutoRouterPath* path);
	void DeleteEdges(CObject* object);

	void AddAllEdges();
	void DeleteAllEdges();

	void AddBoxAndPortEdges(CAutoRouterBox* box);
	void DeleteBoxAndPortEdges(CAutoRouterBox* box);

	CAutoRouterEdgeList& GetEdgeList(bool ishorizontal);

	CAutoRouterEdgeList horizontal;
	CAutoRouterEdgeList vertical;

// --- Path && Edges

	bool CanDeleteTwoEdgesAt(CAutoRouterPath* path, CPointListPath& points, POSITION pos) const;
	void DeleteTwoEdgesAt(CAutoRouterPath* path, CPointListPath& points, POSITION pos);
	void DeleteSamePointsAt(CAutoRouterPath* path, CPointListPath& points, POSITION pos);
	bool SimplifyPaths();
	void CenterStairsInPathPoints(CAutoRouterPath* path, RoutingDirection hintstartdir, RoutingDirection hintenddir);
	void SimplifyPathPoints(CAutoRouterPath* path);
	void ConnectAllDisconnectedPaths();

public:
	CAutoRouterBox* CreateBox(void) const;
	void AddBox(CAutoRouterBox* box);
	void DeleteBox(CAutoRouterBox* box);
	void ShiftBoxBy(CAutoRouterBox* box, const CPoint& offset);

	long AutoRoute(long aspect);
	void DeletePath(CAutoRouterPath* path);
	void DeleteAll(bool addBackSelfEdges = false);
	CAutoRouterPath* GetPathAt(const CPoint& point, long nearness);
	CAutoRouterPath* AddPath(bool isAutoRouted, CAutoRouterPort* startport, CAutoRouterPort* endport);
	bool IsEdgeFixed(CAutoRouterPath* path, const CPoint& startpoint, const CPoint& endpoint);

	CPoint* GetSelfPoints(void) const;
	const CAutoRouterPathList& GetPathList() const { return paths; }

	void Destroy(void);

private:
	CAutoRouterBoxList boxes;
	CAutoRouterPathList paths;

	CPoint selfpoints[4];
	void CalculateSelfPoints();

// --- Debug

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	void AssertValidBox(CAutoRouterBox* box) const;
	void AssertValidPath(CAutoRouterPath* path) const;
	void DumpPaths(int pos, int c);
	void DumpEdgeLists(void);
#endif
};
