//  -*- Mode: C++ -*-
#ifndef _BALL_H
#define _BALL_H

#include <stddef.h>
#include <math.h>
#include <assert.h>
#include <iostream>

// We want to use a fast version of sqrt if possible....
#if defined(sgi)
#define FSQRT(xx) fsqrt(xx)
#else
#define FSQRT(xx) sqrt(xx)
#endif

#ifndef NDEBUG
#include <iostream>
#define WARN_MSG(str) cerr << str
#else
#define WARN_MSG(str)
#endif

using std::cerr;
using std::cout;
using std::endl;

// This should be a 32bit unsigned integer
typedef unsigned long ulong;
#ifndef __GNUC__
// typedef short bool;
#endif

#ifdef DONT_INLINE_CALCDIST

float CalcDistSqrd(ulong num_dim, const float *v1, const float *v2);
float CalcDistSqrd(ulong num_dim, const float *v1, const float *v2,
                   const float *ww);
float CalcDist(ulong num_dim, const float *v1, const float *v2);
float CalcDist(ulong num_dim, const float *v1, const float *v2,
               const float *ww);

#else
inline float CalcDistSqrd(ulong num_dim, const float *v1, const float *v2) {
  assert(num_dim > 1);
  assert(v1 != NULL && v2 != NULL);
  float dist;

  ulong jj;
  float diff;
  diff = v1[0] - v2[0];
  dist = diff * diff;
  for (jj = 1; jj < num_dim; jj++) {
    diff = v1[jj] - v2[jj];
    dist += diff * diff;
  }
  return dist;
}

//
// This calculates a weighted quadratic distance between vectors
//
inline float CalcDistSqrd(ulong num_dim, const float *v1, const float *v2,
                          const float *ww) {
  if (ww == NULL)
    return CalcDistSqrd(num_dim, v1, v2);
  assert(num_dim > 1);
  assert(v1 != NULL && v2 != NULL);

  ulong jj;
  float dist, diff;

  diff = v1[0] - v2[0];
  dist = ww[0] * diff * diff;
  for (jj = 1; jj < num_dim; jj++) {
    diff = v1[jj] - v2[jj];
    dist += ww[jj] * diff * diff;
  }
  return dist;
}

inline float CalcDist(ulong num_dim, const float *v1, const float *v2) {
  return sqrt(CalcDistSqrd(num_dim, v1, v2));
}

inline float CalcDist(ulong num_dim, const float *v1, const float *v2,
                      const float *ww) {
  return sqrt(CalcDistSqrd(num_dim, v1, v2, ww));
}

#endif // #ifdef CANT_INLINE_CALCDIST

// This is only for debugging purposes, and can be removed if desired
void NRprmat(ulong rows, ulong cols, float **matrix, bool isone = 0);

void BallFromBoundaryPoints(ulong num_dim, ulong num_points,
                            const float *const *centers, float *out_center,
                            float &radius_sqrd, const float *weights = NULL,
                            void *work = NULL, ulong work_size = 0);

void BallFromBoundaryBalls(ulong num_dim, ulong num_points,
                           const float *const *centers, const float *radii,
                           float *out_center, float &radius,
                           const float *weights = NULL, void *work = NULL,
                           ulong work_size = 0);

//
// void EnclosingBall(---)
//
// Find enclosing ball of points or balls:
//   num_dim: Dimensionality of data (ie. 3 = 3D)
//   num_points: Number of points or balls to be enclosed
//   centers: An array of size num_points of float arrays of size num_dim
//	      that represent points or centers of balls
//   radii: If points, this is NULL.  Otherwise this is an array of radii
//          of the internal balls of size num_points.
//   out_center: The center of the enclosing ball.
//   radius: The radius of the enclosing ball
//   weights: If not NULL, this specifies that weighted euclidean distance
//            should be used so ellipses can be used instead of balls.
//	      BTW, these weights are squared weights since they are used
//	      directly when calculating the squared distance. (optional)
//   boundary_elem: If non-NULL, this is an array used to hold the
//		    boundary elements on input (if in_boundary_size>0)
//		    and output (if out_boundary_elem is not NULL).  This
//		    array should be assumed to be modified if non-NULL.
//   in_boundary_size: This specifies how many elements of boundary_elem
//		       are valid boundary points.  These points are
//		       considered first.  All other points are permuted
//		       and considered in random order.  If zero, all
//		       elements are considered in random order.
//   out_boundary_size: If non-NULL, this points to an integer that specifies
//		        the size of the output array and on output holds
//			the size of the output boundary (it can only
//			decrease in value).
//   work: If non-NULL, it is a pointer to a block of memory to use for
//	   calculations.  The size of the work memory must also be specified
//	   or else this memory will be ignored.
//   work_size: Size of that block of memory.  If it is not enough memory,
//              then the work memory is ignored and more memory is allocated.
//
void EnclosingBall(ulong num_dim, ulong num_points, const float *const *centers,
                   const float *radii, float *out_center, float &radius,
                   const float *weights = NULL, ulong *boundary_arr = NULL,
                   ulong in_boundary_size = 0, ulong *out_boundary_size = NULL,
                   void *work = NULL, ulong work_size = 0);

#endif // #ifndef _BALL_H
