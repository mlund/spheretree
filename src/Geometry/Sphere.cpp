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

#include "Sphere.h"

#include <math.h>

const Sphere Sphere::ZERO = {0, 0, 0, 0};
const Sphere Sphere::UNIT = {0, 0, 0, 1};
const Sphere Sphere::INVALID = {0, 0, 0, -1};

float Sphere::overlapVolume(const Sphere &other) const {
  float d = this->c.distance(other.c);
  float r1 = this->r, r2 = other.r;
  if (r1 > r2) {
    r1 = other.r;
    r2 = this->r;
  }

  if (d > r2 + r1) {
    return 0.0f; //  no overlap
  } else if (d <= r2 - r1 || d < EPSILON) {
    float r13 = r1 * r1 * r1;
    float v = 4.0 * M_PI * r13 / 3.0f; //  small sphere fully contained
    return v;
  } else {
    float r13 = r1 * r1 * r1;
    float r23 = r2 * r2 * r2;
    float x1 = (d * d + r1 * r1 - r2 * r2) / (2.0 * d);
    float x2 = d - x1;
    float v = 2 * M_PI * (r13 + r23) / 3.0f -
              M_PI * (r1 * r1 * x1 + r2 * r2 * x2 - x1 * x1 * x1 / 3.0f -
                      x2 * x2 * x2 / 3.0f);
    return v;
  }
}

int Sphere::intersectRay(double t[2], const Point3D &rayOrigin,
                         const Vector3D &rayDirn) const {
  Vector3D oc;
  oc.difference(rayOrigin, c);
  float b = oc.dot(rayDirn);
  float c = oc.dot(oc) - r * r;
  float d = b * b - c;

  if (d < 0)
    return 0;
  else if (d < EPSILON) {
    t[0] = t[1] = -b;
    return 1;
  } else {
    float e = sqrt(d);
    t[0] = -b - e;
    t[1] = -b + e;

    Point3D p1, p2;
    rayDirn.add(&p1, rayOrigin, t[0]);
    rayDirn.add(&p2, rayOrigin, t[1]);
    CHECK_DEBUG0(t[0] < t[1]);
    return 2;
  }
}

bool Sphere::intersect(const Point3D &pMin, const Point3D &pMax) const {
#define DO_AXIS(a)                                                             \
  if (c.a < pMin.a) {                                                          \
    p.a = pMin.a;                                                              \
    float s = c.a - pMin.a;                                                    \
    d += s * s;                                                                \
    if (d > rS)                                                                \
      return false;                                                            \
  } else if (c.a > pMax.a) {                                                   \
    p.a = pMax.a;                                                              \
    float s = c.a - pMax.a;                                                    \
    d += s * s;                                                                \
    if (d > rS)                                                                \
      return false;                                                            \
  }

  float d = 0;
  float rS = r * r;
  Point3D p;
  DO_AXIS(x);
  DO_AXIS(y);
  DO_AXIS(z);

  return (d < rS);

#undef DO_AXIS
}
