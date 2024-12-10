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

#include "LineSeg.h"
#include "Vector2D.h"
#include "../Exceptions/CheckDebug.h"
#include <stdlib.h>
#include <math.h>

//  distance
REAL LineSeg::distance(const Point2D &p) const {
  Line2D l;
  getLine(&l);

  //  project point onto line
  Point2D pProj;
  l.project(&pProj, p);

  if (isInSegment(pProj)) {
    //  point projects onto line
    return fabs(l.distanceNorm(p));
  } else {
    REAL d0 = p0.distanceSQR(p);
    REAL d1 = p1.distanceSQR(p);
    if (d1 < d0)
      d0 = d1;

    return sqrt(d0);
  }
}

//  distance
REAL LineSeg::distanceFrom2DLine(const Point2D &p) const {
  Line2D l;
  getLine(&l);
  return fabs(l.distanceNorm(p));
}

//  transform
void LineSeg::transform(LineSeg *seg, Transform2D tr) const {
  CHECK_DEBUG(seg != NULL, "NULL destination");
  tr.transform(&seg->p0, p0);
  tr.transform(&seg->p1, p1);
}

//  bounding box
void LineSeg::getBoundingBox(Point2D *pMin, Point2D *pMax) const {
  CHECK_DEBUG(pMin != NULL && pMax != NULL, "NULL destination");

  *pMin = p0;
  *pMax = p0;

  pMin->storeMin(p1);
  pMax->storeMax(p1);
}

bool LineSeg::isInSegment(const Point2D &p) const {
  /*  Point2D pMin, pMax;
    getBoundingBox(&pMin, &pMax);

    return p.x >= pMin.x-EPSILON && p.x <= pMax.x+EPSILON &&
           p.y >= pMin.y-EPSILON && p.y <= pMax.y+EPSILON;
  */
  REAL d0 = p0.distance(p);
  REAL d1 = p1.distance(p);
  REAL len = p0.distance(p1);

  return d0 <= len && d1 < len;
}

//  intersection
bool LineSeg::intersect(Point2D *p, const LineSeg &other) const {
  CHECK_DEBUG(p != NULL, "NULL Destination");

  Line2D l1, l2;
  getLine(&l1);
  other.getLine(&l2);

  return l1.intersect(p, l2) && isInSegment(*p) && other.isInSegment(*p);
}

REAL LineSeg::closestPoint(Point2D *p, const Point2D &other) const {
  CHECK_DEBUG(p != NULL, "NULL Destination");

  //  project onto line
  if (projectOnto(p, other))
    return other.distance(*p);

  //  else choose closest end point
  if (p1.distance(other) < p0.distance(other))
    p->assign(p1);
  else
    p->assign(p0);

  return other.distance(*p);
}

//  project
bool LineSeg::projectOnto(Point2D *p, const Point2D &q) const {
  Vector2D v0, v1;
  v0.difference(p1, p0);
  v1.difference(q, p0);
  v0.norm();

  float t = v1.dot(v0);
  v0.add(p, p0, t);

  float len = p0.distance(p1);
  if (t < 0 || t > len)
    return false;
  else
    return true;
}
