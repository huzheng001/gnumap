
/*
** $Id: spatialref.cpp,v 1.2 2006-11-06 02:52:08 jpzhang Exp $
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


void MEDestroy(char **papszStrList)
{
    char **papszPtr;

    if (papszStrList)
    {
        papszPtr = papszStrList;
        while(*papszPtr != NULL)
        {
            if (*papszPtr != NULL)
                free(*papszPtr);
            papszPtr++;
        }

        if (papszStrList != NULL)
            free (papszStrList);
    }
}

int MEFindString( char ** papszList, const char * pszTarget )

{
    int         i;

    if( papszList == NULL )
        return -1;

    for( i = 0; papszList[i] != NULL; i++ )
    {
        if( MEEQUAL(papszList[i],pszTarget) )
            return i;
    }

    return -1;
}
/**********************************************************************
 *                       MECount()
 *
 * Return the number of lines in a Stringlist.
 **********************************************************************/
int MECount(char **papszStrList)
{
    int nItems=0;

    if (papszStrList)
    {
        while(*papszStrList != NULL)
        {
            nItems++;
            papszStrList++;
        }
    }

    return nItems;
}

/************************************************************************/
/*                         METokenizeString2()                         */
/*                                                                      */
/*      The ultimate tokenizer?                                         */
/************************************************************************/

char ** METokenizeString2( const char * pszString,
                            const char * pszDelimiters,
                            int nMETFlags )

{
    char        **papszRetList = NULL;
    int         nRetMax = 0, nRetLen = 0;
    char        *pszToken;
    int         nTokenMax, nTokenLen;
    int         bHonourStrings = (nMETFlags & MET_HONOURSTRINGS);
    int         bAllowEmptyTokens = (nMETFlags & MET_ALLOWEMPTYTOKENS);

    pszToken = (char *) calloc(10,1);
    nTokenMax = 10;
    
    while( pszString != NULL && *pszString != '\0' )
    {
        int     bInString = FALSE;

        nTokenLen = 0;
        
        /* Try to find the next delimeter, marking end of token */
        for( ; *pszString != '\0'; pszString++ )
        {

            /* End if this is a delimeter skip it and break. */
            if( !bInString && strchr(pszDelimiters, *pszString) != NULL )
            {
                pszString++;
                break;
            }
            
            /* If this is a quote, and we are honouring constant
               strings, then process the constant strings, with out delim
               but don't copy over the quotes */
            if( bHonourStrings && *pszString == '"' )
            {
                if( nMETFlags & MET_PRESERVEQUOTES )
                {
                    pszToken[nTokenLen] = *pszString;
                    nTokenLen++;
                }

                if( bInString )
                {
                    bInString = FALSE;
                    continue;
                }
                else
                {
                    bInString = TRUE;
                    continue;
                }
            }

            /* Within string constants we allow for escaped quotes, but
               in processing them we will unescape the quotes */
            if( bInString && pszString[0] == '\\' && pszString[1] == '"' )
            {
                if( nMETFlags & MET_PRESERVEESCAPES )
                {
                    pszToken[nTokenLen] = *pszString;
                    nTokenLen++;
                }

                pszString++;
            }

            /* Within string constants a \\ sequence reduces to \ */
            else if( bInString 
                     && pszString[0] == '\\' && pszString[1] == '\\' )
            {
                if( nMETFlags & MET_PRESERVEESCAPES )
                {
                    pszToken[nTokenLen] = *pszString;
                    nTokenLen++;
                }
                pszString++;
            }

            if( nTokenLen >= nTokenMax-3 )
            {
                nTokenMax = nTokenMax * 2 + 10;
                pszToken = (char *) realloc( pszToken, nTokenMax );
            }

            pszToken[nTokenLen] = *pszString;
            nTokenLen++;
        }

        pszToken[nTokenLen] = '\0';

        /*
         * Add the token.
         */
        if( pszToken[0] != '\0' || bAllowEmptyTokens )
        {
            if( nRetLen >= nRetMax - 1 )
            {
                nRetMax = nRetMax * 2 + 10;
                papszRetList = (char **) 
                    realloc(papszRetList, sizeof(char*) * nRetMax );
            }
            
            if (pszToken)
                papszRetList[nRetLen++] = strdup(pszToken);
            else
                papszRetList[nRetLen++] = strdup("");

            papszRetList[nRetLen] = NULL;
        }
    }

    /*
     * If the last token was empty, then we need to capture
     * it now, as the loop would skip it.
     */
    if( *pszString == '\0' && bAllowEmptyTokens && nRetLen > 0 
        && strchr(pszDelimiters,*(pszString-1)) != NULL )
    {
        if( nRetLen >= nRetMax - 1 )
        {
            nRetMax = nRetMax * 2 + 10;
            papszRetList = (char **) 
                realloc(papszRetList, sizeof(char*) * nRetMax );
        }

        papszRetList[nRetLen++] = strdup("");
        papszRetList[nRetLen] = NULL;
    }

    if( papszRetList == NULL )
        papszRetList = (char **) calloc(sizeof(char *),1);

    if( pszToken != NULL)
        free(pszToken);

    return papszRetList;
}
/************************************************************************/
/*                      METokenizeStringComplex()                      */
/*                                                                      */
/*      Obsolete tokenizing api.                                        */
/************************************************************************/

char ** METokenizeStringComplex( const char * pszString,
        const char * pszDelimiters,
        int bHonourStrings, int bAllowEmptyTokens )

{
    int         nFlags = 0;

    if( bHonourStrings )
        nFlags |= MET_HONOURSTRINGS;
    if( bAllowEmptyTokens )
        nFlags |= MET_ALLOWEMPTYTOKENS;

    return METokenizeString2( pszString, pszDelimiters, nFlags );
}
