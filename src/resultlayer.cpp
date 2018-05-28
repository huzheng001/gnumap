#include "layer.h"
#include "feature.h"
#include "resultfeature.h"

MEResultLayer::MEResultLayer(MEMap* parent_map)
    :MELayer(parent_map), last_frid(0)
{

}

MEResultLayer::~MEResultLayer()
{
    ClearFeature();
}

char * MEResultLayer::GetName(void) const
{
    return "meagle_result_layer";
}

void MEResultLayer::Search(MESearcher* searcher)
{
}

int MEResultLayer::AddFeature(MEResultFeature* feature)
{
    last_frid++;
    feature_map[last_frid] = feature;
    return last_frid;
}

void MEResultLayer::RemoveFeature(int frid)
{
    std::map<int, MEResultFeature*>::iterator i;
    i = feature_map.find(frid);
    if (i!= feature_map.end())
    {
        delete i->second;
        feature_map.erase(i);
    }
}

void MEResultLayer::ClearFeature(void)
{
    for (std::map<int, MEResultFeature*>::iterator i= feature_map.begin(); i!= feature_map.end(); i++)
        delete i->second;

    feature_map.clear();
}

void MEResultLayer::Draw(MEDC* dc)
{
    double latx, laty;
    for (std::map<int, MEResultFeature*>::iterator i= feature_map.begin(); i!= feature_map.end(); i++)
    {
        if (*(i->second->GetName())) {
            if (i->second->GetIsll())
            {
                MEColor bg("#000000");
                MEColor fg("#FFFFFF");
                latx = i->second->GetX(); 
                laty = i->second->GetY(); 
                map->DrawTextLat(latx, laty, i->second->GetName(), &fg, &bg);
            }
            else
            {
            }
        }

        i->second->Draw( dc );
    }
}
