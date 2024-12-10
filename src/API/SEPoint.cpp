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

#include "SEPoint.h"
#include "../Geometry/Plane.h"
#include "../Exceptions/CheckDebug.h"

//  constructors
SEPoint::SEPoint() { samples = NULL; }

SEPoint::SEPoint(const Array<Surface::Point> &samples) {
  setSurfaceSamples(samples);
}

//  setup the set of sample points
void SEPoint::setSurfaceSamples(const Array<Surface::Point> &samples) {
  this->samples = &samples;
}

//  evaluate the fit of the given sphere
float SEPoint::evalSphere(const Sphere &s) const {
  CHECK_DEBUG(samples != NULL,
              "Set of sample points needed : use setSurfaceSamples");

  float maxD = 0;
  int numPts = samples->getSize();
  for (int i = 0; i < numPts; i++) {
    Surface::Point p = samples->index(i);
    if (s.contains(p.p)) {
      float d = eval(s, p.p, p.n);
      if (d > maxD)
        maxD = d;
    }
  }

  return maxD;
}

float SEPoint::eval(const Sphere &s, const Point3D &p,
                    const Vector3D &n) const {
  //  construct plane on object at point
  Plane pl;
  pl.assign(n, p);

  //  project sphere onto plane (along normal)
  Point3D cP;
  pl.projectOnto(&cP, s.c, n);

  //  work out if s.c is infront of plane
  Vector3D nN, vC;
  nN.norm(n);
  vC.difference(s.c, p);
  vC.norm();
  float dt = vC.dot(nN); // dt < 0 means behind face

  //  evaluate distance
  float d1 = s.c.distance(cP);
  float d2 = p.distance(cP);
  float d = sqrt(s.r * s.r - d2 * d2);
  if (dt < 0) {
    //  behind face
    return d - d1;
  } else {
    return d + d1;
  }
}

float SEPoint::evalSphere(const Sphere &s, const Array<Surface::Point> &surPts,
                          const Array<int> &inds) const {
  float maxD = 0;
  int numInds = inds.getSize();
  for (int i = 0; i < numInds; i++) {
    Surface::Point p = surPts.index(inds.index(i));
    float d = eval(s, p.p, p.n);
    if (d > maxD)
      maxD = d;
  }

  return maxD;
}
