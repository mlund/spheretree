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

#include "SOSimplex.h"
#include "SurfaceDiv.h"
#include "SOPerSphere.h"
#include "../MinMax/Simplex.h"
#include "../Base/Defs.h"

#define WORST_WEIGHT 100.0f
#define RMS_WEIGHT 1.0f

SOSimplex::SOSimplex() {
  sphereFitter = NULL;
  sphereEval = NULL;
  useVol = false;
  useErr = true;
}

/*
    optimise
*/
void SOSimplex::optimise(Array<Sphere> *inSph, const SurfaceRep &surRep,
                         float stopBelow, const Sphere *parSph,
                         int level) const {
  CHECK_DEBUG0(sphereFitter != NULL);
  CHECK_DEBUG0(!useErr || sphereEval != NULL);

  const Array<Surface::Point> *surPts = surRep.getSurPts();

  int numSph = inSph->getSize();
  int numPts = surPts->getSize();
  if (numSph == 0 || numPts == 0)
    return;

  //  do local sphere optimisation
  if (useErr) {
    SOPerSphere perSphere;
    perSphere.numIter = 5;
    perSphere.eval = sphereEval;
    perSphere.optimise(inSph, surRep);
  }

  //  assign each point to the spheres and setup initial error
  Array<double> initErr;
  initErr.resize(numSph);
  Array<Array<int> /**/> sphInd;
  sphInd.resize(numSph);
  assignPointsAndGetErr(&sphInd, &initErr, inSph, surRep, parSph);
  double bestErr = computeErrorMetric(initErr, *inSph);

  //  temporary space for configuration
  Array<Sphere> workSpheres;

  //  iterative optimisation
  Array<bool> done;
  done.resize(numSph);
  int numIter = numSph > 10 ? 10 : numSph / 2;
  for (int l = 0; l < numIter; l++) {
    done.clear();
    bool changed = false;

    for (int i = 0; i < numSph; i++) {
      //  find the worst sphere that hasn't had a turn
      int worstJ = -1;
      double worstErr = -REAL_MAX;
      for (int j = 0; j < numSph; j++) {
        if (!done.index(j)) {
          double err = initErr.index(j);
          if (err > worstErr) {
            worstErr = err;
            worstJ = j;
          }
        }
      }
      if (worstJ < 0)
        break;

      OUTPUTINFO("Doing Sphere %d(%d)\n", worstJ, l);

      //  do optimisation of that sphere
      workSpheres.clone(*inSph);
      if (optimiseSphere(&workSpheres, &sphInd, &initErr, surRep, worstJ,
                         parSph)) {
        // check worst error
        if (stopBelow > 0) {
          double worstErr = 0;
          int numSph = initErr.getSize();
          for (int i = 0; i < numSph; i++) {
            double e = initErr.index(i);
            if (e > worstErr)
              worstErr = e;
          }

          if (worstErr < stopBelow) {
            inSph->clone(workSpheres);
            OUTPUTINFO("worstErr = %f stopBelow = %f \n", worstErr, stopBelow);
            OUTPUTINFO("Done (Early)...\n");
            return;
          }
        }

        double newErr = computeErrorMetric(initErr, workSpheres);
        if (newErr < bestErr) {
          if ((bestErr - newErr) / bestErr > 1E-3 /*EPSILON_LARGE*/)
            changed = true;

          inSph->clone(workSpheres);
          bestErr = newErr;
        }
      }

      //  flag sphere as done
      done.index(worstJ) = true;
    }

    if (!changed)
      break;
  }

  //  do local sphere optimisation
  if (useErr) {
    SOPerSphere perSphere;
    perSphere.numIter = 5;
    perSphere.eval = sphereEval;
    perSphere.optimise(inSph, surRep);
  }

  OUTPUTINFO("Done...\n");
}

