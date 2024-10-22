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
    Definition of a Vector in 3D Euclidean space
*/
#ifndef _VECTOR_3D_H_
#define _VECTOR_3D_H_

#include <stdlib.h>
#include "../Base/Types.h"
#include "../Exceptions/CheckDebug.h"
#include "Point3D.h"

struct Vector3D {
  REAL x, y, z;

  __inline void difference(const Point3D &p, const Point3D &q) {
    x = p.x - q.x;
    y = p.y - q.y;
    z = p.z - q.z;
  }

  //  assignment
  __inline void assign(REAL xN, REAL yN, REAL zN) {
    x = xN;
    y = yN;
    z = zN;
  }

  __inline void assign(const Vector3D &v) {
    x = v.x;
    y = v.y;
    z = v.z;
  }

  __inline void assign(const Point3D &p) {
    x = p.x;
    y = p.y;
    z = p.z;
  }

  //  comparison
  __inline bool equals(const Vector3D &v) const {
    return (fabs(v.x - x) < EPSILON && fabs(v.y - y) < EPSILON &&
            fabs(v.z - z) < EPSILON);
  }

  //  application
  __inline void add(Point3D *p, REAL mult = 1.0f) const {
    p->x += x * mult;
    p->y += y * mult;
    p->z += z * mult;
  }

  __inline void add(Point3D *p, const Point3D &q, REAL mult = 1.0f) const {
    CHECK_DEBUG(p != NULL, "NULL Destination");

    p->x = q.x + x * mult;
    p->y = q.y + y * mult;
    p->z = q.z + z * mult;
  }

  __inline void sub(Point3D *p) const {
    CHECK_DEBUG(p != NULL, "NULL Destination");

    p->x -= x;
    p->y -= y;
    p->z -= z;
  }

  __inline void sub(Point3D *p, const Point3D &q) const {
    CHECK_DEBUG(p != NULL, "NULL Destination");

    p->x = q.x - x;
    p->y = q.y - y;
    p->z = q.z - z;
  }

  //  vector arithmetic
  __inline void add(const Vector3D &v, REAL mult = 1.0f) {
    x += v.x * mult;
    y += v.y * mult;
    z += v.z * mult;
  }

  __inline void add(const Vector3D &v1, const Vector3D &v2, REAL mult = 1.0f) {
    x = v1.x + v2.x * mult;
    y = v1.y + v2.y * mult;
    z = v1.z + v2.z * mult;
  }

  __inline void sub(const Vector3D &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
  }

  __inline void sub(const Vector3D &v, const Vector3D &v1) {
    x = v.x - v1.x;
    y = v.y - v1.y;
    z = v.z - v1.z;
  }

  __inline void scale(REAL s) {
    x *= s;
    y *= s;
    z *= s;
  }

  __inline void scale(const Vector3D &v, REAL s) {
    x = v.x * s;
    y = v.y * s;
    z = v.z * s;
  }

  //  DOT
  __inline REAL dot(const Vector3D &v) const {
    return x * v.x + y * v.y + z * v.z;
  }

  //  CROSS prod
  __inline void cross(const Vector3D &v1, const Vector3D &v2) {
    x = v1.y * v2.z - v1.z * v2.y;
    y = v1.z * v2.x - v1.x * v2.z;
    z = v1.x * v2.y - v1.y * v2.x;
  }

  //  mag
  __inline REAL mag() const { return (REAL)sqrt(x * x + y * y + z * z); }

  __inline REAL magSQR() const { return (REAL)(x * x + y * y + z * z); }

  //
  REAL getAngle(const Vector3D &v) const;

  //  normalise
  void norm();
  void norm(const Vector3D &v);

  //  statics
  const static Vector3D ZERO;
  const static Vector3D X;
  const static Vector3D Y;
  const static Vector3D Z;
};

#endif
