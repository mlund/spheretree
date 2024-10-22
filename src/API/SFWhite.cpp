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

#include "SFWhite.h"
#include "SFWhite/ball.h"
#include "SFRitter.h"
#include "../Base/Defs.h"

bool SFWhite::makeSphere(Sphere *s, const Array<Surface::Point> &points,
                         const Array<int> &inds) {
  Array<Point3D> pts;
  convertPoints(&pts, points, inds);
  return makeSphere(s, pts);
}

bool SFWhite::makeSphere(Sphere *s, const Array<Surface::Point> &points) {
  Array<Point3D> pts;
  convertPoints(&pts, points);
  return makeSphere(s, pts);
}

bool SFWhite::makeSphere(Sphere *s, const Array<Point3D> &pts) {
  /*
    make stuff for White's algorithm
  */
  int numPts = pts.getSize();
  if (numPts <= 3) {
    //  this can happen when you get bad convergence in the SVD etc.
    return SFRitter::makeSphere(s, pts);
  }

  float *centersInternal = new float[numPts * 3];
  float **centers = new float *[numPts];
  for (int i = 0; i < numPts; i++) {
    centers[i] = &centersInternal[i * 3];

    Point3D p = pts.index(i);
    centers[i][0] = p.x;
    centers[i][1] = p.y;
    centers[i][2] = p.z;
  }

  //  work arrays
  float radius;
  float out_center[3];
  ulong bound_arr[3];
  ulong work_size = (1 << 15);
  void *work = (void *)(new unsigned char[work_size]);

  //  call enclosing
  unsigned long in_bound_size = 0;
  unsigned long out_bound_size = 3;
  EnclosingBall(3, numPts, centers, NULL, out_center, radius, NULL, bound_arr,
                in_bound_size, &out_bound_size, work, work_size);

  s->c.x = out_center[0];
  s->c.y = out_center[1];
  s->c.z = out_center[2];
  s->r = radius;

  delete centers;
  delete centersInternal;
  delete work;

  if (!finite(s->r)) {
    //  this can happen when you get bad convergence in the SVD etc.
    SFRitter::makeSphere(s, pts);
  }

  return true;
}