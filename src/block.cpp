#include <string>

#include "block.h"
#include "map.h"

MEFeatureBlock::MEFeatureBlock(double x1, double y1, double x2, double y2, MEFeatureBlockSet *blockset, MEMap *map)
    : extent(x1, y1, x2, y2), block_set(blockset), memap(map)
{
}

MEFeatureBlock::~MEFeatureBlock()
{
    for (std::map<int,MEMapBlockLayer*>::iterator i = layers.begin(); i != layers.end(); i++)
        delete i->second;

    layers.clear();
}

void MEFeatureBlock::DrawLayer(int id, MEDC * dc, int draw_age)
{
    int lid = memap->GetLayerId(id);
    MEMapBlockLayer* blayer = layers[lid];
    if(memap->GetLayerVisible(id))
        (blayer)->Draw(dc, draw_age);
}

