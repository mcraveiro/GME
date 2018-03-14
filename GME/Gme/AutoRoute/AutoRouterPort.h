// AutoRouterPort.h : Declaration of the CAutoRouterPort

#pragma once
#include "../resource.h"       // main symbols

#include "ArHelper.h" 

#include "AutoRouterBox.h"

#include <vector>

#define ARPORT_EndOnTop					0x0001
#define ARPORT_EndOnRight				0x0002
#define ARPORT_EndOnBottom				0x0004
#define ARPORT_EndOnLeft				0x0008
#define ARPORT_EndOnAll					0x000F

#define ARPORT_StartOnTop				0x0010
#define ARPORT_StartOnRight				0x0020
#define ARPORT_StartOnBottom			0x0040
#define ARPORT_StartOnLeft				0x0080
#define ARPORT_StartOnAll				0x00F0

#define ARPORT_ConnectOnAll				0x00FF
#define ARPORT_ConnectToCenter			0x0100

#define ARPORT_StartEndHorizontal		0x00AA
#define ARPORT_StartEndVertical			0x0055

#define ARPORT_Default					0x00FF



class CAutoRouterBox;
class CAutoRouterEdgeList;
class CAutoRouterGraph;

// CAutoRouterPort

class CAutoRouterPort : public CObject
{
public:
	CAutoRouterPort();
	virtual ~CAutoRouterPort();

private:
	void CalculateSelfPoints();

public:
	CAutoRouterBox* GetOwner(void) const;
	bool HasOwner(void) const;
	void SetOwner(CAutoRouterBox* box);

	CRect GetRect(void) const;
	bool IsRectEmpty(void) const;
	CPoint GetCenter(void) const;
	void SetRect(const CRect& r);
	void ShiftBy(const CPoint& offset);
	CPoint* GetSelfPoints(void) const;

	long GetAttributes(void) const;
	void SetAttributes(long attr);
	bool IsConnectToCenter(void) const;
	bool HasLimitedDirs() const;
	void SetLimitedDirs(bool ltd);

	bool IsPortAt(const CPoint& point, long nearness) const;
	bool IsPortClip(const CRect& r) const;
	bool IsPortIn(const CRect& r) const;
	RoutingDirection OnWhichEdge(const CPoint& p) const;

	bool CanHaveStartEndPointOn(RoutingDirection dir, bool isStart) const;
	bool CanHaveStartEndPoint(bool isStart) const;
	bool CanHaveStartEndPointHorizontal(bool isHorizontal) const;
	RoutingDirection GetStartEndDirTo(const CPoint& point, bool isStart, RoutingDirection notthis = Dir_None) const;

	bool CanCreateStartEndPointAt(const CPoint& point, bool isStart, long nearness) const;
	CPoint CreateStartEndPointAt(const CPoint& point, bool isStart) const;
	CPoint CreateStartEndPointTo(const CPoint& point, bool isStart) const;
	CPoint CreateStartEndPointOn(RoutingDirection dir) const;

private:
	unsigned int attributes;

	bool  limitedDirections; // strictly respect the preferred directions (for ports East/West directions must be respected)
	CRect rect;
	CPoint selfpoints[4];

	CAutoRouterBox* owner;

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	void AssertValidStartEndPoint(const CPoint& point, RoutingDirection dir, bool isStart);
#endif
};
