
#include "spatialref.h"

int MESpatialReference::exportToProj4( char ** ppszProj4 ) const

{
    char        szProj4[512];
    const char *pszProjection = GetAttrValue("PROJECTION");

    szProj4[0] = '\0';

/* -------------------------------------------------------------------- */
/*      Get the prime meridian info.                                    */
/* -------------------------------------------------------------------- */
    const MESRSNode *poPRIMEM = GetAttrNode( "PRIMEM" );
    double dfFromGreenwich = 0.0;

    if( poPRIMEM != NULL && poPRIMEM->GetChildCount() >= 2 
        && atof(poPRIMEM->GetChild(1)->GetValue()) != 0.0 )
    {
        dfFromGreenwich = atof(poPRIMEM->GetChild(1)->GetValue());
    }

/* ==================================================================== */
/*      Handle the projection definition.                               */
/* ==================================================================== */

    if( pszProjection == NULL && IsGeographic() )
    {
        sprintf( szProj4+strlen(szProj4), "+proj=longlat " );
    }
    else if( pszProjection == NULL && !IsGeographic() )
    {
        // LOCAL_CS, or incompletely initialized coordinate systems.
        *ppszProj4 = strdup("");
        return 0;
    }
    else if( MEEQUAL(pszProjection,ME_PT_CYLINDRICAL_EQUAL_AREA) )
    {
        sprintf( szProj4+strlen(szProj4),
           "+proj=cea +lon_0=%.16g +lat_ts=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_STANDARD_PARALLEL_1,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_BONNE) )
    {
        sprintf( szProj4+strlen(szProj4),
           "+proj=bonne +lon_0=%.16g +lat_1=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_STANDARD_PARALLEL_1,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_CASSINI_SOLDNER) )
    {
        sprintf( szProj4+strlen(szProj4),
           "+proj=cass +lat_0=%.16g +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_NEW_ZEALAND_MAP_GRID) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=nzmg +lat_0=%.16g +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_TRANSVERSE_MERCATOR) ||
             MEEQUAL(pszProjection,ME_PT_TRANSVERSE_MERCATOR_MI_21) ||
             MEEQUAL(pszProjection,ME_PT_TRANSVERSE_MERCATOR_MI_22) ||
             MEEQUAL(pszProjection,ME_PT_TRANSVERSE_MERCATOR_MI_23) ||
             MEEQUAL(pszProjection,ME_PT_TRANSVERSE_MERCATOR_MI_24) ||
             MEEQUAL(pszProjection,ME_PT_TRANSVERSE_MERCATOR_MI_25) )
    {
        int bNorth;
        int nZone = GetUTMZone( &bNorth );

        if( nZone != 0 )
        {
            if( bNorth )
                sprintf( szProj4+strlen(szProj4), "+proj=utm +zone=%d ", 
                         nZone );
            else
                sprintf( szProj4+strlen(szProj4),"+proj=utm +zone=%d +south ", 
                         nZone );
        }            
        else
            sprintf( szProj4+strlen(szProj4),
             "+proj=tmerc +lat_0=%.16g +lon_0=%.16g +k=%f +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_SCALE_FACTOR,1.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_MERCATOR_1SP) )
    {
        sprintf( szProj4+strlen(szProj4),
           "+proj=merc +lat_ts=%.16g +lon_0=%.16g +k=%f +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_SCALE_FACTOR,1.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_OBLIQUE_STEREMEAPHIC) )
    {
        sprintf( szProj4+strlen(szProj4),
         "+proj=sterea +lat_0=%.16g +lon_0=%.16g +k=%f +x_0=%.16g +y_0=%.16g ",
//         "+proj=stere +lat_0=%.16g +lon_0=%.16g +k=%f +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_SCALE_FACTOR,1.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_STEREMEAPHIC) )
    {
        sprintf( szProj4+strlen(szProj4),
           "+proj=stere +lat_0=%.16g +lon_0=%.16g +k=%f +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_SCALE_FACTOR,1.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_POLAR_STEREMEAPHIC) )
    {
        if( GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0) >= 0.0 )
            sprintf( szProj4+strlen(szProj4),
                     "+proj=stere +lat_0=90 +lat_ts=%.16g +lon_0=%.16g "
                     "+k=%.16g +x_0=%.16g +y_0=%.16g ",
                     GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,90.0),
                     GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                     GetNormProjParm(ME_PP_SCALE_FACTOR,1.0),
                     GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                     GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
        else
            sprintf( szProj4+strlen(szProj4),
                     "+proj=stere +lat_0=-90 +lat_ts=%.16g +lon_0=%.16g "
                     "+k=%.16g +x_0=%.16g +y_0=%.16g ",
                     GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,-90.0),
                     GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                     GetNormProjParm(ME_PP_SCALE_FACTOR,1.0),
                     GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                     GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_EQUIRECTANGULAR) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=eqc +lat_ts=%.16g +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_GNOMONIC) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=gnom +lat_0=%.16g +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_ORTHMEAPHIC) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=ortho +lat_0=%.16g +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_LAMBERT_AZIMUTHAL_EQUAL_AREA) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=laea +lat_0=%.16g +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_AZIMUTHAL_EQUIDISTANT) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=aeqd +lat_0=%.16g +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_EQUIDISTANT_CONIC) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=eqdc +lat_0=%.16g +lon_0=%.16g +lat_1=%.16g +lat_2=%.16g"
                 " +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_CENTER,0.0),
                 GetNormProjParm(ME_PP_LONGITUDE_OF_CENTER,0.0),
                 GetNormProjParm(ME_PP_STANDARD_PARALLEL_1,0.0),
                 GetNormProjParm(ME_PP_STANDARD_PARALLEL_2,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_MILLER_CYLINDRICAL) )
    {
        sprintf( szProj4+strlen(szProj4),
                "+proj=mill +lat_0=%.16g +lon_0=%.16g +x_0=%.16g +y_0=%.16g +R_A ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_MOLLWEIDE) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=moll +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_ECKERT_IV) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=eck4 +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_ECKERT_VI) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=eck6 +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_POLYCONIC) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=poly +lat_0=%.16g +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_ALBERS_CONIC_EQUAL_AREA) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=aea +lat_1=%.16g +lat_2=%.16g +lat_0=%.16g +lon_0=%.16g"
                 " +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_STANDARD_PARALLEL_1,0.0),
                 GetNormProjParm(ME_PP_STANDARD_PARALLEL_2,0.0),
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_ROBINSON) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=robin +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_VANDERGRINTEN) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=vandg +lon_0=%.16g +x_0=%.16g +y_0=%.16g +R_A ",
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_SINUSOIDAL) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=sinu +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LONGITUDE_OF_CENTER,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_GALL_STEREMEAPHIC) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=gall +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_GOODE_HOMOLOSINE) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=goode +lon_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_GEOSTATIONARY_SATELLITE) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=geos +lon_0=%.16g +h=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_SATELLITE_HEIGHT,35785831.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_LAMBERT_CONFORMAL_CONIC_2SP)
         || MEEQUAL(pszProjection,ME_PT_LAMBERT_CONFORMAL_CONIC_2SP_BELGIUM) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=lcc +lat_1=%.16g +lat_2=%.16g +lat_0=%.16g +lon_0=%.16g"
                 " +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_STANDARD_PARALLEL_1,0.0),
                 GetNormProjParm(ME_PP_STANDARD_PARALLEL_2,0.0),
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }
    
    else if( MEEQUAL(pszProjection,ME_PT_LAMBERT_CONFORMAL_CONIC_1SP) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=lcc +lat_1=%.16g +lat_0=%.16g +lon_0=%.16g"
                 " +k_0=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_SCALE_FACTOR,1.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_HOTINE_OBLIQUE_MERCATOR) )
    {
        /* not clear how ProjParm[3] - angle from rectified to skewed grid -
           should be applied ... see the +not_rot flag for PROJ.4.
           Just ignoring for now. */

        /* special case for swiss oblique mercator : see bug 423 */
        if( fabs(GetNormProjParm(ME_PP_AZIMUTH,0.0) - 90.0) < 0.0001 
            && fabs(GetNormProjParm(ME_PP_RECTIFIED_GRID_ANGLE,0.0)-90.0) < 0.0001 )
        {
            sprintf( szProj4+strlen(szProj4),
                     "+proj=somerc +lat_0=%.16g +lon_0=%.16g"
                     " +x_0=%.16g +y_0=%.16g ",
                     GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                     GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                     GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                     GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
        }
        else
        {
            sprintf( szProj4+strlen(szProj4),
                     "+proj=omerc +lat_0=%.16g +lonc=%.16g +alpha=%.16g"
                     " +k=%.16g +x_0=%.16g +y_0=%.16g ",
                     GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                     GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                     GetNormProjParm(ME_PP_AZIMUTH,0.0),
                     GetNormProjParm(ME_PP_SCALE_FACTOR,1.0),
                     GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                     GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
        }
    }

    else if( MEEQUAL(pszProjection,
                   ME_PT_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=omerc +lat_0=%.16g"
                 " +lon_1=%.16g +lat_1=%.16g +lon_2=%.16g +lat_2=%.16g"
                 " +k=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_LATITUDE_OF_POINT_1,0.0),
                 GetNormProjParm(ME_PP_LONGITUDE_OF_POINT_1,0.0),
                 GetNormProjParm(ME_PP_LATITUDE_OF_POINT_2,0.0),
                 GetNormProjParm(ME_PP_LONGITUDE_OF_POINT_2,0.0),
                 GetNormProjParm(ME_PP_SCALE_FACTOR,1.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_KROVAK) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=krovak +lat_0=%.16g +lon_0=%.16g +alpha=%.16g"
                 " +k=%.16g +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_CENTER,0.0),
                 GetNormProjParm(ME_PP_LONGITUDE_OF_CENTER,0.0),
                 GetNormProjParm(ME_PP_AZIMUTH,0.0),
                 GetNormProjParm(ME_PP_SCALE_FACTOR,1.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    else if( MEEQUAL(pszProjection,ME_PT_TWO_POINT_EQUIDISTANT) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=tpeqd +lat_0=%.16g +lon_0=%.16g "
                 "+lat_1=%.16g +lon_1=%.16g "
                 "+x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_1ST_POINT,0.0),
                 GetNormProjParm(ME_PP_LONGITUDE_OF_1ST_POINT,0.0),
                 GetNormProjParm(ME_PP_LATITUDE_OF_2ND_POINT,0.0),
                 GetNormProjParm(ME_PP_LONGITUDE_OF_2ND_POINT,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

    /* Note: This never really gets used currently.  See bug 423 */
    else if( MEEQUAL(pszProjection,ME_PT_SWISS_OBLIQUE_CYLINDRICAL) )
    {
        sprintf( szProj4+strlen(szProj4),
                 "+proj=somerc +lat_0=%.16g +lon_0=%.16g"
                 " +x_0=%.16g +y_0=%.16g ",
                 GetNormProjParm(ME_PP_LATITUDE_OF_ORIGIN,0.0),
                 GetNormProjParm(ME_PP_CENTRAL_MERIDIAN,0.0),
                 GetNormProjParm(ME_PP_FALSE_EASTING,0.0),
                 GetNormProjParm(ME_PP_FALSE_NORTHING,0.0) );
    }

/* -------------------------------------------------------------------- */
/*      Handle earth model.  For now we just always emit the user       */
/*      defined ellipsoid parameters.                                   */
/* -------------------------------------------------------------------- */
    double      dfSemiMajor = GetSemiMajor();
    double      dfInvFlattening = GetInvFlattening();
    const char  *pszPROJ4Ellipse = NULL;
    const char  *pszDatum = GetAttrValue("DATUM");

    if( ABS(dfSemiMajor-6378249.145) < 0.01
        && ABS(dfInvFlattening-293.465) < 0.0001 )
    {
        pszPROJ4Ellipse = "clrk80";     /* Clark 1880 */
    }
    else if( ABS(dfSemiMajor-6378245.0) < 0.01
             && ABS(dfInvFlattening-298.3) < 0.0001 )
    {
        pszPROJ4Ellipse = "krass";      /* Krassovsky */
    }
    else if( ABS(dfSemiMajor-6378388.0) < 0.01
             && ABS(dfInvFlattening-297.0) < 0.0001 )
    {
        pszPROJ4Ellipse = "intl";       /* International 1924 */
    }
    else if( ABS(dfSemiMajor-6378160.0) < 0.01
             && ABS(dfInvFlattening-298.25) < 0.0001 )
    {
        pszPROJ4Ellipse = "aust_SA";    /* Australian */
    }
    else if( ABS(dfSemiMajor-6377397.155) < 0.01
             && ABS(dfInvFlattening-299.1528128) < 0.0001 )
    {
        pszPROJ4Ellipse = "bessel";     /* Bessel 1841 */
    }
    else if( ABS(dfSemiMajor-6377483.865) < 0.01
             && ABS(dfInvFlattening-299.1528128) < 0.0001 )
    {
        pszPROJ4Ellipse = "bess_nam";   /* Bessel 1841 (Namibia / Schwarzeck)*/
    }
    else if( ABS(dfSemiMajor-6378160.0) < 0.01
             && ABS(dfInvFlattening-298.247167427) < 0.0001 )
    {
        pszPROJ4Ellipse = "GRS67";      /* GRS 1967 */
    }
    else if( ABS(dfSemiMajor-6378137) < 0.01
             && ABS(dfInvFlattening-298.257222101) < 0.000001 )
    {
        pszPROJ4Ellipse = "GRS80";      /* GRS 1980 */
    }
    else if( ABS(dfSemiMajor-6378206.4) < 0.01
             && ABS(dfInvFlattening-294.9786982) < 0.0001 )
    {
        pszPROJ4Ellipse = "clrk66";     /* Clarke 1866 */
    }
    else if( ABS(dfSemiMajor-6378206.4) < 0.01
             && ABS(dfInvFlattening-294.9786982) < 0.0001 )
    {
        pszPROJ4Ellipse = "mod_airy";   /* Modified Airy */
    }
    else if( ABS(dfSemiMajor-6377563.396) < 0.01
             && ABS(dfInvFlattening-299.3249646) < 0.0001 )
    {
        pszPROJ4Ellipse = "airy";       /* Modified Airy */
    }
    else if( ABS(dfSemiMajor-6378200) < 0.01
             && ABS(dfInvFlattening-298.3) < 0.0001 )
    {
        pszPROJ4Ellipse = "helmert";    /* Helmert 1906 */
    }
    else if( ABS(dfSemiMajor-6378155) < 0.01
             && ABS(dfInvFlattening-298.3) < 0.0001 )
    {
        pszPROJ4Ellipse = "fschr60m";   /* Modified Fischer 1960 */
    }
    else if( ABS(dfSemiMajor-6377298.556) < 0.01
             && ABS(dfInvFlattening-300.8017) < 0.0001 )
    {
        pszPROJ4Ellipse = "evrstSS";    /* Everest (Sabah & Sarawak) */
    }
    else if( ABS(dfSemiMajor-6378165.0) < 0.01
             && ABS(dfInvFlattening-298.3) < 0.0001 )
    {
        pszPROJ4Ellipse = "WGS60";      
    }
    else if( ABS(dfSemiMajor-6378145.0) < 0.01
             && ABS(dfInvFlattening-298.25) < 0.0001 )
    {
        pszPROJ4Ellipse = "WGS66";      
    }
    else if( ABS(dfSemiMajor-6378135.0) < 0.01
             && ABS(dfInvFlattening-298.26) < 0.0001 )
    {
        pszPROJ4Ellipse = "WGS72";      
    }
    else if( ABS(dfSemiMajor-6378137.0) < 0.01
             && ABS(dfInvFlattening-298.257223563) < 0.000001 )
    {
        pszPROJ4Ellipse = "WGS84";
    }
    else if( MEEQUAL(pszDatum,"North_American_Datum_1927") )
    {
//        pszPROJ4Ellipse = "clrk66:+datum=nad27"; /* NAD 27 */
        pszPROJ4Ellipse = "clrk66";
    }
    else if( MEEQUAL(pszDatum,"North_American_Datum_1983") )
    {
//        pszPROJ4Ellipse = "GRS80:+datum=nad83";       /* NAD 83 */
        pszPROJ4Ellipse = "GRS80";
    }
    
    if( pszPROJ4Ellipse == NULL )
        sprintf( szProj4+strlen(szProj4), "+a=%.16g +b=%.16g ",
                 GetSemiMajor(), GetSemiMinor() );
    else
        sprintf( szProj4+strlen(szProj4), "+ellps=%s ",
                 pszPROJ4Ellipse );

/* -------------------------------------------------------------------- */
/*      Translate the datum.                                            */
/* -------------------------------------------------------------------- */
    const char *pszPROJ4Datum = NULL;
    const MESRSNode *poTOWGS84 = GetAttrNode( "TOWGS84" );
    char  szTOWGS84[256];
    int nEPSGDatum = -1;
    const char *pszAuthority;
    int nEPSGGeogCS = -1;
    const char *pszGeogCSAuthority;

    pszAuthority = GetAuthorityName( "DATUM" );

    if( pszAuthority != NULL && MEEQUAL(pszAuthority,"EPSG") )
        nEPSGDatum = atoi(GetAuthorityCode( "DATUM" ));

    pszGeogCSAuthority = GetAuthorityName( "GEOGCS" );

    if( pszGeogCSAuthority != NULL && MEEQUAL(pszGeogCSAuthority,"EPSG") )
        nEPSGGeogCS = atoi(GetAuthorityCode( "GEOGCS" ));

    if( pszDatum == NULL )
        /* nothing */;

    else if( MEEQUAL(pszDatum,ME_DN_NAD27) || nEPSGDatum == 6267 )
        pszPROJ4Datum = "+datum=NAD27";

    else if( MEEQUAL(pszDatum,ME_DN_NAD83) || nEPSGDatum == 6269 )
        pszPROJ4Datum = "+datum=NAD83";

    else if( MEEQUAL(pszDatum,ME_DN_WGS84) || nEPSGDatum == 6326 )
        pszPROJ4Datum = "+datum=WGS84";

    else if( nEPSGDatum == 6314 )
        pszPROJ4Datum = "+datum=potsdam";

    else if( nEPSGDatum == 6272 )
        pszPROJ4Datum = "+datum=nzgd49";

    else if( poTOWGS84 != NULL )
    {
        if( poTOWGS84->GetChildCount() > 2
            && (poTOWGS84->GetChildCount() < 6 
                || MEEQUAL(poTOWGS84->GetChild(3)->GetValue(),"")
                && MEEQUAL(poTOWGS84->GetChild(4)->GetValue(),"")
                && MEEQUAL(poTOWGS84->GetChild(5)->GetValue(),"")
                && MEEQUAL(poTOWGS84->GetChild(6)->GetValue(),"")) )
        {
            sprintf( szTOWGS84, "+towgs84=%s,%s,%s",
                     poTOWGS84->GetChild(0)->GetValue(),
                     poTOWGS84->GetChild(1)->GetValue(),
                     poTOWGS84->GetChild(2)->GetValue() );
            pszPROJ4Datum = szTOWGS84;
        }
        else if( poTOWGS84->GetChildCount() > 6 )
        {
            sprintf( szTOWGS84, "+towgs84=%s,%s,%s,%s,%s,%s,%s",
                     poTOWGS84->GetChild(0)->GetValue(),
                     poTOWGS84->GetChild(1)->GetValue(),
                     poTOWGS84->GetChild(2)->GetValue(),
                     poTOWGS84->GetChild(3)->GetValue(),
                     poTOWGS84->GetChild(4)->GetValue(),
                     poTOWGS84->GetChild(5)->GetValue(),
                     poTOWGS84->GetChild(6)->GetValue() );
            pszPROJ4Datum = szTOWGS84;
        }
    }

    else if( nEPSGGeogCS != -1 )
    {
#if 0
        double padfTransform[7];
        if( EPSGGetWGS84Transform( nEPSGGeogCS, padfTransform ) )
        {
            sprintf( szTOWGS84, "+towgs84=%f,%f,%f,%f,%f,%f,%f",
                     padfTransform[0],
                     padfTransform[1],
                     padfTransform[2],
                     padfTransform[3],
                     padfTransform[4],
                     padfTransform[5],
                     padfTransform[6] );
            pszPROJ4Datum = szTOWGS84;
        }
#endif
    }
    
    if( pszPROJ4Datum != NULL )
    {
        strcat( szProj4, pszPROJ4Datum );
        strcat( szProj4, " " );
    }

/* -------------------------------------------------------------------- */
/*      Is there prime meridian info to apply?                          */
/* -------------------------------------------------------------------- */
    if( poPRIMEM != NULL && poPRIMEM->GetChildCount() >= 2 
        && atof(poPRIMEM->GetChild(1)->GetValue()) != 0.0 )
    {
        const char *pszAuthority = GetAuthorityName( "PRIMEM" );
        char szPMValue[128];
        int  nCode = -1;

        if( pszAuthority != NULL && MEEQUAL(pszAuthority,"EPSG") )
            nCode = atoi(GetAuthorityCode( "PRIMEM" ));

        switch( nCode )
        {
          case 8902:
            strcpy( szPMValue, "lisbon" );
            break;

          case 8903:
            strcpy( szPMValue, "paris" );
            break;

          case 8904:
            strcpy( szPMValue, "bogota" );
            break;

          case 8905:
            strcpy( szPMValue, "madrid" );
            break;

          case 8906:
            strcpy( szPMValue, "rome" );
            break;

          case 8907:
            strcpy( szPMValue, "bern" );
            break;

          case 8908:
            strcpy( szPMValue, "jakarta" );
            break;

          case 8909:
            strcpy( szPMValue, "ferro" );
            break;

          case 8910:
            strcpy( szPMValue, "brussels" );
            break;

          case 8911:
            strcpy( szPMValue, "stockholm" );
            break;

          case 8912:
            strcpy( szPMValue, "athens" );
            break;

          case 8913:
            strcpy( szPMValue, "oslo" );
            break;

          default:
            sprintf( szPMValue, "%.16g", dfFromGreenwich );
        }

        sprintf( szProj4+strlen(szProj4), "+pm=%s ", szPMValue );
    }
    
/* -------------------------------------------------------------------- */
/*      Handle linear units.                                            */
/* -------------------------------------------------------------------- */
    const char  *pszPROJ4Units=NULL;
    char        *pszLinearUnits = NULL;
    double      dfLinearConv;

    dfLinearConv = GetLinearUnits( &pszLinearUnits );
        
    if( strstr(szProj4,"longlat") != NULL )
        pszPROJ4Units = NULL;
    
    else if( dfLinearConv == 1.0 )
        pszPROJ4Units = "m";

    else if( dfLinearConv == 1000.0 )
        pszPROJ4Units = "km";
    
    else if( dfLinearConv == 0.0254 )
        pszPROJ4Units = "in";
    
    else if( MEEQUAL(pszLinearUnits,ME_UL_FOOT) )
        pszPROJ4Units = "ft";
    
    else if( MEEQUAL(pszLinearUnits,"IYARD") || dfLinearConv == 0.9144 )
        pszPROJ4Units = "yd";
    
    else if( dfLinearConv == 0.001 )
        pszPROJ4Units = "mm";
    
    else if( dfLinearConv == 0.01 )
        pszPROJ4Units = "cm";

    else if( MEEQUAL(pszLinearUnits,ME_UL_US_FOOT) )
        pszPROJ4Units = "us-ft";

    else if( MEEQUAL(pszLinearUnits,ME_UL_NAUTICAL_MILE) )
        pszPROJ4Units = "kmi";

    else if( MEEQUAL(pszLinearUnits,"Mile") 
             || MEEQUAL(pszLinearUnits,"IMILE") )
        pszPROJ4Units = "mi";

    else
    {
        sprintf( szProj4+strlen(szProj4), "+to_meter=%.16g ",
                 dfLinearConv );
    }

    if( pszPROJ4Units != NULL )
        sprintf( szProj4+strlen(szProj4), "+units=%s ",
                 pszPROJ4Units );

/* -------------------------------------------------------------------- */
/*      Add the no_defs flag to ensure that no values from              */
/*      proj_def.dat are implicitly used with our definitions.          */
/* -------------------------------------------------------------------- */
    sprintf( szProj4+strlen(szProj4), "+no_defs " );
    
    if (szProj4)
        *ppszProj4 = strdup(szProj4);
    else
        *ppszProj4 = strdup("");

    return 0;
}

