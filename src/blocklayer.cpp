#include "feature.h"
#include "geometry.h"
#include "block.h"
#include "filter.h"

MEMapBlockLayer::MEMapBlockLayer(MEFeatureBlock *block)
    : m_block(block)
{
}

MEMapBlockLayer::~MEMapBlockLayer()
{
    features.clear();
}

void MEMapBlockLayer::Draw(MEDC * dc, int draw_age)
{
    MEFilterEx *filters = m_block->GetBlockSet()->GetFilters();
    for (std::list<MEMapFeature*>::iterator i = features.begin(); i!= features.end(); i++)
    {
        if ((*i)->draw_age == draw_age)
            continue;
        (*i)->draw_age = draw_age;
        if (!filters->FilterFeature(*i))
            continue;
        (*i)->Draw(dc);
    }
}
