#include "stdafx.h"

#include "GMEApp.h"
#include "GMEStd.h"
#include "GuiObject.h"
#include "ModelGrid.h"

//////////////////////////////////// CModelGrid //////////////////////////////////

CModelGrid::CModelGrid()
{
#ifdef GRIDBITS
	width = GME_MAX_GRID_DIM / (8 * sizeof(int));
#else
	width = GME_MAX_GRID_DIM;
#endif
	height = GME_MAX_GRID_DIM;
	Clear();
	view = 0;
}

#ifdef GRIDBITS

#define trafo()		ASSERT(sizeof(int) == 4);						\
					unsigned int xi = (x >> 5);						\
					ASSERT(xi < (unsigned int)width);  				\
					ASSERT(y < height);  							\
					unsigned int mask = 1 << ((unsigned int)x & 0x1F);

inline bool CModelGrid::IsAvailable(int x,int y)
{
	trafo();
	return ((grid[y][xi] & mask) == 0);
}

inline void CModelGrid::Set(int x,int y)
{
	trafo();
	grid[y][xi] |= mask;
}

inline void CModelGrid::Reset(int x,int y)
{
	trafo();
	mask = ~mask;
	grid[y][xi] &= mask;
}

#else

inline bool CModelGrid::IsAvailable(int x,int y)
{
	return !grid[y][x];
}

inline void CModelGrid::Set(int x,int y)
{
	grid[y][x] = true;
}

inline void CModelGrid::Reset(int x,int y)
{
	grid[y][x] = false;
}

#endif

void CModelGrid::Clear()
{
	int i;
	memset(grid,0,sizeof(grid));
	for(i = 0; i < width; i++) {	// frame!
#ifdef GRIDBITS
		grid[0][i] = (unsigned int)~0;
		grid[height - 1][i] = (unsigned int)~0;
#else
		Set(i,0);
		Set(i,GME_MAX_GRID_DIM-1);
#endif
	}
	for(i = 0; i < height; i++) {
		Set(0,i);
		Set(GME_MAX_GRID_DIM-1,i);
	}
	view = 0;
}

bool CModelGrid::IsAvailableG(CPoint &pt,CSize &size)
{
	int startx = pt.x;
	int starty = pt.y;
	int endx = pt.x + size.cx;
	int endy = pt.y + size.cy;
	// Hack to prevent labels from overlapping
	//starty -= 2;
	if(startx < 0 || starty < 0 || endx >= GME_MAX_GRID_DIM || endy >= GME_MAX_GRID_DIM)
		return false;
	for(int i = starty; i <= endy; i++)
		for(int j = startx; j <= endx; j++)
			if(!IsAvailable(j,i))
				return false;
	return true;
}


#define FindStartEnd(rect,chk)											\
	int startx = (rect.left / GME_GRID_SIZE); \
	int starty = (rect.top / GME_GRID_SIZE); \
	int endx = ((rect.right + GME_GRID_SIZE - 1) / GME_GRID_SIZE); \
	int endy = ((rect.bottom + GME_GRID_SIZE - 1) / GME_GRID_SIZE); \
	if(chk) {																\
		ASSERT(startx >= 0);												\
		ASSERT(starty >= 0);												\
		ASSERT(endx < GME_MAX_GRID_DIM);									\
		ASSERT(endy < GME_MAX_GRID_DIM);									\
	}


bool CModelGrid::IsAvailable(const CRect& rect)
{
	FindStartEnd(rect,0);
	if(startx < 0 || starty < 0 || (unsigned int)endx >= GME_MAX_GRID_DIM || (unsigned int)endy >= GME_MAX_GRID_DIM)
		return false;
	for(int y = starty; y <= endy; y++)
		for(int x = startx; x <= endx; x++)
			if(!IsAvailable(x,y))
				return false;
	return true;
}


#define	test_and_return_if_found()					\
			if(IsAvailableG(test,size)) {			\
				rect.MoveToXY(test.x * GME_GRID_SIZE, test.y * GME_GRID_SIZE);		\
				return TRUE;						\
			}

