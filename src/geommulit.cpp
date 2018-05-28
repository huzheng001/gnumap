#include "geometry.h"
#include "feature.h"
#include "layer.h"
#include "env.h"

MEGeomMulitiple::MEGeomMulitiple (OGRGeometryCollection * ogr_multi, MEMapFeature* feature)
    :MEGeometry((OGRGeometry*)ogr_multi, feature)
{
    int count = ogr_multi->getNumGeometries();
    MEGeometry * geom;
    for(int i=0; i<count; i++)
    {
        geom = MEGeometry::GetGeometry(ogr_multi->getGeometryRef(i), m_feature);
        if (geom)
            geom_set.push_back(geom);
    }
}

MEGeomMulitiple::MEGeomMulitiple (char **data, MEMapFeature* feature)
    :MEGeometry(data, feature)
{
    unsigned int count = ArchSwapLE32(read_int(*data));
    *data += 4;
    MEGeometry * geom;
    for(unsigned int i=0; i<count; i++)
    {
        geom = MEGeometry::GetGeometry(data, m_feature);
        geom_set.push_back(geom);
    }
}

MEGeomMulitiple::~MEGeomMulitiple()
{
    for (vector<MEGeometry *>::iterator i = geom_set.begin(); i != geom_set.end(); i++)
        delete (*i);
    
    geom_set.clear();
}

void MEGeomMulitiple::UpdateScreenPoint()
{
    for (vector<MEGeometry *>::iterator i = geom_set.begin(); i != geom_set.end(); i++)
        (*i)->UpdateScreenPoint();
}

void MEGeomMulitiple::Draw (MEDC * dc, MEStyle * style)
{
    for (vector<MEGeometry *>::iterator i = geom_set.begin(); i != geom_set.end(); i++)
        (*i)->Draw(dc, style);
}

void MEGeomMulitiple::GetTextPosition (const char * text, int fontsize, int * x, int *y)
{
    if (!text || !x || !y)
        return;

    int offset_x;
    int offset_y;
    m_feature->GetParentLayer()->GetMap()->OrthoToWindow
        ((ortho_ext->min_x + ortho_ext->max_x)/2, (ortho_ext->min_y + ortho_ext->max_y)/2, x, y);
    offset_x = (int)(fontsize * strlen(text)/4);
    offset_y = (int)fontsize/2;
    *x -= offset_x;
    *y -= offset_y;
}

GEO_TYPE MEGeomMulitiple::GetType(void)
{
    return GEO_MULIT;
}
