#include "BrushCache.h"

BrushCache::BrushCache()
{
}

BrushCache::BrushMap* BrushCache::Cache = NULL;

HBRUSH BrushCache::GetSolidBrush(COLORREF Color)
{
	BrushMap::const_iterator it;
	HBRUSH hBrush;

	if(!Cache)
		Cache = new BrushMap();

	it = Cache->find(Color);
	if(it == Cache->end())
	{
		hBrush = CreateSolidBrush(Color);
		Cache->insert(BrushMap::value_type(Color, hBrush));
	}
	else
		hBrush = it->second;

	return hBrush;
}

void BrushCache::Purge()
{
	BrushMap::const_iterator it;

	if(Cache)
	{
		for(it = Cache->begin(); it != Cache->end(); it++)
			DeleteObject(it->second);
		delete Cache;
		Cache = NULL;
	}
}