bool CModelGrid::GetClosestAvailable(CRect& rect)
{
	if(IsAvailable(rect)) {
		rect.MoveToXY((rect.left / GME_GRID_SIZE) * GME_GRID_SIZE,
			(rect.top / GME_GRID_SIZE) * GME_GRID_SIZE);
		return TRUE;
	}

	CPoint pt = rect.TopLeft();
	pt.x /= GME_GRID_SIZE;
	pt.y /= GME_GRID_SIZE;
	pt.x = max(0,min(GME_MAX_GRID_DIM - 1,pt.x));
	pt.y = max(0,min(GME_MAX_GRID_DIM - 1,pt.y));
	CSize size = rect.Size();
	size.cx = (size.cx + GME_GRID_SIZE - 1) / GME_GRID_SIZE;
	size.cy = (size.cy + GME_GRID_SIZE - 1) / GME_GRID_SIZE;
	CPoint test = pt;
	for(int rad = 1; rad < GME_MAX_GRID_DIM; rad++) {
		test.x = pt.x - rad;
		if(test.x >= 0)
			for(test.y = max(0,pt.y - rad); test.y < min(GME_MAX_GRID_DIM,pt.y + rad + 1); test.y++)
				test_and_return_if_found();
		test.x = pt.x + rad;
		if(test.x < GME_MAX_GRID_DIM)
			for(test.y = max(0,pt.y - rad); test.y < min(GME_MAX_GRID_DIM,pt.y + rad + 1); test.y++)
				test_and_return_if_found();
		test.y = pt.y - rad;
		if(test.y >= 0)
			for(test.x = max(0,pt.x - rad) + 1; test.x < min(GME_MAX_GRID_DIM,pt.x + rad + 1) - 1; test.x++)
				test_and_return_if_found();
		test.y = pt.y + rad;
		if(test.y < GME_MAX_GRID_DIM)
			for(test.x = max(0,pt.x - rad) + 1; test.x < min(GME_MAX_GRID_DIM,pt.x + rad + 1) - 1; test.x++)
				test_and_return_if_found();
	}
	return FALSE;
}


void CModelGrid::Set(CRect& rect, bool reset)
{
	FindStartEnd(rect,1);
	for(int y = starty; y <= endy; y++)
		for(int x = startx; x <= endx; x++)
			reset ? Reset(x,y) : Set(x,y);
}



///////////////////////////////////////////////////
// Public methods - accepts objects as parameters

/*
Mod by ZolMol:  If called from CGMEView::SyncOnGrid then
				the object (*model) might not have the same aspects as its parent so
				calling GetLocation or GetNameLocation with default parameter might
				cause null ptr dereferencing
*/
void CModelGrid::Set(CGuiObject *model,bool reset, int aspIdx)
{
	CRect loc = model->GetLocation( aspIdx);
	Set(loc,reset);

	if (theApp.labelAvoidance) {
		loc = model->GetNameLocation( aspIdx);
		Set(loc, reset);
	}
}

void CModelGrid::Reset(CGuiObject *model)
{
	Set(model,TRUE);
}

bool CModelGrid::IsAvailable(CGuiObject *model, int aspIdx) 
{
	CRect rect = model->GetLocation(aspIdx);
	bool avail = modelGrid.IsAvailable(rect);

	if (theApp.labelAvoidance) {
		rect = model->GetNameLocation(aspIdx);
		avail = avail && modelGrid.IsAvailable(rect);
	}

	return avail;
}

/*
Mod by ZolMol:  If called from CGMEView::SyncOnGrid then
				the object (*model) might not have the same aspects as its parent so
				calling GetLocation or GetNameLocation with default parameter might
				cause null ptr dereferencing
*/
bool CModelGrid::GetClosestAvailable(CGuiObject *model, CRect &rect, int aspIdx)
{
	bool ret = GetClosestAvailable(rect);

	return ret;
}

bool CModelGrid::CanNudge(CGuiObject *model,int right,int down)
{
	ASSERT(right == 0 || down == 0); // no diagonal nudge!
	CRect r = model->GetLocation(); // comment by ZolMol: this is correct since the parent aspect is always valid, and an object is moved (nudged) within the parents aspect only
	right *= GME_GRID_SIZE;
	down *= GME_GRID_SIZE;
	r.right += right;
	r.left += right;
	r.top += down;
	r.bottom += down;
	bool can = IsAvailable(r);

	if (theApp.labelAvoidance) {
		r = model->GetNameLocation();
		r.right += right;
		r.left += right;
		r.top += down;
		r.bottom += down;
		can = can && IsAvailable(r);
	}

	return can;
}
