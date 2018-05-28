
/*
** $Id: spatialnode.cpp,v 1.3 2006-11-28 01:03:22 jpzhang Exp $
**
** Spatialref class implement.
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
#include "spatialref.h"

MESRSNode::MESRSNode( const char * pszValueIn )
{
    if (pszValueIn)
        pszValue = strdup(pszValueIn);
    else
        pszValue = strdup("");

    nChildren = 0;
    papoChildNodes = NULL;

    poParent = NULL;
}

MESRSNode::MESRSNode(const OGR_SRSNode * ogrnode)
{
    papoChildNodes = NULL;
    poParent = NULL;
	pszValue = strdup(ogrnode->GetValue());
	nChildren = ogrnode->GetChildCount();

	papoChildNodes = (MESRSNode**)malloc(sizeof(void*) * nChildren);

	for (int i = 0; i < nChildren; i++)
	{
		papoChildNodes[i] = new MESRSNode(ogrnode->GetChild(i)); 
	}

}

MESRSNode::~MESRSNode()
{
    if( pszValue != NULL)
        free (pszValue);

    ClearChildren();
}

void MESRSNode::ClearChildren()
{
    for( int i = 0; i < nChildren; i++ )
    {
        delete papoChildNodes[i];
    }

    if ( papoChildNodes != NULL)
        free(papoChildNodes);

    papoChildNodes = NULL;
    nChildren = 0;
}

MESRSNode *MESRSNode::GetChild( int iChild )
{
    if( iChild < 0 || iChild >= nChildren )
        return NULL;
    else
        return papoChildNodes[iChild];
}

const MESRSNode *MESRSNode::GetChild( int iChild ) const
{
    if( iChild < 0 || iChild >= nChildren )
        return NULL;
    else
        return papoChildNodes[iChild];
}

MESRSNode *MESRSNode::GetNode( const char * pszName )

{
    int  i;

    if( this == NULL )
        return NULL;
    
    if( nChildren > 0 && MEEQUAL(pszName,pszValue) )
        return this;

/* -------------------------------------------------------------------- */
/*      First we check the immediate children so we will get an         */
/*      immediate child in preference to a subchild.                    */
/* -------------------------------------------------------------------- */
    for( i = 0; i < nChildren; i++ )
    {
        if( MEEQUAL(papoChildNodes[i]->pszValue,pszName) 
            && papoChildNodes[i]->nChildren > 0 )
            return papoChildNodes[i];
    }

/* -------------------------------------------------------------------- */
/*      Then get each child to check their children.                    */
/* -------------------------------------------------------------------- */
    for( i = 0; i < nChildren; i++ )
    {
        MESRSNode *poNode;

        poNode = papoChildNodes[i]->GetNode( pszName );
        if( poNode != NULL )
            return poNode;
    }

    return NULL;
}

const MESRSNode *MESRSNode::GetNode( const char * pszName ) const

{
    return ((MESRSNode *) this)->GetNode( pszName );
}

void MESRSNode::AddChild( MESRSNode * poNew )
{
    InsertChild( poNew, nChildren );
}

void MESRSNode::InsertChild( MESRSNode * poNew, int iChild )

{
    if( iChild > nChildren )
        iChild = nChildren;

    nChildren++;
    papoChildNodes = (MESRSNode **)
        realloc( papoChildNodes, sizeof(void*) * nChildren );

    memmove( papoChildNodes + iChild + 1, papoChildNodes + iChild,
             sizeof(void*) * (nChildren - iChild - 1) );
    
    papoChildNodes[iChild] = poNew;
    poNew->poParent = this;
}

void MESRSNode::DestroyChild( int iChild )

{
    if( iChild < 0 || iChild >= nChildren )
        return;

    delete papoChildNodes[iChild];
    while( iChild < nChildren-1 )
    {
        papoChildNodes[iChild] = papoChildNodes[iChild+1];
        iChild++;
    }

    nChildren--;
}

int MESRSNode::FindChild( const char * pszValue ) const

{
    for( int i = 0; i < nChildren; i++ )
    {
        if( MEEQUAL(papoChildNodes[i]->pszValue,pszValue) )
            return i;
    }

    return -1;
}

void MESRSNode::SetValue( const char * pszNewValue )

{
    if ( pszValue != NULL)
        free(pszValue);

    if (pszNewValue)
        pszValue = strdup(pszNewValue);
    else
        pszValue = strdup("");
}

MESRSNode *MESRSNode::Clone() const

{
    MESRSNode *poNew;

    poNew = new MESRSNode( pszValue );

    for( int i = 0; i < nChildren; i++ )
    {
        poNew->AddChild( papoChildNodes[i]->Clone() );
    }

    return poNew;
}

