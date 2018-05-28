
#include "spatialref.h"
#include "proj_api.h"

/* ==================================================================== */
/*      PROJ.4 interface stuff.                                         */
/* ==================================================================== */
#define RAD_TO_DEG      57.29577951308232
#define DEG_TO_RAD      .0174532925199432958

static projPJ       (*pfn_pj_init_plus)(const char *) = NULL;
static projPJ       (*pfn_pj_init)(int, char**) = NULL;
static projUV       (*pfn_pj_fwd)(projUV, projPJ) = NULL;
static projUV       (*pfn_pj_inv)(projUV, projPJ) = NULL;
static void     (*pfn_pj_free)(projPJ) = NULL;
static int      (*pfn_pj_transform)(projPJ, projPJ, long, int, 
                                    double *, double *, double * ) = NULL;
static int         *(*pfn_pj_get_errno_ref)(void) = NULL;
static char        *(*pfn_pj_strerrno)(int) = NULL;
static char        *(*pfn_pj_get_def)(projPJ,int) = NULL;
static void         (*pfn_pj_dalloc)(void *) = NULL;

/************************************************************************/
/*                              MEProj4CT                              */
/************************************************************************/

class MEProj4CT : public MECoordinateTransformation
{
    MESpatialReference *poSRSSource;
    void        *psPJSource;
    int         bSourceLatLong;
    double      dfSourceToRadians;
    double      dfSourceFromRadians;
    

    MESpatialReference *poSRSTarget;
    void        *psPJTarget;
    int         bTargetLatLong;
    double      dfTargetToRadians;
    double      dfTargetFromRadians;

    int         nErrorCount;

public:
                MEProj4CT();
    virtual     ~MEProj4CT();

    int         Initialize( MESpatialReference *poSource, 
                            MESpatialReference *poTarget );
    virtual MESpatialReference *GetSourceCS();
    virtual MESpatialReference *GetTargetCS();
    virtual int Transform( int nCount, 
                           double *x, double *y, double *z = NULL );
    virtual int TransformEx( int nCount, 
                             double *x, double *y, double *z = NULL,
                             int *panSuccess = NULL );

};

/************************************************************************/
/*                          LoadProjLibrary()                           */
/************************************************************************/

static int LoadProjLibrary()

{
    static int  bTriedToLoad = FALSE;
    
    if( bTriedToLoad )
        return( pfn_pj_init != NULL );

    bTriedToLoad = TRUE;

    pfn_pj_init = pj_init;
    pfn_pj_init_plus = pj_init_plus;
    pfn_pj_fwd = pj_fwd;
    pfn_pj_inv = pj_inv;
    pfn_pj_free = pj_free;
    pfn_pj_transform = pj_transform;
    pfn_pj_get_errno_ref = (int *(*)(void)) pj_get_errno_ref;
    pfn_pj_strerrno = pj_strerrno;
    pfn_pj_dalloc = pj_dalloc;
#if PJ_VERSION >= 446
    pfn_pj_get_def = pj_get_def;
#endif

    if( pfn_pj_transform == NULL )
    {
        printf("Attempt to load, but couldn't find pj_transform.\n");

        return FALSE;
    }

    return( TRUE );
}
/************************************************************************/
/*                 MECreateCoordinateTransformation()                  */
/************************************************************************/

/**
 * Create transformation object.
 *
 * This is the same as the C function OCTNewCoordinateTransformation().
 *
 * The delete operator, or OCTDestroyCoordinateTransformation() should
 * be used to destroy transformation objects. 
 *
 * @param poSource source spatial reference system. 
 * @param poTarget target spatial reference system. 
 * @return NULL on failure or a ready to use transformation object.
 */

MECoordinateTransformation*  
MECreateCoordinateTransformation( MESpatialReference *poSource, 
                                   MESpatialReference *poTarget )

{
    MEProj4CT  *poCT;

    if( !LoadProjLibrary() )
    {
        printf("Unable to load PROJ.4 library, creation of\n");
        return NULL;
    }

    poCT = new MEProj4CT();
    
    if( !poCT->Initialize( poSource, poTarget ) )
    {
        delete poCT;
        return NULL;
    }
    else
    {
        return poCT;
    }
}

