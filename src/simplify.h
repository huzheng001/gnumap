/*
** $Id: simplify.h,v 1.2 2006-09-30 08:01:04 jpzhang Exp $
**
** An Douglas-Peucker algorithm for polyline simplification interface.
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Zhang Ji Peng.
**
** Create date: 2006/09/30
**
*/

#ifndef _SIMPLIFY_H_ 
#define _SIMPLIFY_H_ 


#define epsilon  1


int Simplify2d(MEDoublePoint *inpts, int innum, MEDoublePoint ** outpts);




#endif /* _SIMPLIFY_H_ */
