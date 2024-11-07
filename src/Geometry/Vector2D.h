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
    Definition of a Vector in 2D Euclidean space
*/
#ifndef _VECTOR_2D_H_
#define _VECTOR_2D_H_

#include <stdlib.h>
#include "../Base/Types.h"
#include "../Exceptions/CheckDebug.h"
#include "Point2D.h"

struct Vector2D {
  REAL x, y;

  __inline void difference(const Point2D &p, const Point2D &q) {
    x = p.x - q.x;
    y = p.y - q.y;
  }

  //  assignment
  __inline void assign(REAL xN, REAL yN) {
    x = xN;
    y = yN;
  }

  __inline void assign(const Vector2D &v) {
    x = v.x;
    y = v.y;
  }

  //  comparison
  __inline bool equals(const Vector2D &v) const {
    return (fabs(v.x - x) < EPSILON && fabs(v.y - y) < EPSILON);
  }

  //  application to point (should be in point ??)
  __inline void add(Point2D *p) const {
    CHECK_DEBUG(p != NULL, "NULL Destination");

    p->x += x;
    p->y += y;
  }

  __inline void add(Point2D *p, const Point2D &q, REAL mult = 1.0f) const {
    CHECK_DEBUG(p != NULL, "NULL Destination");

    p->x = q.x + x * mult;
    p->y = q.y + y * mult;
  }

  __inline void sub(Point2D *p) const {
    CHECK_DEBUG(p != NULL, "NULL Destination");

    p->x -= x;
    p->y -= y;
  }

  __inline void sub(Point2D *p, const Point2D &q) const {
    CHECK_DEBUG(p != NULL, "NULL Destination");

    p->x = q.x - x;
    p->y = q.y - y;
  }

  __inline void add(const Vector2D &v) {
    x += v.x;
    y += v.y;
  }

  __inline void add(const Vector2D &v1, const Vector2D &v2) {
    x = v1.x + v2.x;
    y = v1.y + v2.y;
  }

  __inline void sub(const Vector2D &v) {
    x -= v.x;
    y -= v.y;
  }

  __inline void sub(const Vector2D &v, const Vector2D &v1) {
    x = v.x - v1.x;
    y = v.y - v1.y;
  }

  __inline void scale(REAL s) {
    x *= s;
    y *= s;
  }

  __inline void scale(const Vector2D &v, REAL s) {
    x = v.x * s;
    y = v.y * s;
  }

  //  DOT
  __inline REAL dot(const Vector2D &v) const { return x * v.x + y * v.y; }

  __inline REAL mag() const { return (REAL)sqrt(x * x + y * y); }

  //  angles
  REAL angle(const Vector2D &v) const;
  REAL angle() const;

  //  normalise
  void norm();
  void norm(const Vector2D &v);

  //  statics
  const static Vector2D ZERO;
};

#endif
