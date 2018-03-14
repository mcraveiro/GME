//COM'd by Tamas (check AutoRouterEdge.h)
//#ifndef __AREDGELS_H
//#define __AREDGELS_H
//
//class CArBox;
//class CArPort;
//class CArPath;
//class CArGraph;
//
//
//// --------------------------- SArEdge
//
//class CArEdgeList;
//struct SArEdge
//{
//	friend CArEdgeList;
//	friend CArGraph;
//private:
//
//	CObject* owner;
//	CPoint* startpoint_prev;
//	CPoint* startpoint;
//	CPoint* endpoint;
//	CPoint* endpoint_next;
//
//	float position_y;
//	long position_x1;
//	long position_x2;
//	int bracket_closing : 1;
//	int bracket_opening : 1;
//	
//	SArEdge* order_next;
//	SArEdge* order_prev;
//
//	long section_x1;
//	long section_x2;
//	SArEdge* section_next;
//	SArEdge* section_down;
//
//	unsigned int edge_fixed : 1;
//	unsigned int edge_canpassed : 1;
//
//	SArEdge* block_prev;
//	SArEdge* block_next;
//	SArEdge* block_trace;
//
//	SArEdge* closest_prev;
//	SArEdge* closest_next;
//};
//
//
//// --------------------------- CArEdgeList
//
//
//class CArEdgeList
//{
//private:
//	CArEdgeList(int ishorizontal);
//	~CArEdgeList();
//
//	friend CArGraph;
//
//private:
//	CArGraph* owner;
//
//// --- Edges
//
//private:
//	SArEdge* CreateEdge() const;
//	void AddEdges(CArPath* path);
//	void AddEdges(CArBox* box);
//	void AddEdges(CArPort* port);
//	void AddEdges(CArGraph* graph);
//	void DeleteEdges(CObject* object);
//	void DeleteAllEdges();
//
//	int IsEmpty() const;
//
//private:
//	int ishorizontal;
//
//private:
//	SArEdge* GetEdge(CPoint* startpoint, CPoint* endpoint) const;
//	SArEdge* GetEdgeAt(CPoint point, int nearness = 0) const;
//
//#ifdef _DEBUG
//public:
//	void AssertValidPathEdges(CArPath* path) const;
//#endif _DEBUG
//
//// --- Position
//
//private:
//	long Position_GetRealY(const SArEdge* edge) const;
//	void Position_SetRealY(SArEdge* edge, long y) const;
//	void Position_GetRealX(const SArEdge* edge, long& x1, long& x2) const;
//	void Position_GetRealO(const SArEdge* edge, long& o1, long& o2) const;
//
//	void Position_LoadY(SArEdge* edge) const;
//	void Position_LoadB(SArEdge* edge) const;
//	void PositionAll_StoreY() const;
//
//	void PositionAll_LoadX() const;
//
//#ifdef _DEBUG
//private:
//	void AssertValidPositions() const;
//#endif
//
//// --- Order
//
//private:
//	void Con_Order();
//	void Des_Order();
//
//private:
//	void InsertBefore(SArEdge* edge, SArEdge* before);
//	void InsertAfter(SArEdge* edge, SArEdge* after);
//	void InsertLast(SArEdge* edge);
//	void Insert(SArEdge* edge);
//	void Remove(SArEdge* edge);
//	void Delete(SArEdge* edge);
//
//	SArEdge* SlideButNotPassEdges(SArEdge* edge, float y);
//
//private:
//	SArEdge* order_first;
//	SArEdge* order_last;
//
//#ifdef _DEBUG
//private:
//	void AssertValidOrder() const;
//#endif
//
//// --- Section
//
//private:
//	void Con_Section();
//	void Des_Section();
//
//private:
//	void Section_Reset();
//	void Section_BeginScan(SArEdge* blocker);
//	int Section_HasBlockedEdge();
//	SArEdge* Section_GetBlockedEdge();
//	int Section_IsImmediate();
//
//private:
//	SArEdge* section_first;
//	SArEdge* section_blocker;
//	SArEdge** section_ptr2blocked;
//
//#ifdef _DEBUG
//public:
//	void Section_AssertLevel(SArEdge* level, long x1, long x2) const;
//	void AssertValidSection() const;
//	void AssertSectionReady() const;
//#endif
//
//// --- Bracket
//
//private:
//	int Bracket_IsClosing(const SArEdge* edge) const;
//	int Bracket_IsOpening(const SArEdge* edge) const;
//	int Bracket_IsSmallGap(const SArEdge* blocked, const SArEdge* blocker) const;
//
//	int Bracket_ShouldBeSwitched(const SArEdge* edge, const SArEdge* next) const;
//
//// --- Block
//
//private:
//	int Block_PushBackward(SArEdge* blocked, SArEdge* blocker);
//	int Block_PushForward(SArEdge* blocked, SArEdge* blocker);
//	int Block_ScanForward();
//	int Block_ScanBackward();
//
//	int Block_SwitchWrongs();
//};
//
//
//#endif//__AREDGELS_H
