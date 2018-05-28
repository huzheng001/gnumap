#include <math.h>
#include "spatialref.h"
// The current opinion is that WKT longitudes like central meridian
// should be relative to greenwich, not the prime meridian in use. 
// Define the following if they should be relative to the prime meridian
// of then geogcs.
#undef WKT_LONGITUDE_RELATIVE_TO_PM

static void MEPrintDouble( char * pszStrBuf, double dfValue )
{
    sprintf( pszStrBuf, "%.16g", dfValue );

    int nLen = strlen(pszStrBuf);

    // The following hack is intended to truncate some "precision" in cases
    // that appear to be roundoff error. 
    if( nLen > 15 
        && (strcmp(pszStrBuf+nLen-6,"999999") == 0
            || strcmp(pszStrBuf+nLen-6,"000001") == 0) )
    {
        sprintf( pszStrBuf, "%.15g", dfValue );
    }
}

MESpatialReference::MESpatialReference( const char * pszWKT )
{
    bNormInfoSet = FALSE;
    nRefCount = 1;
    poRoot = NULL;

    if( pszWKT != NULL )
        importFromWkt( (char **) &pszWKT );
}

MESpatialReference::MESpatialReference( const OGRSpatialReference * ogrRef )

{
    bNormInfoSet = FALSE;
    nRefCount = 1;
    poRoot = NULL;

	poRoot = new MESRSNode(ogrRef->GetRoot());

}

MESpatialReference::~MESpatialReference()
{
    if( poRoot != NULL )
        delete poRoot;
}

void MESpatialReference::Clear()
{
    if( poRoot )
        delete poRoot;

    poRoot = NULL;
}

MESpatialReference &
MESpatialReference::operator=(const MESpatialReference &oSource)
{
    if( poRoot != NULL )
    {
        delete poRoot;
        poRoot = NULL;
    }
    
    if( oSource.poRoot != NULL )
        poRoot = oSource.poRoot->Clone();

    return *this;
}

int MESpatialReference::Reference()
{
    return ++nRefCount;
}

int MESpatialReference::Dereference()
{
    return --nRefCount;
}

void MESpatialReference::Release()
{
    if( this && Dereference() == 0 )
        delete this;
}

void MESpatialReference::SetRoot( MESRSNode * poNewRoot )
{
    if( poRoot != NULL )
        delete poRoot;

    poRoot = poNewRoot;
}

MESRSNode *MESpatialReference::GetAttrNode( const char * pszNodePath )

{
    char        **papszPathTokens;
    MESRSNode *poNode;

    papszPathTokens = METokenizeStringComplex(pszNodePath, "|", TRUE, FALSE);

    if( MECount( papszPathTokens ) < 1 )
        return NULL;

    poNode = GetRoot();
    for( int i = 0; poNode != NULL && papszPathTokens[i] != NULL; i++ )
    {
        poNode = poNode->GetNode( papszPathTokens[i] );
    }

    MEDestroy( papszPathTokens );

    return poNode;
}

const MESRSNode *
MESpatialReference::GetAttrNode( const char * pszNodePath ) const

{
    MESRSNode *poNode;

    poNode = ((MESpatialReference *) this)->GetAttrNode(pszNodePath);

    return poNode;
}

const char *MESpatialReference::GetAttrValue( const char * pszNodeName,
                                               int iAttr ) const

{
    const MESRSNode *poNode;

    poNode = GetAttrNode( pszNodeName );
    if( poNode == NULL )
        return NULL;

    if( iAttr < 0 || iAttr >= poNode->GetChildCount() )
        return NULL;

    return poNode->GetChild(iAttr)->GetValue();
}

MESpatialReference *MESpatialReference::Clone() const

{
    MESpatialReference *poNewRef;

    poNewRef = new MESpatialReference();

    if( poRoot != NULL )
        poNewRef->poRoot = poRoot->Clone();

    return poNewRef;
}

int  MESpatialReference::exportToWkt( char ** ppszResult ) const

