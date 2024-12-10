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

#include <math.h>
#include <float.h>
#include <memory.h>
#include "Simplex.h"
#include "../Base/Defs.h"
#define TINY 1.0e-10 //  A small number.

#define GET_PSUM                                                               \
  for (j = 0; j < ndim; j++) {                                                 \
    sum = 0.0;                                                                 \
    for (i = 0; i < mpts; i++)                                                 \
      sum += p[i][j];                                                          \
    psum[j] = sum;                                                             \
  }

#define SWAP(a, b)                                                             \
  {                                                                            \
    swap = (a);                                                                \
    (a) = (b);                                                                 \
    (b) = swap;                                                                \
  }

void amoeba(double **p, double y[], int ndim, double ftol,
            double (*funk)(double[], void *data, int *canFinish), int *nfunk,
            int NMAX, void *data, int *canFinish)
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
{
  int i, ihi, ilo, inhi, j, mpts = ndim + 1;
  double rtol, sum, swap, ysave, ytry;
  double *psum = new double[ndim];
  *nfunk = 0;
  *canFinish = 0;
  GET_PSUM
  for (;;) {
    // First we must determine which point is the highest (worst), next-highest,
    // and lowest (best), by looping over the points in the simplex.
    ilo = 0;
    ihi = y[0] > y[1] ? (inhi = 1, 0) : (inhi = 0, 1);
    for (i = 0; i < mpts; i++) {
      if (y[i] <= y[ilo])
        ilo = i;
      if (y[i] > y[ihi]) {
        inhi = ihi;
        ihi = i;
      } else if (y[i] > y[inhi] && i != ihi)
        inhi = i;
    }

    rtol = 2.0 * fabs(y[ihi] - y[ilo]) / (fabs(y[ihi]) + fabs(y[ilo]) + TINY);
    *nfunk += 2;

    // Compute the fractional range from highest to lowest and return if
    // satisfactory.
    if (rtol < ftol || *canFinish || *nfunk > NMAX) {
      // If returning, put best point and value in slot 1.
      SWAP(y[0], y[ilo])
      for (i = 0; i < ndim; i++)
        SWAP(p[0][i], p[ilo][i])
      break;
    }

    // Begin a new iteration. First extrapolate by a factor - 1 through the face
    // of the simplex across from the high point, i.e., re ect the simplex from
    // the high point.

    ytry = amotry(p, y, psum, ndim, funk, ihi, -1.0, data, canFinish);
    if (ytry <= y[ilo])
      // Gives a result better than the best point, so try an additional
      // extrapolation by a factor 2.
      ytry = amotry(p, y, psum, ndim, funk, ihi, 2.0, data, canFinish);
    else if (ytry >= y[inhi]) {
      // The re ected point is worse than the second-highest, so look for an
      // intermediate lower point, i.e., do a one-dimensional contraction.
      ysave = y[ihi];
      ytry = amotry(p, y, psum, ndim, funk, ihi, 0.5, data, canFinish);
      if (ytry >= ysave) {
        // Can't seem to get rid of that high point. Better
        // contract around the lowest (best) point.
        for (i = 0; i < mpts; i++) {
          if (i != ilo) {
            for (j = 0; j < ndim; j++)
              p[i][j] = psum[j] = 0.5 * (p[i][j] + p[ilo][j]);
            y[i] = (*funk)(psum, data, canFinish);
          }
        }
        *nfunk += ndim; // Keep track of function evaluations.
        GET_PSUM        // Recompute psum.
      }
    } else
      --(*nfunk); // Correct the evaluation count.
  } // Go back for the test of doneness and the next
    // iteration

  delete[] psum;
}

double amotry(double **p, double y[], double psum[], int ndim,
              double (*funk)(double[], void *data, int *canFinish), int ihi,
              double fac, void *data, int *canFinish)
// Extrapolates by a factor fac through the face of the simplex across from the
// high point, tries it, and replaces the high point if the new point is better.
{
  int j;
  double fac1, fac2, ytry;
  double *ptry = new double[ndim]; // vector(1,ndim);

  fac1 = (1.0 - fac) / ndim;
  fac2 = fac1 - fac;
  for (j = 0; j < ndim; j++)
    ptry[j] = psum[j] * fac1 - p[ihi][j] * fac2;
  ytry = (*funk)(ptry, data,
                 canFinish); // Evaluate the function at the trial point.
  if (ytry <
      y[ihi]) { // If it's better than the highest, then replace the highest.
    y[ihi] = ytry;
    for (j = 0; j < ndim; j++) {
      psum[j] += ptry[j] - p[ihi][j];
      p[ihi][j] = ptry[j];
    }
  }

  delete[] ptry;
  return ytry;
}

/*
    wrapper to provide simpler usage
    numDim is the number of dimensions, p being initial values for each
   dimension
*/
int simplexMin(int numDim, double init[], double size[], double ftol,
               int *maxFunc,
               double (*func)(double[], void *data, int *canFinish), void *data,
               int iters) {
  //  flag for early finish
  int canFinish = 0;

  //  storage
  double *y = new double[numDim + 1];
  double **p = new double *[numDim + 1];
  for (int i = 0; i <= numDim; i++) {
    //  make new vector
    p[i] = new double[numDim];
  }

  //  setup p0/y0 for first iteration
  memcpy(p[0], init, numDim * sizeof(double));
  y[0] = func(p[0], data, &canFinish);

  //  do restarts
  if (iters < 1)
    iters = 1;
  double lastY0 = 0;
  for (int l = 0; l < iters && canFinish == 0; l++) {
    //  make initial matrix from p0
    for (int i = 1; i <= numDim; i++) {
      //  copy values
      memcpy(p[i], p[0], numDim * sizeof(double));

      //  extend by size along dimension
      p[i][i - 1] += size[i - 1];
    }

    //  evaluating func at each point
    for (int i = 1; i <= numDim; i++)
      y[i] = func(p[i], data, &canFinish);

    //  do optimisation
    int nFunc = 0;
    amoeba(p, y, numDim, ftol, func, &nFunc, *maxFunc, data, &canFinish);

    double rImp = (lastY0 - y[0]) / lastY0;
    if (!finite(rImp))
      rImp = 1E10; // so lastY0 == 0 will work
    if (rImp < sqrt(ftol)) {
      *maxFunc = nFunc;
      break;
    }

    lastY0 = y[0];
  }

  //  store result ( result was put into p[0] )
  memcpy(init, p[0], numDim * sizeof(double));

  //  tidy up
  for (int i = 0; i <= numDim; i++)
    delete p[i];
  delete p;
  delete y;

  return canFinish;
}
