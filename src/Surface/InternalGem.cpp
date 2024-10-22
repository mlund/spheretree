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
    Ported from Graphics Gems V Point in Polyhedron Testing using Sphereical
   Polygons Paulo Cezar Pinto Carvalho and Paulo Roma Cavalcanti (Pages 42-49)
*/

#define PI 3.141592653589793324

#include "InternalGem.h"
#include <algorithm>
/*=========================  geo_solid_angle  =========================
  Calculates the solid angle given by the spherical projection of
  a 3D plane polygon
*/
double geo_solid_angle(const Point3D &pTest, const Point3D p[3]) {
  int i;
  double area = 0.0, ang, s, l1, l2;
  Vector3D r1, b, n1, n2, tmp;

  //  get the normal to the triangle  -  anti clockwise
  Vector3D e1, e2, plane;
  e1.difference(p[1], p[0]);
  e2.difference(p[2], p[0]);
  plane.cross(e1, e2);
  plane.norm();

  /*
    WARNING: at this point, a practical implementation should check
    whether p is too close to the polygon plane. If it is, then
    there are two possibilities:
      a) if the projection of p onto the plane is outside the
         polygon, then area zero should be returned;
      b) otherwise, p is on the polyhedron boundary.
  */
  Point3D p1 = p[0];
  Point3D p2 = p[2];
  Vector3D a;
  a.difference(p2, p1);

  for (i = 0; i < 3; i++) {
    r1.difference(pTest, p1);
    p2 = p[(i + 1) % 3];
    b.difference(p2, p1);

    n1.cross(a, r1);
    float l1 = n1.mag();

    n2.cross(r1, b);
    float l2 = n2.mag();

    float s = n1.dot(n2) / (l1 * l2);
    float ang = acos(std::max(-1.0f, std::min(1.0f, s)));

    tmp.cross(b, a);
    s = tmp.dot(plane);
    area += s > 0.0 ? PI - ang : PI + ang;

    a.scale(b, -1);
    p1 = p2;
  }
  area -= PI;

  return (plane.dot(r1) > 0.0) ? -area : area;
}

bool insideSurfaceGem(const Point3D &p, const Surface &s) {
  double area = 0;
  int numTri = s.triangles.getSize();
  for (int i = 0; i < numTri; i++) {
    const Surface::Triangle *tri = &s.triangles.index(i);

    Point3D P[3];
    P[0] = s.vertices.index(tri->v[0]).p;
    P[1] = s.vertices.index(tri->v[1]).p;
    P[2] = s.vertices.index(tri->v[2]).p;

    area += geo_solid_angle(p, P);
  }

  return (area > 2 * PI) || (area < -2 * PI);
}
