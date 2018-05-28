/*
** $Id: key_searcher.cpp,v 1.8 2006-09-20 06:59:21 jpzhang Exp $
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
#include "layer.h"
#include "geometry.h"
#include "searcher.h"

KeySearcher::KeySearcher(const char * key_string)
            : key_string( key_string )
{
}

KeySearcher::~KeySearcher()
{
    std::vector<KeySearchResult*>::iterator it;
    for( it = results.begin(); it != results.end(); it++)
        delete *it;
    results.clear();
}

void KeySearcher::SearchFeature( MEMapFeature* feature )
{
    if( key_string.empty() )
        return;

    if (!feature->GetName())
        return;

    if (strstr(feature->GetName(), key_string.c_str())!=NULL)
    {
        KeySearchResult* result = new KeySearchResult;
        result->feature_id = feature->GetFid();
        result->layer_name = feature->GetParentLayer()->GetName();
        result->feature_name = feature->GetName();
        MEExtent *e = feature->GetGeometry()->GetExtent();
        result->latitude = e->min_x;
        result->longitude = e->min_y;
        results.push_back( result );
    }
}

const KeySearchResult* KeySearcher::GetResult(int index) const
{
    return results[index];
}
int KeySearcher::GetResultSize() const
{
    return results.size();
}
