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

#include "Circle.h"
#include "Angles.h"
#include "../Base/Defs.h"

void Circle::getPoint(Point2D *p, REAL ang) const {
  CHECK_DEBUG(p != NULL, "NULL Destination");

  ang = RAD(ang);

  p->x = c.x + r * cos(ang);
  p->y = c.y + r * sin(ang);
}

REAL Circle::getAngle(const Point2D &p) const {
  REAL dX = p.x - c.x;
  REAL dY = p.y - c.y;
  return DEG(atan2(dY, dX)); //  -180..180
}

//   create by circumcircle, see...
void Circle::createCircle(const Point2D &p1, const Point2D &p2,
                          const Point2D &p3) {
  //  squares
  REAL s1 = SQR(p1.x) + SQR(p1.y);
  REAL s2 = SQR(p2.x) + SQR(p2.y);
  REAL s3 = SQR(p3.x) + SQR(p3.y);

  //  determinants
  REAL a = p1.x * (p2.y - p3.y) + p3.x * (p1.y - p2.y) + p2.x * (p3.y - p1.y);
  REAL d = s1 * (p2.y - p3.y) + s3 * (p1.y - p2.y) + s2 * (p3.y - p1.y);
  REAL f = s1 * (p2.x - p3.x) + s3 * (p1.x - p2.x) + s2 * (p3.x - p1.x);

  c.x = d / (2.0 * a);
  c.y = -f / (2.0 * a);
  r = c.distance(p1);

  /*//  alternate formula
    //  formula presented by OLSON, except division by 2
    double t1 = SQR(p3.x) + SQR(p3.y) - SQR(p2.x) - SQR(p2.y);
    double t2 = SQR(p1.x) + SQR(p1.y) - SQR(p2.x) - SQR(p2.y);
    double b = (p3.x - p2.x)*(p1.y - p2.y) - (p3.y - p2.y)*(p1.x - p2.x);
    c.x = ((p1.y - p2.y)*t1 + (p2.y - p3.y)*t2) / b / 2;
    c.y = ((p2.x - p1.x)*t1 + (p3.x - p2.x)*t2) / b / 2;
    r = c.distance(p1);
  */
}

#define AVERAGE_PT(a, b, c)                                                    \
  {                                                                            \
    a.x = (b.x + c.x) / 2.0f;                                                  \
    a.y = (b.y + c.y) / 2.0f;                                                  \
  }

void Circle::createIncircle(const Point2D &p0, const Point2D &p1,
                            const Point2D &p2) {
  Point2D p01, p12, p20;
  AVERAGE_PT(p01, p0, p1);
  AVERAGE_PT(p12, p1, p2);
  AVERAGE_PT(p20, p2, p0);

  createCircle(p01, p12, p20);
}

//  transform
void Circle::transform(Circle *cir, const Transform2D &tr) const {
  CHECK_DEBUG(cir != NULL, "NULL destination");

  //  transform center
  tr.transform(&cir->c, c);

  //  compute new radius
  Point2D p, q;
  getPoint(&p, 0);
  tr.transform(&q, p);
  cir->r = cir->c.distance(q);
}

//  bounding box
void Circle::getBoundingBox(Point2D *pMin, Point2D *pMax) const {
  CHECK_DEBUG(pMin != NULL && pMax != NULL, "NULL destination");

  pMin->x = c.x - r;
  pMin->y = c.y - r;

  pMax->x = c.x + r;
  pMax->y = c.y + r;
}

int Circle::intersect(Point2D p[2], const Circle &other) const {
  CHECK_DEBUG(p != NULL, "NULL Destination");

  REAL d = c.distance(other.c);
  REAL a = (r * r - other.r * other.r + d * d) / (2 * d);

  Point2D p2;
  p2.x = c.x + (a / d) * (other.c.x - c.x);
  p2.y = c.y + (a / d) * (other.c.y - c.y);

  REAL h = sqrt(r * r - a * a);
  if (!finite(h)) {
    //  no solution
    return 0;
  } else if (fabs(h) < EPSILON || h / d < 0.05) {
    //  one solution
    p[0] = p2;
    return 1;
  } else {
    //  two solutions
    p[0].x = p2.x + (h / d) * (other.c.y - c.y);
    p[0].y = p2.y - (h / d) * (other.c.x - c.x);

    p[1].x = p2.x - (h / d) * (other.c.y - c.y);
    p[1].y = p2.y + (h / d) * (other.c.x - c.x);

    return 2;
  }
}

int Circle::intersect(Point2D p[2], const Point2D &p0,
                      const Point2D &p1) const {
  CHECK_DEBUG(p != NULL, "NULL Destination");

  //  form quadratic
  REAL A, B, C;
  A = SQR(p1.x - p0.x) + SQR(p1.y - p0.y);
  B = 2 * (p1.x - p0.x) * (p0.x - c.x) + 2 * (p1.y - p0.y) * (p0.y - c.y);
  C = SQR(c.x) + SQR(c.y) + SQR(p0.x) + SQR(p0.y) -
      2 * (c.x * p0.x + c.y * p0.y) - SQR(r);

  // u = -b +/- sqrt(b^2 -4ac) / 2a
  REAL d = B * B - 4 * A * C;
  if (d < 0)
    return 0;

  REAL dX = p1.x - p0.x;
  REAL dY = p1.y - p0.y;

  d = sqrt(d) / (2 * A);
  if (d < 0.005) {
    //  one solution
    REAL u = -B / (2 * A);
    p[0].x = p0.x + u * dX;
    p[0].y = p0.y + u * dY;
    return 1;
  } else {
    //  two solutions
    REAL u = -B / (2 * A) + d;
    p[0].x = p0.x + u * dX;
    p[0].y = p0.y + u * dY;

    u = -B / (2 * A) - d;
    p[1].x = p0.x + u * dX;
    p[1].y = p0.y + u * dY;
    return 2;
  }
}

void Circle::project(Point2D *p, const Point2D &q) const {
  CHECK_DEBUG(p != NULL, "NULL Destination");

  getPoint(p, getAngle(q));
}
