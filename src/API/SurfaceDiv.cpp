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

#include "SurfaceDiv.h"
#include "../MinMax/Simplex.h"

/*
    setup the dividing planes between the sets of spheres
*/
void SurfaceDivision::setup(const Array<Sphere> &spheres,
                            const Array<Surface::Point> *surPts) {
  //  copy the spheres
  this->spheres.clone(spheres);
  int numSph = spheres.getSize();

  //  setup the matrix of planes
  const double INF = acos(10);
  if (planes.getM())
    planes.free();
  planes.allocate(numSph, numSph);

  //  create planes
  for (int i = 0; i < numSph; i++) {
    planes.index(i, i).d = INF;

    Sphere testS = spheres.index(i);
    for (int j = i + 1; j < numSph; j++) {
      Sphere s = spheres.index(j);
      if (overlap(testS, s)) {
        //  make the plane
        Plane plane;
        makeDividingPlane(&plane, testS, s, surPts);

        //  store plane in the position for testS
        planes.index(i, j) = plane;

        //  invert plane
        plane.a *= -1;
        plane.b *= -1;
        plane.c *= -1;
        plane.d *= -1;

        //  store inverse plane in the position for s
        planes.index(j, i) = plane;
      } else {
        planes.index(i, j).d = INF;
        planes.index(j, i).d = INF;
      }
    }
  }
}

/*
    test point in region     --  NEED TO VERIFY THIS ALGORITHM
*/
bool SurfaceDivision::pointInRegion(const Point3D &p, int region) const {
  //  point needs to be in sphere
  if (!spheres.index(region).contains(p))
    return false;

  //  point needs to be infront of the planes for
  //  other spheres in which it is contained
  int numSph = spheres.getSize();
  for (int i = 0; i < numSph; i++) {
    if (i == region || !spheres.index(i).contains(p))
      continue;

    Plane pl = planes.index(region, i);
    if (finite(pl.d) && pl.dist(p) <= 0)
      return false;
  }

  return true;
}

/*
    make the dividing plane between two spheres so that the points in testSph
   are infront of the plane
*/
void SurfaceDivision::makeDividingPlane(Plane *pl, const Sphere &testSph,
                                        const Sphere &s,
                                        const Array<Surface::Point> *surPts) {
  //  make dividing plane
  computeIntersectionPlane(pl, testSph, s);

  if (surPts) {
    //  list points in either sphere in various spheres
    Array<int> listTest, listS, listBoth;
    int numPts = surPts->getSize();
    for (int j = 0; j < numPts; j++) {
      Point3D p = surPts->index(j).p;

      bool inTest = testSph.contains(p, 0);
      bool inS = s.contains(p, 0);

      if (inTest && inS)
        listBoth.addItem() = j;
      else if (inTest)
        listTest.addItem() = j;
      else if (inS)
        listS.addItem() = j;
    }

    //  optimise the plane
    //       NOTE : order of listS and listT are vital
    optimiseHalfPlane(pl, *surPts, listBoth, listS, listTest);
  }
}

/*
    get the plane for segmenting two spheres along the plane of intersection
    such that the points inside testSph are infront of the plane

    method from http://www.imamod.ru/jour/conf/IMACS_2000/CP/314-4.pdf
*/
void SurfaceDivision::computeIntersectionPlane(Plane *pl, const Sphere &testSph,
                                               const Sphere &s) {
  Vector3D n;
  n.x = (testSph.c.x - s.c.x);
  n.y = (testSph.c.y - s.c.y);
  n.z = (testSph.c.z - s.c.z);

  //  compute plane from equation
  pl->a = n.x * 2;
  pl->b = n.y * 2;
  pl->c = n.z * 2;
  float d1 = testSph.r * testSph.r -
             (testSph.c.x * testSph.c.x + testSph.c.y * testSph.c.y +
              testSph.c.z * testSph.c.z);
  float d2 = s.r * s.r - (s.c.x * s.c.x + s.c.y * s.c.y + s.c.z * s.c.z);
  pl->d = d1 - d2;
}

/*
    Optimise the half plane between two spheres so that
*/
double SurfaceDivision::evaluatePlane(const Plane &testPlane,
                                      const Array<Surface::Point> &surPts,
                                      const Array<int> &inBoth,
                                      const Array<int> &inFront,
                                      const Array<int> &inBehind) {
  //  count the points on either size of the plane
  int numFront = inFront.getSize();
  int numBehind = inBehind.getSize();
  int numBoth = inBoth.getSize();
  for (int i = 0; i < numBoth; i++) {
    int pI = inBoth.index(i);
    Point3D p = surPts.index(pI).p;
    double d = testPlane.dist(p);

    if (d > 0)
      numFront++;
    else if (d < 0)
      numBehind++;
  }

  //  balance the sets of points
  return abs(numFront - numBehind);
}

double SurfaceDivision::halfPlaneOptFunc(double vals[], void *data,
                                         int *canFinish) {
  HalfPlaneOptInfo *info = (HalfPlaneOptInfo *)data;

  Plane testPlane;
  testPlane.a = vals[0];
  testPlane.b = vals[1];
  testPlane.c = vals[2];
  testPlane.d = vals[3];

  double met = evaluatePlane(testPlane, *info->surPts, *info->inBoth,
                             *info->inFront, *info->behind);

  return met;
}

void SurfaceDivision::optimiseHalfPlane(Plane *pl,
                                        const Array<Surface::Point> &surPts,
                                        const Array<int> &inBoth,
                                        const Array<int> &behind,
                                        const Array<int> &infront) {
  //  setup simplex
  HalfPlaneOptInfo inf;
  inf.surPts = &surPts;
  inf.inBoth = &inBoth;
  inf.inFront = &infront;
  inf.behind = &behind;

  double vals[4] = {pl->a, pl->b, pl->c, pl->d};
  double sizes[4] = {1, 1, 1, 1 + pl->d / 2};

  int numFunc = 100;
  simplexMin(4, vals, sizes, 1E-5, &numFunc, halfPlaneOptFunc, &inf, 1);

  pl->a = vals[0];
  pl->b = vals[1];
  pl->c = vals[2];
  pl->d = vals[3];
}

/*
    test for overlap between the spheres
*/
bool SurfaceDivision::overlap(const Sphere &s1, const Sphere &s2) {
  //  get the large and small spheres
  Sphere sBig = s1, sSmall = s2;
  if (sSmall.r > sBig.r) {
    sBig = s2;
    sSmall = s1;
  }

  //  check if there is there valid overlap (small sphere isn't inside big one)
  float d = sBig.c.distance(sSmall.c);
  return (d < sSmall.r + sBig.r && d > sBig.r - sSmall.r);
}