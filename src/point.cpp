/*
** $Id: point.cpp,v 1.22 2007-07-03 10:42:47 jpzhang Exp $
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
#include "feature.h"
#include "layer.h"
#include "style.h"


MEGeomPoint::MEGeomPoint(OGRPoint * ogr_point, MEMapFeature* feature)
    :MEGeometry((OGRGeometry*)ogr_point, feature)
{
    ll_point.x = ogr_point->getX();
    ll_point.y = ogr_point->getY();
    m_feature->GetParentLayer()->GetMap()->LatlongToOrtho(ll_point.x, ll_point.y, &ortho_point.x, &ortho_point.y);
}

MEGeomPoint::MEGeomPoint(char **data, MEMapFeature* feature)
    :MEGeometry(data, feature)
{
    ll_point.x = ArchSwapLE64(read_double(*data));
    *data+=8;
    ll_point.y = ArchSwapLE64(read_double(*data));
    *data+=8;
    ortho_point.x = ArchSwapLE64(read_double(*data));
    *data+=8;
    ortho_point.y = ArchSwapLE64(read_double(*data));
    *data+=8;
}

MEPointInfo::MEPointInfo(OGRPoint * ogr_point, MEMapFeature* feature)
{
    ll_point.x = ogr_point->getX();
    ll_point.y = ogr_point->getY();
    feature->GetParentLayer()->GetMap()->LatlongToOrtho(ll_point.x, ll_point.y, &ortho_point.x, &ortho_point.y);
}

MEPointInfo::MEPointInfo(char **data, MEMapFeature* feature)
{
    ll_point.x = ArchSwapLE64(read_double(*data));
    *data+=8;
    ll_point.y = ArchSwapLE64(read_double(*data));
    *data+=8;
    ortho_point.x = ArchSwapLE64(read_double(*data));
    *data+=8;
    ortho_point.y = ArchSwapLE64(read_double(*data));
    *data+=8;
}

MEGeomPoint::~MEGeomPoint()
{
}

void MEGeomPoint::UpdateScreenPoint()
{
    m_feature->GetParentLayer()->GetMap()->OrthoToWindow(ortho_point.x, ortho_point.y, &screen_point.x, &screen_point.y);
}

void MEGeomPoint::Draw (MEDC *dc, MEStyle * style)
{
    int win_x, win_y;
    m_feature->GetParentLayer()->GetMap()->ScreenToWindow(screen_point.x, screen_point.y, &win_x, &win_y);

    if (style != MEStyle::last_style) //shouldn 't set DC style
    {
        MEStyle * po = (MEStyle*)MEStyle::last_style;
        if (po)
            po->ResetDC(dc, m_feature);

        style->SetDC(dc, m_feature);
        MEStyle::last_style = style;
    }

    dc->drawPoint(win_x, win_y);
}

void MEGeomPoint::GetTextPosition (const char * text, int fontsize, int * x, int *y)
{
    if (!text || !x || !y)
        return;

    int offset;
    m_feature->GetParentLayer()->GetMap()->ScreenToWindow(screen_point.x, screen_point.y, x, y);
    offset = (int)fontsize/4;
    *x += offset;
    *y += offset;
}

GEO_TYPE MEGeomPoint::GetType(void)
{
    return GEO_POINT;
}
