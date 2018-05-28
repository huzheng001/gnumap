#include "geometry.h"
#include "feature.h"
#include "layer.h"
#include "env.h"

//create geometry object form OGRGeometry
MEGeometry * MEGeometry::GetGeometry(OGRGeometry* geom, MEMapFeature* feature)
{
    if(geom != NULL)
    {
        switch(geom->getGeometryType())
        {
        case  wkbPoint:
            return new MEGeomPoint((OGRPoint*)geom, feature);
        case  wkbLineString:
            return new MEGeomLine((OGRLineString*)geom, feature);
        case  wkbPolygon:
            return new MEGeomPolygon((OGRPolygon*)geom, feature);
        case  wkbGeometryCollection:
        case  wkbMultiPoint:
        case  wkbMultiLineString:
        case  wkbMultiPolygon:
            return new MEGeomMulitiple((OGRGeometryCollection*)geom, feature);
        case  wkbNone:
        case  wkbLinearRing:
        case  wkbPoint25D:
        case  wkbLineString25D:
        case  wkbPolygon25D:
        case  wkbMultiPoint25D:
        case  wkbMultiLineString25D:
        case  wkbMultiPolygon25D:
        case  wkbGeometryCollection25D:
        case  wkbUnknown:
            break;
        }
    }
    return NULL;
}

MEGeometry * MEGeometry::GetGeometry(char **data, MEMapFeature* feature)
{
    unsigned int type = ArchSwapLE32(read_uint(*data));
    *data += 4;
    switch(type)
    {
        case  GEO_POINT:
            return new MEGeomPoint(data, feature);
        case  GEO_LINE:
            return new MEGeomLine(data, feature);
        case  GEO_POLYGON:
            return new MEGeomPolygon(data, feature);
        case  GEO_MULIT:
            return new MEGeomMulitiple(data, feature);
        case GEO_NONE:
            return NULL;
    }
    return NULL;
}

//class method
MEGeometry::MEGeometry (OGRGeometry* geom, MEMapFeature *feature)
    :m_feature(feature)
{
    OGREnvelope env;
    geom->getEnvelope(&env);
    ext = new MEExtent(env.MinX, env.MinY, env.MaxX, env.MaxY);
    double x[2], y[2];
    x[0] = env.MinX;
    x[1] = env.MaxX;
    y[0] = env.MinY;
    y[1] = env.MaxY;
    m_feature->GetParentLayer()->GetMap()->LatlongToOrthoBatch(2, x, y);
    ortho_ext = new MEExtent(x[0], y[0], x[1], y[1]);
}

MEGeometry::MEGeometry (char **data, MEMapFeature *feature)
    :m_feature(feature)
{
    double min_x, max_x, min_y, max_y;
    min_x = ArchSwapLE64(read_double(*data));
    *data += 8;
    max_x = ArchSwapLE64(read_double(*data));
    *data += 8;
    min_y = ArchSwapLE64(read_double(*data));
    *data += 8;
    max_y = ArchSwapLE64(read_double(*data));
    *data += 8;
    ext = new MEExtent(min_x, min_y, max_x, max_y);
    min_x = ArchSwapLE64(read_double(*data));
    *data += 8;
    max_x = ArchSwapLE64(read_double(*data));
    *data += 8;
    min_y = ArchSwapLE64(read_double(*data));
    *data += 8;
    max_y = ArchSwapLE64(read_double(*data));
    *data += 8;
    ortho_ext = new MEExtent(min_x, min_y, max_x, max_y);
}

MEGeometry::~MEGeometry ()
{
    delete ext;
    delete ortho_ext;
}


