/*
** $Id: spatialref.h,v 1.3 2006-11-28 01:03:22 jpzhang Exp $
**
** Spatialref class define
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Yang Yan.
**
** Create date: 2006/08/10
**
*/

#ifndef _SPATIALREF_H_
#define _SPATIALREF_H_

#include <string>
#include <math.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "ogr.h"

#define MEEQUAL(a,b)            (strcasecmp(a,b)==0)
#define MEEQUALN(a,b,n)         (strncasecmp(a,b,n)==0)

#define ME_UA_DEGREE_CONV      "0.0174532925199433"
#define ME_UA_DEGREE           "degree"

#define ME_PM_GREENWICH        "Greenwich"

/* ==================================================================== */
/*      Some "standard" strings.                                        */
/* ==================================================================== */

#define ME_PT_ALBERS_CONIC_EQUAL_AREA                                  \
                                "Albers_Conic_Equal_Area"
#define ME_PT_AZIMUTHAL_EQUIDISTANT "Azimuthal_Equidistant"
#define ME_PT_CASSINI_SOLDNER  "Cassini_Soldner"
#define ME_PT_CYLINDRICAL_EQUAL_AREA "Cylindrical_Equal_Area"
#define ME_PT_BONNE            "Bonne"
#define ME_PT_ECKERT_IV        "Eckert_IV"
#define ME_PT_ECKERT_VI        "Eckert_VI"
#define ME_PT_EQUIDISTANT_CONIC "Equidistant_Conic"
#define ME_PT_EQUIRECTANGULAR  "Equirectangular"
#define ME_PT_GALL_STEREMEAPHIC "Gall_Stereographic"
#define ME_PT_GEOSTATIONARY_SATELLITE "Geostationary_Satellite"
#define ME_PT_GOODE_HOMOLOSINE "Goode_Homolosine"
#define ME_PT_GNOMONIC         "Gnomonic"
#define ME_PT_HOTINE_OBLIQUE_MERCATOR                                  \
                                "Hotine_Oblique_Mercator"
#define ME_PT_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN         \
                            "Hotine_Oblique_Mercator_Two_Point_Natural_Origin"
#define ME_PT_LAMBERT_CONFORMAL_CONIC_1SP                              \
                                "Lambert_Conformal_Conic_1SP"
#define ME_PT_LAMBERT_CONFORMAL_CONIC_2SP                              \
                                "Lambert_Conformal_Conic_2SP"
#define ME_PT_LAMBERT_CONFORMAL_CONIC_2SP_BELGIUM                      \
                                "Lambert_Conformal_Conic_2SP_Belgium)"
#define ME_PT_LAMBERT_AZIMUTHAL_EQUAL_AREA                     \
                                "Lambert_Azimuthal_Equal_Area"
#define ME_PT_MERCATOR_1SP     "Mercator_1SP"

#define ME_PT_MILLER_CYLINDRICAL "Miller_Cylindrical"
#define ME_PT_MOLLWEIDE        "Mollweide"
#define ME_PT_NEW_ZEALAND_MAP_GRID                                     \
                                "New_Zealand_Map_Grid"
#define ME_PT_OBLIQUE_STEREMEAPHIC                                    \
                                "Oblique_Stereographic"
#define ME_PT_ORTHMEAPHIC     "Orthographic"
#define ME_PT_POLAR_STEREMEAPHIC                                      \
                                "Polar_Stereographic"
#define ME_PT_POLYCONIC        "Polyconic"
#define ME_PT_ROBINSON         "Robinson"
#define ME_PT_SINUSOIDAL       "Sinusoidal"
#define ME_PT_STEREMEAPHIC    "Stereographic"
#define ME_PT_SWISS_OBLIQUE_CYLINDRICAL                                \
                                "Swiss_Oblique_Cylindrical"
#define ME_PT_TRANSVERSE_MERCATOR                                      \
                                "Transverse_Mercator"

/* special mapinfo variants on Transverse Mercator */
#define ME_PT_TRANSVERSE_MERCATOR_MI_21 \
                                "Transverse_Mercator_MapInfo_21"
#define ME_PT_TRANSVERSE_MERCATOR_MI_22 \
                                "Transverse_Mercator_MapInfo_22"
#define ME_PT_TRANSVERSE_MERCATOR_MI_23 \
                                "Transverse_Mercator_MapInfo_23"
#define ME_PT_TRANSVERSE_MERCATOR_MI_24 \
                                "Transverse_Mercator_MapInfo_24"
#define ME_PT_TRANSVERSE_MERCATOR_MI_25 \
                                "Transverse_Mercator_MapInfo_25"

