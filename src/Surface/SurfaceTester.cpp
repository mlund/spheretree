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

#include "SurfaceTester.h"
#include "../Geometry/Ray.h"

SurfaceTester::SurfaceTester(){
  sur = NULL;
}

bool SurfaceTester::hasSurface() const{
  return sur != NULL;
}

void SurfaceTester::setSurface(const Surface &sur, int shDiv, int vtLevel){
  this->sur = &sur;

  //  setup spacial hash
  Vector3D v;
  Point3D pMin, pMax;
  v.difference(sur.pMax, sur.pMin);
  v.add(&pMin, sur.pMin, -0.1f);
  v.add(&pMax, sur.pMax, 0.1f);

  //   NOTE : WHEN USING GET-CELLS IT IS BETTER TO MAKE THE HASH SMALLER THAN FOR RINGS
  faceHash.setup(pMin, pMax, shDiv);
  faceHashHiRes.setup(pMin, pMax, 2*shDiv);
  int numTri = sur.triangles.getSize();
  for (int i = 0; i < numTri; i++){
    faceHash.addTriangle(sur, i);
    faceHashHiRes.addTriangle(sur, i);
    }

  //  setup the VoxelTree
  vt.setup(sur, faceHash, faceHashHiRes, vtLevel);
}

bool SurfaceTester::insideSurface(const Point3D &q) const{
  return /*insideBounds(q) &&*/ vt.insideSurface(q);
}

bool SurfaceTester::insideSurface(Point3D *p, const Point3D &q) const{
  //  do the surface test
  ClosestPointInfo inf;
  bool inSurface = ::insideSurface(q, *sur, faceHash, faceHashHiRes, &inf, -FLT_MAX, FALSE);
  CHECK_DEBUG0(inf.type != DIST_TYPE_INVALID);
  *p = inf.pClose;
  return inSurface;
}

void SurfaceTester::getClosestPoint(Point3D *p, const Point3D &q) const{
  ClosestPointInfo inf;
  ::getClosestPoint(&inf, q, *sur, faceHash);
  *p = inf.pClose;
}

bool SurfaceTester::getClosestPointConditional(Point3D *p, const Point3D &q, bool internal, float stopBelow) const{
  ClosestPointInfo inf;
  inf.type = DIST_TYPE_INVALID;
  bool in = vt.insideSurface(q, &inf);
  if (in == internal && inf.type == DIST_TYPE_INVALID){
    ::getClosestPoint(&inf, q, *sur, faceHash, stopBelow);
    *p = inf.pClose;
    }
  else
    *p = inf.pClose;

  return in;
}

//  ray tester
float SurfaceTester::getFirstIntersectPositive(Point3D *p, const Point3D &o, const Vector3D &v) const{
  SpacialHash::Stepper stepper;
  stepper.sh = &faceHash;

  Array<int> *cell = stepper.discreteSetup(o, v);
  while (cell){
    int minI = -1;
    float minT = FLT_MAX;
    int numTri = cell->getSize();
    for (int i = 0; i < numTri; i++){
      //  get triangle
      int tI = cell->index(i);

      //  make triangle
      Point3D p[3];
      const Surface::Triangle *tri = &sur->triangles.index(tI);
      p[0] = sur->vertices.index(tri->v[0]).p;
      p[1] = sur->vertices.index(tri->v[1]).p;
      p[2] = sur->vertices.index(tri->v[2]).p;

      //  find intersect
      float t = -1;
      if (intersectTriangle(o, v, &t, p[2], p[1], p[0]) && t >= 0){
        if (t < minT){
          minT = t;
          minI = i;
          }
        }
      }

    if (minI >= 0){
      v.add(p, o, minT);
      return minT;
      }

    cell = stepper.discreteStep();
    }

  return -1;
}

float SurfaceTester::getFirstIntersect(Point3D *p, const Point3D &o, const Vector3D &v) const{
  float t[2];
  Point3D pt[2];

  t[0] = getFirstIntersectPositive(&pt[0], o, v);

  Vector3D vN = {-v.x, -v.y, -v.z};
  t[1] = getFirstIntersectPositive(&pt[1], o, vN);

  if (t[0] >= 0 && (t[0] < t[1] || t[1] < 0)){
    *p = pt[0];
    return t[0];
    }
  else if (t[1] >= 0){
    *p = pt[1];
    return -t[1];     //  negative
    }
  else 
    return acos(10);
}
