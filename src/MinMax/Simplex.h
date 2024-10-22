/*************************************************************************\

                             C O P Y R I G H T

  Copyright 2003 Image Synthesis Group, Trinity College Dublin, Ireland.
                        All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following paragraphs appear in all copies.


                             D I S C L A I M E R

  IN NO EVENT SHALL TRININTY COLLEGE DUBLIN BE LIABLE TO ANY PARTY FOR
  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING,
  BUT NOT LIMITED TO, LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE
  AND ITS DOCUMENTATION, EVEN IF TRINITY COLLEGE DUBLIN HAS BEEN ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGES.

  TRINITY COLLEGE DUBLIN DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED
  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND TRINITY
  COLLEGE DUBLIN HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
  ENHANCEMENTS, OR MODIFICATIONS.

  The authors may be contacted at the following e-mail addresses:

          Gareth_Bradshaw@yahoo.co.uk    isg@cs.tcd.ie

  Further information about the ISG and it's project can be found at the ISG
  web site :

          isg.cs.tcd.ie

\**************************************************************************/

#ifndef _SIMPLEX_H_
#define _SIMPLEX_H_

/*
  Taken from NR, changes :

    1.  Made the vector stuff into standard C++ new/delete
    2.  Changed base index from 1 to 0
    3.  NMAX is no longer hard coded
*/

double amotry(double **p, double y[], double psum[], int ndim,
              double (*funk)(double[], void *data, int *canFinish), int ihi,
              double fac, void *data, int *canFinish);
void amoeba(double **p, double y[], int ndim, double ftol,
            double (*funk)(double[], void *data, int *canFinish), int *nfunk,
            int NMAX, void *data, int *canFinish);
/*
Multidimensional minimization of the function funk(x) where x[0..ndim-1] is a
vector in ndim dimensions, by the downhill simplex method of Nelder and Mead.
The matrix p[0..ndim][0..ndim-1] is input. Its ndim+1 rows are ndim-dimensional
vectors which are the vertices of the starting simplex. Also input is the vector
y[0..ndim], whose components must be pre-initialized to the values of funk
evaluated at the ndim+1 vertices (rows) of p; and ftol the fractional
convergence tolerance to be achieved in the function value (n.b.!). On output, p
and y will have been reset to ndim+1 new points all within ftol of a minimum
function value, and nfunk gives the number of function evaluations taken.
*/

/*
    wrapper to provide simpler usage
    numDim is the number of dimensions, p being initial values for each
   dimension

    will return TRUE is the objectiveFunction insicated that we could terminate
   early
*/
int simplexMin(int numDim, double init[], double size[], double ftol,
               int *maxFunc,
               double (*func)(double[], void *data, int *canFinish), void *data,
               int iterations = 5);

#endif
