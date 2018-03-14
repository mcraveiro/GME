//COM'd by Tamas (check AutoRouterGraph.h, AutoRouterBox.h, AutoRouterPath.h, AutoRouterPort.h)
//#ifndef __ARGRAPH_H
//#define __ARGRAPH_H
//
//#ifndef __AREDGELS_H
//#include "ArEdgeLs.h"
//#endif
//
//#ifndef __ARHELPER_H
//#include "ArHelper.h"
//#endif
//
//class CArBox;
//class CArPort;
//class CArPath;
//class CArEdgeList;
//class CArGraph;
//
//#define ED_MAXCOORD		100000
//#define ED_MINCOORD		0
//#define ED_SMALLGAP		15
//
//
//// --------------------------- Lists
//
//
//typedef CTypedPtrList<CObList, CArPort*>	CArPortList;
//typedef CTypedPtrList<CObList, CArBox*>		CArBoxList;
//typedef CTypedPtrList<CObList, CArPath*>	CArPathList;
//typedef CList<CPoint, CPoint&>				CPointList;
//
//
//// --------------------------- CArPort
//
//
//#define ARPORT_EndOnTop					0x0001
//#define ARPORT_EndOnRight				0x0002
//#define ARPORT_EndOnBottom				0x0004
//#define ARPORT_EndOnLeft				0x0008
//#define ARPORT_EndOnAll					0x000F
//
//#define ARPORT_StartOnTop				0x0010
//#define ARPORT_StartOnRight				0x0020
//#define ARPORT_StartOnBottom			0x0040
//#define ARPORT_StartOnLeft				0x0080
//#define ARPORT_StartOnAll				0x00F0
//
//#define ARPORT_ConnectOnAll				0x00FF
//#define ARPORT_ConnectToCenter			0x0100
//
//#define ARPORT_StartEndHorizontal		0x00AA
//#define ARPORT_StartEndVertical			0x0055
//
//#define ARPORT_Default					0x00FF
//
//
//class CArPort: public CObject
//{
//public:
//	CArPort();
//	~CArPort();
//
//	friend CArBox;
//	friend CArEdgeList;
//
//
//friend CArGraph;
//friend CArPath;
//friend CArEdgeList;
//
//	CArBox* GetOwner() const;
//
//private:
//	void SetOwner(CArBox* box);
//
//	CArBox* owner;
//
//// --- StartEndPoints (FOR EXTERNAL USE)
//
////vt public:
//private:
//	int CanHaveStartEndPointOn(EArDir dir, int isstart) const;
//	int CanHaveStartEndPoint(int isstart) const;
//	int CanHaveStartEndPointHorizontal(int ishorizontal) const;
//	int IsConnectToCenter() const { return (attributes & ARPORT_ConnectToCenter) != 0; }
//	EArDir GetStartEndDirTo(CPoint point, int isstart, EArDir notthis = Dir_None) const;
//
//	int CanCreateStartEndPointAt(CPoint point, int isstart, int nearness = 0) const;
//	CPoint CreateStartEndPointAt(CPoint point, int isstart) const;
//	CPoint CreateStartEndPointOn(EArDir dir) const;
//	CPoint CreateStartEndPointTo(CPoint point, int isstart) const;
//
//	
//	unsigned int GetAttributes() const { return attributes; }
//	bool HasLimitedDirs() { return limitedDirections; }
//
////vt 
//public:
//	void SetLimitedDirs( bool p_ltd) { limitedDirections = p_ltd; }
//	void SetAttributes(unsigned int attr);
//
//private:
//	unsigned int attributes;
//
//// --- Data (FOR EXTERNAL USE)
//
////vt public:
//private:
//	CRect GetRect() const { return rect; }
//	CPoint GetCenter() const { return rect.CenterPoint(); }
//	int IsRectEmpty() const { return rect.IsRectEmpty(); }
//	EArDir OnWhichEdge(CPoint point) const { return ::OnWhichEdge(rect, point); }
//
//	int IsPortAt(CPoint point, int nearness = 0) const { return IsPointIn(point, rect, nearness); }
//	int IsPortIn(CRect r) const { return IsRectIn(rect, r); }
//	int IsPortClip(CRect r) const { return IsRectClip(rect, r); }
//
//	
//	void ShiftBy(CSize offset);
////vt
//public:
//	void SetRect(CRect rect);
//
//private:
//	void CalculateSelfPoints();
//
//	bool  limitedDirections; // strictly respect the preferred directions (for ports East/West directions must be respected)
//	CRect rect;
//	CPoint selfpoints[4];
//
//// --- Serialize
//
//public:
//	DECLARE_SERIAL(CArPort);
//	virtual void Serialize(CArchive& ar);
//
//
//private:
//	int MatchStoredRef(CArPort* stored_ref) const { return stored_ref == stored_ref; }
//	void ClearStoredRefs() { stored_ref = 0; }
//
//private:
//	CArPort* stored_ref;
//
//// --- Debug
//
//#ifdef _DEBUG
//public:
//	virtual void AssertValid() const;
//	void AssertValidStartEndPoint(CPoint point, EArDir dir, int isstart) const;
//#endif
//};
//
//
//// --------------------------- CArBox
//
//
//class CArBox: public CObject
//{
//public:
//	CArBox();
//	~CArBox();
//
//	friend CArGraph;
//	friend CArEdgeList;
//
//	//vt:
//friend CArBox;
//friend CArPort;
//friend CArPath;
//
//
//	CArGraph* GetOwner() const;
//
//private:
//	void SetOwner(CArGraph* graph);
//
//	CArGraph* owner;
//
//// --- Ports (FOR EXTERNAL USE)
//
//public:
//	CArPort* CreatePort() const;
//	void Add(CArPort* port);
//	void Delete(CArPort* port);
//
//	//vt:
//private:
//	void DeleteAllPorts();
//	const CArPortList& GetPortList() const { return ports; }
//	int HasNoPort() const { return ports.IsEmpty(); }
//	int GetPortCount() const { return ports.GetCount(); }
//
//	CArPort* GetPortAt(CPoint point, int nearness = 0) const;
//
//	CArPort* AddPort(CRect rect, unsigned attr);
//
//private:
//	CArPortList ports;
//
//// --- Atomic Port (FOR EXTERNAL USE)
//
////vt public:
//private:
//	void SetAtomicPort(CRect rect, unsigned int attr);
//	void SetAtomicPort(CArPort* port);
//	CArPort* GetAtomicPort() const;
//	int IsAtomic() const { return atomic; }
//
//private:
//	int atomic;
//
//// --- Data (FOR EXTERNAL USE)
//
////vt public:
//private:
//	CRect GetRect() const { return rect; }
//	int IsRectEmpty() const { return rect.IsRectEmpty(); }
//
//	int IsBoxAt(CPoint point, int nearness = 0) const { return IsPointIn(point, rect, nearness); }
//	int IsBoxClip(CRect r) const { return IsRectClip(rect, r); }
//	int IsBoxIn(CRect r) const { return IsRectIn(rect, r); }
//
//
//	void SetRect(CPoint point) { ShiftBy(point - rect.TopLeft()); }
//	void ShiftBy(CSize offset);
//
////vt:
//public:
//	void SetRect(CRect rect);
//
//private:
//	void CalculateSelfPoints();
//
//	CRect rect;
//	CPoint selfpoints[4];
//
//// --- Serialize
//
//public:
//	DECLARE_SERIAL(CArBox);
//	virtual void Serialize(CArchive& ar);
////vt public:
//private:
//	CArPort* ResolveStoredRef(CArPort* stored_ref);
//	void ClearStoredRefs();
//
//// --- Debug
//
//#ifdef _DEBUG
//public:
//	virtual void AssertValid() const;
//	void AssertValidPort(const CArPort* port) const;
//#endif
//};
//
//
//// --------------------------- CArPath
//
//#define ARPATH_EndOnDefault		0x0000
//#define ARPATH_EndOnTop			0x0010
//#define ARPATH_EndOnRight		0x0020
//#define ARPATH_EndOnBottom		0x0040
//#define ARPATH_EndOnLeft		0x0080
//#define ARPATH_EndMask			(ARPATH_EndOnTop | ARPATH_EndOnRight | ARPATH_EndOnBottom | ARPATH_EndOnLeft)
//
//
//#define ARPATH_StartOnDefault	0x0000
//#define ARPATH_StartOnTop		0x0100
//#define ARPATH_StartOnRight		0x0200
//#define ARPATH_StartOnBottom	0x0400
//#define ARPATH_StartOnLeft		0x0800
//#define ARPATH_StartMask		(ARPATH_StartOnTop | ARPATH_StartOnRight | ARPATH_StartOnBottom | ARPATH_StartOnLeft)
//
//#define ARPATH_HighLighted		0x0002		// attributes
//#define ARPATH_Fixed			0x0001
//#define ARPATH_Default			0x0000
//
//#define ARPATHST_Connected		0x0001		// states
//#define ARPATHST_Default		0x0000
//
//class CArPath: public CObject
//{
//public:
//	CArPath();
//	~CArPath();
//
//	friend CArGraph;
//	friend CArEdgeList;
//	friend CArPort;
//	friend CArBox;
//
//	CArGraph* GetOwner() const;
//
//private:
//	void SetOwner(CArGraph* graph);
//
//	CArGraph* owner;
//
//// --- Ports (FOR EXTERNAL USE)
//
//public:
//private:
//	void SetStartPort(CArPort* port);
//	void SetEndPort(CArPort* port);
//	void ClearPorts();
//	CArPort* GetStartPort() const;
//	CArPort* GetEndPort() const;
//	CArBox* GetStartBox() const { return GetStartPort()->GetOwner(); }
//	CArBox* GetEndBox() const { return GetEndPort()->GetOwner(); }
//
//private:
//	CArPort* startport;							// reference
//	CArPort* endport;							// reference
//
//// --- Points (FOR EXTERNAL USE)
//
//public:
//	const CPointList& GetPointList() const { return points; }
//private:
//	void AddTail(CPoint point);
//	void DeleteAll();
//	
//	int HasNoPoint() const { return points.IsEmpty(); }
//	int GetPointCount() const { return points.GetCount(); }
//
//	CPoint GetStartPoint() const;
//	CPoint GetEndPoint() const;
//	CPoint GetOutOfBoxStartPoint() const;
//	CPoint GetOutOfBoxEndPoint() const;
//
//	POSITION GetPointPosAt(CPoint point, int nearness = 0) const;
//	POSITION GetEdgePosAt(CPoint point, int nearness = 0) const;
//
//private:
//	void SimplifyTrivially();
//
//	CPointList points;
//
//// --- Edges
//
//public:
//private:
//	POSITION GetHeadEdge(CPoint& start, CPoint& end) const;
//	POSITION GetTailEdge(CPoint& start, CPoint& end) const;
//	void GetNextEdge(POSITION& pos, CPoint& start, CPoint& end) const;
//	void GetPrevEdge(POSITION& pos, CPoint& start, CPoint& end) const;
//	void GetEdge(POSITION pos, CPoint& start, CPoint& end) const;
//
//private:
//	POSITION GetHeadEdgePtrs(CPoint*& start, CPoint*& end);
//	POSITION GetTailEdgePtrs(CPoint*& start, CPoint*& end);
//	void GetNextEdgePtrs(POSITION& pos, CPoint*& start, CPoint*& end);
//	void GetPrevEdgePtrs(POSITION& pos, CPoint*& start, CPoint*& end);
//	void GetEdgePtrs(POSITION pos, CPoint*& start, CPoint*& end);
//	CPoint* GetStartPoint(POSITION pos);
//	CPoint* GetEndPoint(POSITION pos);
// 	CPoint* GetPointBeforeEdge(POSITION pos);
//	CPoint* GetPointAfterEdge(POSITION pos);
//
//	POSITION GetEdgePosBeforePoint(POSITION pos) const;
//	POSITION GetEdgePosAfterPoint(POSITION pos) const;
//	POSITION GetEdgePosForStartPoint(CPoint* startpoint);
//
//	int IsEdgeHorizontal(POSITION pos);
//
//// --- Data (FOR EXTERNAL USE)
//
//public:
//private:
//	CRect GetSurroundRect() const;
//
//	int IsPathAt(CPoint point, int nearness = 0) const { return GetEdgePosAt(point, nearness) != NULL; }
//	int IsPathClip(CRect rect) const;
//
//	void SetAttributes(unsigned int attr);
//	unsigned int GetAttributes() const { return attributes; }
//
//	int IsFixed() const { return (attributes & ARPATH_Fixed) != 0; }
//	int IsMoveable() const { return (attributes & ARPATH_Fixed) == 0; }
//	int IsHighLighted() const { return (attributes & ARPATH_HighLighted) != 0; }
//
//	int GetState() const { return state; };
//	int IsConnected() const { return (state & ARPATHST_Connected) != 0; }
//
//	EArDir GetEndDir() const;
//	EArDir GetStartDir() const;
//public:
//	void SetEndDir(unsigned int arpath_end) { attributes = (attributes & ~ARPATH_EndMask) + arpath_end; }
//	void SetStartDir(unsigned int arpath_start) { attributes = (attributes & ~ARPATH_StartMask) + arpath_start; }
//
//private:
//	void SetState(int state);
//
//public:
//private:
//	unsigned int attributes;
//	int state;
//
//// --- External Pointer (ONLY FOR EXTERNAL USE)
//
//public:
//	void* GetExtPtr() const { return extptr; }
//	void SetExtPtr(void* p) { extptr = p; }
//private:
//	
//	
//
//	void* extptr;
//
//// --- Serialize
//
//public:
//private:
//	DECLARE_SERIAL(CArPath);
//	virtual void Serialize(CArchive& ar);
//
//	void LocateRefs();
//	int HasRealReferences() const { return ref_type == ref_type_valid; }
//	enum { ref_type_stored, ref_type_valid } ref_type;
//
//// --- Debug
//
//#ifdef _DEBUG
//public:
//	virtual void AssertValid() const;
//	void AssertValidPos(POSITION pos) const;
//	void AssertValidPoints() const;
//#endif
//};
//
//
//// --------------------------- CArGraph
//
//
//class CArGraph: public CObject
//{
//public:
//	CArGraph();
//	~CArGraph();
//
//	friend CArEdgeList;
//	friend CArBox;
//	friend CArPort;
//	friend CArPath;
//
//// --- Boxes (FOR EXTERNAL USE)
//
//public:
//	CArBox* CreateBox() const;
//	void Add(CArBox* box);
//	void Delete(CArBox* box);
//	void ShiftBy(CArBox* box, CSize offset);
//
//private:
//	void Remove(CArBox* box);
//
//	void DeleteAllBoxes();
//	const CArBoxList& GetBoxList() const { return boxes; }
//	int HasNoBox() const { return boxes.IsEmpty(); }
//	int GetBoxCount() const { return boxes.GetCount(); }
//
//	CArBox* GetBoxAt(CPoint point, int nearness = 0) const;
//	CArPort* GetPortAt(CPoint point, int nearness = 0) const;
//
//	void SetPortAttr(CArPort* port, unsigned int attr);
//
//	int IsRectClipBoxes(CRect rect) const;
//	int IsLineClipBoxes(CPoint p1, CPoint p2) const;
//	int CanBoxAt(CRect rect) const;
//
//
//	void SetBox(CArBox* box, CPoint topleft) { ShiftBy(box, topleft - box->rect.TopLeft()); }
//	int CanShiftBy(CArBox* box, CSize offset) const;
//	int CanSetBox(CArBox* box, CPoint topleft) const { return CanShiftBy(box, topleft - box->rect.TopLeft()); }
//
//	CArBox* AddAtomicPort(CRect rect, unsigned int attr);
//
//private:
//	CArBoxList boxes;
//
//// --- Paths (FOR EXTERNAL USE)
//
//private:
//	CArPath* CreatePath() const;
//	void Add(CArPath* path);
//	void Remove(CArPath* path);
//
//	void DeleteAllPaths();
//	const CArPathList& GetPathList() const { return paths; }
//	int HasNoPath() const { return paths.IsEmpty(); }
//	int GetPathCount() const { return paths.GetCount(); }
//
//
//	CArPath* GetPathPointAt(CPoint point, POSITION& pos, int nearness = 0) const;
//	CArPath* GetPathEdgeAt(CPoint point, POSITION& pos, int nearness = 0) const;
//
//	SArEdge* GetPathEdgeAt(CArPath* path, POSITION pos) const;
//	SArEdge* GetPathEdgeAt(CPoint point, int nearness = 0) const;
//
//	SArEdge* GetListEdgeAt(CPoint point, int nearness = 0) const;
//
//	CArPath* AddPath(const CPointList& points, unsigned int attr);
//	CArPath* AddPath(CPoint startpoint, CPoint endpoint);
//
//
//private:
//	CArPathList paths;
//
//// --- Boxes && Paths (FOR EXTERNAL USE)
//
//private:
//	int IsEmpty() const { return boxes.IsEmpty() && paths.IsEmpty(); }
//
//	CRect GetSurroundRect() const;
//
//public:
//	int AutoRoute();
//	void Delete(CArPath* path);
//	void DeleteAll() { DeleteAllPaths(); DeleteAllBoxes(); }
//	CArPath* GetPathAt(CPoint point, int nearness = 0) const;
//	CArPath* AddPath(CArPort* startport, CArPort* endport);
//
//// --- Navigation
//
//private:
//	CArBox* GetOutOfBox(CPoint& point, EArDir dir) const;
//
//	CArBox* GoToNextBox(CPoint& point, EArDir dir, long stophere) const;
//	CArBox* GoToNextBox(CPoint& point, EArDir dir, CPoint stophere) const 
//		{ return GoToNextBox(point, dir, GetPointCoord(stophere, dir)); }
//
//	CArBox* GoToNextBox(CPoint& point, EArDir dir, long stop1, long stop2) const
//		{ return GoToNextBox(point, dir, ChooseInDir(stop1, stop2,ReverseDir(dir))); }
//	CArBox* GoToNextBox(CPoint& point, EArDir dir, CPoint stop1, CPoint stop2) const
//		{ return GoToNextBox(point, dir, GetPointCoord(stop1, dir), GetPointCoord(stop2, dir)); }
//
//	void GetLimitsOfEdge(CPoint start, CPoint end, long& min, long& max) const;
//
//	int IsPointInBox(CPoint point) const { return GetBoxAt(point) != NULL; }
//	int CanGoInDir(CPoint point, EArDir dir) const { return !IsPointInBox(StepOneInDir(point, dir)); }
//
//private:
//	void Connect(CArPath* path);
//	void Connect(CArPath* path, CPoint startpoint, CPoint endpoint);
//
//	void ConnectPoints(CPointList& ret, CPoint startpoint, CPoint endpoint, EArDir hintstartdir, EArDir hintenddir);
//
//	void DisconnectAll();
//	void Disconnect(CArPath* path);
//
//	void DisconnectPathsClipping(CRect rect);
//	void DisconnectPathsFrom(CArBox* box);
//	void DisconnectPathsFrom(CArPort* port);
//
//// --- Edges
//
//private:
//	void AddEdges(CArGraph* graph) { horizontal.AddEdges(graph); vertical.AddEdges(graph); }
//	void AddEdges(CArBox* box) { horizontal.AddEdges(box); vertical.AddEdges(box); }
//	void AddEdges(CArPort* port) { horizontal.AddEdges(port); vertical.AddEdges(port); }
//	void AddEdges(CArPath* path) { horizontal.AddEdges(path); vertical.AddEdges(path); }
//	void DeleteEdges(CObject* object) { horizontal.DeleteEdges(object); vertical.DeleteEdges(object); }
//
//	void AddAllEdges();
//	void DeleteAllEdges() { horizontal.DeleteAllEdges(); vertical.DeleteAllEdges(); }
//
//	void AddBoxAndPortEdges(CArBox* box);
//	void DeleteBoxAndPortEdges(CArBox* box);
//
//	CArEdgeList& GetEdgeList(int ishorizontal) { return ishorizontal ? horizontal : vertical; }
//
//private://private:
//
//	CArEdgeList horizontal;
//	CArEdgeList vertical;
//
//// --- Path && Edges
//
//private:
//	int CanDeleteTwoEdgesAt(CArPath* path, POSITION pos);
//	void DeleteTwoEdgesAt(CArPath* path, POSITION pos);
//	void DeleteSamePointsAt(CArPath* path, POSITION pos);
//	int SimplifyPaths();
//	void CenterStairsInPathPoints(CArPath* path);
//	void SimplifyPathPoints(CArPath* path);
//	void ConnectAllDisconnectedPaths();
//
//// --- Data
//
//private:
//	void CalculateSelfPoints();
//
//	CPoint selfpoints[4];
//
//// --- Serialize
//
//public:
//	DECLARE_SERIAL(CArGraph);
//	virtual void Serialize(CArchive& ar);
//
//private:
//
//	CArPort* ResolveStoredRef(CArPort* stored_ref);
//	void LocateRefs();
//	void ClearStoredRefs();
//
//// --- Debug
//
//#ifdef _DEBUG
//public:
//	virtual void AssertValid() const;
//	void AssertValidBox(CArBox* box) const;
//	void AssertValidPath(CArPath* path) const;
//#endif
//
//};
//
//
//// --------------------------- Inline functions
//
//
//inline CPoint CArPort::CreateStartEndPointTo(CPoint point, int isstart) const
//{
//	return CreateStartEndPointOn(GetStartEndDirTo(point, isstart));
//}
//
//
//#endif//__ARGRAPH_H