bool SOSimplex::optimiseSphere(Array<Sphere> *sph,
                               Array<Array<int> /**/> *sphInd,
                               Array<double> *initErr, const SurfaceRep &surRep,
                               int optNum, const Sphere *parSph) const {
  const Array<Surface::Point> *surPts = surRep.getSurPts();

  int numSph = sph->getSize();
  int numPts = surPts->getSize();

  //  get initial info
  assignPointsAndGetErr(sphInd, initErr, sph, surRep, parSph);
  double startErr = computeErrorMetric(*initErr, *sph);
  OUTPUTINFO("StartErr = %g\n", startErr);

  //  setup for simplex
  OptInfo optInfo;
  optInfo.repInd = optNum;
  optInfo.sphInd = sphInd;
  optInfo.srcSph = sph;
  optInfo.startErr = initErr;
  optInfo.surRep = &surRep;
  optInfo.parSph = parSph;
  optInfo.that = this;
  optInfo.resErr = DBL_MAX;
  Sphere s = sph->index(optNum);

  //  do optimise initial guess comes from existing sphere
  int maxFunc = 50;
  double vals[4] = {s.c.x, s.c.y, s.c.z, s.r};
  double sizes[4] = {s.r / 2, s.r / 2, s.r / 2, s.r / 2};
  simplexMin(4, vals, sizes, EPSILON_LARGE, &maxFunc, configEval, &optInfo, 3);

  //  get final error
  double finErr = optInfo.resErr;
  OUTPUTINFO("Final Error : %g\n", finErr);

  if (finErr > startErr + 1E-3) {
    //  this does occasionally happen - i have investigated it and found out
    //  that everything is the same for both run - but that sometimes the
    //  minimum volume sphere is different
    //  therefore we now store the best config in optInfo
    OUTPUTINFO("ERROR GOT WORSE ???\n");
  }

  if ((startErr - finErr) / startErr > EPSILON_LARGE) {
    //  update the state of the system
    for (int i = 0; i < numSph; i++) {
      sph->index(i) = optInfo.resSph.index(i);
      initErr->index(i) = optInfo.resErrors.index(i);
      sphInd->index(i).clone(optInfo.resInd.index(i));
    }

    return true;
  } else {
    OUTPUTINFO("Unchanged...\n");
    return false;
  }
}

/*
    Evaluation function for the simplex algorithm

    vals[0] = s.c.x
    vals[1] = s.c.y
    vals[2] = s.c.z
    vals[3] = s.r

*/
double SOSimplex::configEval(double vals[], void *data, int *canFinish) {
  //  get info
  SOSimplex::OptInfo *optInfo = (OptInfo *)data;

  // make sure the radius is positive
  if (vals[3] < 0)
    vals[3] *= -1;

  //  make the replacement sphere
  Sphere newS = {vals[0], vals[1], vals[2], vals[3]};

  Array<Sphere> newSph;
  Array<double> newErr;
  Array<Array<int> /**/> newSphInd;
  optInfo->that->generateNewConfig(&newSph, &newSphInd, &newErr,
                                   *optInfo->srcSph, *optInfo->sphInd,
                                   *optInfo->startErr, *optInfo->surRep, newS,
                                   optInfo->repInd, optInfo->parSph);

  double err = optInfo->that->computeErrorMetric(newErr, newSph);
  if (!finite(err))
    return DBL_MAX;

  //  store config - so we are guaranteed to be able to reproduce it
  if (err < optInfo->resErr) {
    optInfo->resErr = err;
    optInfo->resErrors.clone(newErr);
    optInfo->resSph.clone(newSph);

    //  copy indices
    int numInd = newSphInd.getSize();
    optInfo->resInd.resize(numInd);
    for (int i = 0; i < numInd; i++)
      optInfo->resInd.index(i).clone(newSphInd.index(i));
  }

  return err;
}

/*
    Error metric
*/
double SOSimplex::evalSphere(const Sphere &s, const Sphere *parSph) const {
  double e = 0;

  if (useErr) {
    e = sphereEval->evalSphere(s);
    if (useVol)
      e *= 4.0 / 3.0 * 3.1415927 * s.r;
  } else if (useVol) {
    e = 4.0 / 3.0 * 3.1415927 * s.r;
  }

  return e;
}

double SOSimplex::myRefitSphere(Sphere *s, const Array<Surface::Point> &pts,
                                const Array<int> &inds,
                                const Point3D &pC) const {
  CHECK_DEBUG0(s != NULL);

  if (!useErr) {
    //  jusr fit minVol sphere
    sphereFitter->fitSphere(s, pts, inds);
    return evalSphere(*s, NULL);
  } else {
    //  records for the spheres
    struct SPHEREINFO {
      Sphere s;
      double err;
    } spheres[2];

    //  use the sphere fitting algorithm
    sphereFitter->fitSphere(&spheres[0].s, pts, inds);
    spheres[0].err = evalSphere(spheres[0].s, NULL);

    //  generate from existing center
    double maxR = 0;
    int numInds = inds.getSize();
    for (int j = 0; j < numInds; j++) {
      int pI = inds.index(j);
      double d = pC.distanceSQR(pts.index(pI).p);
      if (d > maxR)
        maxR = d;
    }
    spheres[1].s.c = pC;
    spheres[1].s.r = sqrt(maxR);
    spheres[1].err = evalSphere(spheres[1].s, NULL);

    //  search for sphere with min error
    int minI = -1;
    double minErr = FLT_MAX;
    for (int i = 0; i < 2; i++) {
      double err = spheres[i].err;
      if (finite(err) && err < minErr) {
        minI = i;
        minErr = err;
      }
    }

    *s = spheres[minI].s;
    return minErr;
  }
}