int MESRSNode::NeedsQuoting() const

{
    // non-terminals are never quoted.
    if( GetChildCount() != 0 )
        return FALSE;

    // As per bugzilla bug 201, the OGC spec says the authority code
    // needs to be quoted even though it appears well behaved.
    if( poParent != NULL && MEEQUAL(poParent->GetValue(),"AUTHORITY") )
        return TRUE;
    
    // As per bugzilla bug 294, the OGC spec says the direction
    // values for the AXIS keywords should *not* be quoted.
    if( poParent != NULL && MEEQUAL(poParent->GetValue(),"AXIS") 
        && this != poParent->GetChild(0) )
        return FALSE;

    // Non-numeric tokens are generally quoted while clean numeric values
    // are generally not. 
    for( int i = 0; pszValue[i] != '\0'; i++ )
    {
        if( (pszValue[i] < '0' || pszValue[i] > '9')
            && pszValue[i] != '.'
            && pszValue[i] != '-' && pszValue[i] != '+'
            && pszValue[i] != 'e' && pszValue[i] != 'E' )
            return TRUE;
    }

    return FALSE;
}

int MESRSNode::exportToWkt( char ** ppszResult ) const

{
    char        **papszChildrenWkt = NULL;
    int         nLength = strlen(pszValue)+4;
    int         i;

/* -------------------------------------------------------------------- */
/*      Build a list of the WKT format for the children.                */
/* -------------------------------------------------------------------- */
    papszChildrenWkt = (char **) calloc(sizeof(char*),(nChildren+1));
    
    for( i = 0; i < nChildren; i++ )
    {
        papoChildNodes[i]->exportToWkt( papszChildrenWkt + i );
        nLength += strlen(papszChildrenWkt[i]) + 1;
    }

/* -------------------------------------------------------------------- */
/*      Allocate the result string.                                     */
/* -------------------------------------------------------------------- */
    *ppszResult = (char *) malloc(nLength);
    *ppszResult[0] = '\0';
    
/* -------------------------------------------------------------------- */
/*      Capture this nodes value.  We put it in double quotes if        */
/*      this is a leaf node, otherwise we assume it is a well formed    */
/*      node name.                                                      */
/* -------------------------------------------------------------------- */
    if( NeedsQuoting() )
    {
        strcat( *ppszResult, "\"" );
        strcat( *ppszResult, pszValue ); /* should we do quoting? */
        strcat( *ppszResult, "\"" );
    }
    else
        strcat( *ppszResult, pszValue );

/* -------------------------------------------------------------------- */
/*      Add the children strings with appropriate brackets and commas.  */
/* -------------------------------------------------------------------- */
    if( nChildren > 0 )
        strcat( *ppszResult, "[" );
    
    for( i = 0; i < nChildren; i++ )
    {
        strcat( *ppszResult, papszChildrenWkt[i] );
        if( i == nChildren-1 )
            strcat( *ppszResult, "]" );
        else
            strcat( *ppszResult, "," );
    }

    MEDestroy( papszChildrenWkt );

    return 0;
}

int MESRSNode::exportToPrettyWkt( char ** ppszResult, int nDepth ) const

{
    char        **papszChildrenWkt = NULL;
    int         nLength = strlen(pszValue)+4;
    int         i;

/* -------------------------------------------------------------------- */
/*      Build a list of the WKT format for the children.                */
/* -------------------------------------------------------------------- */
    papszChildrenWkt = (char **) calloc(sizeof(char*),(nChildren+1));
    
    for( i = 0; i < nChildren; i++ )
    {
        papoChildNodes[i]->exportToPrettyWkt( papszChildrenWkt + i,
                                              nDepth + 1);
        nLength += strlen(papszChildrenWkt[i]) + 2 + nDepth*4;
    }

/* -------------------------------------------------------------------- */
/*      Allocate the result string.                                     */
/* -------------------------------------------------------------------- */
    *ppszResult = (char *) malloc(nLength);
    *ppszResult[0] = '\0';
    
/* -------------------------------------------------------------------- */
/*      Capture this nodes value.  We put it in double quotes if        */
/*      this is a leaf node, otherwise we assume it is a well formed    */
/*      node name.                                                      */
/* -------------------------------------------------------------------- */
    if( NeedsQuoting() )
    {
        strcat( *ppszResult, "\"" );
        strcat( *ppszResult, pszValue ); /* should we do quoting? */
        strcat( *ppszResult, "\"" );
    }
    else
        strcat( *ppszResult, pszValue );

/* -------------------------------------------------------------------- */
/*      Add the children strings with appropriate brackets and commas.  */
/* -------------------------------------------------------------------- */
    if( nChildren > 0 )
        strcat( *ppszResult, "[" );
    
    for( i = 0; i < nChildren; i++ )
    {
        if( papoChildNodes[i]->GetChildCount() > 0 )
        {
            int  j;

            strcat( *ppszResult, "\n" );
            for( j = 0; j < 4*nDepth; j++ )
                strcat( *ppszResult, " " );
        }
        strcat( *ppszResult, papszChildrenWkt[i] );
        if( i < nChildren-1 )
            strcat( *ppszResult, "," );
    }

    if( nChildren > 0 )
    {
        if( (*ppszResult)[strlen(*ppszResult)-1] == ',' )
            (*ppszResult)[strlen(*ppszResult)-1] = '\0';
        
        strcat( *ppszResult, "]" );
    }

    MEDestroy( papszChildrenWkt );

    return 0;
}