{
    if( poRoot == NULL )
    {
        *ppszResult = strdup("");
        return 0;
    }
    else
    {
        return poRoot->exportToWkt(ppszResult);
    }
}

int MESpatialReference::importFromWkt( char ** ppszInput )

{
    if( poRoot != NULL )
        delete poRoot;

    bNormInfoSet = FALSE;

    poRoot = new MESRSNode();

    return poRoot->importFromWkt( ppszInput );
}

int MESpatialReference::SetNode( const char * pszNodePath,
                                     const char * pszNewNodeValue )

{
    char        **papszPathTokens;
    int         i;
    MESRSNode *poNode;

    papszPathTokens = METokenizeStringComplex(pszNodePath, "|", TRUE, FALSE);

    if( MECount( papszPathTokens ) < 1 )
        return 6;

    if( GetRoot() == NULL || !MEEQUAL(papszPathTokens[0],GetRoot()->GetValue()) )
    {
        SetRoot( new MESRSNode( papszPathTokens[0] ) );
    }

    poNode = GetRoot();
    for( i = 1; papszPathTokens[i] != NULL; i++ )
    {
        int     j;
        
        for( j = 0; j < poNode->GetChildCount(); j++ )
        {
            if( MEEQUAL(poNode->GetChild( j )->GetValue(),papszPathTokens[i]) )
            {
                poNode = poNode->GetChild(j);
                j = -1;
                break;
            }
        }

        if( j != -1 )
        {
            MESRSNode *poNewNode = new MESRSNode( papszPathTokens[i] );
            poNode->AddChild( poNewNode );
            poNode = poNewNode;
        }
    }

    MEDestroy( papszPathTokens );

    if( pszNewNodeValue != NULL )
    {
        if( poNode->GetChildCount() > 0 )
            poNode->GetChild(0)->SetValue( pszNewNodeValue );
        else
            poNode->AddChild( new MESRSNode( pszNewNodeValue ) );
    }

    return 0;
}

int MESpatialReference::SetNode( const char *pszNodePath,
                                     double dfValue )
{
    char        szValue[64];

    if( ABS(dfValue - (int) dfValue) == 0.0 )
        sprintf( szValue, "%d", (int) dfValue );
    else
        // notdef: sprintf( szValue, "%.16g", dfValue );
        MEPrintDouble( szValue, dfValue );

    return SetNode( pszNodePath, szValue );
}

double MESpatialReference::GetAngularUnits( char ** ppszName ) const

{
    const MESRSNode *poCS = GetAttrNode( "GEOGCS" );

    if( ppszName != NULL )
        *ppszName = "degree";
        
    if( poCS == NULL )
        return atof(ME_UA_DEGREE_CONV);

    for( int iChild = 0; iChild < poCS->GetChildCount(); iChild++ )
    {
        const MESRSNode     *poChild = poCS->GetChild(iChild);
        
        if( MEEQUAL(poChild->GetValue(),"UNIT")
            && poChild->GetChildCount() >= 2 )
        {
            if( ppszName != NULL )
                *ppszName = (char *) poChild->GetChild(0)->GetValue();
            
            return atof( poChild->GetChild(1)->GetValue() );
        }
    }

    return 1.0;
}

double MESpatialReference::GetLinearUnits( char ** ppszName ) const

{
    const MESRSNode *poCS = GetAttrNode( "PROJCS" );

    if( poCS == NULL )
        poCS = GetAttrNode( "LOCAL_CS" );

    if( ppszName != NULL )
        *ppszName = "unknown";
        
    if( poCS == NULL )
        return 1.0;

    for( int iChild = 0; iChild < poCS->GetChildCount(); iChild++ )
    {
        const MESRSNode     *poChild = poCS->GetChild(iChild);
        
        if( MEEQUAL(poChild->GetValue(),"UNIT")
            && poChild->GetChildCount() >= 2 )
        {
            if( ppszName != NULL )
                *ppszName = (char *) poChild->GetChild(0)->GetValue();
            
            return atof( poChild->GetChild(1)->GetValue() );
        }
    }

    return 1.0;
}

