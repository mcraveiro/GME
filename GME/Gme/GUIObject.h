#ifndef _GUIOBJECT_INCLUDED_
#define _GUIOBJECT_INCLUDED_

#include "Autoroute/AutoRouter.h"
#include "GMEStd.h"
#include <vector>
#include <memory>

class CGMEView;
class CModelGrid;
class CDecoratorEventSink;
class CAnnotatorEventSink;

extern CModelGrid modelGrid;

void SetLocation(CRect& location, CPoint pt);

class CGuiBase : public CObject
{
public:
	CGuiBase() : flag(0) {}
	CString& GetName() {return name;}
	CString& GetID() {return id;}

public:
	CString name;
	CString id;

protected:
	int flag;
};

class CGuiAspect : public CGuiBase
{
public:
	CGuiAspect(CGuiMetaAspect* meta, CGuiObject* p, int ind, int pind, CComPtr<IMgaDecorator>& decor,
			   CComPtr<IMgaElementDecorator>& newDecor, CComObjPtr<CDecoratorEventSink>& decorEventSink);
	virtual ~CGuiAspect();


public:
	void InitPorts();
	
	CAutoRouterBox* GetRouterBox()					{ return routerBox; }
	void SetRouterBox(CAutoRouterBox* rbox)			{ routerBox = rbox; }
	CAutoRouterBox* GetRouterNameBox()				{ return routerNameBox; }
	void SetRouterNameBox(CAutoRouterBox* rnbox)	{ routerNameBox = rnbox; }

	const CRect& GetLocation()						{ return loc; }
	const CRect& GetNameLocation()					{ return nameLoc; }
	void SetLocation(const CRect& location);
	CComPtr<IMgaDecorator> GetDecorator()			{ return decorator; }
	CComPtr<IMgaElementDecorator> GetNewDecorator()	{ return newDecorator; }
	int	GetIndex()									{ return index; }
	CGuiPortList& GetPortList()						{ return ports; }
	CGuiObject* GetParent()							{ return parent; }
	
public:
	CGuiObject*				parent;
	CGuiMetaAspect*			guiMeta;

private:
	int						parentIndex;
	int						index;
	CGuiPortList			ports;
	CComPtr<IMgaDecorator>	decorator;
	CComPtr<IMgaElementDecorator>	newDecorator;
	CComObjPtr<CDecoratorEventSink>	decoratorEventSink;
	unsigned long			features;
	CRect					loc;
	CRect					nameLoc;
	CAutoRouterBox*			routerBox;
	CAutoRouterBox*			routerNameBox;
};

class CGuiPort : public CGuiBase
{
public:
	CGuiPort(CGuiAspect *asp,CComPtr<IMgaFCO> fco);
public:
	const CGuiConnectionList& GetInConns()		{ return inConns; }
	const CGuiConnectionList& GetOutConns()		{ return outConns; }

	void ReadARPreferences();
	bool GetPreference(CString& val, CString path);
	bool GetARPref(int dir);
	bool IsVisible();
	void SetVisible(bool v);
	CRect GetLocation();
	CAutoRouterPort* GetRouterPort()			{ return routerPort; }
	void SetRouterPort(CAutoRouterPort* port)	{ routerPort = port; }
	CString GetInfoText()						{ return name; }
	bool IsRealPort();

public:
	CComPtr<IMgaFCO> mgaFco;
	CGuiAspect* parent;
	CGuiConnectionList inConns;
	CGuiConnectionList outConns;
	CComPtr<IMgaMetaFCO> metaFco;
	CComPtr<IMgaMetaRole> metaRole;

private:
	bool autorouterPrefs[GME_AR_NUM];
	CAutoRouterPort* routerPort;
};

class AnnotatorDecoratorData
{
public:
	AnnotatorDecoratorData();
	AnnotatorDecoratorData(CComPtr<IMgaElementDecorator>& nD, CComObjPtr<CAnnotatorEventSink>& aES, const CRect& loc);
	virtual ~AnnotatorDecoratorData();

	CComPtr<IMgaElementDecorator>	decorator;
	CComObjPtr<CAnnotatorEventSink>	annotatorEventSink;
	CRect							location;
};

