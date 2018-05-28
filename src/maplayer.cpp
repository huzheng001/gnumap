#include "layer.h"
#include "feature.h"
#include "geometry.h"
#include "searcher.h"

MEMapLayer::MEMapLayer(MEMap* parent_map, OGRLayer* poLayer, int id)
    :MELayer(parent_map),layer_visible(true),m_id(id)
{
    MEMapFeature *mapfeature;
    OGRFeature *poFeature;

    layer_name = strdup(poLayer->GetLayerDefn()->GetName());
    
    poLayer->ResetReading();
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
        mapfeature = new MEMapFeature(this, poFeature);
        pool[mapfeature->GetFid()] = mapfeature;
        OGRFeature::DestroyFeature( poFeature );
    }
}

MEMapLayer::MEMapLayer(MEMap* parent_map, char *p, char *name, int id, std::vector<const char *> &Styles)
    :MELayer(parent_map),layer_visible(true),m_id(id)
{
    layer_name = strdup(name);
    char *fontname;
    fontname = strstr(p, "FontName=");
    fontname += sizeof("FontName=")-1;
    char *p1 = strchr(fontname, '\n');
    *p1 = '\0';
    char *charset;
    charset = strstr(p1+1, "CharSet=");
    charset += sizeof("CharSet=")-1;
    p1 = strchr(charset, '\n');
    *p1 = '\0';
    char *size;
    size = strstr(p1+1, "FontSize=");
    size += sizeof("FontSize=")-1;
    p1 = strchr(size, '\n');
    *p1 = '\0';
    char *angle;
    angle = strstr(p1+1, "RotatAngle=");
    angle += sizeof("RotatAngle=")-1;
    p1 = strchr(angle, '\n');
    *p1 = '\0';
    MEFont *font = new MEFont(fontname, atoi(size), 0, charset, atoi(angle));
    SetLayerFont(font);
    p = p1 +1;
    p = p + strlen(p) +1;
    unsigned int count  = ArchSwapLE32(read_uint(p));
    p+=4;
    MEMapFeature *mapfeature;
    for (unsigned int i = 0; i< count; i++)
    {
        mapfeature = new MEMapFeature(this, &p, Styles);
        pool[mapfeature->GetFid()] = mapfeature;
    }
}

MEMapLayer::~MEMapLayer()
{
    FeaturePool::iterator i;
    for (i = pool.begin(); i != pool.end(); i++)
        delete i->second;

    pool.clear();
    free (layer_name);
}

char * MEMapLayer::GetName(void) const
{
    return layer_name;
}

void MEMapLayer::Draw(MEDC* dc)
{

}

void MEMapLayer::UpdateScreenPoint()
{
    FeatureRewind();
    MEMapFeature* feature = GetNextFeature();
    MEGeometry* geom;
    while (feature != NULL)
    {
        geom = feature->GetGeometry();
        if (geom)
            geom->UpdateScreenPoint();
        feature = GetNextFeature();
    }
}

void MEMapLayer::Search(MESearcher* searcher)
{
    FeatureRewind();
    MEMapFeature* feature = GetNextFeature();
    while (feature != NULL)
    {
        searcher->SearchFeature(feature);
        feature = GetNextFeature();
    }
}

void MEMapLayer::FeatureRewind()
{
    pos = pool.begin();
}

MEMapFeature* MEMapLayer::GetNextFeature()
{
    if (pos == pool.end())
        return NULL;
    MEMapFeature *feature = pos->second;
    pos++;
    return feature;
}

MEMapFeature* MEMapLayer::GetFeatureByFid(long fid)
{
    FeaturePool::iterator p;
    
    p = pool.find (fid);
    if (p == pool.end())
        return NULL;

    return p->second;
}

int MEMapLayer::GetFeatureCount()
{
    return pool.size();
}