double MESpatialReference::GetPrimeMeridian( char **ppszName ) const 

{
    const MESRSNode *poPRIMEM = GetAttrNode( "PRIMEM" );

    if( poPRIMEM != NULL && poPRIMEM->GetChildCount() >= 2 
        && atof(poPRIMEM->GetChild(1)->GetValue()) != 0.0 )
    {
        if( ppszName != NULL )
            *ppszName = (char *) poPRIMEM->GetChild(0)->GetValue();
        return atof(poPRIMEM->GetChild(1)->GetValue());
    }
    
    if( ppszName != NULL )
        *ppszName = ME_PM_GREENWICH;

    return 0.0;
}

int
MESpatialReference::SetGeogCS( const char * pszGeogName,
                                const char * pszDatumName,
                                const char * pszSpheroidName,
                                double dfSemiMajor, double dfInvFlattening,
                                const char * pszPMName, double dfPMOffset,
                                const char * pszAngularUnits,
                                double dfConvertToRadians )

{
    bNormInfoSet = FALSE;

/* -------------------------------------------------------------------- */
/*      Do we already have a GEOGCS?  If so, blow it away so it can     */
/*      be properly replaced.                                           */
/* -------------------------------------------------------------------- */
    if( GetAttrNode( "GEOGCS" ) != NULL )
    {
        MESRSNode *poPROJCS;

        if( MEEQUAL(GetRoot()->GetValue(),"GEOGCS") )
            Clear();
        else if( (poPROJCS = GetAttrNode( "PROJCS" )) != NULL
                 && poPROJCS->FindChild( "GEOGCS" ) != -1 )
            poPROJCS->DestroyChild( poPROJCS->FindChild( "GEOGCS" ) );
        else
            return 6;
    }

/* -------------------------------------------------------------------- */
/*      Set defaults for various parameters.                            */
/* -------------------------------------------------------------------- */
    if( pszGeogName == NULL )
        pszGeogName = "unnamed";

    if( pszPMName == NULL )
        pszPMName = ME_PM_GREENWICH;

    if( pszDatumName == NULL )
        pszDatumName = "unknown";

    if( pszSpheroidName == NULL )
        pszSpheroidName = "unnamed";

    if( pszAngularUnits == NULL )
    {
        pszAngularUnits = ME_UA_DEGREE;
        dfConvertToRadians = atof(ME_UA_DEGREE_CONV);
    }

/* -------------------------------------------------------------------- */
/*      Build the GEOGCS object.                                        */
/* -------------------------------------------------------------------- */
    char                szValue[128];
    MESRSNode         *poGeogCS, *poSpheroid, *poDatum, *poPM, *poUnits;

    poGeogCS = new MESRSNode( "GEOGCS" );
    poGeogCS->AddChild( new MESRSNode( pszGeogName ) );
    
/* -------------------------------------------------------------------- */
/*      Setup the spheroid.                                             */
/* -------------------------------------------------------------------- */
    poSpheroid = new MESRSNode( "SPHEROID" );
    poSpheroid->AddChild( new MESRSNode( pszSpheroidName ) );

    MEPrintDouble( szValue, dfSemiMajor );
    poSpheroid->AddChild( new MESRSNode(szValue) );

    MEPrintDouble( szValue, dfInvFlattening );
    poSpheroid->AddChild( new MESRSNode(szValue) );

/* -------------------------------------------------------------------- */
/*      Setup the Datum.                                                */
/* -------------------------------------------------------------------- */
    poDatum = new MESRSNode( "DATUM" );
    poDatum->AddChild( new MESRSNode(pszDatumName) );
    poDatum->AddChild( poSpheroid );

/* -------------------------------------------------------------------- */
/*      Setup the prime meridian.                                       */
/* -------------------------------------------------------------------- */
    if( dfPMOffset == 0.0 )
        strcpy( szValue, "0" );
    else
        MEPrintDouble( szValue, dfPMOffset );
    
    poPM = new MESRSNode( "PRIMEM" );
    poPM->AddChild( new MESRSNode( pszPMName ) );
    poPM->AddChild( new MESRSNode( szValue ) );

/* -------------------------------------------------------------------- */
/*      Setup the rotational units.                                     */
/* -------------------------------------------------------------------- */
    MEPrintDouble( szValue, dfConvertToRadians );
    
    poUnits = new MESRSNode( "UNIT" );
    poUnits->AddChild( new MESRSNode(pszAngularUnits) );
    poUnits->AddChild( new MESRSNode(szValue) );
    
/* -------------------------------------------------------------------- */
/*      Complete the GeogCS                                             */
/* -------------------------------------------------------------------- */
    poGeogCS->AddChild( poDatum );
    poGeogCS->AddChild( poPM );
    poGeogCS->AddChild( poUnits );

/* -------------------------------------------------------------------- */
/*      Attach below the PROJCS if there is one, or make this the root. */
/* -------------------------------------------------------------------- */
    if( GetRoot() != NULL && MEEQUAL(GetRoot()->GetValue(),"PROJCS") )
        poRoot->InsertChild( poGeogCS, 1 );
    else
        SetRoot( poGeogCS );

    return 0;
}

