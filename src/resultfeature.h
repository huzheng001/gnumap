/*
** $Id: resultfeature.h,v 1.16 2006-11-14 05:00:08 jpzhang Exp $
**
** Result feature classes define
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


#ifndef _ME_RESULT_FEATURE_H_
#define _ME_RESULT_FEATURE_H_

#include "dc.h"

//result feature abstract class
class MEResultFeature : public MEFeature
{
public:
    MEResultFeature(MELayer * parent_layer, const char* color, long feature_id, const char *name);
    virtual ~MEResultFeature();

    void Draw(MEDC* dc);
    void SetColor (const char *color_pixel);
    MEExtent* GetExtent(void) const;

    bool GetIsll() { return isll; }
    virtual void SetX(double x){}
    virtual void SetY(double y){}
    virtual double GetX(void) { return 0; }
    virtual double GetY(void) { return 0; }

    void SetFilled (bool filled){m_filled = filled;}
    bool IsFilled (void) const{ return m_filled;}
    long GetFid(void) const{ return feature_id; }
    char * GetName(void) const{ return feature_name; }
protected:
    MEResultFeature(MELayer * parent_layer);
    virtual void draw_feature(MEDC* dc);

    MEColor* m_color;
    MEExtent * m_ext;
    long feature_id;
    char * feature_name;
    bool m_filled;
    bool isll;
private:
    MEResultFeature(const MEResultFeature & o);
    MEResultFeature & operator=(const MEResultFeature & o);
};

// result point feature
class PointFeature : public MEResultFeature
{
public:
    PointFeature(MELayer * parent_layer, double latlong_x, double latlong_y, int size,
            const char* color, long feature_id, const char *name);
    PointFeature(MELayer * parent_layer, int win_x, int win_y, int size,
            const char* color, long feature_id, const char *name);

    virtual ~PointFeature();

    double GetX(void) { return ll.latlong_x; }
    double GetY(void) { return ll.latlong_y; }
    void SetX(double x) {ll.latlong_x = x;} 
    void SetY(double y) {ll.latlong_y = y;} 
protected:
    void draw_feature(MEDC* dc);
    virtual void draw_certain_feature(MEDC* dc, int win_x, int win_y);
    union {
        struct {
            double latlong_x;
            double latlong_y;
        } ll;
        struct {
            int x;
            int y;
        } win;
    };
    int size;
private:
    PointFeature(const PointFeature & o);
    PointFeature & operator=(const PointFeature & o);
};

//clicle featire
class CircleFeature : public PointFeature
{
public:
    CircleFeature(MELayer * parent_layer, double latlong_x, double latlong_y, int size,
                                                const char* color, long feature_id, const char *name);
    CircleFeature(MELayer * parent_layer, int win_x, int win_y, int size,
                                                const char* color, long feature_id, const char *name);
    virtual ~CircleFeature();

private:
    CircleFeature(const CircleFeature & o);
    CircleFeature & operator=(const CircleFeature & o);

    void draw_certain_feature(MEDC* dc, int win_x, int win_y);
};

//triangle frature
class TriangleFeature : public PointFeature 
{
public:
    TriangleFeature(MELayer * parent_layer, double latlong_x, double latlong_y, int size,
                                                const char* color, long feature_id, const char *name );
    TriangleFeature(MELayer * parent_layer, int win_x, int win_y, int size,
                                                const char* color, long feature_id, const char *name );
    virtual ~TriangleFeature();
private:
    TriangleFeature(const TriangleFeature & o);
    TriangleFeature & operator=(const TriangleFeature & o);

    void draw_certain_feature(MEDC* dc, int win_x, int win_y);
    MEPoint points[4];
};

//square feature 
class SquareFeature : public PointFeature
{
public:
    SquareFeature(MELayer * parent_layer, double latlong_x, double latlong_y, int size,
                                                const char* color, long feature_id, const char *name );
    SquareFeature(MELayer * parent_layer, int win_x, int win_y, int size,
                                                const char* color, long feature_id, const char *name );
    virtual ~SquareFeature();
private:
    SquareFeature(const SquareFeature & o);
    SquareFeature & operator=(const SquareFeature & o);

    void draw_certain_feature(MEDC* dc, int win_x, int win_y);
    MEPoint points[5];
};

//bitmap feature
class BitmapFeature : public PointFeature
{
public:
    BitmapFeature(MELayer * parent_layer, double latlong_x, double latlong_y, int size, 
                int width, int height, long feature_id, const PBITMAP pBitmap, const char *name);
    BitmapFeature(MELayer * parent_layer, int win_x, int win_y, int size, 
                int width, int height, long feature_id, const PBITMAP pBitmap, const char *name);
    virtual ~BitmapFeature();
private:
    BitmapFeature(const BitmapFeature & o);
    BitmapFeature & operator=(const BitmapFeature & o);

    void draw_certain_feature(MEDC* dc, int win_x, int win_y);
    PBITMAP m_pBitmap;
    int m_width;
    int m_height;
};

//line feature
class LinesFeature : public MEResultFeature
{
public:
    LinesFeature(MELayer * parent_layer, const char* color, long feature_id, const char * name, int pen_width);
    virtual ~LinesFeature();

    void AddPoint(double x, double y);

protected:
    void draw_feature(MEDC* dc);
    virtual void draw_certain_feature(MEDC* dc, const MEPoint* pts, int npts);

    int pen_width;
    std::vector<MEDoublePoint*> points;
private:
    LinesFeature(const LinesFeature & o);
    LinesFeature & operator=(const LinesFeature & o);
};

//polygon feature
class PolygonFeature : public LinesFeature
{
public:
    PolygonFeature(MELayer * parent_layer, const char* color, long feature_id, const char *name, int pen_width);
    virtual ~PolygonFeature();

private:    
    PolygonFeature(const PolygonFeature & o);
    PolygonFeature & operator=(const PolygonFeature & o);

    bool m_filled;
    void draw_certain_feature(MEDC* dc, const MEPoint* pts, int npts);
};

//big circle feature
class BigCircleFeature : public MEResultFeature
{
public:
    BigCircleFeature(MELayer * parent_layer, double latlong_x, double latlong_y, double size, const char* color,
                    long feature_id, const char *name, int pen_width);

    virtual ~BigCircleFeature();
private:
    BigCircleFeature(const BigCircleFeature & o);
    BigCircleFeature & operator=(const BigCircleFeature & o);

    void draw_feature(MEDC* dc);

    double latlong_x;
    double latlong_y;
    double size;
    int pen_width;
};

#endif