double SOSimplex::computeErrorMetric(const Array<double> &err,
                                     const Array<Sphere> &sph) {
  //  compute fit measures
  int numSph = err.getSize();
  double sumSqr = 0, worstErr = 0;
  for (int i = 0; i < numSph; i++) {
    double e = fabs(err.index(i)) + 1;
    sumSqr += e * e;
    if (e > worstErr)
      worstErr = e;
  }
  double errMet =
      WORST_WEIGHT * numSph * worstErr * worstErr + RMS_WEIGHT * sumSqr;
  return errMet;

  /*
    //  compute overlap measures
    double sumSqrOverlap = 0, worstOverlap = 0;
    for (i = 0; i < numSph; i++){
      Sphere s1 = sph.index(i);
      for (int j = i+1; j < numSph; j++){
        Sphere s2 = sph.index(j);
        if (s1.overlap(s2)){
          Vector3D v(s2.c, s1.c);
          v.norm();

          Point3D p1;
          v.add(&p1, s1.c, s1.r);

          Point3D p2;
          v.add(&p2, s2.c, -1*s2.r);

          double overlap = p2.distance(p1);
          sumSqrOverlap += overlap * overlap;
          if (overlap > worstOverlap)
            worstOverlap = overlap;
          }
        }
      }
    double ovrMet = WORST_WEIGHT*numSph*worstOverlap*worstOverlap +
    RMS_WEIGHT*sumSqrOverlap;
  */
}

/*
    Generate new configuration

    when we make a change to a sphere we will need to do the following to
   guanantee full surface coverage is efficiently maintained

      1. Assign each uncovered point to a new sphere
      2. Remove any newly covered points from their existing spheres

    Any spheres can be altered except the control one.  They are reassigned a
   new set of points and the error is computed to give a quantity to the
   configuration

    ** The control sphere may be reassigned points if it is as the end if the
   model **
*/
void SOSimplex::generateNewPointConfig(Array<Array<int> /**/> *newSphInd,
                                       Array<bool> *changed,
                                       const Array<Array<int> /**/> &sphInd,
                                       const Array<Sphere> &srcSph,
                                       const SurfaceRep &surRep,
                                       const Sphere &repSph, int repInd) const {

  const Array<Surface::Point> *surPts = surRep.getSurPts();

  int numSph = srcSph.getSize();
  int numPts = surPts->getSize();
  newSphInd->resize(numSph);

  //  make list of points contained in repSph
  Array<bool> cont;
  cont.resize(numPts);
  cont.clear();

  Array<int> *repDest = &newSphInd->index(repInd);
  repDest->setSize(0);
  surRep.listContainedPoints(repDest, &cont, repSph);

  //  flag all spheres as unchanged
  changed->resize(numSph);
  changed->clear();

  //  generate lists of contained points
  //  exclusing the ones covered by the replacement sphere
  for (int i = 0; i < numSph; i++) {
    if (i == repInd) {
      continue;
    }
    if (!repSph.overlap(srcSph.index(i))) {
      newSphInd->index(i).clone(sphInd.index(i));
      continue;
    }

    const Array<int> *src = &sphInd.index(i);
    int numInd = src->getSize();

    Array<int> *dst = &newSphInd->index(i);
    dst->resize(0);

    //  process the list of points
    for (int j = 0; j < numInd; j++) {
      int ind = src->index(j);
      if (!cont.index(ind))
        dst->addItem() = ind;
    }

    //  record if the set of indices had changed
    changed->index(i) = dst->getSize() != numInd;
  }

  //  add uncovered points from the modified sphere
  //  to the closest of the other spheres
  const Array<int> *repPts = &sphInd.index(repInd);
  int numRepPts = repPts->getSize();
  for (int i = 0; i < numRepPts; i++) {
    int ind = repPts->index(i);
    if (!cont.index(ind)) {
      Point3D p = surPts->index(ind).p;

      //  assign the point to a new sphere
      int closestJ = -1;
      double closestD = DBL_MAX;
      for (int j = 0; j < numSph; j++) {
        if (j == repInd)
          continue;

        Sphere testSph = srcSph.index(j);
        double d = testSph.c.distance(p) - testSph.r;
        if (d < closestD) {
          closestD = d;
          closestJ = j;

          if (d <= 0)
            break;
        }
      }

      //  REMOVED 03/2002 as it messes with volume based optimisation
      //                  probably not too good for the error opt anyways
      // reassign point to control sphere if needed
      // double dRep = repSph.c.distance(p) - repSph.r;
      // if (dRep < closestD && closestD > repSph.r*1.5)
      //  closestJ = repInd;

      newSphInd->index(closestJ).addItem() = ind;
      changed->index(closestJ) = true;
    }
  }
}