int MESpatialReference::CopyGeogCSFrom( 
    const MESpatialReference * poSrcSRS )

{
    const MESRSNode  *poGeogCS = NULL;

    bNormInfoSet = FALSE;

/* -------------------------------------------------------------------- */
/*      Do we already have a GEOGCS?  If so, blow it away so it can     */
/*      be properly replaced.                                           */
/* -------------------------------------------------------------------- */
    if( GetAttrNode( "GEOGCS" ) != NULL )
    {
        MESRSNode *poPROJCS;

        if( MEEQUAL(GetRoot()->GetValue(),"GEOGCS") )
            Clear();
        else if( (poPROJCS = GetAttrNode( "PROJCS" )) != NULL
                 && poPROJCS->FindChild( "GEOGCS" ) != -1 )
            poPROJCS->DestroyChild( poPROJCS->FindChild( "GEOGCS" ) );
        else
            return 6;
    }

/* -------------------------------------------------------------------- */
/*      Find the GEOGCS node on the source.                             */
/* -------------------------------------------------------------------- */
    poGeogCS = poSrcSRS->GetAttrNode( "GEOGCS" );
    if( poGeogCS == NULL )
        return 6;

/* -------------------------------------------------------------------- */
/*      Attach below the PROJCS if there is one, or make this the root. */
/* -------------------------------------------------------------------- */
    if( GetRoot() != NULL && MEEQUAL(GetRoot()->GetValue(),"PROJCS") )
        poRoot->InsertChild( poGeogCS->Clone(), 1 );
    else
        SetRoot( poGeogCS->Clone() );

    return 0;
}

double MESpatialReference::GetSemiMajor( int * pnErr ) const

{
    const MESRSNode *poSpheroid = GetAttrNode( "SPHEROID" );
    
    if( pnErr != NULL )
        *pnErr = 0;

    if( poSpheroid != NULL && poSpheroid->GetChildCount() >= 3 )
    {
        return atof( poSpheroid->GetChild(1)->GetValue() );
    }
    else
    {
        if( pnErr != NULL )
            *pnErr = 6;

        return ME_WGS84_SEMIMAJOR;
    }
}

double MESpatialReference::GetInvFlattening( int * pnErr ) const

{
    const MESRSNode *poSpheroid = GetAttrNode( "SPHEROID" );
    
    if( pnErr != NULL )
        *pnErr = 0;

    if( poSpheroid != NULL && poSpheroid->GetChildCount() >= 3 )
    {
        return atof( poSpheroid->GetChild(2)->GetValue() );
    }
    else
    {
        if( pnErr != NULL )
            *pnErr = 6;

        return ME_WGS84_INVFLATTENING;
    }
}

