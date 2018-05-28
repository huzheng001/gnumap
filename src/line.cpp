/*
** $Id: line.cpp,v 1.32 2007-07-03 10:42:47 jpzhang Exp $
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
#include "feature.h"
#include "layer.h"
#include "env.h"

#include "simplify.h"

MEGeomLine::MEGeomLine (OGRLineString * ogr_line, MEMapFeature* feature)
    :MEGeometry((OGRGeometry*)ogr_line, feature)
{
    MEPointInfo *point;
    OGRPoint op;
    num_points = ogr_line->getNumPoints();
    line_points.resize(num_points);

    for (int i =0; i< num_points; i++) {
        ogr_line->getPoint(i, &op);
        point = new MEPointInfo(&op, feature);
        line_points[i] = point;
    }

    MEDoublePoint * points;

    points = new MEDoublePoint[num_points];
    for (int i=0; i < num_points;i++)
        points[i] = line_points[i]->ortho_point; 

    num_points = Simplify2d (points, num_points, &d_points); //d_points will be alloc by malloc function.

    delete [] points;
    pPts = new MEPoint[num_points];

    s_points = new MEPoint[num_points];
}

MEGeomLine::MEGeomLine (char **data, MEMapFeature* feature)
    :MEGeometry(data, feature)
{
    num_points =ArchSwapLE32(read_int(*data));
    *data += 4;
    line_points.resize(num_points);

    MEPointInfo *point;
    for (int i=0; i < num_points; i++)
    {
        point = new MEPointInfo(data, feature);
        line_points[i] = point;
    }
    d_points = (MEDoublePoint *)malloc(num_points * sizeof(MEDoublePoint));
    for (int i=0; i < num_points; i++)
    {
        d_points[i].x = line_points[i]->ortho_point.x;
        d_points[i].y = line_points[i]->ortho_point.y;
    }
    pPts = new MEPoint[num_points];
    s_points = new MEPoint[num_points];
}

MEGeomLine::~MEGeomLine ()
{
    for (vector<MEPointInfo *>::iterator i = line_points.begin(); i != line_points.end(); i++)
        delete (*i);
    
    line_points.clear();

    delete [] pPts;
    delete [] s_points;
    free (d_points);
}

void MEGeomLine::UpdateScreenPoint()
{
    m_feature->GetParentLayer()->GetMap()->OrthoToWindowBatch(num_points, d_points, s_points);
}

void MEGeomLine::Draw (MEDC * dc, MEStyle * style)
{
    m_feature->GetParentLayer()->GetMap()->
            ScreenToWindowBatch(num_points, s_points, pPts);

    if (style != MEStyle::last_style) //shouldn 't set DC style
    {
        MEStyle * po = (MEStyle*)MEStyle::last_style;
        if (po)
            po->ResetDC(dc, m_feature);

        style->SetDC(dc, m_feature);
        MEStyle::last_style = style;
    }

    for (int i=1; i< num_points; i++)
        dc->drawLine(pPts[i-1].x, pPts[i-1].y, pPts[i].x, pPts[i].y);

}

void MEGeomLine::GetTextPosition (const char * text, int fontsize, int * x, int *y)
{
    if (!text || !x || !y)
        return;

    m_feature->GetParentLayer()->GetMap()->OrthoToWindow((ortho_ext->min_x + ortho_ext->max_x)/2, ortho_ext->max_y, x, y);
}

GEO_TYPE MEGeomLine::GetType(void)
{
    return GEO_LINE;
}
