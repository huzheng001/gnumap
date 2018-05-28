#include "block.h"
#include "layer.h"
#include "geometry.h"
#include "feature.h"
#include "filter.h"

#include <math.h>
#include "map.h"
#include "style.h"

MEFeatureBlockSet::MEFeatureBlockSet(double min_x, double max_x, double min_y, double max_y, int feature_count, MEMap *map)
{
    memap = map;
    filters = new MEFilterEx;
    int nw;
    int nh;
    get_block_wh(max_x - min_x, max_y - min_y, feature_count, nw, nh);

    double width = (max_x - min_x)/nw;
    double height = (max_y - min_y)/nh;
    double x1, y1, x2, y2;

    for (int i=0; i < nw; i++)
    {
        for (int j=0; j< nh; j++)
        {
            x1 = min_x + i*width;
            x2 = x1 + width;
            y1 = min_y + j*height;
            y2 = y1 + height;
            blocks.push_back(new MEFeatureBlock(x1, y1, x2, y2, this, map));
        }
    }
}

MEFeatureBlockSet::~MEFeatureBlockSet()
{
    delete filters;
    for (std::vector<MEFeatureBlock*>::iterator i = blocks.begin(); i != blocks.end(); i++)
        delete (*i);

    blocks.clear();
}

MEMap *MEFeatureBlockSet::GetMap()
{
    return memap;
}

MEFilterEx *MEFeatureBlockSet::GetFilters()
{
        return filters;
}

void MEFeatureBlockSet::get_block_wh(double width, double height, int feature_count, int &nw, int &nh)
{
    int count = feature_count/10;
    double a = height/width;
    nw = (int)(sqrt(count/a));
    nh = (int)(nw*a);
    if (nw == 0)
        nw =1;
    if (nh == 0)
        nh =1;
}

int MEFeatureBlockSet::GetBlockCount()
{
    return blocks.size();
}

std::vector<int>* MEFeatureBlockSet::get_blocks(void)
{
    double x0, y0, x1, y1;
    memap->WindowToOrtho(0, 0, &x0, &y0);
    memap->WindowToOrtho(memap->GetWidth(), memap->GetHeight(), &x1, &y1);
    double width = x1 - x0;
    double height = y1 - y0;
    double radius = sqrt(width*width + height*height)/2;
    double center_x = (x0 + x1)/2;
    double center_y = (y0 + y1)/2;
    x0 = center_x - radius;
    y0 = center_y - radius;
    x1 = center_x + radius;
    y1 = center_y + radius;
    
    MEExtentFilter *extent_filter = new MEExtentFilter(x0, y0, x1, y1);
    filters->ResetFilter();
    filters->AddFilter(extent_filter);
    
    MEExtent extent(x0, y0, x1, y1);
    std::vector<int> *iblocks = new std::vector<int>;
    MEExtent *bextent;
    for ( std::vector<MEFeatureBlock*>::size_type i=0; i< blocks.size(); i++)
    {
        bextent = blocks[i]->GetExtent();
        if (bextent->intersect(extent))
        {
            iblocks->push_back(i);
        }
    }
    return iblocks;
}

void MEFeatureBlockSet::AddLayer(MEMapLayer *layer)
{
    int id = layer->GetLayerId();
    MEMapBlockLayer* blayer;
    for (std::vector<MEFeatureBlock*>::iterator i = blocks.begin(); i != blocks.end(); i++)
    {
        blayer = new MEMapBlockLayer(*i);
        (*i)->layers[id] = blayer;
    }
    layer->FeatureRewind();
    MEMapFeature *feature;
    MEGeometry * geom;
    MEExtent *extent;
    while ((feature = layer->GetNextFeature()))
    {
        geom = feature->GetGeometry();
        if (geom)
        {
            extent = geom->GetOrthoExtent();
            for (std::vector<MEFeatureBlock*>::iterator i = blocks.begin(); i != blocks.end(); i++)
            {
                if (extent->intersect(*((*i)->GetExtent())))
                    (*i)->layers[id]->features.push_back(feature);
            }
        }
    }
}
void MEFeatureBlockSet::Draw(MEDC * dc, int draw_age)
{
    std::vector<int> *iblocks = get_blocks();
    for (int i =0; i< memap->GetLayerCount(); i++)
    {
        for (std::vector<int>::size_type j=0; j< iblocks->size(); j++)
        {
            blocks[(*iblocks)[j]]->DrawLayer(i, dc, draw_age);
        }
    }
    delete iblocks;

    MEStyle * ps = (MEStyle *)MEStyle::last_style;
    if (ps)
        ps->ResetDC(dc, NULL); // restore the default dc

    MEStyle::last_style = NULL;
}
