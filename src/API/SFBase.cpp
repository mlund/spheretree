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

#include "SFBase.h"

//  fit the sphere around all the samples in a set
bool SFBase::fitSphere(Sphere *s, const Array<Surface::Point> &points) const {
  Array<Point3D> pts;
  convertPoints(&pts, points);
  return fitSphere(s, pts);
}

//  fit the sphere around a sub set of the samples
bool SFBase::fitSphere(Sphere *s, const Array<Surface::Point> &points,
                       const Array<int> &inds) const {
  Array<Point3D> pts;
  convertPoints(&pts, points, inds);
  return fitSphere(s, pts);
}

//  utilities
void SFBase::convertPoints(Array<Point3D> *dest,
                           const Array<Surface::Point> &points) {
  int numPts = points.getSize();
  dest->resize(numPts);

  for (int i = 0; i < numPts; i++)
    dest->index(i) = points.index(i).p;
}

void SFBase::convertPoints(Array<Point3D> *dest,
                           const Array<Surface::Point> &points,
                           const Array<int> &inds) {
  int numPts = inds.getSize();
  dest->resize(numPts);

  for (int i = 0; i < numPts; i++) {
    int pI = inds.index(i);
    dest->index(i) = points.index(pI).p;
  }
}

//  refit
double refitSphere(Sphere *s, const Array<Surface::Point> &pts,
                   const Array<int> &inds, const SEBase *se, const SFBase *sf,
                   const Point3D *p1, const Point3D *p2) {
  CHECK_DEBUG0(sf != NULL && se != NULL);
  CHECK_DEBUG0(s != NULL);

  //  decode params
  Point3D c[2];
  int numC = 0;
  if (p1) {
    c[0] = *p1;
    numC = 1;

    if (p2) {
      c[1] = *p2;
      numC = 2;
    }
  }

  //  records for the spheres
  struct SPHEREINFO {
    Sphere s;
    double err;
  } spheres[3];

  //  use the sphere fitting algorithm
  sf->fitSphere(&spheres[0].s, pts, inds);
  spheres[0].err = se->evalSphere(spheres[0].s);

  //  generate from existing centers
  int numInds = inds.getSize();
  for (int i = 0; i < numC; i++) {
    Point3D pC = c[i];
    float maxR = 0;
    for (int j = 0; j < numInds; j++) {
      int pI = inds.index(j);
      float d = pC.distanceSQR(pts.index(pI).p);
      if (d > maxR)
        maxR = d;
    }

    spheres[i + 1].s.c = pC;
    spheres[i + 1].s.r = sqrt(maxR);

    spheres[i + 1].err = se->evalSphere(spheres[i + 1].s);
  }

  //  search for sphere with min error
  int minI = -1;
  double minErr = FLT_MAX;
  for (int i = 0; i < numC + 1; i++) {
    double err = spheres[i].err;
    if (finite(err) && err < minErr) {
      minI = i;
      minErr = err;
    }
  }

  *s = spheres[minI].s;
  return minErr;
}
