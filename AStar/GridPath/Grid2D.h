#pragma once

template <class Type=int> struct Grid2D
{
	int GridSX, GridSY;
	int nGridBytes, nGrid;
	Type* vGrid;

	Grid2D(int GridSX, int GridSY, bool Zero = false);
	~Grid2D();

	void SetPoint(int x, int y, Type v);
	void SetHorizontalLine(int x, int y, int x2, Type v);
	void SetVerticalLine(int x, int y, int y2, Type v);
	void SetRectangle(int x, int y, int x2, int y2, Type v);
	void Fill(Type v);
	void FillRectangle(int x, int y, int x2, int y2, Type v);
	void SwapType(Type Source, Type Target);
	Type GetVal(int x, int y) const;
	Type* GetPtr(int x, int y) const;
};

#include <stdlib.h> //malloc, free
#include <memory.h> //memset

#define TGrid2DN template<class Type> Grid2D<Type>
#define TGrid2D(rtype)template<class Type> rtype Grid2D<Type>
#define Grid(x,y)(vGrid + (x) + (y) * GridSX)
#define OutBoundX(x)((x) < 0 || (x) >= GridSX)
#define OutBoundY(y)((y) < 0 || (y) >= GridSY)
#define OutBound(x,y)(OutBoundX(x) || OutBoundY(y))

TGrid2DN::Grid2D(int GridSX, int GridSY, bool Zero = false) : GridSX(GridSX), GridSY(GridSY)
{
	nGrid = GridSX * GridSY;
	nGridBytes = nGrid * sizeof(Type);
	vGrid = (Type*)malloc(nGridBytes);
	if(Zero)
		memset(vGrid, 0, nGridBytes);
}

TGrid2DN::~Grid2D()
{
	if(vGrid)
	{
		free(vGrid);
		vGrid = NULL;
	}
}

TGrid2D(void)::SetPoint(int x, int y, Type v)
{
	if(OutBound(x,y))
		return;

	*Grid(x, y) = v;
}

TGrid2D(void)::SetHorizontalLine(int x, int y, int x2, Type v)
{
	int *p;

	if(OutBound(x,y) || OutBoundX(x2))
		return;

	p = Grid(x < x2 ? x : x2, y);
	x2 = (x < x2 ? x2 - x : x - x2) + 1;
	for(x = 0; x < x2; x++, p++)
		*p = v;
}

TGrid2D(void)::SetVerticalLine(int x, int y, int y2, Type v)
{
	int *p;

	if(OutBound(x,y) || OutBoundY(y2))
		return;

	p = Grid(x, y < y2 ? y : y2);
	y2 = (y < y2 ? y2 - y : y - y2) + 1;
	for(y = 0; y < y2; y++, p+=GridSX)
		*p = v;
}

TGrid2D(void)::SetRectangle(int x, int y, int x2, int y2, Type v)
{
	if(OutBound(x,y) || OutBound(x2,y2))
		return;

	SetHorizontalLine(x, y, x2, v);
	SetHorizontalLine(x, y2, x2, v);
	SetVerticalLine(x, y, y2, v);
	SetVerticalLine(x2, y, y2, v);
}

TGrid2D(void)::Fill(Type v)
{
	int *p = vGrid, *e = p + nGrid;
	for(; p != e; p++)
		*p = v;
}

TGrid2D(void)::FillRectangle(int x, int y, int x2, int y2, Type v)
{
	int i;

	if(OutBound(x,y) || OutBound(x2,y2))
		return;

	y2 = (y < y2 ? y2 - y : y - y2) + 1;
	for(i = y; i < y2; i++)
		SetHorizontalLine(x, i, x2, v);
}

TGrid2D(void)::SwapType(Type Source, Type Target)
{
	int *p = vGrid, *e = p + nGrid;
	for(; p != e; p++)
		if(*p == Source)
			*p = Target;
}

TGrid2D(Type)::GetVal(int x, int y) const
{
	if(OutBound(x,y))
		return NULL;
	else
		return *Grid(x, y);
}

TGrid2D(Type*)::GetPtr(int x, int y) const
{
	if(OutBound(x,y))
		return NULL;
	else
		return Grid(x, y);
}