void SOSimplex::generateNewConfig(
    Array<Sphere> *newSph, Array<Array<int> /**/> *newSphInd,
    Array<double> *newErr, const Array<Sphere> &srcSph,
    const Array<Array<int> /**/> &srcSphInd, const Array<double> &srcErr,
    const SurfaceRep &surRep, const Sphere &repSph, int repInd,
    const Sphere *parSph) const {
  CHECK_DEBUG(sphereFitter != NULL, "need sphere fitter");
  CHECK_DEBUG(!useErr || sphereEval != NULL, "need sphere eval");

  //  setup arrays
  int numSph = srcSph.getSize();
  newSph->resize(numSph);
  newSphInd->resize(numSph);
  newErr->resize(numSph);

  //  generate point config
  Array<bool> changed;
  generateNewPointConfig(newSphInd, &changed, srcSphInd, srcSph, surRep, repSph,
                         repInd);

  const Array<Surface::Point> *surPts = surRep.getSurPts();

  //  generate the new bounding spheres
  for (int i = 0; i < numSph; i++) {
    if (i == repInd) {
      if (changed.index(i)) {
        newErr->index(i) = myRefitSphere(&newSph->index(i), *surPts,
                                         newSphInd->index(i), repSph.c);
      } else {
        newSph->index(i) = repSph;
        newErr->index(i) = evalSphere(repSph, parSph);
      }
    } else if (!changed.index(i)) {
      newSph->index(i) = srcSph.index(i);
      newSphInd->index(i).clone(srcSphInd.index(i));
      newErr->index(i) = srcErr.index(i);
    } else {
      newErr->index(i) = myRefitSphere(&newSph->index(i), *surPts,
                                       newSphInd->index(i), srcSph.index(i).c);
      if (!finite(newErr->index(i))) {
        newSph->index(i) =
            srcSph.index(i); //  keep old - what about new cover ??
        newErr->index(i) = srcErr.index(i);
      }
    }
  }
}

/*
  assign the points to the spheres and compute the initial errors for each
  sphere
*/
void SOSimplex::assignPointsAndGetErr(Array<Array<int> /**/> *sphInd,
                                      Array<double> *initErr,
                                      Array<Sphere> *sph,
                                      const SurfaceRep &surRep,
                                      const Sphere *parSph) const {
  CHECK_DEBUG(sphereFitter != NULL, "need sphere fitter");
  CHECK_DEBUG(!useErr || sphereEval != NULL, "need sphere eval");

  //  get points
  const Array<Surface::Point> *surPts = surRep.getSurPts();
  int numSph = sph->getSize();
  int numPts = surPts->getSize();

  //  flags for which points are used
  Array<bool> used;
  used.resize(numPts);
  used.clear();

  //  make half spaces for this sphere
  SurfaceDivision surDiv;
  surDiv.setup(*sph, surPts);

  //  assign points to sphere
  for (int i = 0; i < numSph; i++) {
    //  get points in sphere
    Array<int> *ptInd = &sphInd->index(i);
    ptInd->setSize(0);
    surRep.listContainedPoints(ptInd, NULL, sph->index(i), &used);

    //  check against the half spaces
    int oI = 0;
    int numInd = ptInd->getSize();
    for (int j = 0; j < numInd; j++) {
      int pI = ptInd->index(j);
      Point3D p = surPts->index(pI).p;
      if (surDiv.pointInRegion(p, i)) {
        ptInd->index(oI++) = pI;
        used.index(pI) = true;
      }
    }
    ptInd->setSize(oI);
  }

  //  for those unassigned points - assign to closest point
  for (int i = 0; i < numPts; i++) {
    if (!used.index(i)) {
      //  find the closest sphere
      int minJ = -1;
      double minD = REAL_MAX;
      Point3D p = surPts->index(i).p;
      for (int j = 0; j < numSph; j++) {
        double d = sph->index(j).c.distance(p) - sph->index(j).r;
        if (d < minD) {
          minD = d;
          minJ = j;
        }
      }

      sphInd->index(minJ).addItem() = i;
    }
  }

  //  do sphere refits
  for (int i = 0; i < numSph; i++) {
    Sphere newS;
    Point3D pOld = sph->index(i).c;
    double err = myRefitSphere(&newS, *surPts, sphInd->index(i), pOld);
    if (finite(err)) {
      sph->index(i) = newS;
      initErr->index(i) = err;
    }
  }
}

/*
    Overlap function
*/
double SOSimplex::computeOverlap(const Array<Sphere> &sph, int J) {
  double maxOverlap = 0;
  int numSph = sph.getSize();
  Sphere s1 = sph.index(J);
  for (int i = 0; i < numSph; i++) {
    if (i == J)
      continue;

    Sphere s2 = sph.index(i);
    if (s1.overlap(s2)) {
      double overlap = (s1.r + s2.r - s1.c.distance(s2.c));
      if (overlap > maxOverlap)
        maxOverlap = overlap;
    }
  }

  return maxOverlap;
}
