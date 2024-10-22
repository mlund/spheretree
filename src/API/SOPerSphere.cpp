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

#include "SOPerSphere.h"
#include "SurfaceDiv.h"
#include "../MinMax/Simplex.h"

SOPerSphere::SOPerSphere() {
  eval = NULL;
  numIter = 1;
}

//  optimise
void SOPerSphere::optimise(Array<Sphere> *spheres, const SurfaceRep &surRep,
                           float stopBelow, const Sphere *parSph,
                           int level) const {
  CHECK_DEBUG0(eval != NULL);
  CHECK_DEBUG0(spheres != NULL);

  const Array<Surface::Point> *surPts = surRep.getSurPts();
  int numSpheres = spheres->getSize();
  int numPts = surPts->getSize();
  if (numSpheres < 1 || numPts < 1)
    return;

  //  setup optimise
  Array<Array<Point3D> /**/> pointsInSphere(numSpheres);
  Array<bool> coveredPts(numPts);

  //  optimise
  for (int l = 0; l < numIter; l++) {
    // OUTPUTINFO("Per Sphere Optimise Iter %d\n", l);

    //  make the half spaces to segment the object
    SurfaceDivision surDiv;
    surDiv.setup(*spheres, surPts);

    //  flags for which points are covered
    coveredPts.clear();

    //  assign points to each sphere
    for (int i = 0; i < numSpheres; i++) {
      Sphere *s = &spheres->index(i);
      Array<Point3D> *pts = &pointsInSphere.index(i);
      pts->resize(0);

      Array<Point3D> selPts;
      for (int j = 0; j < numPts; j++) {
        Point3D p = surPts->index(j).p;
        if (!coveredPts.index(j) && surDiv.pointInRegion(p, i)) {
          coveredPts.index(j) = true;
          pts->addItem() = p;
        }
      }
    }

    //  assign uncovered points
    for (int i = 0; i < numPts; i++)
      if (!coveredPts.index(i)) {
        Point3D p = surPts->index(i).p;

        int minJ = 0;
        float minD = FLT_MAX;
        for (int j = 0; j < numSpheres; j++) {
          Sphere s = spheres->index(j);
          float d = s.c.distance(p) - s.r;
          if (d < minD) {
            minD = d;
            minJ = j;
          }
        }

        pointsInSphere.index(minJ).addItem() = p;
      }

    //  do optimisations
    for (int i = 0; i < numSpheres; i++) {
      Sphere s = spheres->index(i);
      optimise(&s, pointsInSphere.index(i));
      spheres->index(i) = s;
    }
  }
}

void SOPerSphere::optimise(Sphere *s, const Array<Point3D> &pts) const {
  CHECK_DEBUG0(eval != NULL);
  CHECK_DEBUG0(s != NULL);

  OptInfo optInf;
  optInf.eval = eval;
  optInf.selPts = &pts;

  //  initial vals
  double vals[3] = {s->c.x, s->c.y, s->c.z};
  double sizes[3] = {s->r, s->r, s->r};

  int maxFunc = 100;
  simplexMin(3, vals, sizes, 1E-5, &maxFunc, fitFunc, &optInf, 3);

  s->c.x = vals[0];
  s->c.y = vals[1];
  s->c.z = vals[2];

  //  generate radius
  s->r = -1;
  int numPts = pts.getSize();
  for (int i = 0; i < numPts; i++) {
    Point3D p = pts.index(i);
    float d = p.distance(s->c);
    if (d > s->r)
      s->r = d;
  }
}

double SOPerSphere::fitFunc(double vals[], void *data, int *canFinish) {
  OptInfo *optInf = (OptInfo *)data;

  Sphere s;
  s.c.x = vals[0];
  s.c.y = vals[1];
  s.c.z = vals[2];
  s.r = -1;

  //  generate radius
  int numPts = optInf->selPts->getSize();
  for (int i = 0; i < numPts; i++) {
    Point3D p = optInf->selPts->index(i);
    float d = p.distance(s.c);
    if (d > s.r)
      s.r = d;
  }

  if (!finite(s.r))
    return DBL_MAX;
  else
    return optInf->eval->evalSphere(s);
}
