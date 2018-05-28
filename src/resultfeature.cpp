/*
** $Id: resultfeature.cpp,v 1.15 2006-11-14 05:00:08 jpzhang Exp $
**
** Result feature classes implement
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Hu Zheng.
**
** Create date: 2006/08/30
**
*/
#include <math.h>

#include "feature.h"
#include "layer.h"
#include "resultfeature.h"

static inline int get_int(double value)
{
    return (int)(value>=0?(value+0.5):(value-0.5));
}

//result feature class

MEResultFeature::MEResultFeature(MELayer * parent_layer)
    : MEFeature(parent_layer),m_ext(NULL), feature_id(-1), feature_name(NULL),m_filled(true), isll(true)
{
    m_color = new MEColor((unsigned long)0x000000);
    feature_name = strdup("");
}

MEResultFeature::MEResultFeature(MELayer * parent_layer, const char* color, long feature_id, const char *name)
    : MEFeature(parent_layer),m_ext(NULL), feature_id(feature_id), feature_name(NULL),m_filled(true)
{
    if (!color)
        m_color = new MEColor((unsigned long)0x000000);
    else
        m_color = new MEColor(color);

    feature_name = strdup(name);
}

MEResultFeature::~MEResultFeature()
{
    free(feature_name);
    delete m_color;
}

void MEResultFeature::SetColor (const char *color_pixel)
{
    delete m_color;
    m_color = new MEColor(color_pixel);
}

void MEResultFeature::Draw(MEDC* dc)
{
    int old_pen_width;
    old_pen_width = dc->getPenWidth();
    
    MEColor old_pen_color = dc->getPenColor();
    MEColor old_brush_color = dc->getBrushFgColor();

    dc->setPenColor(*m_color);
    dc->setBrushFgColor(*m_color);
    dc->setPenWidth(2);
    
    draw_feature( dc );
    
    dc->setPenColor(old_pen_color);
    dc->setBrushFgColor(old_brush_color);
    dc->setPenWidth(old_pen_width);
}

MEExtent* MEResultFeature::GetExtent(void) const
{
    return m_ext;
}

void MEResultFeature::draw_feature(MEDC* dc)
{

}

//point feature class

PointFeature::PointFeature(MELayer * parent_layer, double latlong_x, double latlong_y, int size,
                                            const char* color, long feature_id, const char *name)
            : MEResultFeature(parent_layer, color, feature_id, name),
                size(size)
{
    isll = true;
    ll.latlong_x = latlong_x;
    ll.latlong_y = latlong_y;
}

PointFeature::PointFeature(MELayer * parent_layer, int win_x, int win_y, int size,
                                            const char* color, long feature_id, const char *name)
            : MEResultFeature(parent_layer, color, feature_id, name),
                size(size)
{
    isll = false;
    win.x = win_x;
    win.y = win_y;
}

PointFeature::~PointFeature()
{
}

void PointFeature::draw_feature(MEDC* dc)
{
    if (isll)
    {
        int x, y;
        parent_layer->GetMap()->LatlongToWindow(ll.latlong_x, ll.latlong_y, &x, &y);
        draw_certain_feature(dc, x, y);
    }
    else
    {
        draw_certain_feature(dc, win.x, win.y);
    }
}

void PointFeature::draw_certain_feature(MEDC* dc, int win_x, int win_y)
{

}
    

//circle feature
CircleFeature::CircleFeature(MELayer * parent_layer, double latlong_x, double latlong_y, int size,
                     const char* color, long feature_id, const char *name)
    : PointFeature( parent_layer, latlong_x, latlong_y, size, color, feature_id, name) 
{

}

CircleFeature::CircleFeature(MELayer * parent_layer, int win_x, int win_y, int size,
                     const char* color, long feature_id, const char *name)
    : PointFeature( parent_layer, win_x, win_y, size, color, feature_id, name) 
{

}

CircleFeature::~CircleFeature()
{
}

void CircleFeature::draw_certain_feature(MEDC* dc, int win_x, int win_y)
{
    if(m_filled)
        dc->fillCircle(win_x, win_y, get_int(size/2));
    else
        dc->drawCircle(win_x, win_y, get_int(size/2));
}


// Triangle feature

TriangleFeature::TriangleFeature(MELayer * parent_layer, double latlong_x, double latlong_y, int size,
                                const char* color, long feature_id, const char *name )
                : PointFeature(parent_layer, latlong_x, latlong_y, size, color, feature_id, name) 
{
}
TriangleFeature::TriangleFeature(MELayer * parent_layer, int win_x, int win_y, int size,
                                const char* color, long feature_id, const char *name )
                : PointFeature(parent_layer, win_x, win_y, size, color, feature_id, name) 
{
}

TriangleFeature::~TriangleFeature()
{
}

void TriangleFeature::draw_certain_feature(MEDC* dc, int win_x, int win_y)
{
    points[0].x = win_x;
    points[0].y = win_y-size;

    double sqrt_3 = sqrt( 3.0 );
    points[1].x = win_x - get_int(size*sqrt_3/2);
    points[1].y = win_y + size/2;

    points[2].x = win_x + get_int(size*sqrt_3/2);
    points[2].y = win_y + size/2;

    points[3].x = win_x;
    points[3].y = win_y-size;

    if(m_filled)
        dc->fillPolygon(points, 4);
    else
        dc->drawPolygon(points, 4);
}

