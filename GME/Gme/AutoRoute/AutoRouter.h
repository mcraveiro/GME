#ifndef GME_AutoRouter_h
#define GME_AutoRouter_h

#include "stdafx.h"
#include "afxcoll.h"
#include "afxtempl.h"

#include "AutoRouterPath.h"

class CGuiConnection;

typedef CList<CPoint, CPoint&>	CPointList;


class CAutoRouter {
public:
	CAutoRouter();
	~CAutoRouter();

private:
	CAutoRouterGraph* router;
	int routeret;

public:
	bool AutoRoute(long aspect);
	void AutoRoute(CGuiFcoList& fcos, long aspect);
	void Fill(CGuiFcoList& fcos);
	void Clear(CGuiFcoList& fcos);
	void SetPortPreferences(CAutoRouterPort* portBox, CGuiPort* object);
	void SetPathPreferences(CAutoRouterPath* path, CGuiConnection* conn);
	bool RemoveDeletedCustomPathDataFromGuiConnections(void);
	void AddObjects(CGuiFcoList& fcos);
	void AddFco(CGuiFco* fco);
	void AddObject(CGuiObject* obj);
	void AddConnections(CGuiConnectionList& connList);
	void AddConnection(CGuiConnection* conn);
	void DeleteObjects(CGuiObjectList& objectList);
	void DeleteObject(CGuiObject* model);
	void ClearRouterPorts(CGuiPortList& portList);
	void DeleteConnection(CGuiConnection* conn);
	CGuiConnection* FindConnection(CPoint& pt) const;
	void NudgeObjects(CGuiObjectList& objectList, int right, int down, long aspect);
};

#endif // whole file