double MESpatialReference::GetSemiMinor( int * pnErr ) const

{
    double      dfInvFlattening, dfSemiMajor;

    dfSemiMajor = GetSemiMajor( pnErr );
    dfInvFlattening = GetInvFlattening( pnErr );

    if( ABS(dfInvFlattening) < 0.000000000001 )
        return dfSemiMajor;
    else
        return dfSemiMajor * (1.0 - 1.0/dfInvFlattening);
}

int MESpatialReference::SetProjection( const char * pszProjection )

{
    MESRSNode *poGeogCS = NULL;
    int eErr;

    if( poRoot != NULL && MEEQUAL(poRoot->GetValue(),"GEOGCS") )
    {
        poGeogCS = poRoot;
        poRoot = NULL;
    }

    if( !GetAttrNode( "PROJCS" ) )
    {
        SetNode( "PROJCS", "unnamed" );
    }

    eErr = SetNode( "PROJCS|PROJECTION", pszProjection );
    if( eErr != 0 )
        return eErr;

    if( poGeogCS != NULL )
        poRoot->InsertChild( poGeogCS, 1 );

    return 0;
}

int MESpatialReference::SetProjParm( const char * pszParmName,
                                         double dfValue )

{
    MESRSNode *poPROJCS = GetAttrNode( "PROJCS" );
    MESRSNode *poParm;
    char        szValue[64];

    if( poPROJCS == NULL )
        return 6;

    MEPrintDouble( szValue, dfValue );

/* -------------------------------------------------------------------- */
/*      Try to find existing parameter with this name.                  */
/* -------------------------------------------------------------------- */
    for( int iChild = 0; iChild < poPROJCS->GetChildCount(); iChild++ )
    {
        poParm = poPROJCS->GetChild( iChild );

        if( MEEQUAL(poParm->GetValue(),"PARAMETER")
            && poParm->GetChildCount() == 2 
            && MEEQUAL(poParm->GetChild(0)->GetValue(),pszParmName) )
        {
            poParm->GetChild(1)->SetValue( szValue );
            return 0;
        }
    }
    
/* -------------------------------------------------------------------- */
/*      Otherwise create a new parameter and append.                    */
/* -------------------------------------------------------------------- */
    poParm = new MESRSNode( "PARAMETER" );
    poParm->AddChild( new MESRSNode( pszParmName ) );
    poParm->AddChild( new MESRSNode( szValue ) );

    poPROJCS->AddChild( poParm );

    return 0;
}

double MESpatialReference::GetProjParm( const char * pszName,
                                         double dfDefaultValue,
                                         int *pnErr ) const

