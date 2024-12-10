//  -*- Mode: C++ -*-
#ifndef _BALL_H
#include "ball.h"
#endif

#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include "svd.h"

#define MINFLOAT FLT_MIN
//
// This code uses "Numerical Recipes in C" code to do single precision SVD
// This book is by William H. Press, Saul A. Teukolsky,
// William T. Vellerling, and Brain P. Flannery and is available
// through Cambridge University Press.  The code is copyrighted, so it can
// not be distributed with this.
//
// Other packages offering SVD, such as fortran packages, would also work,
// but this code will require modifications.  For example, the
// vectors and arrays use with numerical recipies are 1 relative
// instead of 0 relative.
//
// #include "nr.h"
#include "nrutil.h"

// This is the percentage error allowed so that a ball is still
// considered enclosed.  The output of the algorithm could be off
// by this much, but it probably won't happen.
const float RAD_SLACK = 0.015;

// This is the percentage difference allowed between points that
// are supposed to be equidistant.  Singularities might cause this error.
// This is only used during debugging to check that calculations are OK...
const float BOUND_RAD_ERR = 0.01;

#ifdef DONT_INLINE_CALCDIST
float CalcDistSqrd(ulong num_dim, const float *v1, const float *v2) {
  // assert(num_dim > 1);
  // assert(v1 != NULL && v2 != NULL);
  float dist;

  ulong jj;
  float diff;
  diff = v1[0] - v2[0];
  dist = diff * diff;
  for (jj = 1; jj < num_dim; jj++) {
    diff = v1[jj] - v2[jj];
    dist += diff * diff;
  }
  // assert(!isnan(dist));
  return dist;
}

//
// This calculates a weighted quadratic distance between vectors
//
float CalcDistSqrd(ulong num_dim, const float *v1, const float *v2,
                   const float *ww) {
  if (ww == NULL)
    return CalcDistSqrd(num_dim, v1, v2);
  // assert(num_dim > 1);
  // assert(v1 != NULL && v2 != NULL);

  ulong jj;
  float dist, diff;

  diff = v1[0] - v2[0];
  dist = ww[0] * diff * diff;
  for (jj = 1; jj < num_dim; jj++) {
    diff = v1[jj] - v2[jj];
    dist += ww[jj] * diff * diff;
  }
  // assert(!isnan(dist));
  return dist;
}

float CalcDist(ulong num_dim, const float *v1, const float *v2) {
  return FSQRT(CalcDistSqrd(num_dim, v1, v2));
}

float CalcDist(ulong num_dim, const float *v1, const float *v2,
               const float *ww) {
  return FSQRT(CalcDistSqrd(num_dim, v1, v2, ww));
}

#endif // #ifdef DONT_INLINE_CALCDIST

inline float **GrabMatrix(float **&ptr_curr, float *&fl_curr, ulong start1,
                          ulong end1, ulong start2, ulong end2) {
  float **mat = ptr_curr - start1;
  ptr_curr += end1 - start1 + 1;
  for (ulong ii = start1; ii <= end1; ii++) {
    mat[ii] = fl_curr - start2;
    fl_curr += end2 - start2 + 1;
  }
  return (float **)mat;
}

inline float *GrabVector(float *&fl_curr, ulong start1, ulong end1) {
  float *vec = fl_curr - start1;
  fl_curr += end1 - start1 + 1;
  return vec;
}

inline ulong *GrabIntVector(ulong *&i_curr, ulong start1, ulong end1) {
  ulong *vec = i_curr - start1;
  i_curr += end1 - start1 + 1;
  return vec;
}

inline const float **GrabPtrVector(float **&ptr_curr, ulong start1,
                                   ulong end1) {
  const float **vec = (const float **)ptr_curr - start1;
  ptr_curr += end1 - start1 + 1;
  return vec;
}