class CGuiAnnotator : public CGuiBase
{
public:
	CGuiAnnotator(CComPtr<IMgaModel>& pModel, CComPtr<IMgaRegNode>& mRootNode, CGMEView* vw, int numAsp);
	virtual ~CGuiAnnotator();

	void InitDecorator(int asp);
	void ToggleGrayOut()							{ GrayOut(!grayedOut); }
	void SetAspect(int a)							{ parentAspect = (a < numParentAspects ? a : 0); }
	bool IsVisible(int aspect = -1);
	bool IsResizable(void) const;
	bool IsSpecial()                                { return special; }
	void Draw(HDC pDC, Gdiplus::Graphics* gdip);
	void GrayOut(bool set = true);

	const CRect& GetLocation(int aspect = -1);
	void  SetLocation(const CRect& loc, int aspect = -1, bool doMga = true);
	void  ReadLocation(int aspect, CComPtr<IMgaRegNode> &aspNode);
	void  WriteLocation(int aspect = -1);

	static void SetAspect(CGuiAnnotatorList& list, int asp);
	static void GrayOutAnnotations(CGuiAnnotatorList& list, bool set = true);
	static void NudgeAnnotations(CGuiAnnotatorList& annotatorList, int right, int down);
	static void GetExtent(CGuiAnnotatorList& annotatorList, CRect& rect);
	static void GetRectList(CGuiAnnotatorList& selectedAnnotations, CRectList& annRects);
	static void MoveAnnotations(CGuiAnnotatorList& annList, CPoint& pt);
	static void FindUpperLeft(CGuiAnnotatorList& anns, int& left, int& top);
	static void ShiftAnnotations(CGuiAnnotatorList& annList, CPoint& shiftBy);
	static bool Showable( CComPtr<IMgaRegNode>& mRootNode, CComPtr<IMgaFCO>& baseType);
	static int  Hide( CComPtr<IMgaRegNode>& mRootNode);
	CComPtr<IMgaElementDecorator> GetDecorator(int asp) { return decoratorData[asp]->decorator; }

public:
	CComPtr<IMgaRegNode>						rootNode;

protected:
	CComPtr<IMgaModel>							model;
	int											numParentAspects;
	int											parentAspect;
	bool										grayedOut;
	CGMEView*									view;
	std::vector<AnnotatorDecoratorData*>		decoratorData;
	bool										special;
};

class CGuiReference;
class CGuiCompoundReference;
class CGuiSet;

class CGuiFco : public CGuiBase
{
public:
	CGuiFco(CComPtr<IMgaFCO>& pt, CComPtr<IMgaMetaRole>& role, CGMEView* vw, int numAsp);
	virtual ~CGuiFco() {}

	// This is a trick to speed up dynamic_cast
	virtual CGuiObject*				dynamic_cast_CGuiObject(void)				{ return NULL; }
	virtual CGuiCompound*			dynamic_cast_CGuiCompound(void)				{ return NULL; }
	virtual CGuiModel*				dynamic_cast_CGuiModel(void)				{ return NULL; }
	virtual CGuiReference*			dynamic_cast_CGuiReference(void)			{ return NULL; }
	virtual CGuiCompoundReference*	dynamic_cast_CGuiCompoundReference(void)	{ return NULL; }
	virtual CGuiSet*				dynamic_cast_CGuiSet(void)					{ return NULL; }
	virtual CGuiConnection*			dynamic_cast_CGuiConnection(void)			{ return NULL; }

public:
	bool IsReal()									{return (mgaFco != NULL);}
	bool IsType()									{return isType;}
	void ToggleGrayOut()							{ GrayOut(!grayedOut); }
	void SetAspect(int a)							{ /*VERIFY(a < numParentAspects);*/ parentAspect = (a < numParentAspects ? a : 0); }
	metaref_type GetRoleMetaRef();
	bool IsPrimary(CGuiMetaModel* guiMetaModel, int aspectInd);
	bool IsPrimary(CGuiMetaModel* guiMetaModel, CGuiMetaAspect* guiAspect);
	bool GetPreference(CString& val, CString path);
	bool GetPreference(int& val, CString path, bool hex);
	bool GetColorPreference(unsigned long& color, CString path);
	int	 GetParentAspect()							{ return parentAspect; }
	CGuiMetaModel*	GetGuiMetaParent();
	CGMEView*		GetView()						{ return view; }
	
