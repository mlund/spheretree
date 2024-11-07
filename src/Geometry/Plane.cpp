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

#include <math.h>
#include <stdlib.h>
#include <float.h>

#include "Plane.h"
#include "Point3D.h"
#include "Vector3D.h"

//  assign
void Plane::assign(const Point3D &p1, const Point3D &p2, const Point3D &p3) {
  Vector3D v1, v2;
  v1.difference(p3, p2);
  v2.difference(p1, p2);

  Vector3D vC;
  vC.cross(v1, v2);
  vC.norm();

  assign(vC, p1);
}

void Plane::transform(Plane *plT, const Transform3D tr) const {
  CHECK_DEBUG(plT != NULL, "NULL Destination");

  Point3D p, pT;
  getPoint(&p, 0, 0);
  tr.transform(&pT, p);

  Vector3D vN, vT;
  getNormal(&vN);
  tr.transform(&vT, vN);

  plT->a = vT.x;
  plT->b = vT.y;
  plT->c = vT.z;
  plT->d = -(vT.x * pT.x + vT.y * pT.y + vT.z * pT.z);
}

void Plane::getPoint(Point3D *p, REAL u, REAL v) const {
  CHECK_DEBUG(p != NULL, "NULL Destination");

  if (c != 0) {
    p->x = u;
    p->y = v;
    p->z = -(a * u + b * v + d) / c;
  } else if (b != 0) {
    p->x = u;
    p->y = -(a * u + c * v + d) / b;
    p->z = v;
  } else {
    p->x = -(b * u + c * v + d) / a;
    p->y = u;
    p->z = v;
  }
}

void Plane::getNormal(Vector3D *v) const {
  CHECK_DEBUG(v != NULL, "NULL Destination");

  v->x = a;
  v->y = b;
  v->z = c;
  v->norm();
}

void Plane::getTransformTo2D(Transform3D *tr) const {
  CHECK_DEBUG(tr != NULL, "NULL Destination");

  Vector3D vN;
  getNormal(&vN);

  tr->alignToZ(vN);
}

REAL Plane::projectOnto(Point3D *p, const Point3D &q, const Vector3D &v) const {
  CHECK_DEBUG(p != NULL, "NULL Destination");

  float t = -(q.x * a + q.y * b + q.z * c + d) / (v.x * a + b * v.y + c * v.z);
  p->x = q.x + t * v.x;
  p->y = q.y + t * v.y;
  p->z = q.z + t * v.z;

  return t;
}

//  normalize
void Plane::normalise() {
  float mag = sqrt(a * a + b * b + c * c);
  a /= mag;
  b /= mag;
  c /= mag;
  d /= mag;
}