//
// This routine determines the center of a ball given points
// on it, and weights determining it's ellipsoid shape
//
// This is done as follows:
// 1. Create a set of num_points - 1 linear equations to solve
// 2. Solve, and use to calculate the center point and radius
//
// The work memory required is calculated below...
// The work_size variable is provided for debugging purposes...
//
// NOTE: All points input to this routine must be on the boundary.  If not,
// the results will not be correct.
//
void BallFromBoundaryPoints(ulong num_dim, ulong num_points,
                            const float *const *centers, float *out_center,
                            float &radius_sqrd, const float *weights,
                            void *work, ulong work_size) {
  // assert(num_dim >= num_points-1);
  ulong ii, jj, kk;

  // Handle the easy cases explicitly...
  if (num_points <= 2)
    switch (num_points) {
    case 0:
      // Ball centered at origin with zero radius
      for (ii = 0; ii < num_dim; ii++)
        out_center[ii] = 0.0;
      radius_sqrd = 0.0;
      return;

    case 1:
      // Return the ball centered at that point of minimum size
      memcpy((void *)out_center, (const void *)centers[0],
             num_dim * sizeof(float));
      radius_sqrd = 16 * MINFLOAT;
      return;

    case 2: {
      float dist = 0.0;
      for (ii = 0; ii < num_dim; ii++) {
        float diff = centers[0][ii] - centers[1][ii];
        dist += diff * diff;
        out_center[ii] = (centers[0][ii] + centers[1][ii]) / 2.0;
      }
      radius_sqrd = dist / 4.0;
      return;
    }
    }

  // Allocate all memory needed
  ulong red_dim = num_points - 1;
  unsigned char *work_mem;
  ulong ptr_size = sizeof(float *) * (3 * red_dim);
  ulong fl_size = sizeof(float) *
                  (3 * red_dim + (2 * red_dim) * red_dim + red_dim * num_dim);
  ulong mem_size = ptr_size + fl_size;

  // assert((work==NULL && work_size==0) || (work!=NULL && work_size>0));
  bool need_alloc = (work == NULL || work_size < mem_size);
  if (need_alloc)
    work_mem = new unsigned char[mem_size];
  else
    work_mem = (unsigned char *)work;

  float **ptr_mem = (float **)work_mem;
  float *fl_mem = (float *)(work_mem + ptr_size);

  float *b_vec = GrabVector(fl_mem, 1, red_dim);
  float *ww = GrabVector(fl_mem, 1, red_dim);
  float *x_vec = GrabVector(fl_mem, 1, red_dim);
  float **centers2 = GrabMatrix(ptr_mem, fl_mem, 1, red_dim, 1, num_dim);
  float **u_mat = GrabMatrix(ptr_mem, fl_mem, 1, red_dim, 1, red_dim);
  float **v_mat = GrabMatrix(ptr_mem, fl_mem, 1, red_dim, 1, red_dim);
  // assert((unsigned char*)(work_mem + ptr_size)==(unsigned char*)ptr_mem);
  // assert((unsigned char*)(work_mem + mem_size)==(unsigned char*)fl_mem);

  // Make center2 and b_vec: only mess with weights if needed
  if (weights != NULL)
    for (ii = 1; ii <= red_dim; ii++) {
      float total = 0;
      for (jj = 0; jj < num_dim; jj++) {
        float diff = (centers[ii][jj] - centers[0][jj]) * weights[jj];
        centers2[ii][jj + 1] = diff;
        total += diff * diff;
      }
      b_vec[ii] = total / 2.0;
    }
  else
    for (ii = 1; ii <= red_dim; ii++) {
      float total = 0;
      for (jj = 0; jj < num_dim; jj++) {
        float diff = centers[ii][jj] - centers[0][jj];
        centers2[ii][jj + 1] = diff;
        total += diff * diff;
      }
      b_vec[ii] = total / 2.0;
    }

  // Fill in u_mat using centers2
  // It is symmetric, so only do half the work...
  for (jj = 1; jj <= red_dim; jj++)
    for (kk = 1; kk <= jj; kk++) {
      float total = 0;
      for (ii = 1; ii <= num_dim; ii++)
        total += centers2[jj][ii] * centers2[kk][ii];
      u_mat[jj][kk] = total;
      u_mat[kk][jj] = total;
    }

  // OK, we now have a system of linear equations, so solve it
  svdcmp(u_mat, red_dim, red_dim, ww, v_mat); // Augmented u_mat...
  float wmax = 0.0;
  for (jj = 1; jj <= red_dim; jj++)
    if (ww[jj] > wmax)
      wmax = ww[jj];
  float wmin = wmax * 5.0e-8;
  for (jj = 1; jj <= red_dim; jj++)
    if (ww[jj] < wmin) {
      ww[jj] = 0.0; // Try to "fix" the singularity
      // WARN_MSG("Singularity in matrix, ignoring: try increasing RAD_SLACK.");
      // //  GRB don't want to know
    }

  svbksb(u_mat, ww, v_mat, red_dim, red_dim, b_vec, x_vec);

  // OK, now x_vec holds the center in the reduced dimensionality
  // so we just need to project it back...
  if (weights != NULL)
    for (ii = 0; ii < num_dim; ii++) {
      float total = 0;
      for (jj = 1; jj <= red_dim; jj++)
        total += x_vec[jj] * centers2[jj][ii + 1];
      out_center[ii] = centers[0][ii] + total / weights[ii];
    }
  else
    for (ii = 0; ii < num_dim; ii++) {
      float total = centers[0][ii];
      for (jj = 1; jj <= red_dim; jj++)
        total += x_vec[jj] * centers2[jj][ii + 1];
      out_center[ii] = total;
    }

  float max_dist = 0;
  for (ii = 0; ii < num_points; ii++) {
    float dist = CalcDistSqrd(num_dim, out_center, centers[ii], weights);
    //	//assert(max_dist==0 || fabs(max_dist-dist) <= 2.0*BOUND_RAD_ERR*dist);
    //  GRB - happens when SVD doesn't converge !!!
    if (dist > max_dist)
      max_dist = dist;
  }
  radius_sqrd = max_dist;

  if (need_alloc)
    delete[] work_mem;
}

