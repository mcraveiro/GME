// AutoRouterBox.h : Declaration of the CAutoRouterBox

#pragma once
#include "../resource.h"       // main symbols



#include "ArHelper.h"

#define ED_MAXCOORD		100000
#define ED_MINCOORD		0
#define ED_SMALLGAP		15

class CAutoRouterPort;
class CAutoRouterGraph;
class CAutoRouterEdgeList;
class CAutoRouterPath;

#include "AutoRouterPort.h"
#include "AutoRouterGraph.h"

#include <vector>
typedef std::vector<CAutoRouterPort*> CAutoRouterPortList;

// CAutoRouterBox

class CAutoRouterBox : public CObject
{
public:
	CAutoRouterBox();

private:
	void CalculateSelfPoints();
	void DeleteAllPorts();

public:
	CAutoRouterGraph* GetOwner(void) const;
	bool HasOwner(void) const;
	void SetOwner(CAutoRouterGraph* graph);

	CAutoRouterPort* CreatePort(void) const;
	bool HasNoPort(void) const;
	long GetPortCount(void) const;
	bool IsAtomic(void) const;
	void AddPort(CAutoRouterPort* port);
	void DeletePort(CAutoRouterPort* port);
	const CAutoRouterPortList& GetPortList(void) const;

	CRect GetRect(void) const;
	bool IsRectEmpty(void) const;
	void SetRect(const CRect& r);
	void SetRectByPoint(const CPoint& point);
	void ShiftBy(const CPoint& offset);
	CPoint* GetSelfPoints(void) const;

	bool IsBoxAt(const CPoint& point, long nearness) const;
	bool IsBoxClip(const CRect& r) const;
	bool IsBoxIn(const CRect& r) const;

	void Destroy(void);

private:
	CRect rect;
	CPoint selfpoints[4];
	bool atomic;

	CAutoRouterPortList ports;
	CAutoRouterGraph* owner;

// --- Debug

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	void AssertValidPort(CAutoRouterPort* port) const;
#endif
};
