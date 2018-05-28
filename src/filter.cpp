/*
** $Id: filter.cpp,v 1.6 2006-09-07 08:39:41 jpzhang Exp $
**
** Filter class implement
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Hu Zheng.
**
** Create date: 2006/08/24
**
*/

#include "feature.h"
#include "layer.h"
#include "filter.h"
#include "geometry.h"


MEFilterEx::MEFilterEx()
{
}

MEFilterEx::~MEFilterEx()
{
    ResetFilter();
}

bool MEFilterEx::FilterLayer(const MELayer* layer) const
{
    bool ret = true;
    vector<MEFilter*>::const_iterator i = filters.begin();
    while(ret && i != filters.end())
    {
        ret = (*i)->FilterLayer(layer);
        i++;
    }
    return ret;
}

bool MEFilterEx::FilterFeature(const MEMapFeature* feature) const
{
    bool ret = true;
    vector<MEFilter*>::const_iterator i = filters.begin();
    while(ret && i != filters.end())
    {
        ret = (*i)->FilterFeature(feature);
        i++;
    }
    return ret;
}

void MEFilterEx::AddFilter(MEFilter* filter)
{
    filters.push_back(filter);
}

void MEFilterEx::ResetFilter(void)
{
    std::vector<MEFilter*>::iterator i = filters.begin();
    for (; i != filters.end(); i++)
        delete *i;
    filters.clear();
}


//extent filter

MEExtentFilter::MEExtentFilter(double x1, double y1, double x2, double y2)
    : extent(x1, y1, x2, y2)
{
}

MEExtentFilter::~MEExtentFilter()
{
}

bool MEExtentFilter::FilterLayer(const MELayer* layer) const
{
    return true;
}

bool MEExtentFilter::FilterFeature(const MEMapFeature* feature) const
{

    MEGeometry * geom = feature->GetGeometry();

    if (geom) {
        MEExtent *e =geom->GetOrthoExtent();

        if (geom->GetType() != GEO_POINT) {
            double pl = feature->GetParentLayer()->GetMap()->GetMapScale();
            if ((e->max_x-e->min_x) < pl && (e->max_y-e->min_y) < pl)
                return false;
        }

        return e->intersect(extent);
    }
    return false;
}

void MEExtentFilter::SetExtent(double x1, double y1, double x2, double y2)
{
    extent.set(x1, y1, x2, y2);
}
