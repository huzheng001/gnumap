/*
** $Id: polygon.cpp,v 1.31 2007-07-03 10:42:47 jpzhang Exp $
**
** Classes for Geometry
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Hu Zheng.
**
** Create date: 2006/08/09
*/

#include "geometry.h"
#include "style.h"
#include "dc.h"
#include "feature.h"
#include "layer.h"
#include "env.h"

#include "simplify.h"


MEGeomPolygon::MEGeomPolygon (OGRPolygon * ogr_polygon, MEMapFeature* feature)
    :MEGeometry((OGRGeometry*)ogr_polygon, feature)
{
    fetch_points_ring(ogr_polygon->getExteriorRing());
    int size = ogr_polygon->getNumInteriorRings();
    for(int i=0; i<size; i++)
        fetch_points_ring(ogr_polygon->getInteriorRing(i));

    MEDoublePoint * points;

    num_points = polygon_points.size();

    points = new MEDoublePoint[num_points];
    for (int i=0; i < num_points;i++)
        points[i] = polygon_points[i]->ortho_point; 


    num_points = Simplify2d (points, num_points, &d_points); //d_points will be alloc by malloc function. 
    
    delete [] points;

    pPts = new MEPoint[num_points];

    s_points = new MEPoint[num_points];
}

MEGeomPolygon::MEGeomPolygon (char **data, MEMapFeature* feature)
    :MEGeometry(data, feature)
{
    num_points = ArchSwapLE32(read_int(*data));
    *data += 4;
    polygon_points.resize(num_points);
    MEPointInfo *point;
    for (int i = 0; i < num_points; i++)
    {
        point = new MEPointInfo(data, feature);
        polygon_points[i] = point;
    }
    d_points = (MEDoublePoint *)malloc(num_points * sizeof(MEDoublePoint));
    for (int i = 0; i < num_points; i++)
    {
        d_points[i].x = polygon_points[i]->ortho_point.x;
        d_points[i].y = polygon_points[i]->ortho_point.y;
    }
    pPts = new MEPoint[num_points];
    s_points = new MEPoint[num_points];
}

MEGeomPolygon::~MEGeomPolygon()
{
    for (vector<MEPointInfo *>::iterator i = polygon_points.begin(); i!= polygon_points.end(); i++)
        delete (*i);

    polygon_points.clear();
    delete [] pPts;
    delete [] s_points;
    free (d_points);
}

void MEGeomPolygon::fetch_points_ring(OGRLinearRing* ring)
{
    if(ring == NULL)
        return;
    OGRPoint point;
    MEPointInfo *gpoint;
    int ring_point_count = ring->getNumPoints();
    int csize = polygon_points.size();
    polygon_points.resize(csize + ring_point_count);
    for(int i=0; i<ring_point_count; i++)
    {
        ring->getPoint(i, &point);
        gpoint = new MEPointInfo(&point, m_feature);
        polygon_points[csize+i]= gpoint;
    }
}

void MEGeomPolygon::UpdateScreenPoint()
{
    m_feature->GetParentLayer()->GetMap()->OrthoToWindowBatch(num_points, d_points, s_points);
}

void MEGeomPolygon::Draw (MEDC * dc, MEStyle * style)
{
    m_feature->GetParentLayer()->GetMap()->ScreenToWindowBatch(num_points, s_points, pPts);


    if (style != MEStyle::last_style) //shouldn 't set DC style
    {
        MEStyle * po = (MEStyle*)MEStyle::last_style;
        if (po)
            po->ResetDC(dc, m_feature);

        style->SetDC(dc, m_feature);
        MEStyle::last_style = style;
    }
    
    dc->fillPolygon(pPts, num_points);
    dc->drawPolygon(pPts, num_points);

}

void MEGeomPolygon::GetTextPosition (const char * text, int fontsize, int * x, int *y)
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

GEO_TYPE MEGeomPolygon::GetType(void)
{
    return GEO_POLYGON;
}