	static bool IsPrimary(CGuiMetaModel* guiMetaModel, CGuiMetaAspect* guiAspect, CComPtr<IMgaMetaRole>& metaRole);
	static void SetAspect(CGuiFcoList& list, int asp);
	static CGuiObject* FindObject(CComPtr<IMgaFCO>& fco, CGuiFcoList& fcoList);
	static CGuiConnection* FindConnection(CComPtr<IMgaFCO>& fco, CGuiConnectionList& conns);
	static void GrayOutFcos(CGuiFcoList &list, bool set = true);
	static void GrayOutFcos(CGuiConnectionList &list, bool set = true);
	static void GrayOutNonInternalConnections(CGuiConnectionList& list);
	static void ResetFlags(CGuiFcoList &list);

	virtual CString GetInfoText()					{ return name + " (" + roleDisplayedName + ")"; }
	virtual CGuiMetaAttributeList* GetMetaAttributes();
	virtual void RemoveFromRouter(CAutoRouter &router) = 0;
	virtual bool IsVisible(int aspect = -1) = 0;
	virtual void Draw(HDC pDC, Gdiplus::Graphics* gdip) = 0;
	virtual void GrayOut(bool set = true)			{ grayedOut = set; }

public:
	CComPtr<IMgaFCO> mgaFco;
	CComPtr<IMgaMetaRole> metaRole;
	CGuiMetaFco *guiMeta;
	CString kindName;
	CString roleName;
	CString kindDisplayedName;
	CString roleDisplayedName;
	CComPtr<IMgaMetaFCO> metaFco;

protected:
	int numParentAspects;
	int parentAspect;
	bool grayedOut;
	bool isType;
	CGMEView* view;
};

class CGuiObject : public CGuiFco
{
public:
	CGuiObject(CComPtr<IMgaFCO>& pt, CComPtr<IMgaMetaRole>& role, CGMEView* vw, int numAsp);					// regular objects
	virtual ~CGuiObject();
	void InitObject(CWnd* viewWnd);								// Need this because of virtual functions

	// This is a trick to speed up dynamic_cast
	virtual CGuiObject*				dynamic_cast_CGuiObject(void)				{ return this; }

public:
	void InitAspect(int asp, CComPtr<IMgaMetaPart>& metaPart, CString& decorStr, CWnd* viewWnd);
	void GetDecoratorStr(CString& str);
	const CRect& GetLocation(int aspect = -1)			{ aspect = (aspect < 0) ? parentAspect : aspect;  return guiAspects[aspect]->GetLocation(); }
	const CRect& GetNameLocation(int aspect = -1)		{ aspect = (aspect < 0) ? parentAspect : aspect;  return guiAspects[aspect]->GetNameLocation(); }
	CSize GetSize(int aspect = -1)						{ return GetLocation(aspect).Size(); }
	CPoint GetCenter()									{ CRect rect = GetLocation(); return rect.CenterPoint();}
	void SetObjectLocation(CRect& rect, int aspect = -1, bool doMga = true);
	void SetSize(CSize& s, int aspect = -1, bool doMga = true);
	void SetAllSizes(CSize& s, bool doMga = true);
	void SetLocation(CRect& r, int aspect = -1, bool doMga = true);
	CSize GetNativeSize(int aspect = -1);
	void SetAllSizesToNative();							// Used by non real objects
	int	MapAspect(int parent)							{ return guiAspects[parent] ? guiAspects[parent]->GetIndex() : -1; }
	void GetNeighbors(CGuiFcoList& list);
	void GetRelationsInOut(CGuiConnectionList& list, bool inOrOut);
	CGuiPortList& GetPorts()							{ VERIFY(GetCurrentAspect()); return GetCurrentAspect()->GetPortList(); }
	CGuiAspect* GetCurrentAspect()						{ return guiAspects[parentAspect]; }
	CGuiPort* FindPort(CPoint& pt, bool lookNearToo = false);
	CGuiPort* FindPort(CComPtr<IMgaFCO> mgaFco);
	CAutoRouterBox* GetRouterBox()						{ return GetCurrentAspect()->GetRouterBox(); }
	CAutoRouterBox* GetRouterNameBox()					{ return GetCurrentAspect()->GetRouterNameBox(); }
	void SetRouterBox(CAutoRouterBox* rbox)				{ GetCurrentAspect()->SetRouterBox(rbox) ; }
	void SetRouterNameBox(CAutoRouterBox* rnbox)		{ GetCurrentAspect()->SetRouterNameBox(rnbox) ; }
	void ReadAllLocations();
	void WriteLocation(int aspect = -1);
	bool IsHotspotEnabled() const						{ return isHotspotEnabled; }
	void DeleteCustomizationOfConnections(CGuiConnectionList& conns, long aspect);
	void DeleteCustomizationOfInOutConnections(long aspect);

public:
	void GrayOutNeighbors();
	bool IsInside(CPoint& pt, bool lookNearToo = false);
	bool IsLabelInside(CPoint& pt, bool lookNearToo = false);

