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

#include "CubeTri.h"
#include "pcube/pcube.h"
#include "Vector3D.h"

//  Graphics gems code uses a unit cube centered about the origin
//  therefore we need to transform the triangle into this space

bool overlapTest(/*const*/ Point3D /*&*/ pMin, float edgeLength,
                 const Point3D p[3], double growBy) {
  //  make box slightly larger just to be safe
  pMin.x -= growBy;
  pMin.y -= growBy;
  pMin.z -= growBy;
  edgeLength += 2 * growBy;

  Point3D pC;
  pC.x = pMin.x + edgeLength / 2.0;
  pC.y = pMin.y + edgeLength / 2.0;
  pC.z = pMin.z + edgeLength / 2.0;

  real verts[3][3];
  for (int i = 0; i < 3; i++) {
    verts[i][0] = (p[i].x - pC.x) / edgeLength;
    verts[i][1] = (p[i].y - pC.y) / edgeLength;
    verts[i][2] = (p[i].z - pC.z) / edgeLength;
  }

  //  compute the polygon normal
  Vector3D v1, v2, vC;
  v1.difference(p[1], p[0]);
  v2.difference(p[2], p[0]);
  vC.cross(v1, v2);
  vC.norm();

  real norm[3];
  norm[0] = vC.x;
  norm[1] = vC.y;
  norm[2] = vC.z;

  int res = fast_polygon_intersects_cube(3, verts, norm, 0, 0);
  return (res != 0); //  only ever returns 0/1 ??
}

bool overlapTest(/*const*/ Point3D /*&*/ pMin, /*const*/ Point3D /*&*/ pMax,
                 const Point3D p[3], double growBy) {
  //  make box slightly larger just to be safe
  pMin.x -= growBy;
  pMin.y -= growBy;
  pMin.z -= growBy;
  pMax.x += growBy;
  pMax.y += growBy;
  pMax.z += growBy;

  Point3D pC;
  pC.x = (pMin.x + pMax.x) / 2.0;
  pC.y = (pMin.y + pMax.y) / 2.0;
  pC.z = (pMin.z + pMax.z) / 2.0;

  Vector3D v;
  v.difference(pMax, pMin);

  real verts[3][3];
  for (int i = 0; i < 3; i++) {
    verts[i][0] = (p[i].x - pC.x) / v.x;
    verts[i][1] = (p[i].y - pC.y) / v.y;
    verts[i][2] = (p[i].z - pC.z) / v.z;
  }

  //  compute the polygon normal
  Vector3D v1, v2, vC;
  v1.difference(p[1], p[0]);
  v2.difference(p[2], p[0]);
  vC.cross(v1, v2);
  vC.norm();

  real norm[3];
  norm[0] = vC.x;
  norm[1] = vC.y;
  norm[2] = vC.z;

  return (fast_polygon_intersects_cube(3, verts, norm, 0, 0) != 0);
}
