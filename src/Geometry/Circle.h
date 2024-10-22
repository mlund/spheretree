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
    Circle on a 2D Euclidean plane
*/
#ifndef _CIRCLE_H_
#define _CIRCLE_H_

#include "Point2D.h"
#include "Transform2D.h"
#include <math.h>

struct Circle {
public:
  Point2D c;
  REAL r;

  //  construction
  __inline Circle() {}

  __inline Circle(const Point2D &p, REAL r) { assign(p, r); }

  __inline Circle(const Circle &c) { assign(c); }

  __inline Circle(const Point2D &pC, const Point2D &pR) { assign(pC, pR); }

  __inline void assign(const Point2D &p, REAL r) {
    this->c = p;
    this->r = r;
  }

  __inline void assign(const Circle &c) {
    this->c = c.c;
    this->r = fabs(c.r);
  }

  __inline void assign(const Point2D &pC, const Point2D &pR) {
    c = pC;
    r = pC.distance(pR);
  }

  //  inside test
  __inline bool isInside(const Point2D &p) const { return c.distance(p) <= r; }

  //  distance from edge (signed -ve for inside)
  __inline REAL distance(const Point2D &p) const {
    REAL d = c.distance(p);
    return d - r;
  }

  //  create from 3 points
  void createCircle(const Point2D &p0, const Point2D &p1, const Point2D &p2);
  void createIncircle(const Point2D &p0, const Point2D &p1, const Point2D &p2);

  //  get point on circle
  void getPoint(Point2D *p, REAL ang) const;

  //  get angle of point
  REAL getAngle(const Point2D &p) const;

  //  transform
  void transform(Circle *cir, const Transform2D &tr) const;

  //  bounding box
  void getBoundingBox(Point2D *pMin, Point2D *pMax) const;

  //  intersection
  int intersect(Point2D p[2], const Circle &other) const;
  int intersect(Point2D p[2], const Point2D &p1, const Point2D &p2) const;

  //  project
  void project(Point2D *p, const Point2D &q) const;
};

#endif