	void IncrementAnimRefCnt();
	void DecrementAnimRefCnt();

	static void GetExtent(CGuiFcoList& objectList, CRect& rect);
	static void GetExtent(CGuiObjectList& objectList,CRect& rect);
	static CSize GetExtent(CGuiFcoList& list)			{CRect rect; GetExtent(list, rect); return rect.Size();}
	static void MoveObjects(CGuiFcoList& fcoList, CPoint& pt);
	static void MoveObjects(CGuiObjectList& objectList, CPoint& pt);
	static void FindUpperLeft(CGuiObjectList& objs, int& left, int& top);
	static void ShiftModels(CGuiObjectList& objList, CPoint& shiftBy);
	void ResizeObject(const CRect& newLocation);
	static bool NudgeObjects(CGuiObjectList& modelList, int right, int down);
	static void GetRectList(CGuiObjectList& objList, CRectList& rects);

	virtual bool IsVisible(int aspect = -1)				{ return guiAspects[(aspect < 0 ? parentAspect : aspect)] != NULL; }
	virtual bool IsResizable(void);
	virtual void RemoveFromRouter(CAutoRouter& router)	{ router.DeleteObject(this); }
	virtual void Draw(HDC pDC, Gdiplus::Graphics* gdip);
	virtual void GrayOut(bool set);
	virtual CGuiMetaAspect* GetKindAspect(CComPtr<IMgaMetaPart> metaPart);

protected:
	CArray<CGuiAspect*,CGuiAspect*>	guiAspects;
	bool isHotspotEnabled;
};


class CGuiCompound : public CGuiObject
{
public:
	CGuiCompound(CComPtr<IMgaFCO>& pt,CComPtr<IMgaMetaRole>& role, CGMEView* vw, int numAsp) : CGuiObject(pt, role, vw, numAsp) {}

	// This is a trick to speed up dynamic_cast
	virtual CGuiCompound*			dynamic_cast_CGuiCompound(void)				{ return this; }
};

class CGuiModel : public CGuiCompound
{
public:
	CGuiModel(CComPtr<IMgaFCO>& pt, CComPtr<IMgaMetaRole>& role, CGMEView* vw, int numAsp);

	// This is a trick to speed up dynamic_cast
	virtual CGuiModel*				dynamic_cast_CGuiModel(void)				{ return this; }

	virtual CGuiMetaAspect* GetKindAspect(CComPtr<IMgaMetaPart> metaPart);
	virtual CGuiMetaAttributeList* GetMetaAttributes();
};

class CReference
{
public:
	CReference(CComPtr<IMgaFCO> mgaRefd, CComPtr<IMgaFCO> mgaTermRefd);

public:
	bool IsNull()							{ return mgaReferee == NULL; }
	CComPtr<IMgaFCO>& GetTerminalReferee()	{ return mgaTerminalReferee; }
	CString GetInfoText(CString &name);

protected:
	CComPtr<IMgaFCO> mgaReferee;
	CComPtr<IMgaFCO> mgaTerminalReferee;
	CString targetName;
	CString targetKindDisplayedName;
};

// both null and atom ref
class CGuiReference : public CGuiObject, public CReference
{
public:
	CGuiReference(CComPtr<IMgaFCO>& pt, CComPtr<IMgaMetaRole>& role, CGMEView* vw, int numAsp, CComPtr<IMgaFCO> mgaRefd, CComPtr<IMgaFCO> mgaTermRefd) : CGuiObject(pt, role, vw, numAsp), CReference(mgaRefd, mgaTermRefd) {}

