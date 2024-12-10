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
    definition of line of form : ax+by*c = 0
*/
#ifndef _LINE_2D_H_
#define _LINE_2D_H_

#include "Point2D.h"
#include <math.h>

class Line2D {
public:
  REAL a, b, c;

  //  constructor
  __inline Line2D() {}

  __inline Line2D(REAL a, REAL b, REAL c) {
    this->a = a;
    this->b = b;
    this->c = c;
  }

  __inline Line2D(const Line2D &l) { assign(l); }

  __inline Line2D(const Point2D &p0, const Point2D &p1) { assign(p0, p1); }

  //  assignment
  __inline void assign(REAL a, REAL b, REAL c) {
    this->a = a;
    this->b = b;
    this->c = c;
  }

  __inline void assign(const Line2D &l) {
    a = l.a;
    b = l.b;
    c = l.c;
  }

  __inline void assign(const Point2D &p0, const Point2D &p1) {
    a = p0.y - p1.y;
    b = p1.x - p0.x;
    c = p1.y * p0.x - p1.x * p0.y;
  }

  //  intersection
  bool intersect(Point2D *p, const Line2D &l) const;

  //  distance
  __inline REAL distance(const Point2D &p) const {
    return p.x * a + p.y * b + c;
  }

  __inline REAL distanceNorm(const Point2D &p) const {
    return (p.x * a + p.y * b + c) / sqrt(a * a + b * b);
  }

  //  projection
  void project(Point2D *p, const Point2D &q) const;
};

#endif