// square feature
SquareFeature::SquareFeature(MELayer * parent_layer, double latlong_x, double latlong_y, int size,
                            const char* color, long feature_id, const char *name )
                : PointFeature(parent_layer, latlong_x, latlong_y, size, color, feature_id, name) 
{
}

SquareFeature::SquareFeature(MELayer * parent_layer, int win_x, int win_y, int size,
                            const char* color, long feature_id, const char *name )
                : PointFeature(parent_layer, win_x, win_y, size, color, feature_id, name) 
{
}

SquareFeature::~SquareFeature()
{
}

void SquareFeature::draw_certain_feature(MEDC* dc, int win_x, int win_y)
{
    int tmp = get_int(size*sqrt(2.0)/2);

    points[0].x = win_x - tmp;
    points[0].y = win_y - tmp;

    points[1].x = win_x + tmp;
    points[1].y = win_y - tmp;

    points[2].x = win_x + tmp;
    points[2].y = win_y + tmp;

    points[3].x = win_x - tmp;
    points[3].y = win_y + tmp;

    points[4].x = win_x - tmp;
    points[4].y = win_y - tmp;

    if(m_filled)
        dc->fillPolygon(points, 5);
    else
        dc->drawPolygon(points, 5);
}

//bitmap feature 
BitmapFeature::BitmapFeature(MELayer * parent_layer, double latlong_x, double latlong_y, int size,
                int width, int height, long feature_id, const PBITMAP pBitmap, const char * name)
                : PointFeature(parent_layer, latlong_x, latlong_y, size, NULL, feature_id, name),
                m_pBitmap(pBitmap),m_width(width),m_height(height)  
{

}

BitmapFeature::BitmapFeature(MELayer * parent_layer, int win_x, int win_y, int size, int width, int height, 
                 long feature_id, const PBITMAP pBitmap, const char * name)
                : PointFeature(parent_layer, win_x, win_y, size, NULL, feature_id, name),
                m_pBitmap(pBitmap),m_width(width),m_height(height) 
{

}

BitmapFeature::~BitmapFeature()
{
}

void BitmapFeature::draw_certain_feature(MEDC* dc, int win_x, int win_y)
{
    dc->drawBitmap(m_pBitmap, win_x-(m_width/2), win_y-(m_height/2), m_width, m_height);
}

//line feature
LinesFeature::LinesFeature(MELayer * parent_layer, const char* color, long feature_id, const char *name, int pen_width)
            : MEResultFeature( parent_layer, color, feature_id, name ), pen_width(pen_width)
{
}

LinesFeature::~LinesFeature()
{
    for (vector<MEDoublePoint*>::iterator it = points.begin(); it != points.end(); it++)
	    delete *it;

    points.clear();
}

void LinesFeature::AddPoint(double x, double y)
{
    MEDoublePoint *point = new MEDoublePoint;
    point->x = x;
    point->y = y;
    points.push_back( point );
}

void LinesFeature::draw_feature(MEDC* dc)
{
    int old_pen_width;
    int i;

    if(points.size() < 2)
        return;

    MEPoint* pts = new MEPoint[points.size()];
    i = 0;

    for (vector<MEDoublePoint*>::iterator it = points.begin(); it != points.end(); it++)
    {
        parent_layer->GetMap()->LatlongToWindow((*it)->x, (*it)->y, &(pts[i].x), &(pts[i].y));
        i++;
    }

    old_pen_width = dc->getPenWidth();
    dc->setPenWidth( pen_width );

    draw_certain_feature(dc, pts , points.size());

    dc->setPenWidth( old_pen_width );
    delete [] pts;
}

void LinesFeature::draw_certain_feature(MEDC* dc, const MEPoint *pts, int npts)
{
    dc->drawPolygon(pts, npts);
}


//-----------------------------------------------------------------------------
PolygonFeature::PolygonFeature(MELayer * parent_layer, const char* color, long feature_id, const char* name, int pen_width)
            : LinesFeature(parent_layer, color, feature_id, name, pen_width)
{

}

PolygonFeature::~PolygonFeature()
{
}

void PolygonFeature::draw_certain_feature(MEDC* dc, const MEPoint* pts, int npts)
{
    if(m_filled)
        dc->fillPolygon(pts, npts);
    else
        dc->drawPolygon(pts, npts);
}



//big circle feature
BigCircleFeature::BigCircleFeature(MELayer * parent_layer, double latlong_x, double latlong_y, double size, const char* color,
                                long feature_id, const char *name, int pen_width)
                : MEResultFeature( parent_layer, color, feature_id, name ),
                latlong_x(latlong_x), latlong_y(latlong_y), size(size), pen_width(pen_width)
{

}

BigCircleFeature::~BigCircleFeature()
{
}

void BigCircleFeature::draw_feature(MEDC* dc)
{
    int win_x, win_y;
    parent_layer->GetMap()->LatlongToWindow(latlong_x, latlong_y, &win_x, &win_y);
    int win_size = (int)(size / parent_layer->GetMap()->GetMapScale());
    int old_pen_width = dc->getPenWidth();
    dc->setPenWidth(pen_width);
    dc->drawCircle(win_x, win_y, get_int(win_size/2));
    dc->setPenWidth(old_pen_width);
}