	// This is a trick to speed up dynamic_cast
	virtual CGuiReference*			dynamic_cast_CGuiReference(void)			{ return this; }

public:
	virtual CString GetInfoText() { return CReference::GetInfoText(name); }
};

class CGuiCompoundReference : public CGuiCompound, public CReference
{
public:
	CGuiCompoundReference(CComPtr<IMgaFCO>& pt, CComPtr<IMgaMetaRole>& role, CGMEView* vw, int numAsp, CComPtr<IMgaFCO> mgaRefd, CComPtr<IMgaFCO> mgaTermRefd) : CGuiCompound(pt, role, vw, numAsp), CReference(mgaRefd, mgaTermRefd) {}

	// This is a trick to speed up dynamic_cast
	virtual CGuiCompoundReference*	dynamic_cast_CGuiCompoundReference(void)	{ return this; }

public:
	virtual CString GetInfoText() { return CReference::GetInfoText(name); }
	virtual CGuiMetaAttributeList* GetMetaAttributes();
	virtual CGuiMetaAspect* GetKindAspect(CComPtr<IMgaMetaPart> metaPart);
};


class CGuiSet : public CGuiObject
{
public:
	CGuiSet(CComPtr<IMgaFCO>& pt, CComPtr<IMgaMetaRole>& role, CGMEView* vw, int numAsp) : CGuiObject(pt, role, vw, numAsp) {}

	// This is a trick to speed up dynamic_cast
	virtual CGuiSet*				dynamic_cast_CGuiSet(void)					{ return this; }

public:
	void Init(CGuiFcoList& fcos, CGuiConnectionList& conns);
	bool CheckMember(CGuiFco* fco);
	bool ToggleMember(CGuiFco* member);
	void GrayOutMembers(bool set = true)	{ GrayOutFcos(members, set); }

protected:
	CGuiFcoList members;
};

class CGuiConnectionLabel {
private:
	CString label;
	CPoint loc;
	int alignment;
	bool primary;
public:
	CGuiConnectionLabel();
	virtual ~CGuiConnectionLabel();
	void SetLabel(const CString& l);
	void SetPrimary(bool prim);
	void SetLocation(const CPoint& endPoint, const CPoint& nextPoint, const CRect& box);
	void Draw(Gdiplus::Graphics* gdip, COLORREF color, CGuiConnection* conn);
	CPoint GetLocation(void) const;
	int GetAlignment(void) const;
	CString GetLabel(void) const;
};

class CGuiConnectionLabelSet {
public:
	CGuiConnectionLabelSet();
	virtual ~CGuiConnectionLabelSet();
private:
	CGuiConnectionLabel labels[GME_CONN_LABEL_NUM];
public:
	void SetLabel(int index, const CString& label);
	void SetLocation(int index, const CPoint& endPoint, const CPoint& nextPoint, const CRect& box);
	void Draw(Gdiplus::Graphics* gdip, COLORREF color, CGuiConnection* conn);
	CPoint GetLocation(int index) const;
	int GetAlignment(int index) const;
	CString GetLabel(int index) const;
};

enum ConnectionPartMoveType {
	HorizontalEdgeMove			= 0,
	VerticalEdgeMove			= 1,
	AdjacentEdgeMove			= 2,
	InsertNewCustomPoint		= 3,
	ModifyExistingCustomPoint	= 4
};

class CGuiConnection : public CGuiFco
{
public:
	CGuiConnection(CComPtr<IMgaFCO>& pt, CComPtr<IMgaMetaRole>& role, CGMEView* vw, int numAsp, bool resolve = true);
	virtual ~CGuiConnection() { }

	// This is a trick to speed up dynamic_cast
	virtual CGuiConnection*			dynamic_cast_CGuiConnection(void)			{ return this; }

