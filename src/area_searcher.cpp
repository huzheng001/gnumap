/*
** $Id: area_searcher.cpp,v 1.5 2006-10-11 09:20:13 yangyan Exp $
**
** Classes implement for Search 
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Hu Zheng.
**
** Create date: 2006/08/11
*/

#include "feature.h"
#include "searcher.h"
#include "geometry.h"
#include "layer.h"

AreaSearcher::AreaSearcher(double latitude, double longitude, int distance)
            : latitude( latitude ), longitude( longitude ), distance( distance )
{
}

AreaSearcher::~AreaSearcher()
{
}

void AreaSearcher::SearchFeature(MEMapFeature* feature)
{
    if(feature->GetName()==NULL ||feature->GetName()[0] == '\0' || distance == 0)
        return;
    double x, y;
    double ortho_x, ortho_y;
    MEExtent *e = feature->GetGeometry()->GetExtent();
    feature->GetParentLayer()->GetMap()->LatlongToOrtho(e->min_x, e->min_y, &x, &y);
    feature->GetParentLayer()->GetMap()->LatlongToOrtho(latitude, longitude, &ortho_x, &ortho_y);
    double ret = (x - ortho_x)*(x - ortho_x) + (y -ortho_y)*(y -ortho_y);
    if( distance > (int)sqrt(ret))
    {
        KeySearchResult* result = new KeySearchResult;
        result->feature_id = feature->GetFid();
        result->layer_name = feature->GetParentLayer()->GetName();
        result->feature_name = feature->GetName();
        result->latitude = e->min_x;
        result->longitude = e->min_y;
        results.push_back( result );
    }
}
