/*
** $Id: geometry.h,v 1.24 2006-10-31 01:53:22 jpzhang Exp $
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

#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <vector>

#include "dc.h"
#include "ogr.h"
#include "utils.h"

using std::vector;

class MEDC;
class MEStyle;
class MEMapFeature;

enum GEO_TYPE{
    GEO_POINT,
    GEO_LINE,
    GEO_POLYGON,
    GEO_MULIT,
    GEO_NONE,
};

class MEGeometry
{
public:
    virtual ~MEGeometry();

    virtual void Draw (MEDC * dc, MEStyle * style)=0;//draw method
    virtual void GetTextPosition (const char * text, int fontsize, int * x, int *y)=0;
    virtual GEO_TYPE GetType(void)=0;
    virtual void UpdateScreenPoint()=0;

    static MEGeometry* GetGeometry(OGRGeometry* geom, MEMapFeature* feature); //create geometry boject with new keyword
    static MEGeometry* GetGeometry(char **p, MEMapFeature* feature);

    MEExtent * GetExtent(void) const{return ext;}//get bandbox
    MEExtent * GetOrthoExtent(void) const{return ortho_ext;}//get bandbox
protected:
    MEGeometry (OGRGeometry* geom, MEMapFeature* feature);
    MEGeometry (char **data, MEMapFeature* feature);
    MEExtent * ext; //bandbox
    MEExtent * ortho_ext; //bandbox
    MEMapFeature * m_feature;
private:
    MEGeometry(const MEGeometry & o);
    MEGeometry & operator = (const MEGeometry & o);
};

//geometry point class
class MEGeomPoint : public MEGeometry
{
public:
    MEGeomPoint (OGRPoint * ogr_point, MEMapFeature* feature);
    MEGeomPoint (char **data, MEMapFeature* feature);
    virtual ~MEGeomPoint();

    void Draw (MEDC *dc, MEStyle * style);
    void GetTextPosition (const char * text, int fontsize, int * x, int *y);
    GEO_TYPE GetType(void);
    void UpdateScreenPoint();
public:
    MEDoublePoint ll_point;
    MEDoublePoint ortho_point;
    MEPoint screen_point;
private:
    MEGeomPoint (const MEGeomPoint & o);
    MEGeomPoint & operator= (const MEGeomPoint & o);
};

struct MEPointInfo {
    MEPointInfo(OGRPoint * ogr_point, MEMapFeature* feature);
    MEPointInfo (char **data, MEMapFeature* feature);

    MEDoublePoint ll_point;
    MEDoublePoint ortho_point;
};

//geometry line class
class MEGeomLine : public MEGeometry
{
public:
    MEGeomLine (OGRLineString * ogr_line, MEMapFeature* feature);
    MEGeomLine (char **data, MEMapFeature* feature);
    virtual ~MEGeomLine();

    void Draw (MEDC * dc, MEStyle * style);
    void GetTextPosition (const char * text, int fontsize, int * x, int *y);
    GEO_TYPE GetType(void);
    void UpdateScreenPoint();
public:
    vector<MEPointInfo*> line_points;
private:
    MEGeomLine (const MEGeomLine & o);
    MEGeomLine & operator=(const MEGeomLine & o);

    MEDoublePoint * d_points;
    MEPoint *s_points;
    int num_points;
    MEPoint * pPts;
};

//geometry polygon class
class MEGeomPolygon : public MEGeometry
{
public:
    MEGeomPolygon (OGRPolygon * ogr_polygon, MEMapFeature* feature);
    MEGeomPolygon (char **data, MEMapFeature* feature);
    virtual ~MEGeomPolygon();

    void Draw (MEDC * dc, MEStyle * style);
    void GetTextPosition (const char * text, int fontsize, int * x, int *y);
    GEO_TYPE GetType(void);
    void UpdateScreenPoint();
public:
    vector<MEPointInfo *> polygon_points;
private:
    MEGeomPolygon (const MEGeomPolygon & o);
    MEGeomPolygon & operator=(const MEGeomPolygon & o);
    void fetch_points_ring(OGRLinearRing* ring);
    MEDoublePoint * d_points;
    MEPoint *s_points;
    int num_points;
    MEPoint * pPts; //polygon points
};

//geometry mulitple class
class MEGeomMulitiple : public MEGeometry
{
public:
    MEGeomMulitiple(OGRGeometryCollection * ogr_multi, MEMapFeature* feature);
    MEGeomMulitiple(char **data, MEMapFeature* feature);
    virtual ~MEGeomMulitiple();

    void Draw (MEDC * dc, MEStyle * style);
    void GetTextPosition (const char * text, int fontsize, int * x, int *y);
    GEO_TYPE GetType(void);
    void UpdateScreenPoint();
public:
    vector<MEGeometry *> geom_set;
private:
    MEGeomMulitiple(const MEGeomMulitiple & o);
    MEGeomMulitiple & operator=(const MEGeomMulitiple & o);
};

#endif /* _GEOMETRY_H_ */

