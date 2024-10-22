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

#include "SFRitter.h"
#include "../Base/Defs.h"

bool SFRitter::makeSphere(Sphere *s, const Array<Surface::Point> &points,
                          const Array<int> &inds) {
  Array<Point3D> pts;
  convertPoints(&pts, points, inds);
  return makeSphere(s, pts);
}

bool SFRitter::makeSphere(Sphere *s, const Array<Surface::Point> &points) {
  Array<Point3D> pts;
  convertPoints(&pts, points);
  return makeSphere(s, pts);
}

//  algorithm implementation
bool SFRitter::makeSphere(Sphere *s, const Array<Point3D> &pts) {
  int numPts = pts.getSize();
  if (numPts == 1) {
    s->c = pts.index(0);
    s->r = 0;
    return true;
  } else if (numPts == 0) {
    s->c.x = s->c.y = s->c.z = 0;
    s->r = 0;
    return false;
  }

  //  first pass - find maxima/minima points for the 3 axes
  Point3D xMin, xMax, yMin, yMax, zMin, zMax;
  xMin.x = yMin.y = zMin.z = REAL_MAX;
  xMax.x = yMax.y = zMax.z = REAL_MIN;
  for (int i = 0; i < numPts; i++) {
    Point3D p = pts.index(i);

    if (p.x < xMin.x)
      xMin = p;
    if (p.x > xMax.x)
      xMax = p;

    if (p.y < yMin.y)
      yMin = p;
    if (p.y > yMax.y)
      yMax = p;

    if (p.z < zMin.z)
      zMin = p;
    if (p.z > zMax.z)
      zMax = p;
  }

  //  x-span is square distance between xmin & xmax
  float xspan = xMin.distanceSQR(xMax);
  float yspan = yMin.distanceSQR(yMax);
  float zspan = zMin.distanceSQR(zMax);

  //  dMin and dMax are the maximally separated pair
  float maxSpan = xspan;
  Point3D dMin = xMin, dMax = xMax;
  if (yspan > maxSpan) {
    maxSpan = yspan;
    dMin = yMin;
    dMax = yMax;
  }
  if (zspan > maxSpan) {
    maxSpan = zspan;
    dMin = zMin;
    dMax = zMax;
  }

  //  work out center and radius of sphere
  Point3D pC;
  pC.x = (dMin.x + dMax.x) / 2.0f;
  pC.y = (dMin.y + dMax.y) / 2.0f;
  pC.z = (dMin.z + dMax.z) / 2.0f;
  float rCS = pC.distanceSQR(dMax);
  float rC = sqrt(rCS);

  //  second pass - increment current sphere
  for (int i = 0; i < numPts; i++) {
    Point3D p = pts.index(i);

    float oldToPS = pC.distanceSQR(p);
    if (oldToPS > rCS) {
      //  this point is outside the sphere
      float oldToP = sqrt(oldToPS);
      rC = (rC + oldToP) / 2.0;
      rCS = rC * rC;
      float oldToNew = oldToP - rC;

      //  new sphere center
      pC.x = (rC * pC.x + oldToNew * p.x) / oldToP;
      pC.y = (rC * pC.y + oldToNew * p.y) / oldToP;
      pC.z = (rC * pC.z + oldToNew * p.z) / oldToP;
    }
  }

  s->c = pC;
  s->r = rC;

  return (finite(s->r) != 0);
}
