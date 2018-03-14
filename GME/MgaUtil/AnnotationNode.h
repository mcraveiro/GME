// AnnotationNode.h: interface for the CAnnotationNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANNOTATIONNODE_H__A60720A0_33C3_4FFE_9995_8439D58246E8__INCLUDED_)
#define AFX_ANNOTATIONNODE_H__A60720A0_33C3_4FFE_9995_8439D58246E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Annotator\AnnotationDefs.h"
#include <deque>

class CAnnotationBrowserDlg;
class CAnnotationAspect;
class CAnnotationNode;

typedef CTypedPtrList<CPtrList, CAnnotationNode *> CAnnotationNodeList;

class CAnnotationAspect {
public:
	bool	m_isVisible;
	CPoint	m_loc;
	bool	m_isLocDef;

	CAnnotationAspect() : m_isVisible(false), m_loc(0,0), m_isLocDef(true) {}
};

typedef CArray<CAnnotationAspect,CAnnotationAspect&> CAnnotationAspectArray;

class CAnnotationNode  
{
public:
	CComPtr<IMgaRegNode> m_regNode;
	IMgaFCOPtr m_archetype;
	LOGFONT		m_logfont;
	COLORREF	m_color;
	COLORREF	m_bgcolor;
	COLORREF	m_crGradient;
	COLORREF	m_crShadow;
	CString		m_text;
	CString		m_name;
	bool		m_bGradientFill;
	int			m_iGradientDirection;
	bool		m_bCastShadow;
	int			m_iShadowDepth;
	int			m_iShadowDirection;
	bool		m_bRoundCornerRect;
	int			m_iRoundCornerRadius;

	bool		m_canBeRederived;	// the "broken_deriv" entry signals that
									// the annotation was once inherited,
									// so it could be rederived
	
	bool		m_inheritable;		// can be shown in subtypes/instances
	bool		m_hidden;			// hide inherited annotation in this subtype/instance only
	CAnnotationAspectArray	m_aspects;

	CAnnotationNode(const CComPtr<IMgaRegNode> &regNode, IMgaFCOPtr& archetype);
	void Read(CAnnotationBrowserDlg *dlg);
	void Write(CAnnotationBrowserDlg *dlg);

	template<typename Func>
	void for_each_subnode(Func f)
	{
		std::deque<IMgaRegNodePtr> regnodes;
		regnodes.push_back(IMgaRegNodePtr(m_regNode.p));
		while (regnodes.size())
		{
			IMgaRegNodePtr regnode = regnodes.front();
			regnodes.pop_front();
			f(regnode);
			IMgaRegNodesPtr subnodes = regnode->GetSubNodes(VARIANT_FALSE);
			for (int i = 1; i <= subnodes->Count; i++)
			{
				auto subnode = subnodes->GetItem(i);
				if (regnode == m_regNode && wcscmp(subnode->Name, AN_HIDDEN) == 0)
				{
					continue;
				}
				regnodes.push_back(subnodes->GetItem(i));
			}
		}
	}


public:
	static void InitializeClass();
	static bool classIsInitialized;
	static bool		defCanBeRederived;
	static bool		defInheritable;
	static bool		defHidden;
	static LOGFONT	defFont;
	static COLORREF	defColor;
	static COLORREF	defBgcolor;
	static COLORREF	defShadowcolor;
	static COLORREF	defGradientcolor;
	static bool		defGradientFill;
	static int		defGradientDirection;
	static bool		defCastShadow;
	static int		defShadowDepth;
	static int		defShadowDirection;
	static bool		defRoundCornerRect;
	static int		defRoundCornerRadius;
};

#endif // !defined(AFX_ANNOTATIONNODE_H__A60720A0_33C3_4FFE_9995_8439D58246E8__INCLUDED_)