#define ME_PT_TWO_POINT_EQUIDISTANT                                    \
                                "Two_Point_Equidistant"
#define ME_PT_VANDERGRINTEN    "VanDerGrinten"
#define ME_PT_KROVAK           "Krovak"

#define ME_PP_CENTRAL_MERIDIAN         "central_meridian"
#define ME_PP_SCALE_FACTOR             "scale_factor"
#define ME_PP_STANDARD_PARALLEL_1      "standard_parallel_1"
#define ME_PP_STANDARD_PARALLEL_2      "standard_parallel_2"

#define ME_PP_LONGITUDE_OF_CENTER      "longitude_of_center"
#define ME_PP_LATITUDE_OF_CENTER       "latitude_of_center"
#define ME_PP_LONGITUDE_OF_ORIGIN      "longitude_of_origin"
#define ME_PP_LATITUDE_OF_ORIGIN       "latitude_of_origin"
#define ME_PP_FALSE_EASTING            "false_easting"
#define ME_PP_FALSE_NORTHING           "false_northing"
#define ME_PP_AZIMUTH                  "azimuth"
#define ME_PP_LONGITUDE_OF_POINT_1     "longitude_of_point_1"
#define ME_PP_LATITUDE_OF_POINT_1      "latitude_of_point_1"
#define ME_PP_LONGITUDE_OF_POINT_2     "longitude_of_point_2"
#define ME_PP_LATITUDE_OF_POINT_2      "latitude_of_point_2"
#define ME_PP_LONGITUDE_OF_POINT_3     "longitude_of_point_3"
#define ME_PP_RECTIFIED_GRID_ANGLE     "rectified_grid_angle"
#define ME_PP_SATELLITE_HEIGHT         "satellite_height"

#define ME_PP_LATITUDE_OF_1ST_POINT    "Latitude_Of_1st_Point"
#define ME_PP_LONGITUDE_OF_1ST_POINT   "Longitude_Of_1st_Point"
#define ME_PP_LATITUDE_OF_2ND_POINT    "Latitude_Of_2nd_Point"
#define ME_PP_LONGITUDE_OF_2ND_POINT   "Longitude_Of_2nd_Point"


#define ME_WGS84_INVFLATTENING 298.257223563
#define ME_WGS84_SEMIMAJOR     6378137.0

#define MET_HONOURSTRINGS      0x0001
#define MET_ALLOWEMPTYTOKENS   0x0002
#define MET_PRESERVEQUOTES     0x0004
#define MET_PRESERVEESCAPES    0x0008

#define ME_DN_NAD27            "North_American_Datum_1927"
#define ME_DN_NAD83            "North_American_Datum_1983"
#define ME_DN_WGS84            "WGS_1984"

#define ME_UL_FOOT             "Foot (International)" /* or just "FOOT"? */
#define ME_UL_US_FOOT          "U.S. Foot" /* or "US survey foot" */
#define ME_UL_NAUTICAL_MILE    "Nautical Mile"

class MESRSNode
{
public:
    MESRSNode(const OGR_SRSNode * ogrnode);
    MESRSNode(const char * = NULL);
    ~MESRSNode();

    int IsLeafNode() const { return nChildren == 0; }

    int GetChildCount() const { return nChildren; }
    MESRSNode *GetChild( int );
    const MESRSNode *GetChild( int ) const;

    MESRSNode *GetNode( const char * );
    const MESRSNode *GetNode( const char * ) const;

    void InsertChild( MESRSNode *, int );
    void AddChild( MESRSNode * );
    int  FindChild( const char * ) const;
    void DestroyChild( int );
    void StripNodes( const char * );

    const char  *GetValue() const { return pszValue; }
    void  SetValue( const char * );

    void MakeValueSafe();
    int FixupOrdering();

    MESRSNode *Clone() const;

    int importFromWkt( char ** );
    int exportToWkt( char ** ) const;
    int exportToPrettyWkt( char **, int = 1) const;

    int applyRemapper( const char *pszNode, 
                               char **papszSrcValues, 
                               char **papszDstValues, 
                               int nStepSize = 1,
                               int bChildOfHit = false );
private:
    MESRSNode(const MESRSNode & o);
    MESRSNode operator=(const MESRSNode & o);

    char *pszValue;

    MESRSNode **papoChildNodes;
    MESRSNode *poParent;

    int nChildren;

    void ClearChildren();
    int NeedsQuoting() const;
};

/************************************************************************/
/*                         MESpatialReference                          */
/************************************************************************/
class MESpatialReference
{
    double      dfFromGreenwich;
    double      dfToMeter;
    double      dfToDegrees;

    MESRSNode *poRoot;

    int         nRefCount;
    int         bNormInfoSet;