{
    const MESRSNode *poPROJCS = GetAttrNode( "PROJCS" );
    const MESRSNode *poParameter = NULL;

    if( pnErr != NULL )
        *pnErr = 0;
    
/* -------------------------------------------------------------------- */
/*      Search for requested parameter.                                 */
/* -------------------------------------------------------------------- */
    if( poPROJCS != NULL )
    {
        for( int iChild = 0; iChild < poPROJCS->GetChildCount(); iChild++ )
        {
            poParameter = poPROJCS->GetChild(iChild);
            
            if( MEEQUAL(poParameter->GetValue(),"PARAMETER")
                && poParameter->GetChildCount() == 2 
                && MEEQUAL(poPROJCS->GetChild(iChild)->GetChild(0)->GetValue(),
                         pszName) )
            {
                return atof(poParameter->GetChild(1)->GetValue());
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      Try similar names, for selected parameters.                     */
/* -------------------------------------------------------------------- */
    double      dfValue;
    int      nSubErr;
    
    if( MEEQUAL(pszName,ME_PP_LATITUDE_OF_ORIGIN) )
    {
        dfValue = GetProjParm(ME_PP_LATITUDE_OF_CENTER,0.0,&nSubErr);
        
        if( nSubErr == 0)
            return dfValue;
    }
    else if( MEEQUAL(pszName,ME_PP_CENTRAL_MERIDIAN) )
    {
        dfValue = GetProjParm(ME_PP_LONGITUDE_OF_CENTER,0.0,&nSubErr);
        if( nSubErr != 0 )
            dfValue = GetProjParm(ME_PP_LONGITUDE_OF_ORIGIN,0.0,&nSubErr);

        if( nSubErr == 0 )
            return dfValue;
    }
    
/* -------------------------------------------------------------------- */
/*      Return default value on failure.                                */
/* -------------------------------------------------------------------- */
    if( pnErr != NULL )
        *pnErr = 6;

    return dfDefaultValue;
}

double MESpatialReference::GetNormProjParm( const char * pszName,
                                             double dfDefaultValue,
                                             int *pnErr ) const

{
    double dfRawResult;
    int nError;

    if( pnErr == NULL )
        pnErr = &nError;

    GetNormInfo();

    dfRawResult = GetProjParm( pszName, dfDefaultValue, pnErr );

    // If we got the default just return it unadjusted.
    if( *pnErr != 0 )
        return dfRawResult;

    if( dfToDegrees != 1.0 && IsAngularParameter(pszName) )
        dfRawResult *= dfToDegrees;

    if( dfToMeter != 1.0 && IsLinearParameter( pszName ) )
        return dfRawResult * dfToMeter;
#ifdef WKT_LONGITUDE_RELATIVE_TO_PM
    else if( dfFromGreenwich != 0.0 && IsLongitudeParameter( pszName ) )
        return dfRawResult + dfFromGreenwich;
#endif
    else
        return dfRawResult;
}

int MESpatialReference::SetNormProjParm( const char * pszName,
                                             double dfValue )

{
    GetNormInfo();

    if( (dfToDegrees != 1.0 || dfFromGreenwich != 0.0) 
        && IsAngularParameter(pszName) )
    {
#ifdef WKT_LONGITUDE_RELATIVE_TO_PM
        if( dfFromGreenwich != 0.0 && IsLongitudeParameter( pszName ) )
            dfValue -= dfFromGreenwich;
#endif

        dfValue /= dfToDegrees;
    }
    else if( dfToMeter != 1.0 && IsLinearParameter( pszName ) )
        dfValue /= dfToMeter;

    return SetProjParm( pszName, dfValue );
}

int MESpatialReference::SetGS( double dfCentralMeridian,
                                   double dfFalseEasting,
                                   double dfFalseNorthing )

{
    SetProjection( ME_PT_GALL_STEREMEAPHIC );
    SetNormProjParm( ME_PP_CENTRAL_MERIDIAN, dfCentralMeridian );
    SetNormProjParm( ME_PP_FALSE_EASTING, dfFalseEasting );
    SetNormProjParm( ME_PP_FALSE_NORTHING, dfFalseNorthing );

    return 0;
}

int MESpatialReference::GetUTMZone( int * pbNorth ) const

{
    const char  *pszProjection = GetAttrValue( "PROJECTION" );

    if( pszProjection == NULL
        || !MEEQUAL(pszProjection,ME_PT_TRANSVERSE_MERCATOR) )
        return 0;

    if( GetNormProjParm( ME_PP_LATITUDE_OF_ORIGIN, 0.0 ) != 0.0 )
        return 0;

    if( GetProjParm( ME_PP_SCALE_FACTOR, 1.0 ) != 0.9996 )
        return 0;
          
    if( fabs(GetNormProjParm( ME_PP_FALSE_EASTING, 0.0 )-500000.0) > 0.001 )
        return 0;

    double      dfFalseNorthing = GetNormProjParm( ME_PP_FALSE_NORTHING, 0.0);

    if( dfFalseNorthing != 0.0 
        && fabs(dfFalseNorthing-10000000.0) > 0.001 )
        return 0;

    if( pbNorth != NULL )
        *pbNorth = (dfFalseNorthing == 0);

    double      dfCentralMeridian = GetNormProjParm( ME_PP_CENTRAL_MERIDIAN, 
                                                     0.0);
    double      dfZone = (dfCentralMeridian+183) / 6.0 + 0.000000001;

    if( ABS(dfZone - (int) dfZone) > 0.00001
        || dfCentralMeridian < -177.00001
        || dfCentralMeridian > 177.000001 )
        return 0;
    else
        return (int) dfZone;
}

int MESpatialReference::SetAuthority( const char *pszTargetKey,
                                          const char * pszAuthority, 
                                          int nCode )
{
/* -------------------------------------------------------------------- */
/*      Find the node below which the authority should be put.          */
/* -------------------------------------------------------------------- */
    MESRSNode  *poNode = GetAttrNode( pszTargetKey );

    if( poNode == NULL )
        return 6;

/* -------------------------------------------------------------------- */
/*      If there is an existing AUTHORITY child blow it away before     */
/*      trying to set a new one.                                        */
/* -------------------------------------------------------------------- */
    int iOldChild = poNode->FindChild( "AUTHORITY" );
    if( iOldChild != -1 )
        poNode->DestroyChild( iOldChild );

/* -------------------------------------------------------------------- */
/*      Create a new authority node.                                    */
/* -------------------------------------------------------------------- */
    char   szCode[32];
    MESRSNode *poAuthNode;

    sprintf( szCode, "%d", nCode );

    poAuthNode = new MESRSNode( "AUTHORITY" );
    poAuthNode->AddChild( new MESRSNode( pszAuthority ) );
    poAuthNode->AddChild( new MESRSNode( szCode ) );
    
    poNode->AddChild( poAuthNode );

    return 0;
}

const char *
MESpatialReference::GetAuthorityCode( const char *pszTargetKey ) const

{
/* -------------------------------------------------------------------- */
/*      Find the node below which the authority should be put.          */
/* -------------------------------------------------------------------- */
    MESRSNode  *poNode = 
        ((MESpatialReference *) this)->GetAttrNode( pszTargetKey );

    if( poNode == NULL )
        return NULL;

/* -------------------------------------------------------------------- */
/*      Fetch AUTHORITY child if there is one.                          */
/* -------------------------------------------------------------------- */
    if( poNode->FindChild("AUTHORITY") == -1 )
        return NULL;
        
    poNode = poNode->GetChild(poNode->FindChild("AUTHORITY"));

/* -------------------------------------------------------------------- */
/*      Create a new authority node.                                    */
/* -------------------------------------------------------------------- */
    if( poNode->GetChildCount() < 2 )
        return NULL;

    return poNode->GetChild(1)->GetValue();
}

const char *
MESpatialReference::GetAuthorityName( const char *pszTargetKey ) const

{
/* -------------------------------------------------------------------- */
/*      Find the node below which the authority should be put.          */
/* -------------------------------------------------------------------- */
    MESRSNode  *poNode = 
        ((MESpatialReference *) this)->GetAttrNode( pszTargetKey );

    if( poNode == NULL )
        return NULL;

/* -------------------------------------------------------------------- */
/*      Fetch AUTHORITY child if there is one.                          */
/* -------------------------------------------------------------------- */
    if( poNode->FindChild("AUTHORITY") == -1 )
        return NULL;
        
    poNode = poNode->GetChild(poNode->FindChild("AUTHORITY"));

/* -------------------------------------------------------------------- */
/*      Create a new authority node.                                    */
/* -------------------------------------------------------------------- */
    if( poNode->GetChildCount() < 2 )
        return NULL;

    return poNode->GetChild(0)->GetValue();
}

int MESpatialReference::IsProjected() const

{
    if( poRoot == NULL )
        return FALSE;

    // If we eventually support composite coordinate systems this will
    // need to improve. 

    return MEEQUAL(poRoot->GetValue(),"PROJCS");
}

int MESpatialReference::IsGeographic() const

{
    if( GetRoot() == NULL )
        return FALSE;

    return MEEQUAL(GetRoot()->GetValue(),"GEOGCS");
}

int MESpatialReference::IsLocal() const

{
    if( GetRoot() == NULL )
        return FALSE;

    return MEEQUAL(GetRoot()->GetValue(),"LOCAL_CS");
}

int MESpatialReference::SetTOWGS84( double dfDX, double dfDY, double dfDZ,
                                        double dfEX, double dfEY, double dfEZ, 
                                        double dfPPM )

{
    MESRSNode     *poDatum, *poTOWGS84;
    int             iPosition;
    char            szValue[64];

    poDatum = GetAttrNode( "DATUM" );
    if( poDatum == NULL )
        return 6;
    
    if( poDatum->FindChild( "TOWGS84" ) != -1 )
        poDatum->DestroyChild( poDatum->FindChild( "TOWGS84" ) );

    iPosition = poDatum->GetChildCount();
    if( poDatum->FindChild("AUTHORITY") != -1 )
    {
        iPosition = poDatum->FindChild("AUTHORITY");
    }

    poTOWGS84 = new MESRSNode("TOWGS84");

    MEPrintDouble( szValue, dfDX );
    poTOWGS84->AddChild( new MESRSNode( szValue ) );

    MEPrintDouble( szValue, dfDY );
    poTOWGS84->AddChild( new MESRSNode( szValue ) );

    MEPrintDouble( szValue, dfDZ );
    poTOWGS84->AddChild( new MESRSNode( szValue ) );

    MEPrintDouble( szValue, dfEX );
    poTOWGS84->AddChild( new MESRSNode( szValue ) );

    MEPrintDouble( szValue, dfEY );
    poTOWGS84->AddChild( new MESRSNode( szValue ) );

    MEPrintDouble( szValue, dfEZ );
    poTOWGS84->AddChild( new MESRSNode( szValue ) );

    MEPrintDouble( szValue, dfPPM );
    poTOWGS84->AddChild( new MESRSNode( szValue ) );

    poDatum->InsertChild( poTOWGS84, iPosition );

    return 0;
}

int MESpatialReference::IsAngularParameter( const char *pszParameterName )

{
    if( MEEQUALN(pszParameterName,"long",4)
        || MEEQUALN(pszParameterName,"lati",4)
        || MEEQUAL(pszParameterName,ME_PP_CENTRAL_MERIDIAN)
        || MEEQUALN(pszParameterName,"standard_parallel",17)
        || MEEQUAL(pszParameterName,ME_PP_AZIMUTH)
        || MEEQUAL(pszParameterName,ME_PP_RECTIFIED_GRID_ANGLE) )
        return TRUE;
    else
        return FALSE;
}

int MESpatialReference::IsLongitudeParameter( const char *pszParameterName )

{
    if( MEEQUALN(pszParameterName,"long",4)
        || MEEQUAL(pszParameterName,ME_PP_CENTRAL_MERIDIAN) )
        return TRUE;
    else
        return FALSE;
}

int MESpatialReference::IsLinearParameter( const char *pszParameterName )

{
    if( MEEQUALN(pszParameterName,"false_",6) 
        || MEEQUAL(pszParameterName,ME_PP_SATELLITE_HEIGHT) )
        return TRUE;
    else
        return FALSE;
}

void MESpatialReference::GetNormInfo(void) const

{
    if( bNormInfoSet )
        return;

/* -------------------------------------------------------------------- */
/*      Initialize values.                                              */
/* -------------------------------------------------------------------- */
    MESpatialReference *poThis = (MESpatialReference *) this;

    poThis->bNormInfoSet = TRUE;

    poThis->dfFromGreenwich = GetPrimeMeridian(NULL);
    poThis->dfToMeter = GetLinearUnits(NULL);
    poThis->dfToDegrees = GetAngularUnits(NULL) / atof(ME_UA_DEGREE_CONV);
    if( fabs(poThis->dfToDegrees-1.0) < 0.000000001 )
        poThis->dfToDegrees = 1.0;
}