#ifndef NDEBUG
//
// Output a matrix: Useful for debugging purposes
//
void NRprmat(ulong rows, ulong cols, float **matrix, bool isone) {
  ulong off = isone != 0;
  for (ulong ii = 0; ii < rows; ii++) {
    for (ulong jj = 0; jj < cols; jj++)
      cout << matrix[ii + off][jj + off] << " ";
    cout << endl;
  }
}
#endif

//
// NOTE: All balls input to this routine must be on the boundary.  If not,
// the results will not be correct.  (ie. one ball encloses other balls)
// It is detected in the 2 ball case, but not handled in general.
//
void BallFromBoundaryBalls(ulong num_dim, ulong num_points,
                           const float *const *centers, const float *radii,
                           float *out_center, float &radius,
                           const float *weights, void *work, ulong work_size) {
  // assert(num_dim >= num_points-1);
  ulong ii, jj, kk;

  // Handle the easy cases...
  if (num_points <= 2)
    switch (num_points) {
    case 0:
      // Ball centered at origin with zero radius
      for (ii = 0; ii < num_dim; ii++)
        out_center[ii] = 0.0;
      radius = 0.0;
      return;

    case 1:
      // Return the ball centered at that point of minimum size
      memcpy((void *)out_center, (const void *)centers[0],
             num_dim * sizeof(float));
      radius = radii[0];
      return;

    case 2: {
      float dist = CalcDist(num_dim, centers[0], centers[1], weights);

      // Handle rare cases where one boundary covers the other side
      ulong low_rad, high_rad;
      if (radii[0] >= radii[1]) {
        low_rad = 1;
        high_rad = 0;
      } else {
        low_rad = 0;
        high_rad = 1;
      }

      if (radii[high_rad] > dist + radii[low_rad]) {
        WARN_MSG("Weird case: one ball encloses all");
        memcpy((void *)out_center, (const void *)centers[0],
               sizeof(float) * num_dim);
        radius = radii[high_rad];
      }

      float final_dist = (radii[0] + radii[1] + dist) / 2.0;

      // .5 is between centers
      // 0 is at point1
      // 1 is at point2
      // Other values are as expected...
      float center_ratio;
      if (dist < MINFLOAT * 4) {
        center_ratio = 0.5;
        final_dist += MINFLOAT * 4;
      } else
        center_ratio = (dist - radii[0] + radii[1]) / (2.0 * dist);

      for (jj = 0; jj < num_dim; jj++)
        out_center[jj] =
            centers[0][jj] + (centers[1][jj] - centers[0][jj]) * center_ratio;

      radius = final_dist;
      return;
    }
    }

  // Allocate all memory needed
  ulong red_dim = num_points - 1;
  ulong ptr_size = sizeof(float *) * (2 * red_dim + 2 * (red_dim + 1));
  ulong fl_size = sizeof(float) *
                  (5 * (red_dim + 1) + 2 * num_dim + red_dim * num_dim +
                   red_dim * (red_dim + 1) + 2 * (red_dim + 1) * (red_dim + 1));
  ulong mem_size = ptr_size + fl_size;
  unsigned char *work_mem;

  // assert((work==NULL && work_size==0) || (work!=NULL && work_size>0));
  bool need_alloc = (work == NULL || work_size < mem_size);
  if (need_alloc)
    work_mem = new unsigned char[mem_size];
  else
    work_mem = (unsigned char *)work;

  float **ptr_mem = (float **)work_mem;
  float *fl_mem = (float *)(work_mem + ptr_size);

  float *line_vec = GrabVector(fl_mem, 1, red_dim + 1);
  float *temp_vec = GrabVector(fl_mem, 1, red_dim + 1);
  float *x_vec = GrabVector(fl_mem, 1, red_dim + 1);
  float *b_vec = GrabVector(fl_mem, 1, red_dim + 1);
  float *ww = GrabVector(fl_mem, 1, red_dim + 1);
  float *base_vec = GrabVector(fl_mem, 1, num_dim);
  float *off_vec = GrabVector(fl_mem, 1, num_dim);
  float **centers2 = GrabMatrix(ptr_mem, fl_mem, 1, red_dim, 1, num_dim);
  float **temp_mat = GrabMatrix(ptr_mem, fl_mem, 1, red_dim, 1, red_dim + 1);
  float **u_mat = GrabMatrix(ptr_mem, fl_mem, 1, red_dim + 1, 1, red_dim + 1);
  float **v_mat = GrabMatrix(ptr_mem, fl_mem, 1, red_dim + 1, 1, red_dim + 1);
  // assert((float**)(work_mem + ptr_size)==ptr_mem);
  // assert((float*)(work_mem + mem_size)==fl_mem);

  float radius0sqrd = radii[0] * radii[0];

  // Make center2 and b_vec: only mess with weights if needed
  if (weights != NULL)
    for (ii = 1; ii <= red_dim; ii++) {
      float total = radius0sqrd - radii[ii] * radii[ii];
      for (jj = 0; jj < num_dim; jj++) {
        float diff = (centers[ii][jj] - centers[0][jj]) * weights[jj];
        centers2[ii][jj + 1] = diff;
        total += diff * diff;
      }
      temp_vec[ii] = total / 2.0;
    }
  else
    for (ii = 1; ii <= red_dim; ii++) {
      float total = radius0sqrd - radii[ii] * radii[ii];
      for (jj = 0; jj < num_dim; jj++) {
        float diff = centers[ii][jj] - centers[0][jj];
        centers2[ii][jj + 1] = diff;
        total += diff * diff;
      }
      temp_vec[ii] = total / 2.0;
    }

  // Fill in temp_mat using centers2
  // It is symmetric, so only do half the work...
  for (jj = 1; jj <= red_dim; jj++)
    for (kk = 1; kk <= jj; kk++) {
      float total = 0;
      for (ii = 1; ii <= num_dim; ii++)
        total += centers2[jj][ii] * centers2[kk][ii];
      temp_mat[jj][kk] = total;
      temp_mat[kk][jj] = total;
    }
  for (jj = 1; jj <= red_dim; jj++)
    temp_mat[jj][red_dim + 1] = radii[0] - radii[jj];

  // To make this solvable by SVD, multiply both sides by
  // the transpose of tempmat
  for (ii = 1; ii <= red_dim + 1; ii++) {
    float total = 0;
    for (kk = 1; kk <= red_dim; kk++)
      total += temp_vec[kk] * temp_mat[kk][ii];
    b_vec[ii] = total;
    for (jj = 1; jj <= red_dim + 1; jj++) {
      float total = 0;
      for (kk = 1; kk <= red_dim; kk++)
        total += temp_mat[kk][ii] * temp_mat[kk][jj];
      u_mat[ii][jj] = total;
    }
  }

  // OK, we now have a system of linear equations, so find the
  // solution space (which is one dimensional in this case)
  svdcmp(u_mat, red_dim + 1, red_dim + 1, ww, v_mat); // Augmented u_mat...
  float wmax = 0.0;
  for (jj = 1; jj <= red_dim + 1; jj++)
    if (ww[jj] > wmax)
      wmax = ww[jj];
  float wmin = wmax * 5.0e-8;
  bool found_singularity = 0;
  for (jj = 1; jj <= red_dim + 1; jj++)
    if (ww[jj] < wmin) {
      ww[jj] = 0.0;
      if (found_singularity) {
        WARN_MSG("Extra singularity: Automatically fixing: Try increasing "
                 "RAD_SLACK");
        for (ii = 1; ii <= red_dim + 1; ii++)
          line_vec[ii] = v_mat[ii][jj] * v_mat[red_dim + 1][jj] +
                         line_vec[ii] * line_vec[red_dim + 1];
      } else {
        found_singularity = 1;
        for (ii = 1; ii <= red_dim + 1; ii++)
          line_vec[ii] = v_mat[ii][jj];
      }
    }
  // assert(found_singularity);  // Must find at least one singularity
  // assert(fabs(line_vec[red_dim+1]) > MINFLOAT*16);

  // Multiply vector so the r is 1 (Assume mult is slower than division)
  float factor = 1 / line_vec[red_dim + 1];
  for (ii = 1; ii <= red_dim; ii++)
    line_vec[ii] *= factor;
  // line_vec[red_dim+1] = 1.0;

  svbksb(u_mat, ww, v_mat, red_dim + 1, red_dim + 1, b_vec, x_vec);

  // Subtract the line_vec so the r is 0
  for (ii = 1; ii <= red_dim; ii++)
    x_vec[ii] -= line_vec[ii] * x_vec[red_dim + 1];
  // x_vec[red_dim+1] = 0.0;  // This is ignored also, but should be 0.0

  // Project into full dimensionality
  for (ii = 1; ii <= num_dim; ii++) {
    float base_total = 0;
    float off_total = 0;
    for (jj = 1; jj <= red_dim; jj++) {
      base_total += centers2[jj][ii] * x_vec[jj];
      off_total += centers2[jj][ii] * line_vec[jj];
    }
    base_vec[ii] = base_total;
    off_vec[ii] = off_total;
  }

  float a_sqrd_sum = 0;
  float b_sqrd_sum = 0;
  float ab_sum = 0;
  for (ii = 1; ii <= num_dim; ii++) {
    a_sqrd_sum += off_vec[ii] * off_vec[ii];
    b_sqrd_sum += base_vec[ii] * base_vec[ii];
    ab_sum += base_vec[ii] * off_vec[ii];
  }

  float aa = 1 - a_sqrd_sum;
  float bb = -2.0 * (ab_sum + radii[0]);
  float cc = radii[0] * radii[0] - b_sqrd_sum;

  // Quadratic equation
  float sqrt_term = bb * bb - 4.0 * aa * cc;
  // assert(sqrt_term >= 0.0);
  float rr;
  // assert(aa != 0.0);
  if (aa > 0)
    rr = (FSQRT(sqrt_term) - bb) / (2.0 * aa);
  else
    rr = (-FSQRT(sqrt_term) - bb) / (2.0 * aa);
  // assert(rr>0.0);

  // Reuse x_vec[] for the values of the final center
  if (weights != NULL)
    for (ii = 1; ii <= num_dim; ii++) {
      float weighted = base_vec[ii] + rr * off_vec[ii];
      out_center[ii - 1] = centers[0][ii - 1] + weighted / weights[ii - 1];
    }
  else
    for (ii = 1; ii <= num_dim; ii++)
      out_center[ii - 1] = centers[0][ii - 1] + base_vec[ii] + rr * off_vec[ii];

  // Double check the solution
  float max_dist = 0;
  for (ii = 0; ii < num_points; ii++) {
    float dist =
        radii[ii] + CalcDist(num_dim, out_center, centers[ii], weights);
    // assert(fabs(rr - dist) <= BOUND_RAD_ERR*rr);
    if (dist > max_dist)
      max_dist = dist;
  }

  // If the solution is incorrect, then there probably is a ball
  // included that is not on the outside of the bounding ball.
  // Therefore, we could try throwing away a point and redoing
  // the calculations, but I may not need this, so I'll put it off...
  radius = max_dist;
  if (need_alloc)
    delete[] work_mem;
}