	void RefreshAttributeCache();
	CAutoRouterPath* GetRouterPath()						{ return routerPath; }
	void SetRouterPath(CAutoRouterPath* path) 				{ routerPath = path; }
	void GrayOutEndPoints();
	void ToggleHover()										{ hovered = !hovered; }
	void SetSelect(bool sel)								{ selected = sel; }
	bool IsSelected() { return selected; }
	void GiveConnectionEndErroMessage(const TCHAR* mainMsg, const TCHAR* srcOrDst, const CGuiPort* otherPort) const;
	void Resolve();
	void ReadARPreferences();
	bool GetARPref(int dir) const { return autorouterPrefs[dir]; }
	void GetPointList(CPointList& points) const;
	int GetEdgeCount(void) const;
	CRect GetBounds(void) const;
	CPoint GetCenter(void) const;
	bool AdjustCoordLimits(CPointList& points, int edgeIndex, bool isPathEnd, bool xOrY, POSITION pos,
						   int ptCoord, int lastlastCoord, long& coordMinLimit, long& coordMaxLimit) const;
	int GetEdgeIndex(const CPoint& point, CPoint& startPoint, CPoint& endPoint, CPoint& thirdPoint,
					 ConnectionPartMoveType& connectionMoveMethod, bool& horizontalOrVerticalEdge,
					 bool& isPartFixed, long& xMinLimit, long& xMaxLimit,
					 long& yMinLimit, long& yMaxLimit, bool checkPointOnEdge = true) const;
	int IsPathAt(const CPoint& point, ConnectionPartMoveType& connectionMoveMethod, bool& horizontalOrVerticalEdge,
				 bool& isPartFixed) const;
	long IsPointOnSectionAndDeletable(long edgeIndex, const CPoint& point);
	std::vector<long> GetRelevantCustomizedEdgeIndexes(void);
	void FillOutCustomPathData(CustomPathData& pathData, PathCustomizationType custType, long asp, int newPosX, int newPosY,
							   int edgeIndex, int edgeCount, bool horizontalOrVerticalEdge);
	std::vector<CustomPathData> GetCurrentPathCustomizations(void);
	bool HasPathCustomization(void) const;
	bool HasPathCustomizationForCurrentAspect(int edgeIndex = -1) const;
	bool HasPathCustomizationForAspect(long asp, int edgeIndex = -1) const;
	bool HasPathCustomizationForTypeAndCurrentAspect(PathCustomizationType custType = Invalid, int edgeIndex = -1) const;
	bool HasPathCustomizationForTypeAndAspect(long asp, PathCustomizationType custType = Invalid, int edgeIndex = -1) const;
	void ReadCustomPathData(void);
	void WriteCustomPathData(bool handleTransaction = true);
	void InsertCustomPathData(const CustomPathData& pathData);
	void UpdateCustomPathData(const CustomPathData& pathData);
	void DeletePathCustomization(const CustomPathData& pathData);
	bool DeleteAllPathCustomizationsForAnAspect(long asp);
	bool DeleteAllPathCustomizationsForCurrentAspect(void);
	void DeleteAllPathCustomizationsForAllAspects(void);
	void RemoveDeletedPathCustomizations(const std::vector<CustomPathData>& customPathDat);
	void SnapCoordIfApplicable(CustomPathData* coordToSet, const CPoint& last, const CPoint& pt);
	bool VerticalAndHorizontalSnappingOfConnectionLineSegments(long asp, int edgeIndex = -1);
	bool IsAutoRouted(void) const;
	void SetAutoRouted(bool autoRouteState);
	bool NeedsRouterPathConversion(bool expectedAutoRouterState = true);
	void ConvertAutoRoutedPathToCustom(long asp, bool handleTransaction = false, bool expectedAutoRouterState = true);
	bool ReadAutoRouteState(void);
	void WriteAutoRouteState(bool handleTransaction = true);
	CGuiConnectionLabelSet& GetLabelSet(void);

	virtual bool IsVisible(int aspect = -1)				{ return visible && visible[aspect < 0 ? parentAspect : aspect]; }
	virtual void RemoveFromRouter(CAutoRouter &router);
	virtual void Draw(HDC pDC, Gdiplus::Graphics* gdip);

protected:

public:
	CGuiObject* src;
	CGuiPort* srcPort;
	CGuiObject* dst;
	CGuiPort* dstPort;
	CMapStringToString	attributeCache;

private:
	CAutoRouterPath* routerPath;
	CGuiConnectionLabelSet labelset;
	std::unique_ptr<bool[]> visible;
	GMEConnLineType lineType;
	int srcStyle;
	int dstStyle;
	COLORREF color;
	COLORREF nameColor;
	bool autorouterPrefs[GME_AR_NUM];
	bool hovered;
	bool selected;
	bool connRegAutoRouteNotSet;
	bool isAutoRouted;
	std::vector<CustomPathData> customPathData;
};





#endif // whole file