/************************************************************************/
/*                             MEProj4CT()                             */
/************************************************************************/

MEProj4CT::MEProj4CT()

{
    poSRSSource = NULL;
    poSRSTarget = NULL;
    psPJSource = NULL;
    psPJTarget = NULL;

    nErrorCount = 0;
}

/************************************************************************/
/*                            ~MEProj4CT()                             */
/************************************************************************/

MEProj4CT::~MEProj4CT()

{
    if( poSRSSource != NULL )
    {
        if( poSRSSource->Dereference() <= 0 )
            delete poSRSSource;
    }

    if( poSRSTarget != NULL )
    {
        if( poSRSTarget->Dereference() <= 0 )
            delete poSRSTarget;
    }

    if( psPJSource != NULL )
        pfn_pj_free( psPJSource );

    if( psPJTarget != NULL )
        pfn_pj_free( psPJTarget );
}

/************************************************************************/
/*                             Initialize()                             */
/************************************************************************/

int MEProj4CT::Initialize( MESpatialReference * poSourceIn, 
                            MESpatialReference * poTargetIn )

{
    if( poSourceIn == NULL || poTargetIn == NULL )
        return FALSE;

    poSRSSource = poSourceIn->Clone();
    poSRSTarget = poTargetIn->Clone();

    bSourceLatLong = poSRSSource->IsGeographic();
    bTargetLatLong = poSRSTarget->IsGeographic();

/* -------------------------------------------------------------------- */
/*      Setup source and target translations to radians for lat/long    */
/*      systems.                                                        */
/* -------------------------------------------------------------------- */
    dfSourceToRadians = DEG_TO_RAD;
    dfSourceFromRadians = RAD_TO_DEG;

    if( bSourceLatLong )
    {
        MESRSNode *poUNITS = poSRSSource->GetAttrNode( "GEOGCS|UNIT" );
        if( poUNITS && poUNITS->GetChildCount() >= 2 )
        {
            dfSourceToRadians = atof(poUNITS->GetChild(1)->GetValue());
            if( dfSourceToRadians == 0.0 )
                dfSourceToRadians = DEG_TO_RAD;
            else
                dfSourceFromRadians = 1 / dfSourceToRadians;
        }
    }

    dfTargetToRadians = DEG_TO_RAD;
    dfTargetFromRadians = RAD_TO_DEG;

    if( bTargetLatLong )
    {
        MESRSNode *poUNITS = poSRSTarget->GetAttrNode( "GEOGCS|UNIT" );
        if( poUNITS && poUNITS->GetChildCount() >= 2 )
        {
            dfTargetToRadians = atof(poUNITS->GetChild(1)->GetValue());
            if( dfTargetToRadians == 0.0 )
                dfTargetToRadians = DEG_TO_RAD;
            else
                dfTargetFromRadians = 1 / dfTargetToRadians;
        }
    }


/* -------------------------------------------------------------------- */
/*      Establish PROJ.4 handle for source if projection.               */
/* -------------------------------------------------------------------- */
    char        *pszProj4Defn;

    if( poSRSSource->exportToProj4( &pszProj4Defn ) != 0)
        return FALSE;

    if( strlen(pszProj4Defn) == 0 )
    {
        printf("No PROJ.4 translation for source SRS, coordinate\n");
        return FALSE;
    }

    psPJSource = pfn_pj_init_plus( pszProj4Defn );
    
    if( psPJSource == NULL )
    {
        if( pfn_pj_get_errno_ref != NULL
            && pfn_pj_strerrno != NULL )
        {
            int *p_pj_errno = pfn_pj_get_errno_ref();

            printf("Failed to initialize PROJ.4. %d\n", *p_pj_errno);
        }
        else
        {
            printf("Failed to initialize PROJ.4.\n");
        }
    }
    
    if ( pszProj4Defn != NULL)
        free (pszProj4Defn);
    
    if( psPJSource == NULL )
        return FALSE;

/* -------------------------------------------------------------------- */
/*      Establish PROJ.4 handle for target if projection.               */
/* -------------------------------------------------------------------- */
    if( poSRSTarget->exportToProj4( &pszProj4Defn ) != 0 )
        return FALSE;

    if( strlen(pszProj4Defn) == 0 )
    {
        printf("No PROJ.4 translation for destination SRS, coordinate\n");
        return FALSE;
    }

    psPJTarget = pfn_pj_init_plus( pszProj4Defn );
    
    if( psPJTarget == NULL )
        printf("Failed to initialize PROJ.4 with.\n");
    
    if ( pszProj4Defn != NULL)
        free ( pszProj4Defn);
    
    if( psPJTarget == NULL )
        return FALSE;

    return TRUE;
}
/************************************************************************/
/*                            GetSourceCS()                             */
/************************************************************************/