    int      ValidateProjection();
    int         IsAliasFor( const char *, const char * );
    void        GetNormInfo() const;

  public:
                MESpatialReference(const OGRSpatialReference *);
                MESpatialReference(const char * = NULL);
                
    virtual    ~MESpatialReference();
                
    MESpatialReference &operator=(const MESpatialReference&);

    int         Reference();
    int         Dereference();
    int         GetReferenceCount() const { return nRefCount; }
    void        Release();

    MESpatialReference *Clone() const;

    int      exportToWkt( char ** ) const;
    int      exportToProj4( char ** ) const;
    
    int      importFromWkt( char ** );
    int      importFromEPSG( int );
    
    // Machinary for accessing parse nodes
    MESRSNode *GetRoot() { return poRoot; }
    const MESRSNode *GetRoot() const { return poRoot; }
    void        SetRoot( MESRSNode * );
    
    MESRSNode *GetAttrNode(const char *);
    const MESRSNode *GetAttrNode(const char *) const;
    const char  *GetAttrValue(const char *, int = 0) const;

    int      SetNode( const char *, const char * );
    int      SetNode( const char *, double );
    
    double      GetLinearUnits( char ** = NULL ) const;

    double      GetAngularUnits( char ** = NULL ) const;

    double      GetPrimeMeridian( char ** = NULL ) const;

    int         IsGeographic() const;
    int         IsProjected() const;
    int         IsLocal() const;

    void        Clear();
    int      SetProjection( const char * );
    int      SetGeogCS( const char * pszGeogName,
                           const char * pszDatumName,
                           const char * pszEllipsoidName,
                           double dfSemiMajor, double dfInvFlattening,
                           const char * pszPMName = NULL,
                           double dfPMOffset = 0.0,
                           const char * pszUnits = NULL,
                           double dfConvertToRadians = 0.0 );
    int      CopyGeogCSFrom( const MESpatialReference * poSrcSRS );

    int      SetFromUserInput( const char * );

    int      SetTOWGS84( double, double, double,
                            double = 0.0, double = 0.0, double = 0.0,
                            double = 0.0 );
    
    double      GetSemiMajor( int * = NULL ) const;
    double      GetSemiMinor( int * = NULL ) const;
    double      GetInvFlattening( int * = NULL ) const;

    int      SetAuthority( const char * pszTargetKey, 
                              const char * pszAuthority, 
                              int nCode );

    const char *GetAuthorityCode( const char * pszTargetKey ) const;
    const char *GetAuthorityName( const char * pszTargetKey ) const;
                           
    int      SetProjParm( const char *, double );
    double      GetProjParm( const char *, double =0.0, int* = NULL ) const;

    int      SetNormProjParm( const char *, double );
    double      GetNormProjParm( const char *, double=0.0, int* =NULL)const;
    static int  IsAngularParameter( const char * );
    static int  IsLongitudeParameter( const char * );
    static int  IsLinearParameter( const char * );
    /** Gall Stereograpic */
    int      SetGS( double dfCentralMeridian,
                       double dfFalseEasting, double dfFalseNorthing );

    /** Universal Transverse Mercator */
    int         GetUTMZone( int *pbNorth = NULL ) const;

    /** State Plane */
    int      SetStatePlane( int nZone, int bNAD83 = TRUE,
                               const char *pszOverrideUnitName = NULL,
                               double dfOverrideUnit = 0.0 );
};

/************************************************************************/
/*                     MECoordinateTransformation                      */
/************************************************************************/
 
class MECoordinateTransformation
{
public:
    virtual ~MECoordinateTransformation() {}

    // From CT_CoordinateTransformation

    /** Fetch internal source coordinate system. */
    virtual MESpatialReference *GetSourceCS() = 0;

    /** Fetch internal target coordinate system. */
    virtual MESpatialReference *GetTargetCS() = 0;

    virtual int Transform( int nCount, 
                           double *x, double *y, double *z = NULL ) = 0;
    virtual int TransformEx( int nCount, 
                             double *x, double *y, double *z = NULL,
                             int *pabSuccess = NULL ) = 0;

};

MECoordinateTransformation *
MECreateCoordinateTransformation( MESpatialReference *poSource, 
                                   MESpatialReference *poTarget );
int MECount(char **papszStrList);

void MEDestroy(char **papszStrList);
int MEFindString( char ** papszList, const char * pszTarget );

char ** METokenizeString2( const char * pszString,
                            const char * pszDelimiters,
                            int nMETFlags );

char ** METokenizeStringComplex( const char * pszString,
        const char * pszDelimiters,
        int bHonourStrings, int bAllowEmptyTokens );


#endif /* ndef _SPATIALREF_H_ */