int MESRSNode::importFromWkt( char ** ppszInput )

{
    const char  *pszInput = *ppszInput;
    int         bInQuotedString = FALSE;
    
/* -------------------------------------------------------------------- */
/*      Clear any existing children of this node.                       */
/* -------------------------------------------------------------------- */
    ClearChildren();
    
/* -------------------------------------------------------------------- */
/*      Read the ``value'' for this node.                               */
/* -------------------------------------------------------------------- */
    char        szToken[512];
    int         nTokenLen = 0;
    
    while( *pszInput != '\0' && nTokenLen < (int) sizeof(szToken)-1 )
    {
        if( *pszInput == '"' )
        {
            bInQuotedString = !bInQuotedString;
        }
        else if( !bInQuotedString
              && (*pszInput == '[' || *pszInput == ']' || *pszInput == ','
                  || *pszInput == '(' || *pszInput == ')' ) )
        {
            break;
        }
        else if( !bInQuotedString 
                 && (*pszInput == ' ' || *pszInput == '\t' 
                     || *pszInput == 10 || *pszInput == 13) )
        {
            /* just skip over whitespace */
        } 
        else
        {
            szToken[nTokenLen++] = *pszInput;
        }

        pszInput++;
    }

    if( *pszInput == '\0' || nTokenLen == sizeof(szToken) - 1 )
        return 5;

    szToken[nTokenLen++] = '\0';
    SetValue( szToken );

/* -------------------------------------------------------------------- */
/*      Read children, if we have a sublist.                            */
/* -------------------------------------------------------------------- */
    if( *pszInput == '[' || *pszInput == '(' )
    {
        do
        {
            MESRSNode *poNewChild;
            int      eErr;

            pszInput++; // Skip bracket or comma.

            poNewChild = new MESRSNode();

            eErr = poNewChild->importFromWkt( (char **) &pszInput );
            if( eErr != 0 )
                return eErr;

            AddChild( poNewChild );
            
        } while( *pszInput == ',' );

        if( *pszInput != ')' && *pszInput != ']' )
            return 5;

        pszInput++;
    }

    *ppszInput = (char *) pszInput;

    return 0;
}

void MESRSNode::MakeValueSafe()

{
    int         i, j;

/* -------------------------------------------------------------------- */
/*      First process subnodes.                                         */
/* -------------------------------------------------------------------- */
    for( int iChild = 0; iChild < GetChildCount(); iChild++ )
    {
        GetChild(iChild)->MakeValueSafe();
    }

/* -------------------------------------------------------------------- */
/*      Skip numeric nodes.                                             */
/* -------------------------------------------------------------------- */
    if( (pszValue[0] >= '0' && pszValue[0] <= '9') || pszValue[0] != '.' )
        return;
    
/* -------------------------------------------------------------------- */
/*      Translate non-alphanumeric values to underscores.               */
/* -------------------------------------------------------------------- */
    for( i = 0; pszValue[i] != '\0'; i++ )
    {
        if( !(pszValue[i] >= 'A' && pszValue[i] <= 'Z')
            && !(pszValue[i] >= 'a' && pszValue[i] <= 'z')
            && !(pszValue[i] >= '0' && pszValue[i] <= '9') )
        {
            pszValue[i] = '_';
        }
    }

/* -------------------------------------------------------------------- */
/*      Remove repeated and trailing underscores.                       */
/* -------------------------------------------------------------------- */
    for( i = 1, j = 0; pszValue[i] != '\0'; i++ )
    {
        if( pszValue[j] == '_' && pszValue[i] == '_' )
            continue;

        pszValue[++j] = pszValue[i];
    }
    
    if( pszValue[j] == '_' )
        pszValue[j] = '\0';
    else
        pszValue[j+1] = '\0';
}

int MESRSNode::applyRemapper( const char *pszNode, 
                                   char **papszSrcValues, 
                                   char **papszDstValues, 
                                   int nStepSize, int bChildOfHit )

