// AutoRouterPath.h : Declaration of the CAutoRouterPath

#pragma once
#include "../resource.h"       // main symbols

#include <vector>

#include "ArHelper.h"

#include "AutoRouterPort.h"
#include "AutoRouterBox.h"

#define ARPATH_EndOnDefault		0x0000
#define ARPATH_EndOnTop			0x0010
#define ARPATH_EndOnRight		0x0020
#define ARPATH_EndOnBottom		0x0040
#define ARPATH_EndOnLeft		0x0080
#define ARPATH_EndMask			(ARPATH_EndOnTop | ARPATH_EndOnRight | ARPATH_EndOnBottom | ARPATH_EndOnLeft)


#define ARPATH_StartOnDefault	0x0000
#define ARPATH_StartOnTop		0x0100
#define ARPATH_StartOnRight		0x0200
#define ARPATH_StartOnBottom	0x0400
#define ARPATH_StartOnLeft		0x0800
#define ARPATH_StartMask		(ARPATH_StartOnTop | ARPATH_StartOnRight | ARPATH_StartOnBottom | ARPATH_StartOnLeft)

#define ARPATH_HighLighted		0x0002		// attributes
#define ARPATH_Fixed			0x0001
#define ARPATH_Default			0x0000

#define ARPATHST_Connected		0x0001		// states
#define ARPATHST_Default		0x0000


typedef CList<CPoint, CPoint&>	CPointList;


class CAutoRouterGraph;
class CAutoRouterPort;

// CAutoRouterPath

class CAutoRouterPath : public CObject
{
public:
	CAutoRouterPath();
	~CAutoRouterPath();

private:
	CAutoRouterGraph* owner;

// --- Ports

	CAutoRouterPort* startport;							// reference
	CAutoRouterPort* endport;							// reference

	POSITION GetPointPosAt(const CPoint& point, int nearness = 0) const;
	POSITION GetEdgePosAt(const CPoint& point, int nearness = 0) const;

// --- Edges
	CPointListPath points;

private:
	unsigned int attributes;
	int state;
	bool isAutoRoutingOn;

	std::vector<CustomPathData> customPathData;
	std::vector<CustomPathData> pathDataToDelete;

public:
	CAutoRouterGraph* GetOwner(void) const;
	bool HasOwner(void) const;
	void SetOwner(CAutoRouterGraph* graph);
	// Ports
	void SetStartPort(CAutoRouterPort* port);
	void SetEndPort(CAutoRouterPort* port);
	void ClearPorts(void);
	CAutoRouterPort* GetStartPort(void);
	CAutoRouterPort* GetEndPort(void);
	// Points
	void AddTail(CPoint& p);
	void DeleteAll(void);

	bool HasNoPoint(void) const;
	long GetPointCount(void) const;

	CPoint GetStartPoint(void) const;
	CPoint GetEndPoint(void) const;
	CRect GetStartBox(void) const;
	CRect GetEndBox(void) const;
	CPoint GetOutOfBoxStartPoint(RoutingDirection hintDir) const;
	CPoint GetOutOfBoxEndPoint(RoutingDirection hintDir) const;

	void SimplifyTrivially(void);

	CPointListPath& GetPointList(void);
	void SetPoints(CPointListPath& pls);
	// Edges

	CRect GetSurroundRect(void) const;

	bool IsEmpty(void) const;
	bool IsPathAt(const CPoint& point, long nearness) const;
	bool IsPathClip(const CRect& r, bool isStartOrEndRect = false) const;

	void SetAttributes(long attr);
	long GetAttributes(void) const;

	bool IsFixed(void) const;
	bool IsMoveable(void) const;
	bool IsHighLighted(void) const;

	long GetState(void) const;
	bool IsConnected(void) const;
	void SetState(long state);

	RoutingDirection GetEndDir(void) const;
	RoutingDirection GetStartDir(void) const;

	void SetEndDir(long arpath_end);
	void SetStartDir(long arpath_start);
	// CustomData
	void SetCustomPathData(const std::vector<CustomPathData>& pDat);
	void ApplyCustomizationsBeforeAutoConnectPoints(CPointListPath& plist);
	void ApplyCustomizationsAfterAutoConnectPointsAndStuff(void);
	void RemovePathCustomizations(void);
	void MarkPathCustomizationsForDeletion(long asp);
	void RemoveInvalidPathCustomizations(long asp);
	bool AreTherePathCustomizations(void) const;
	bool AreThereDeletedPathCustomizations(void) const;
	void GetDeletedCustomPathData(std::vector<CustomPathData>& cpd) const;
	void GetCustomizedEdgeIndexes(std::vector<int>& indexes) const;

	bool IsAutoRouted(void) const;
	void SetAutoRouting(bool autoRoutingState);
	// Other
	void Destroy(void);

// --- External Pointer (ONLY FOR EXTERNAL USE)

	void* GetExtPtr() const;
	void SetExtPtr(void* p);
private:
	void* extptr;

// --- Debug
#ifdef _DEBUG
public:
	virtual void AssertValid();
	void AssertValidPos(POSITION pos) const;
	void AssertValidPoints() const;
#endif
};
