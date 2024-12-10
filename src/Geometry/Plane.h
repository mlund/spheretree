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

/*
    Parametric equation of a plane

      ax + by + cz + d = 0

    { a, b, c } is the normal to the plane

    Note : d is from plane equation, therefore it is measured by

      ^
      |       ^(0,0,1)
      |       |
      |  -----------
      |                    d = -100
     -----------  (Z = 0)
*/
#ifndef _PLANE_H_
#define _PLANE_H_

#include "../Base/Types.h"
#include "Point3D.h"
#include "Vector3D.h"
#include "Transform3D.h"
#include <math.h>

struct Plane {
  REAL a, b, c, d;

  //  constructor
  __inline Plane() {};

  __inline Plane(REAL a, REAL b, REAL c, REAL d) { assign(a, b, c, d); }

  __inline Plane(const Plane &p) { assign(p); }

  //  assign
  __inline void assign(const Plane &p) {
    a = p.a;
    b = p.b;
    c = p.c;
    d = p.d;
  }

  __inline void assign(const Vector3D &v, const Point3D &p) {
    Vector3D n;
    n.norm(v);

    a = n.x;
    b = n.y;
    c = n.z;
    d = -(n.x * p.x + n.y * p.y + n.z * p.z);
  }

  __inline void assign(REAL a, REAL b, REAL c, REAL d) {
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
  }

  //  construct plane from 3 points
  void assign(const Point3D &p1, const Point3D &p2, const Point3D &p3);

  //  distance from plane
  __inline REAL dist(const Point3D &pt) const {
    return (a * pt.x + b * pt.y + c * pt.z + d) / sqrt(a * a + b * b + c * c);
  }

  __inline REAL absDist(const Point3D &pt) const { return fabs(dist(pt)); }

  //  normalize
  void normalise();

  //  vector intersection test
  __inline REAL intersect(const Vector3D &v) const {
    //  vector is always coming from origin
    return -d / (a * v.x + b * v.y + c * v.z);
  }

  //  transform
  void transform(Plane *p, const Transform3D tr) const;

  //  projection onto plane
  REAL projectOnto(Point3D *p, const Point3D &q, const Vector3D &v) const;

  //  transform to standard 2D plane
  void getTransformTo2D(Transform3D *tr) const;
  void getNormal(Vector3D *v) const;

private:
  void getPoint(Point3D *p, REAL u, REAL v) const;
};

#endif