static int int_comp(const void *p1, const void *p2) {
  return *((long *)p1) - *((long *)p2);
}

static void IntSort(ulong *bound_arr, ulong num_entries) {
  qsort((void *)bound_arr, num_entries, sizeof(ulong), int_comp);
}

void EnclosingBall(ulong num_dim, ulong num_points, const float *const *centers,
                   const float *radii, float *out_center, float &out_radius,
                   const float *weights, ulong *boundary_arr,
                   ulong in_boundary_size, ulong *out_boundary_size, void *work,
                   ulong work_size) {
  // assert(num_points>0);
  // assert(num_dim>1);
  // assert(centers!=NULL);
  // assert(out_center!=NULL);
  // assert((out_boundary_size==NULL) || (boundary_arr!=NULL));
  ulong ii;
  float common_radius = 0;

  // Directly solve the easy cases
  if (num_points <= 2) {
    if (radii == NULL) {
      BallFromBoundaryPoints(num_dim, num_points, centers, out_center,
                             out_radius, weights);
      out_radius = FSQRT(out_radius);
    } else
      BallFromBoundaryBalls(num_dim, num_points, centers, radii, out_center,
                            out_radius, weights);
    if (out_boundary_size) {
      *out_boundary_size = 1;
      boundary_arr[0] = 0;
    }
    return;
  }

  // If all the same radius, then just do the point calculation
  // and add on the extra radius at the end...
  if (radii != NULL) {
    for (ii = 1; ii < num_points; ii++)
      if (radii[ii] != radii[0])
        break;
    if (ii == num_points) {
      common_radius = radii[0];
      radii = NULL;
    }
  }

  // Maximum boundary points and level of recursion
  ulong max_level = num_points > num_dim + 1 ? num_dim + 1 : num_points;

  ulong int_size = sizeof(ulong) * (2 * num_points + 2 * max_level);
  ulong ptr_size = sizeof(float *) * max_level;
  ulong fl_size = sizeof(float) * (radii != NULL ? max_level : 0);
  ulong mem_size = ptr_size + int_size + fl_size;
  ulong extra_size;
  unsigned char *work_mem;
  void *extra_mem;
  // assert((work==NULL && work_size==0) || (work!=NULL && work_size>0));
  bool need_alloc = (work == NULL || work_size < mem_size);
  if (need_alloc) {
    extra_size =
        sizeof(float *) * 4 * max_level +
        sizeof(float) * (5 * max_level + 2 * num_dim + max_level * num_dim +
                         3 * max_level * max_level);
    // Shouldn't happen, but handle this case
    if (mem_size > (1 << 13))
      extra_size = 0;
    else if (extra_size + mem_size > (1 << 14))
      extra_size = (1 << 14) - mem_size; // Limit total to 16K
    work_mem = new unsigned char[mem_size + extra_size];
    extra_mem = (void *)(work_mem + mem_size);
  } else {
    work_mem = (unsigned char *)work;
    extra_mem = (void *)(work_mem + mem_size);
    extra_size = work_size - mem_size;
  }

  float **ptr_mem = (float **)work_mem;
  ulong *int_mem = (ulong *)(work_mem + ptr_size);
  float *fl_mem = (float *)(work_mem + ptr_size + int_size);

  // Used to create permuted next_elem
  ulong *array = GrabIntVector(int_mem, 0, num_points - 1);
  // Linked list of elements
  ulong *next_elem = GrabIntVector(int_mem, 0, num_points - 1);
  // Array of boundary ball elements
  ulong *bound_elem = GrabIntVector(int_mem, 0, max_level - 1);
  ulong *last_stack = GrabIntVector(int_mem, 0, max_level - 1);
  const float **bound_centers = GrabPtrVector(ptr_mem, 0, max_level - 1);
  float *bound_radii =
      radii != NULL ? GrabVector(fl_mem, 0, max_level - 1) : NULL;

  // assert((float**)(work_mem + ptr_size)==ptr_mem);
  // assert((ulong*)(work_mem + ptr_size + int_size)==int_mem);
  // assert((float*)(work_mem + mem_size)==fl_mem);

  ulong first; // First in list of elements
  ulong bound_size;
  ulong curr_level;
  ulong prev, curr, last;
  float radius, true_radius; // Use local variable so a register can be used
  // BTW, radius is radius squared if dealing with points
#ifdef SHOW_WORK
  ulong old_first_elem = 1000000;
#endif

  for (ii = 0; ii < num_points; ii++)
    array[ii] = ii;

  // If a boundary was given, make it the first elements
  // assert(in_boundary_size <= num_points);
  if (in_boundary_size > 0) {
    // Sort to avoid swapping problems
    IntSort(boundary_arr, in_boundary_size);
    for (ii = 0; ii < in_boundary_size; ii++) {
      ulong other = boundary_arr[ii];
      // assert(other < num_points);  // Valid element
      // assert(ii==0 || other!=boundary_arr[ii-1]); // No repeats...
      ulong tmp = array[ii];
      array[ii] = array[other];
      array[other] = tmp;
    }
  }

  // Permute the ordering other elements...
  for (ii = in_boundary_size; ii < num_points - 1; ii++) {
    ulong pick = rand() % (num_points - ii);
    ulong tmp = array[ii];
    array[ii] = array[ii + pick];
    array[ii + pick] = tmp;
  }

  first = array[0];
  for (ii = 1; ii < num_points; ii++)
    next_elem[array[ii - 1]] = array[ii];
  next_elem[array[num_points - 1]] = num_points; // End list terminator

  // Init Center and boundary to enclose the first point
  // This avoid some special cases below
  memset((void *)out_center, 0, num_dim * sizeof(float));
  radius = MINFLOAT;
  true_radius = MINFLOAT;
  bound_elem[0] = first;
  bound_centers[0] = centers[first];
  if (radii != NULL)
    bound_radii[0] = radii[first];
  bound_size = 1;

  curr_level = 0;
  prev = first;
  curr = next_elem[first];
  last = num_points;

  while (curr != num_points) {
    while (1) {
      // Is it in the ball...
      float total_dist =
          (radii != NULL)
              ? CalcDist(num_dim, out_center, centers[curr], weights) +
                    radii[curr]
              :
              // Here total_dist and radius are squared
              CalcDistSqrd(num_dim, out_center, centers[curr], weights);

      // Quit loop if enclosed by ball...
      if (total_dist <= true_radius)
        break;
      if (total_dist <= radius) {
        // Yes quit loop but update true_radius
        true_radius = total_dist;
        break;
      }

      // This shouldn't happen, but might happen due to numeric error...
      // I'll handle this later if necessary
      // assert(curr_level<=max_level);

      bound_elem[curr_level] = curr;
      bound_centers[curr_level] = centers[curr];
      if (radii != NULL)
        bound_radii[curr_level] = radii[curr];
      bound_size = curr_level + 1;

      if (radii != NULL) {
        BallFromBoundaryBalls(num_dim, bound_size, bound_centers, bound_radii,
                              out_center, true_radius, weights, extra_mem,
                              extra_size);
        // Add a little extra space (avoid singularities)
        radius = true_radius * (1.0 + RAD_SLACK);
      } else {
        BallFromBoundaryPoints(num_dim, bound_size, bound_centers, out_center,
                               true_radius, weights, extra_mem, extra_size);
        // Add a little extra space (avoid singularities)
        // This is squared dist so the error tolerance is doubled
        radius = true_radius * (1.0 + 2.0 * RAD_SLACK);
      }

      if (curr == first)
        break;
      last_stack[curr_level] = last;
      curr_level++;
      last = prev;
      curr = first;
    }

#ifdef SHOW_WORK
    if (curr_level == 0 && bound_size > 1 && bound_elem[0] != old_first_elem) {
      for (ii = 0; ii < bound_size; ii++)
        cout << bound_elem[ii] << " ";
      cout << endl;
      old_first_elem = bound_elem[0];
    }
#endif

    prev = curr;
    curr = next_elem[curr];

    ulong test_prev = prev;
    while (test_prev == last) {
      // assert(curr_level != 0);
      curr_level--;
      last = last_stack[curr_level];
      ulong next = next_elem[curr];

      // Now do move to front...
      // assert(curr!=first);
      next_elem[prev] = next;
      next_elem[curr] = first;
      test_prev = first = curr;
      curr = next;
    }
  }
  // assert(curr_level==0);

  const float FLOAT_ERROR_SAFETY = 1.001;
  if (radii != NULL)
    out_radius = true_radius * FLOAT_ERROR_SAFETY;
  else
    out_radius = (FSQRT(true_radius) + common_radius) * FLOAT_ERROR_SAFETY;

  // Output boundary points
  if (boundary_arr != NULL && out_boundary_size != NULL) {
    if (bound_size < *out_boundary_size)
      *out_boundary_size = bound_size;
    ulong out_size = *out_boundary_size;
    for (ii = 0; ii < out_size; ii++)
      boundary_arr[ii] = bound_elem[ii];
  }

#ifndef NDEBUG
  if (radii == NULL)
    for (ii = 0; ii < num_points; ii++) {
      float dist = CalcDist(num_dim, out_center, centers[ii], weights);
      // assert(dist <= out_radius);
    }
  else
    for (ii = 0; ii < num_points; ii++) {
      float dist = CalcDist(num_dim, out_center, centers[ii], weights);
      dist += radii[ii];
      // assert(dist <= out_radius);
    }
#endif

  if (need_alloc)
    delete[] work_mem;
}