{
    int i;

/* -------------------------------------------------------------------- */
/*      Scan for value, and replace if our parent was a "hit".          */
/* -------------------------------------------------------------------- */
    if( bChildOfHit || pszNode == NULL )
    {
        for( i = 0; papszSrcValues[i] != NULL; i += nStepSize )
        {
            if( MEEQUAL(papszSrcValues[i],pszValue) )
            {
                SetValue( papszDstValues[i] );
                break;
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      Are the the target node?                                        */
/* -------------------------------------------------------------------- */
    if( pszNode != NULL )
        bChildOfHit = MEEQUAL(pszValue,pszNode);

/* -------------------------------------------------------------------- */
/*      Recurse                                                         */
/* -------------------------------------------------------------------- */
    for( i = 0; i < GetChildCount(); i++ )
    {
        GetChild(i)->applyRemapper( pszNode, papszSrcValues, 
                                    papszDstValues, nStepSize, bChildOfHit );
    }

    return 0;
}
                                   
void MESRSNode::StripNodes( const char * pszName )

{
/* -------------------------------------------------------------------- */
/*      Strip any children matching this name.                          */
/* -------------------------------------------------------------------- */
    while( FindChild( pszName ) >= 0 )
        DestroyChild( FindChild( pszName ) );

/* -------------------------------------------------------------------- */
/*      Recurse                                                         */
/* -------------------------------------------------------------------- */
    for( int i = 0; i < GetChildCount(); i++ )
        GetChild(i)->StripNodes( pszName );
}

static char *apszPROJCSRule[] = 
{ "PROJCS", "GEOGCS", "PROJECTION", "PARAMETER", "UNIT", "AXIS", "AUTHORITY", 
  NULL };

static char *apszDATUMRule[] = 
{ "DATUM", "SPHEROID", "TOWGS84", "AUTHORITY", NULL };

static char *apszGEOGCSRule[] = 
{ "GEOGCS", "DATUM", "PRIMEM", "UNIT", "AXIS", "AUTHORITY", NULL };

static char **apszOrderingRules[] = {
    apszPROJCSRule, apszGEOGCSRule, apszDATUMRule, NULL };

int MESRSNode::FixupOrdering()

{
    int    i;

/* -------------------------------------------------------------------- */
/*      Recurse ordering children.                                      */
/* -------------------------------------------------------------------- */
    for( i = 0; i < GetChildCount(); i++ )
        GetChild(i)->FixupOrdering();

    if( GetChildCount() < 3 )
        return 0;

/* -------------------------------------------------------------------- */
/*      Is this a node for which an ordering rule exists?               */
/* -------------------------------------------------------------------- */
    char **papszRule = NULL;

    for( i = 0; apszOrderingRules[i] != NULL; i++ )
    {
        if( MEEQUAL(apszOrderingRules[i][0],pszValue) )
        {
            papszRule = apszOrderingRules[i] + 1;
            break;
        }
    }

    if( papszRule == NULL )
        return 0;

/* -------------------------------------------------------------------- */
/*      If we have a rule, apply it.  We create an array                */
/*      (panChildPr) with the priority code for each child (derived     */
/*      from the rule) and we then bubble sort based on this.           */
/* -------------------------------------------------------------------- */
    int  *panChildKey = (int *) calloc(sizeof(int),GetChildCount());

    for( i = 1; i < GetChildCount(); i++ )
    {
        panChildKey[i] = MEFindString( papszRule, GetChild(i)->GetValue() );
        if( panChildKey[i] == -1 )
            printf("Found unexpected key when trying to order SRS nodes.");
    }

    /* -------------------------------------------------------------------- */
/*      Sort - Note we don't try to do anything with the first child    */
/*      which we assume is a name string.                               */
/* -------------------------------------------------------------------- */
    int j, bChange = TRUE;

    for( i = 1; bChange && i < GetChildCount()-1; i++ )
    {
        bChange = FALSE;
        for( j = 1; j < GetChildCount()-i; j++ )
        {
            if( panChildKey[j] == -1 || panChildKey[j+1] == -1 )
                continue;

            if( panChildKey[j] > panChildKey[j+1] )
            {
                MESRSNode *poTemp = papoChildNodes[j];
                int          nKeyTemp = panChildKey[j];

                papoChildNodes[j] = papoChildNodes[j+1];
                papoChildNodes[j+1] = poTemp;

                nKeyTemp = panChildKey[j];
                panChildKey[j] = panChildKey[j+1];
                panChildKey[j+1] = nKeyTemp;

                bChange = TRUE;
            }
        }
    }

    if ( panChildKey != NULL)
        free(panChildKey);

    return 0;
}