MESpatialReference *MEProj4CT::GetSourceCS()

{
    return poSRSSource;
}

/************************************************************************/
/*                            GetTargetCS()                             */
/************************************************************************/

MESpatialReference *MEProj4CT::GetTargetCS()

{
    return poSRSTarget;
}
/************************************************************************/
/*                             Transform()                              */
/*                                                                      */
/*      This is a small wrapper for the extended transform version.     */
/************************************************************************/

int MEProj4CT::Transform( int nCount, double *x, double *y, double *z )

{
    int *pabSuccess = (int *) malloc(sizeof(int) * nCount );
    int bOverallSuccess, i;

    bOverallSuccess = TransformEx( nCount, x, y, z, pabSuccess );

    for( i = 0; i < nCount; i++ )
    {
        if( !pabSuccess[i] )
        {
            bOverallSuccess = FALSE;
            break;
        }
    }

    if ( pabSuccess != NULL)
        free (pabSuccess);

    return bOverallSuccess;
}
/************************************************************************/
/*                            TransformEx()                             */
/************************************************************************/

int MEProj4CT::TransformEx( int nCount, double *x, double *y, double *z,
                             int *pabSuccess )

{
    int   err, i;

/* -------------------------------------------------------------------- */
/*      Potentially transform to radians.                               */
/* -------------------------------------------------------------------- */
    if( bSourceLatLong )
    {
        for( i = 0; i < nCount; i++ )
        {
            x[i] *= dfSourceToRadians;
            y[i] *= dfSourceToRadians;
        }
    }

/* -------------------------------------------------------------------- */
/*      Do the transformation using PROJ.4.                             */
/* -------------------------------------------------------------------- */
    err = pfn_pj_transform( psPJSource, psPJTarget, nCount, 1, x, y, z );

/* -------------------------------------------------------------------- */
/*      Try to report an error through ME.  Get proj.4 error string    */
/*      if possible.  Try to avoid reporting thousands of error         */
/*      ... supress further error reporting on this MEProj4CT if we    */
/*      have already reported 20 errors.                                */
/* -------------------------------------------------------------------- */
    if( err != 0 )
    {
        if( pabSuccess )
            memset( pabSuccess, 0, sizeof(int) * nCount );

        if( ++nErrorCount < 20 )
        {
            const char *pszError = NULL;
            if( pfn_pj_strerrno != NULL )
                pszError = pfn_pj_strerrno( err );
            
            if( pszError == NULL )
                printf("Reprojection failed");
        }
        else if( nErrorCount == 20 )
        {
            printf("Reprojection failed, further errors will be supressed on the transform object.");
        }

        return FALSE;
    }

/* -------------------------------------------------------------------- */
/*      Potentially transform back to degrees.                          */
/* -------------------------------------------------------------------- */
    if( bTargetLatLong )
    {
        for( i = 0; i < nCount; i++ )
        {
            if( x[i] != HUGE_VAL && y[i] != HUGE_VAL )
            {
                x[i] *= dfTargetFromRadians;
                y[i] *= dfTargetFromRadians;
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      Establish error information if pabSuccess provided.             */
/* -------------------------------------------------------------------- */
    if( pabSuccess )
    {
        for( i = 0; i < nCount; i++ )
        {
            if( x[i] == HUGE_VAL || y[i] == HUGE_VAL )
                pabSuccess[i] = FALSE;
            else
                pabSuccess[i] = TRUE;
        }
    }

    return TRUE;
}

