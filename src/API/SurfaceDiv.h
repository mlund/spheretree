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
    divides surface/space into regions in a set of spheres
*/
#ifndef _SURFACE_DIVISION_H_
#define _SURFACE_DIVISION_H_

#include "../Geometry/Plane.h"
#include "../Geometry/Sphere.h"
#include "../Storage/Array2D.h"
#include "../Surface/Surface.h"

class SurfaceDivision {
public:
  //  setup the subdivision
  void setup(const Array<Sphere> &spheres,
             const Array<Surface::Point> *surPts = NULL);

  //   test point in region
  bool pointInRegion(const Point3D &p, int region) const;

private:
  //  make the dividing plane
  void makeDividingPlane(Plane *pl, const Sphere &testSph, const Sphere &s,
                         const Array<Surface::Point> *surPts);

  //  create the plane of intersection between two spheres such that
  //  points in testSph are infront of the plane
  void computeIntersectionPlane(Plane *pl, const Sphere &testSph,
                                const Sphere &s);

  //  plane optimise
  struct HalfPlaneOptInfo {
    const Array<Surface::Point> *surPts;
    const Array<int> *inBoth, *inFront, *behind;
  };

  static double evaluatePlane(const Plane &testPlane,
                              const Array<Surface::Point> &surPts,
                              const Array<int> &inBoth,
                              const Array<int> &inFront,
                              const Array<int> &inBehind);

  static double halfPlaneOptFunc(double vals[], void *data, int *canFinish);

  static void optimiseHalfPlane(Plane *pl, const Array<Surface::Point> &surPts,
                                const Array<int> &inBoth,
                                const Array<int> &behind,
                                const Array<int> &infront);

  //  check for valid overlap
  static bool overlap(const Sphere &s1, const Sphere &s2);

private:
  Array<Sphere> spheres;
  Array2D<Plane> planes;
};

#endif