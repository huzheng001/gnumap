/*
** $Id: simplify.cpp,v 1.3 2006-11-13 09:35:13 jpzhang Exp $
**
** An Douglas-Peucker algorithm for polyline simplification implementation.
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Zhang Ji Peng.
**
** Create date: 2006/09/30
*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "dc.h"
#include "simplify.h"



static inline double distance2d_pt_pt(MEDoublePoint *p1, MEDoublePoint *p2)
{
	double hside = p2->x - p1->x;
	double vside = p2->y - p1->y;

	return sqrt ( hside*hside + vside*vside );
}

/*distance2d from p to line A->B */
static double distance2d_pt_seg(MEDoublePoint *p, MEDoublePoint *A, MEDoublePoint *B)
{
	double	r,s;

	/*if start==end, then use pt distance */
	if (  ( A->x == B->x) && (A->y == B->y) )
		return distance2d_pt_pt(p,A);

	/*
	 * otherwise, we use comp.graphics.algorithms
	 * Frequently Asked Questions method
	 *
	 *  (1)     	      AC dot AB
         *         r = ---------
         *               ||AB||^2
	 *	r has the following meaning:
	 *	r=0 P = A
	 *	r=1 P = B
	 *	r<0 P is on the backward extension of AB
	 *	r>1 P is on the forward extension of AB
	 *	0<r<1 P is interior to AB
	 */

	r = ( (p->x-A->x) * (B->x-A->x) + (p->y-A->y) * (B->y-A->y) )/( (B->x-A->x)*(B->x-A->x) +(B->y-A->y)*(B->y-A->y) );

	if (r<0) return distance2d_pt_pt(p,A);
	if (r>1) return distance2d_pt_pt(p,B);


	/*
	 * (2)
	 *	     (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
	 *	s = -----------------------------
	 *	             	L^2
	 *
	 *	Then the distance from C to P = |s|*L.
	 *
	 */

	s = ( (A->y-p->y)*(B->x-A->x)- (A->x-p->x)*(B->y-A->y) ) /
		( (B->x-A->x)*(B->x-A->x) +(B->y-A->y)*(B->y-A->y) );

	return ABS(s) * sqrt((B->x-A->x)*(B->x-A->x) + (B->y-A->y)*(B->y-A->y));
}
/*
 * Search farthest point from segment p1-p2
 * returns distance in an int pointer
 */
static void DP_findsplit2d(MEDoublePoint *pts, int p1, int p2, int *split, double *dist)
{
   int k;
   MEDoublePoint pa, pb, pk;
   double tmp;

   *dist = -1;
   *split = p1;

   if (p1 + 1 < p2)
   {
     pa = pts[p1]; 
     pb = pts[p2];

      for (k=p1+1; k<p2; k++)
      {
         pk = pts[k];
         /* distance computation */
         tmp = distance2d_pt_seg(&pk, &pa, &pb);

         if (tmp > *dist) 
         {
            *dist = tmp;	/* record the maximum */
            *split = k;
         }
      }

   } /* length---should be redone if can == 0 */

}



int Simplify2d(MEDoublePoint *inpts, int innum, MEDoublePoint ** outpts)
{
	int *stack;			/* recursion stack */
	int sp=-1;			/* recursion stack pointer */
	int p1, split, outnum; 
	double dist;
	int ptsize = sizeof(MEDoublePoint);

	/* Allocate recursion stack */
	stack =(int*)malloc(sizeof(int)*innum);

	p1 = 0;
	stack[++sp] = innum-1;

	/* allocate space for output array */
	outnum=1;
	*outpts = (MEDoublePoint*)malloc(ptsize * innum);
	(*outpts)[0] =inpts[0];

	do
	{
		DP_findsplit2d(inpts, p1, stack[sp], &split, &dist);

		if (dist > epsilon) {
			stack[++sp] = split;
		} else {
			outnum++;
			(*outpts)[outnum-1] = inpts[stack[sp]];
			p1 = stack[sp--];
		}
	}
	while (! (sp<0) );

	/*
	 * If we have reduced the number of points realloc
	 * outpoints array to free up some memory.
	 */
	if ( outnum < innum )
	{
		*outpts = (MEDoublePoint*)realloc(*outpts, ptsize*outnum);
		if ( *outpts == NULL ) {
			printf("Out of virtual memory");
		}
	}

	free(stack);
	return outnum;
}
