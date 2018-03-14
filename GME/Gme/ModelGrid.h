#ifndef _MODELGRID_INCLUDED_
#define _MODELGRID_INCLUDED_

class CModelGrid {
public:
	CModelGrid();
private:
	CGMEView *view;
#ifdef GRIDBITS
	unsigned int grid[GME_MAX_GRID_DIM][GME_MAX_GRID_DIM / (8 * sizeof(int))];
#else
	bool grid[GME_MAX_GRID_DIM][GME_MAX_GRID_DIM];
#endif
	int width;
	int height;
public:
	CGMEView *GetSource()				{ return view; }
	void SetSource(CGMEView *vw)		{ view = vw; }

	void Clear();
	
	void Set(CGuiObject *model,bool reset = FALSE, int aspIdx = -1);//last param introd by zolmol
	void Reset(CGuiObject *model);

	bool IsAvailable(CGuiObject *model, int aspIdx = -1);
	bool GetClosestAvailable(CGuiObject *model, CRect &pt, int aspIdx = -1);//last param introd by zolmol
	
	bool CanNudge(CGuiObject *model,int right,int down);	

private:
	
	void Set(int x,int y);
	void Reset(int x,int y);
	bool IsAvailable(int x,int y);

	void Set(CRect& rect, bool reset = FALSE);
	bool IsAvailable(const CRect& rect);
	bool IsAvailableG(CPoint &pt,CSize &size);

	bool GetClosestAvailable(CRect& rect);
};

#endif // whole file