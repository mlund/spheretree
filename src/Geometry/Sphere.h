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

#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Point3D.h"
#include "Vector3D.h"
#include "../Base/Defs.h"
#include "../Exceptions/CheckDebug.h"
#include <math.h>

struct Sphere {
  Point3D c;
  REAL r;

  __inline void assign(const Point3D &C, float R) {
    c = C;
    r = R;
  }

  __inline bool contains(const Point3D &pt, float tol = EPSILON) const {
    return (pt.distanceSQR(c) <= r * r + tol);
  }

  __inline bool overlap(const Sphere &s, float tol = EPSILON) const {
    double sR = s.r + r;
    return s.c.distanceSQR(c) <= sR * sR + tol;
  }

  __inline double volume() const { return 4.0 / 3.0 * M_PI * r * r * r; }

  float overlapVolume(const Sphere &other) const;

  int intersectRay(double t[2], const Point3D &rayOrigin,
                   const Vector3D &rayDirn) const;

  bool intersect(const Point3D &pMin, const Point3D &pMax) const;

  const static Sphere ZERO;
  const static Sphere UNIT;
  const static Sphere INVALID;
};

#endif